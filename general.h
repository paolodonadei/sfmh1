#ifndef GENERAL_HPP_INCLUDED
#define GENERAL_HPP_INCLUDED


#include <iostream>
#include <sstream>
#include <string>
#include <typeinfo>
#include <stdexcept>
#include <cv.h>
#include <highgui.h>
#include "sift.h"
#include "HRImage.hpp"


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
#endif //HRPRIMITIVES_HPP_INCLUDED
