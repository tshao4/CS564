////////////////////////////////////////////////////////////////////////////////
//                   ALL STUDENTS COMPLETE THESE SECTIONS
// Main Class File:  minirel.cpp
// File:             destroy.cpp
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
#include <stdlib.h>

//
// Destroys a relation. It performs the following steps:
//
// 	removes the catalog entry for the relation
// 	destroys the heap file containing the tuples in the relation
//
// Returns:
// 	OK on success
// 	error code otherwise
//

const Status RelCatalog::destroyRel(const string & relation)
{
	Status status;

	if (relation.empty() || 
		relation == string(RELCATNAME) || 
		relation == string(ATTRCATNAME))
		  return BADCATPARM;

	// delete attrcat entries

	if ((status = attrCat->dropRelation(relation)) != OK) {
		return status;
	}

    // delete entry from relcat

	if ((status = removeInfo(relation)) != OK) {
		return status;
	}

    // destroy file
	if ((status = destroyHeapFile(relation)) != OK) {
		return status;
	}

	return OK;
}


//
// Drops a relation. It performs the following steps:
//
// 	removes the catalog entries for the relation
//
// Returns:
// 	OK on success
// 	error code otherwise
//

const Status AttrCatalog::dropRelation(const string & relation)
{
	Status status;
	AttrDesc *attrs;
	int attrCnt, i;

	if (relation.empty()) return BADCATPARM;

	// get attribute information

	if ((status = getRelInfo(relation, attrCnt, attrs)) != OK)
		return status;

	// remove entries from catalog

	for(i = 0; i < attrCnt; i++) {
		if ((status = removeInfo(relation, attrs[i].attrName)) != OK){
			return status;
		}
	}

	free(attrs);

	return OK;
}


