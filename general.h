#ifndef GENERAL_HPP_INCLUDED
#define GENERAL_HPP_INCLUDED


#include <iostream>
#include <sstream>
#include <string>
#include <vector>
#include <typeinfo>
#include <stdexcept>
#include <cv.h>

#include "sift.h"
#include "HRImage.hpp"


//some general definitions go here
#define INDEXFNAME "motionindex.txt"


using namespace std;


class HRImage;

Image HRImagetoDLImage(HRImage& hr_im);

class BadConversion : public std::runtime_error
{
public:
    BadConversion(const std::string& s)
            : std::runtime_error(s)
    { }
};
bool FileExists(string strFilename);

template <class T> T from_string(const std::string& s,std::ios_base& (*f)(std::ios_base&))
{
    T t;



    std::istringstream iss(s);
    if ((iss >> f >> t).fail())
        return -1;
    else
        return t;
}

template<typename T>
inline std::string stringify(const T& x)
{
    std::ostringstream o;
    if (!(o << x))
        throw BadConversion(std::string("stringify(")
                            + typeid(x).name() + ")");
    return o.str();
}

IplImage* concatImagesVertical(IplImage* im1,IplImage* im2);
void  draw_cross(CvPoint2D32f center, CvScalar color, int d,IplImage* img );
bool checkTempPath();
string combineFnames(const string& f1,const string& f2,string extension);
int printLine(const HRImage& im1,const HRImage& im2, CvPoint p1, CvPoint p2, int indexname);
void writeCVMatrix(ostream &stream,const CvMat* M);
string findSeedDirName(const vector<string>& oArray);
void readCvMatFfromfile(CvMat** tmodel,const string& mfname);
void writeCVMatrix(char* fname,const CvMat* M);
int cvMatrixtoBuffer(CvMat* mat,double** buffer, int allocate=0);
int BuffertocvMatrix(double* buffer,CvMat** mat,int allocate=0,int rows=0, int cols=0);
int indexMax(vector<double>& myvec);

#endif //HRPRIMITIVES_HPP_INCLUDED
