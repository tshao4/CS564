////////////////////////////////////////////////////////////////////////////////
//                   ALL STUDENTS COMPLETE THESE SECTIONS
// Main Class File:  minirel.cpp
// File:             select.cpp
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
#include <stdio.h>
#include <stdlib.h>
using namespace std;


// forward declaration
const Status ScanSelect(const string & result, 
			const int projCnt, 
			const AttrDesc projNames[],
			const AttrDesc *attrDesc, 
			const Operator op, 
			const char *filter,
			const int reclen);

/*
 * Selects records from the specified relation.
 *
 * Returns:
 * 	OK on success
 * 	an error code otherwise
 */

const Status QU_Select(const string & result, 
		       const int projCnt, 
		       const attrInfo projNames[],
		       const attrInfo *attr, 
		       const Operator op, 
		       const char *attrValue)
{
   // Qu_Select sets up things and then calls ScanSelect to do the actual work
    cout << "Doing QU_Select " << endl;
	
	Status status;
	AttrDesc projNames_Descs[projCnt];
	
	for (int i = 0; i < projCnt; i++) {
		status = attrCat->getInfo(projNames[i].relName, projNames[i].attrName, projNames_Descs[i]);
		if (status != OK){
			return status;
		}
	}
	
	AttrDesc *attrDescWhere = NULL;
	int attrValueLen = 0;
	if(attr != NULL) {
		attrDescWhere = new AttrDesc;
		status = attrCat->getInfo(attr->relName, attr->attrName, *attrDescWhere);
		attrValueLen = attrDescWhere->attrLen;
		if (status != OK) {
			return status;
		}
	}

	return ScanSelect(	result, 
						projCnt, 
						projNames_Descs,
						attrDescWhere, 
						op,
						attrValue,
	  					attrValueLen);
}


const Status ScanSelect(const string & result, 
			const int projCnt, 
			const AttrDesc projNames[],
			const AttrDesc *attrDesc, 
			const Operator op, 
			const char *filter,
			const int reclen)
{
    cout << "Doing HeapFileScan Selection using ScanSelect()" << endl;
	Record rec;
	RID rid;
	Status status;

	HeapFileScan* hfs = new HeapFileScan(projNames[0].relName, status);
	if (status != OK) {
		delete hfs;
		return status;
	}

	if(attrDesc == NULL) {
		if ((status = hfs->startScan(0, 0, STRING,  NULL, EQ)) != OK) {
			delete hfs;
			return status;
		}
	}
	else {
		int intValue;
		float floatValue;
		switch(attrDesc->attrType) {
			case STRING:
				status = hfs->startScan(attrDesc->attrOffset, attrDesc->attrLen, (Datatype) attrDesc->attrType, filter, (Operator) op);
				break;
	
			case INTEGER:
		 		intValue = atoi(filter);
				status = hfs->startScan(attrDesc->attrOffset, attrDesc->attrLen, (Datatype) attrDesc->attrType, (char *)&intValue, (Operator) op);
				break;
	
			case FLOAT:
				floatValue = atof(filter);
				status = hfs->startScan(attrDesc->attrOffset, attrDesc->attrLen, (Datatype) attrDesc->attrType, (char *)&floatValue, (Operator) op);
				break;
		}
	
		if(status != OK) {
			delete hfs;
			return status;
		}
	}

	while ((status = hfs->scanNext(rid)) == OK) {
 		if (status == OK) {
  			status = hfs->getRecord(rec);
			if (status != OK) {
				break;
			}
      	
    		attrInfo attrList[projCnt];
			int value = 0; char buffer[33];
			float fValue;
    		for(int i = 0; i < projCnt; i++) {
				AttrDesc attrDesc = projNames[i];
  			
  	  			strcpy(attrList[i].relName, attrDesc.relName);
  	  			strcpy(attrList[i].attrName, attrDesc.attrName);
  	  			attrList[i].attrType = attrDesc.attrType;
  	  			attrList[i].attrLen = attrDesc.attrLen;
  			
  	  			attrList[i].attrValue = (void *) malloc(attrDesc.attrLen);
  			
				switch(attrList[i].attrType) {
					case STRING: 
						 memcpy((char *)attrList[i].attrValue, (char *)(rec.data + attrDesc.attrOffset), attrDesc.attrLen);
						 break;
					 
					case INTEGER: 
						memcpy(&value, (int *)(rec.data + attrDesc.attrOffset), attrDesc.attrLen);
						sprintf((char *)attrList[i].attrValue, "%d", value);
						break;
					 
					case FLOAT: 
 						memcpy(&fValue, (float *)(rec.data + attrDesc.attrOffset), attrDesc.attrLen);
 						sprintf((char *)attrList[i].attrValue, "%f", fValue);
						break;
				}
  			}
	
  			status = QU_Insert(result, projCnt, attrList);
  			if(status != OK) {
				delete hfs;
				return status;
			}
  		}
	}
	return OK;
}
