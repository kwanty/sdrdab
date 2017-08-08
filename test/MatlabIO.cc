/*
 * MatlabIO.cc
 *
 *  Created on: 23 lut 2015
 *      Author: kwant
 */

#include <cstdio>
#include <cstdlib>
#include <cmath>
#include <iostream>
#include <fstream>

#include "MatlabIO.h"

MatlabIO::MatlabIO() {
	// TODO Auto-generated constructor stub

}

MatlabIO::~MatlabIO() {
	// TODO Auto-generated destructor stub
}

bool MatlabIO::ReadData(float* data, const char* filename, size_t length) {
	FILE *fp = fopen( filename, "r" );
	if( !fp )
		return false;

	char buff[128];
	for( size_t i=0; i<length; i++ ) {
		if( NULL == fgets( buff, 128, fp ) ){
			fclose( fp );
			return false;
		}
		data[i] = atof( buff );
	}

	fclose( fp );
	return true;
}

bool MatlabIO::WriteData(const  float* data, const char* filename, size_t length) {
	FILE *fp = fopen( filename, "w" );
	if( !fp )
		return false;

	for( size_t i=0; i<length; i++ ) {
		fprintf( fp, "%1.18f\n", data[i] );
	}

	fclose( fp );
	return true;
}

bool MatlabIO::ReadData(unsigned char* data, const char* filename, size_t length) {
	FILE *fp = fopen( filename, "r" );
	if( !fp )
		return false;

	char buff[128];
	for( size_t i=0; i<length; i++ ) {
		if( NULL==fgets( buff, 128, fp ) ){
			fclose( fp );
			return false;
		}
		data[i] = atof( buff );
	}

	fclose( fp );
	return true;
}

bool MatlabIO::WriteData(unsigned char* data, const char* filename, size_t length) {
	FILE *fp = fopen( filename, "w" );
	if( !fp )
		return false;

	for( size_t i=0; i<length; i++ ) {
		fprintf( fp, "%d\n", data[i] );
	}

	fclose( fp );
	return true;
}


bool MatlabIO::ReadData(size_t* data, const char* filename, size_t length) {
	FILE *fp = fopen( filename, "r" );
	if( !fp )
		return false;

	char buff[128];
	for( size_t i=0; i<length; i++ ) {
		if( NULL==fgets( buff, 128, fp ) ){
			fclose( fp );
			return false;
		}

		data[i] = strtoul( buff, NULL, 10 );
	}

	fclose( fp );
	return true;
}

bool MatlabIO::WriteData(size_t* data, const char* filename, size_t length) {
	FILE *fp = fopen( filename, "w" );
	if( !fp )
		return false;

	for( size_t i=0; i<length; i++ ) {
		fprintf( fp, "%lu\n", data[i] );
	}

	fclose( fp );
	return true;
}

bool MatlabIO::ReadData(size_t datasize, const char* filename, unsigned char* ldata){

	fstream file(filename);
	if (!file.good())return false;

	size_t * temp = new size_t[datasize * 8];
	size_t linecount = 0;

	while (!file.eof()) {
		file >> temp[linecount];
		linecount++;

	}

	for (size_t i = 0; i < datasize; i++){
		*ldata = 0;
		for (size_t k = 0; k < 8; k++){
			if (*temp == 1){
				*ldata |= 128 >> k;
			}
			temp++;
		}
		ldata++;
	}
	temp -= datasize * 8;
	ldata -= datasize;

	delete [] temp;

	return true;
}
