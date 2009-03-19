#ifndef GENERAL_HPP_INCLUDED
#define GENERAL_HPP_INCLUDED


 #include <iostream>
 #include <sstream>
 #include <string>
 #include <typeinfo>
 #include <stdexcept>
#include <cv.h>
#include <highgui.h>
 class BadConversion : public std::runtime_error {
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

void  draw_cross(CvPoint2D32f center, CvScalar color, int d,IplImage* img );
#endif //HRPRIMITIVES_HPP_INCLUDED
