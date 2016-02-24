////////////////////////////////////////////////////////////////////////////////
//                   ALL STUDENTS COMPLETE THESE SECTIONS
// Main Class File:  minirel.cpp
// File:             load.cpp
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

#include <unistd.h>
#include <fcntl.h>
#include "catalog.h"
#include "utility.h"


//
// Loads a file of (binary) tuples from a standard file into the relation.
// Any indices on the relation are updated appropriately.
//
// Returns:
// 	OK on success
// 	an error code otherwise
//

const Status UT_Load(const string & relation, const string & fileName)
{
	Status status;
	RelDesc rd;
	AttrDesc *attrs;
	int attrCnt;
	InsertFileScan * iFile;
	int width = 0;

	if (relation.empty() || fileName.empty() || relation == string(RELCATNAME)
			|| relation == string(ATTRCATNAME))
		return BADCATPARM;

	// open Unix data file

	int fd;
	if ((fd = open(fileName.c_str(), O_RDONLY, 0)) < 0) {
		return UNIXERR;
	}

	// get relation data

	if ((status = relCat->getInfo(relation, rd)) != OK) {
		return status;
	}
	
	// get attribute data
	if ((status = attrCat->getRelInfo(rd.relName, attrCnt, attrs)) != OK) {
		return status;
	}
	
	// start insertFileScan on relation

	iFile = new InsertFileScan(rd.relName, status);
	if (!iFile) {
		return INSUFMEM;
	}
	if (status != OK) {
		return status;
	}

	// compute width of tuple and open index files, if any
	int i;

	for(i = 0; i < attrCnt; i++) {
		width += attrs[i].attrLen;
	}
	// allocate buffer to hold record read from unix file
	char *record;
	if (!(record = new char [width])) {
		return INSUFMEM;
	}

	int records = 0;
	int nbytes;
	Record rec;

	// read next input record from Unix file and insert it into relation
	while((nbytes = read(fd, record, width)) == width) {
		RID rid;
		rec.data = record;
		rec.length = width;
		if ((status = iFile->insertRecord(rec, rid)) != OK) {
			return status;
		}
		records++;
	}

	// close heap file and unix file
	delete iFile;
	if (close(fd) < 0) {
		return UNIXERR;
	}

	delete [] record;
	free(attrs);

	return OK;
}

