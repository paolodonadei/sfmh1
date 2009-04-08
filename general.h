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

#define TEMPDIR "tempdir"
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

template<typename T>  // See section on templates for more
 class Matrix {
 public:
   Matrix(unsigned nrows, unsigned ncols);
   // Throws a BadSize object if either size is zero
   class BadSize { };

   // No need for any of The Big Three!

   // Access methods to get the (i,j) element:
   T&       operator() (unsigned i, unsigned j);
   const T& operator() (unsigned i, unsigned j) const;
   // These throw a BoundsViolation object if i or j is too big
   class BoundsViolation { };

   unsigned nrows() const;  // #rows in this matrix
   unsigned ncols() const;  // #columns in this matrix

 private:
   std::vector<std::vector<T> > data_;
 };

 template<typename T>
 inline unsigned Matrix<T>::nrows() const
 { return data_.size(); }

 template<typename T>
 inline unsigned Matrix<T>::ncols() const
 { return data_[0].size(); }

 template<typename T>
 inline T& Matrix<T>::operator() (unsigned row, unsigned col)
 {
   if (row >= nrows() || col >= ncols()) throw BoundsViolation();
   return data_[row][col];
 }

 template<typename T>
 inline const T& Matrix<T>::operator() (unsigned row, unsigned col) const
 {
   if (row >= nrows() || col >= ncols()) throw BoundsViolation();
   return data_[row][col];
 }

 template<typename T>
 Matrix<T>::Matrix(unsigned nrows, unsigned ncols)
   : data_ (nrows)
 {
   if (nrows == 0 || ncols == 0)
     throw BadSize();
   for (unsigned i = 0; i < nrows; ++i)
     data_[i].resize(ncols);
 }


IplImage* concatImagesVertical(IplImage* im1,IplImage* im2);
void  draw_cross(CvPoint2D32f center, CvScalar color, int d,IplImage* img );
bool checkTempPath();
string combineFnames(const string& f1,const string& f2,string extension);
int printLine(const HRImage& im1,const HRImage& im2, CvPoint p1, CvPoint p2, int indexname);

#endif //HRPRIMITIVES_HPP_INCLUDED
