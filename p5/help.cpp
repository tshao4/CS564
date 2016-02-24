////////////////////////////////////////////////////////////////////////////////
//                   ALL STUDENTS COMPLETE THESE SECTIONS
// Main Class File:  minirel.cpp
// File:             help.cpp
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

#include <sys/types.h>
#include <functional>
#include <string.h>
#include <stdio.h>
using namespace std;

#include "error.h"
#include "utility.h"
#include "catalog.h"

// define if debug output wanted


//
// Retrieves and prints information from the catalogs about the for
// the user. If no relation is given (relation.empty() is true), then
// it lists all the relations in the database, along with the width in
// bytes of the relation, the number of attributes in the relation,
// and the number of attributes that are indexed.	If a relation is
// given, then it lists all of the attributes of the relation, as well
// as its type, length, and offset, whether it's indexed or not, and
// its index number.
//
// Returns:
// 	OK on success
// 	error code otherwise
//

const Status RelCatalog::help(const string & relation)
{
	Status status;
	RelDesc rd;
	AttrDesc *attrs;
	int attrCnt;

	if (relation.empty()) {
		return UT_Print(RELCATNAME);
	}
	
	// get relation data

	if ((status = getInfo(relation, rd)) != OK) {
		return status;
	}

	// get attribute data

	if ((status = attrCat->getRelInfo(relation, attrCnt, attrs)) != OK) {
		return status;
	}

	// print relation information

	cout << "Relation name: " << rd.relName << endl;

	printf("Attrname	Offset		Type		Length\n");
	printf("----------	----------	----------	----------\n");
	Datatype t;
	for(int i = 0; i < attrCnt; i++) {
		t = (Datatype)attrs[i].attrType;
		printf("%s		%d		%s		%d\n", attrs[i].attrName,
			attrs[i].attrOffset,
			(t == INTEGER ? "int" : (t == FLOAT ? "float" : "string")),
			attrs[i].attrLen);
	}

	free(attrs);

	return OK;
}
