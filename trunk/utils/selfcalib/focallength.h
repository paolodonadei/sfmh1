
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


enum SELFCALIBMETHOD{STRUM, POLLEFEY, HARTLEY};


using namespace std;
int HRSelfCalibtwoFrame(const CvMat* pF,int width1, int height1, int width2, int height2,double& f1,double& f2,SELFCALIBMETHOD method);
int HRSelfCalibtwoFrame(const CvMat* pF,int width1, int height1, int width2, int height2,CvMat* K1,CvMat* K2,SELFCALIBMETHOD method);
int  estimateFocalLengthsPollefey(const CvMat* pF, int width1, int height1,int width2, int height2,double& foc1,double& foc2);
int  estimateFocalLengthStrum(const CvMat* pF,int width, int height, double& foc);

int createPseudoFundMatrix(const CvMat* pF,CvMat* pG,int width, int height);

int solveFfromUVW(double& F1, double& F2, const CvMat* pU,const CvMat* pV,const CvMat* pW);



int  solveFfromUVWL1(double& F1, double& F2, const CvMat* pU,const CvMat* pV,const CvMat* pW);
   int  solveFfromUVWL2(double& F1, double& F2, const CvMat* pU,const CvMat* pV,const CvMat* pW);
   int  solveFfromUVWLQ(double& F1, double& F2, const CvMat* pU,const CvMat* pV,const CvMat* pW);
