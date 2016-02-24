////////////////////////////////////////////////////////////////////////////////
//                   ALL STUDENTS COMPLETE THESE SECTIONS
// Main Class File:  testbuf.cpp
// File:             buf.cpp
// Semester:         CS564 Fall 2014
//
// Author:           Tianyu Shao
// Email:            tshao4@wisc.edu
// CS Login:         tshao
// Lecturer's Name:  Jeff Naughton
//
//////////////////// PAIR PROGRAMMERS COMPLETE THIS SECTION ////////////////////
//
// Pair Partner:     Jia Zhong
// Email:            jhzhong@wisc.edu
// CS Login:         jzhong
// Lecturer's Name:  Jeff Naughton
//
//////////////////////////// 80 columns wide ///////////////////////////////////

#include <memory.h>
#include <unistd.h>
#include <errno.h>
#include <stdlib.h>
#include <fcntl.h>
#include <iostream>
#include <stdio.h>
#include "page.h"
#include "buf.h"

#define ASSERT(c)  { if (!(c)) { \
		       cerr << "At line " << __LINE__ << ":" << endl << "  "; \
                       cerr << "This condition should hold: " #c << endl; \
                       exit(1); \
		     } \
                   }

//----------------------------------------
// Constructor of the class BufMgr
//----------------------------------------

BufMgr::BufMgr(const int bufs)
{
    numBufs = bufs;

    bufTable = new BufDesc[bufs];
    memset(bufTable, 0, bufs * sizeof(BufDesc));
    for (int i = 0; i < bufs; i++) 
    {
        bufTable[i].frameNo = i;
        bufTable[i].valid = false;
    }

    bufPool = new Page[bufs];
    memset(bufPool, 0, bufs * sizeof(Page));

    int htsize = ((((int) (bufs * 1.2))*2)/2)+1;
    hashTable = new BufHashTbl (htsize);  // allocate the buffer hash table

    clockHand = bufs - 1;
}

/**
 *	Flushes out all dirty pages and deallocates the buffer pool and the 
 *	BufDesc table.
 */
BufMgr::~BufMgr() {
    
    // flush pages
    for (int i = 0; i < numBufs; i++) {
        BufDesc* tmp = &bufTable[i];
        if (tmp->valid && tmp->dirty) {
            tmp->file->writePage(tmp->pageNo, &(bufPool[i]));
        }
    }
    delete hashTable;
    delete [] bufTable;
    delete [] bufPool;
}

/**
 *	Allocates a free frame using the clock algorithm; if necessary, writing 
 *	a dirty page back to disk. 
 *
 *  Returns BUFFEREXCEEDED if all buffer frames are pinned, 
 *          UNIXERR if the call to the I/O layer returned an error when a 
 *                  dirty page was being written to disk
 *          OK otherwise.
 */
const Status BufMgr::allocBuf(int & frame) {
    
    // find free buffer frame
    Status status = OK;
    int nDone = 0;
    bool fnd = false;
    while (nDone < 2*numBufs) {
        advanceClock();
        nDone++;
        
        // invalid, use it
        if (!bufTable[clockHand].valid) {
            break;
        }
        
        // valid, check refbit
        if (! bufTable[clockHand].refbit) {
            // check pin count
            if (bufTable[clockHand].pinCnt == 0) {
                // !referenced && !pinned, use it
                hashTable->remove(bufTable[clockHand].file,
                                  bufTable[clockHand].pageNo);
                fnd = true;
                break;
            }
        }
        else {
            // referenced, clear refbit
            bufStats.accesses++;
            bufTable[clockHand].refbit = false;
        }
    }
    
    // check if buffer is full
    if (!fnd && nDone >= numBufs * 2) {
        return BUFFEREXCEEDED;
    }
    
    // flush to disk if dirty bit is set
    if (bufTable[clockHand].dirty) {
        bufStats.diskwrites++;
        
        status = bufTable[clockHand].file->writePage(bufTable[clockHand].pageNo,
                                                     &bufPool[clockHand]);
        if (status != OK) {
            return status;
        }
    }
    frame = clockHand;
    
    return OK;
}

/**
 *  First check whether the page is already in the buffer pool by invoking 
 *  the lookup() method on the hashtable to get a frame number. There are
 *  two cases to be handled depending on the outcome of the lookup() call:
 *
 *  Case 1) Page is not in the buffer pool. Call allocBuf() to allocate a
 *      buffer frame and then call the method file->readPage() to read the
 *      page from disk into the buffer pool frame. Next, insert the page into
 *      the hashtable. Finally, invoke Set() on the frame to set it up
 *      properly. Set() will leave the pinCnt for the page set to 1. Return
 *      a pointer to the frame containing the page via the page parameter.
 *
 *  Case 2) Page is in the buffer pool. In this case set the appropriate
 *      refbit, increment the pinCnt for the page, and then return a pointer
 *      to the frame containing the page via the page parameter.
 *
 *  Returns OK if no errors occurred, 
 *          UNIXERR if a Unix error occurred, 
 *          BUFFEREXCEEDED if all buffer frames are pinned, 
 *          HASHTBLERROR if a hash table error occurred.
 */
