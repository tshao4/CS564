////////////////////////////////////////////////////////////////////////////////
//                   ALL STUDENTS COMPLETE THESE SECTIONS
// Main Class File:  minirel.cpp
// File:             create.cpp
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


const Status RelCatalog::createRel(const string & relation, 
					 const int attrCnt,
					 const attrInfo attrList[])
{
	Status status;
	RelDesc rd;
	AttrDesc ad;

	if (relation.empty() || attrCnt < 1)
		return BADCATPARM;

	if (relation.length() >= sizeof rd.relName)
		return NAMETOOLONG;
	
	// make sure the relation doesn't already exist

	status = getInfo(relation, rd);
	if (status == OK) {
		return RELEXISTS;
	}
	if (status != RELNOTFOUND) {
		return status;
	}

	// make sure there are no duplicate attribute names

	unsigned int tupleWidth = attrList[0].attrLen;

	if (attrCnt > 1) {
		for(int i = 1; i < attrCnt; i++) {
			tupleWidth += attrList[i].attrLen;
			for(int j = 0; j < i; j++) {
				if (strcmp(attrList[i].attrName, attrList[j].attrName) == 0) {
					return DUPLATTR;
				}
			}
		}
	}

	if (tupleWidth > PAGESIZE) {
		return ATTRTOOLONG;
	}

	// insert information about relation

	strcpy(rd.relName, relation.c_str());
	rd.attrCnt = attrCnt;
	if ((status = addInfo(rd)) != OK) {
		return status;
	}

	// insert information about attributes

	strcpy(ad.relName, relation.c_str());
	int offset = 0;
	for(int i = 0; i < attrCnt; i++) {
		if (strlen(attrList[i].attrName) >= sizeof ad.attrName) {
			return NAMETOOLONG;
		}
		strcpy(ad.attrName, attrList[i].attrName);
		ad.attrOffset = offset;
		ad.attrType = attrList[i].attrType;
		ad.attrLen = attrList[i].attrLen;
		if ((status = attrCat->addInfo(ad)) != OK) {
			return status;
		}
		offset += ad.attrLen;
	}

	// now create the actual heapfile to hold the relation
	status = createHeapFile (relation);
	return status;
}