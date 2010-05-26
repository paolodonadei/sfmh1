#ifndef FUNDUTIL_HPP_INCLUDED
#define FUNDUTIL_HPP_INCLUDED
////////////////////////////////////////////////////////////////////////
//
// hello-world.cpp
//
// This is a simple, introductory OpenCV program. The program reads an
// image from a file, inverts it, and displays the result.
//
////////////////////////////////////////////////////////////////////////
#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include <cv.h>
#include <highgui.h>
#include <vector>
#include <iostream>
#include <fstream>
#include <iomanip>
#include <sstream>

using namespace std;


enum MotionT {FUNDAMENTAL, HOMOGRAPHY};




bool inBound(CvPoint2D32f inter, double width, double height);

double givenXfindY(double a, double b, double c, double X);


double givenYfindX(double a, double b, double c, double Y);


void  draw_cross(CvPoint2D32f center, CvScalar color, int d,IplImage* img );


void imgclick(int event, int x, int y, int flags, void* param);




void readFfromfile(CvMat** tmodel,const string& mfname);


#endif
