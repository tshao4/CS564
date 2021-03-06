////////////////////////////////////////////////////////////////////////////////
//                   ALL STUDENTS COMPLETE THESE SECTIONS
// Main Class File:  minirel.cpp
// File:             insert.cpp
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

#include "catalog.h"
#include "query.h"
#include <stdlib.h>


/*
 * Inserts a record into the specified relation.
 *
 * Returns:
 * 	OK on success
 * 	an error code otherwise
 */

const Status QU_Insert(const string & relation, 
	const int attrCnt, 
	const attrInfo attrList[])
{
	AttrDesc *attrs;
  	int actualAttrCnt;
  	Status status;

  	if((status = attrCat->getRelInfo(relation, actualAttrCnt, attrs)) != OK) {
    	return status;
	}

	if(actualAttrCnt != attrCnt) {
		return UNIXERR;
	}
	
  	int reclen = 0;
  	for(int i = 0; i < attrCnt; i++) {
    	reclen += attrs[i].attrLen;
	}

  	InsertFileScan ifs(relation, status);
  	ASSERT(status == OK);

  	char *insertData;
  	if(!(insertData = new char [reclen])) {
  		return INSUFMEM;
	}

	int insertOffset = 0;
	int value = 0;
	float fValue = 0;
	for(int i = 0; i < attrCnt; i++) {
		bool attrFound = false;
		for(int j = 0; j < attrCnt; j++) {
			if(strcmp(attrs[i].attrName, attrList[j].attrName) == 0) {
				insertOffset = attrs[i].attrOffset;
			
				switch(attrList[j].attrType) {
					case STRING: 
						memcpy((char *)insertData + insertOffset, (char *)attrList[j].attrValue, attrs[i].attrLen);
						break;
			 		
					case INTEGER: 
						value = atoi((char *)attrList[j].attrValue);
				 		memcpy((char *)insertData + insertOffset, &value, attrs[i].attrLen);
				 		break;
			 		
					case FLOAT: 
						fValue = atof((char *)attrList[j].attrValue);		
						memcpy((char *)insertData + insertOffset, &fValue, attrs[i].attrLen);
				 		break;
				}
			
				attrFound = true;
				break;
			}
		}
	
		if(attrFound == false) {
			delete [] insertData;
			free(attrs);
			return UNIXERR;
		}
	}

  	Record insertRec;
  	insertRec.data = (void *) insertData;
  	insertRec.length = reclen;

  	RID insertRID;
  	status = ifs.insertRecord(insertRec, insertRID);

	delete [] insertData;
	free(attrs);

	return status;
}