const Status BufMgr::readPage(File* file, const int PageNo, Page*& page){
    // check if exist in buffer pool
    int frameID = 0;
    Status status;
    
    if ((status = hashTable->lookup(file, PageNo, frameID)) == OK) {
        // set refbit
        bufTable[frameID].refbit = true;
        bufTable[frameID].pinCnt++;
        page = &bufPool[frameID];
    }
    else {
        // allocate new frame, read page into it
        if ((status = allocBuf(frameID)) != OK) {
            return status;
        }
        bufStats.diskreads++;
        if ((status = file->readPage(PageNo, &bufPool[frameID])) != OK) {
            return status;
        }
        bufTable[frameID].Set(file, PageNo);
        page = &bufPool[frameID];
        // put into hashtable
        if ((status = hashTable->insert(file, PageNo, frameID)) != OK) {
            return status;
        }
        
    }
    
    return OK;
}

/**
 *  Decrements the pinCnt of the frame containing (file, PageNo) and, if 
 *  dirty == true, sets the dirty bit.
 *
 *  Returns OK if no errors occurred, 
 *          HASHNOTFOUND if the page is not in the buffer pool hash table, 
 *          PAGENOTPINNED if the pin count is already 0.
 */
const Status BufMgr::unPinPage(File* file, const int PageNo,
                               const bool dirty) {
    Status status = OK;
    int frameID = 0;
    // lookup the page
    if ((status = hashTable->lookup(file, PageNo, frameID)) != OK) {
        return status;
    }
	// if dirty, set dirty bit
    if (dirty) {
        bufTable[frameID].dirty = dirty;
    }
	// pin count already 0
    if (bufTable[frameID].pinCnt == 0) {
        return PAGENOTPINNED;
    }
	// decrement the pin count
    else {
        bufTable[frameID].pinCnt--;
    }
    return OK;
}

/**
 *  This method will be called by DB::closeFile() when all instances of a 
 *  file have been closed (in which case all pages of the file should have 
 *  been unpinned). flushFile() should scan bufTable for pages belonging to 
 *  the file. For each page encountered it should:
 *
 *  a) if the page is dirty, call file->writePage() to flush the page to 
 *      disk and then set the dirty bit for the page to false
 *  b) remove the page from the hashtable (whether the page is clean or dirty)
 *  c) invoke the Clear() method on the page frame.
 *
 *  Returns OK if no errors occurred and 
 *          PAGEPINNED if some page of the file is pinned.
 */
const Status BufMgr::flushFile(const File* file)
{
    Status status;
    
	// flush buffer frames
    for (int i = 0; i < numBufs; i++) {
        BufDesc* tmp = &(bufTable[i]);
        if (tmp->valid && tmp->file == file) {
            if (tmp->pinCnt > 0) {
                return PAGEPINNED;
            }
            // if dirty, flush the page
            if (tmp->dirty) {
                status = tmp->file->writePage(tmp->pageNo, &(bufPool[i]));
                if (status != OK) {
                    return status;
                }
                tmp->dirty = false;
            }
			// remove page from hashtable
            hashTable->remove(file,tmp->pageNo);
            
            tmp->file = NULL;
            tmp->pageNo = -1;
            tmp->valid = false;
        }
        else if (!tmp->valid && tmp->file == file) {
            return BADBUFFER;
        }
    }
    return OK;
}


/**
 *  Check to see if the page actually exists in the buffer pool by looking 
 *  it up in the hash table. If it does exists, clear the page, remove the 
 *  corresponding entry from the hash table and dispose the page in the file 
 *  as well. 
 *
 *  Return the status of the call to dispose the page in the file.
 */
const Status BufMgr::disposePage(File* file, const int pageNo) {
    Status status;
    int frameID = 0;
	// find & clear frame
    if ((status = hashTable->lookup(file, pageNo, frameID)) == OK) {
        bufTable[frameID].Clear();
    }
	// remove page from hashtable
    hashTable->remove(file, pageNo);
    
    return file->disposePage(pageNo);
}

/**
 *  This call is kind of weird. The first step is to to allocate an empty 
 *  page in the specified file by invoking the file->allocatePage() method.
 *  This method will return the page number of the newly allocated page.
 *  Then allocBuf() is called to obtain a buffer pool frame. Next, an entry 
 *  is inserted into the hash table and Set() is invoked on the frame to set 
 *  it up properly. The method returns both the page number of the newly 
 *  allocated page to the caller via the pageNo parameter and a pointer to 
 *  the buffer frame allocated for the page via the page parameter. 
 *
 *  Returns OK if no errors occurred, 
 *          UNIXERR if a Unix error occurred, 
 *          BUFFEREXCEEDED if all buffer frames are pinned and 
 *          HASHTBLERROR if a hash table error occurred.
 */
const Status BufMgr::allocPage(File* file, int& pageNo, Page*& page) {
    int frameID;

    Status status;
	// allocate a page
    if ((status = file->allocatePage(pageNo)) != OK)  {
        return status;
    }
    // allocate buffer
    if ((status = allocBuf(frameID))!= OK) {
        return status;
    }
	// add to hashtable
    bufTable[frameID].Set(file, pageNo);
    page = &bufPool[frameID];
    
    if ((status = hashTable->insert(file, pageNo, frameID)) != OK) {
        return status;
    }
    return OK;
}


void BufMgr::printSelf(void) 
{
    BufDesc* tmp;
  
    cout << endl << "Print buffer...\n";
    for (int i=0; i<numBufs; i++) {
        tmp = &(bufTable[i]);
        cout << i << "\t" << (char*)(&bufPool[i]) 
             << "\tpinCnt: " << tmp->pinCnt;
    
        if (tmp->valid == true)
            cout << "\tvalid\n";
        cout << endl;
    };
}


