/*
 * MatlabIO.h
 *
 *  Created on: 23 lut 2015
 *      Author: kwant
 */

#ifndef MATLABIO_H_
#define MATLABIO_H_

//#include <vector>

using namespace std;

/*
 * read/write data in txt format from file of "filename" name to data pointer
 * "data" have to be allocated,
 * data in file should be in decimal format, one number == one line
 * length is a number of elements (=number of lines)
 */

class MatlabIO {
    public:
        MatlabIO();
        virtual ~MatlabIO();
        //static bool ReadData( vector <float> &data, const char* filename, size_t length );
        //static bool WriteData( vector <float> data, const char* filename, size_t length );
        //no one seems to be using vector<float>; uncomment if necessary
        static bool ReadData( float* data, const char* filename, size_t length );
        static bool WriteData(const  float* data, const char* filename, size_t length );
        static bool ReadData( unsigned char* data, const char* filename, size_t length );
        static bool WriteData( unsigned char* data, const char* filename, size_t length );
        static bool ReadData( size_t* data, const char* filename, size_t length );
        static bool WriteData( size_t* data, const char* filename, size_t length );
        static bool ReadData(size_t datasize, const char* filename, unsigned char* ldata);

};

#endif /* MATLABIO_H_ */
