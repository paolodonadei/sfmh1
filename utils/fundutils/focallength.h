
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

int  estimateFocalLengthStrum(const CvMat* pF,int width, int height, double& foc1, double& foc2);

int createPseudoFundMatrix(const CvMat* pF,CvMat* pG,int width, int height);

int solveFfromUVW(double& F1, double& F2, const CvMat* pU,const CvMat* pV,const CvMat* pW);
