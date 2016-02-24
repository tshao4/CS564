////////////////////////////////////////////////////////////////////////////////
//                   ALL STUDENTS COMPLETE THESE SECTIONS
// Main Class File:  minirel.cpp
// File:             delete.cpp
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
 * Deletes records from a specified relation.
 *
 * Returns:
 * 	OK on success
 * 	an error code otherwise
 */

const Status QU_Delete(const string & relation, 
		       const string & attrName, 
		       const Operator op,
		       const Datatype type, 
		       const char *attrValue)
{
	Status status;
  	HeapFileScan* hfs = new HeapFileScan(relation, status);
  	if(status != OK) {
  		return status;
	}

  	AttrDesc attrDesc;
  	RID rid;

  	attrCat->getInfo(relation, attrName, attrDesc);

	int offset = attrDesc.attrOffset;
	int length = attrDesc.attrLen;

	int intValue;
	float floatValue;

	switch(type) {
		case STRING:
			status = hfs->startScan(offset, length, type, attrValue, op);
			break;
	
		case INTEGER:
		 	intValue = atoi(attrValue);
			status = hfs->startScan(offset, length, type, (char *)&intValue, op);
			break;
	
		case FLOAT:
			floatValue = atof(attrValue);
			status = hfs->startScan(offset, length, type, (char *)&floatValue, op);
			break;
	}
	
  	if (status != OK) {
    	delete hfs;
    	return status;
  	}

  	while((status = hfs->scanNext(rid)) == OK) {
    	if ((status = hfs->deleteRecord()) != OK)
    		return status;
  	}

	hfs->endScan();
    delete hfs;

  	return OK;
}


