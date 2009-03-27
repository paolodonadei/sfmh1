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

Image HRImagetoDLImage(HRImage& hr_im);

class BadConversion : public std::runtime_error
{
public:
    BadConversion(const std::string& s)
            : std::runtime_error(s)
    { }
};

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
#endif //HRPRIMITIVES_HPP_INCLUDED
