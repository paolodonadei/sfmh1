#ifndef FOCALLENGTH_HPP_INCLUDED
#define FOCALLENGTH_HPP_INCLUDED
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

#include <vector>
#include <iostream>
#include <fstream>
#include <iomanip>
#include <sstream>
//#include "nonlinSClvm.h"

enum SELFCALIBMETHOD {STRUM, POLLEFEY, HARTLEY,POLLEFEYVISUAL,NONLINSIMPLE,NONLINSIMPLEMULTISTEP,MESTIMATOR};


using namespace std;
int denormalizeK(CvMat*  K);
int enforceRank3forQ(CvMat* pQ);
int extractKfromQ(const CvMat* pQ,const CvMat* pPnormalized,CvMat* pK);
int normalizeProjectionMatrix(const CvMat* in, CvMat* inNormalized,int width, int height);
int absoluteQuadricfromAVisual(const CvMat* pA, CvMat* Q,CvMat* P2R3);
int formAfromP1P2PollefeyVisual(const CvMat* pP1,const CvMat* pP2, CvMat* pA);
int absoluteQuadricfromAY(const CvMat* pA, CvMat* pY,CvMat* Q);
int formAYfromP1P2Pollefey(const CvMat* pP1,const CvMat* pP2, CvMat* pA, CvMat* pY);
int HRSelfCalibtwoFrame(const CvMat* pF,int width1, int height1, int width2, int height2,double& f1,double& f2,SELFCALIBMETHOD method);
int HRSelfCalibtwoFrame(const CvMat* pF,int width1, int height1, int width2, int height2,CvMat* K1,CvMat* K2,SELFCALIBMETHOD method);
void errnonLinFunctionSelfCalib(double *p, double *hx, int m, int n, void *adata);
int HRSelfCalibtwoFrame(vector< vector<CvMat*> >const &PFV,  vector<CvMat*>  &KV ,int width, int height, vector<double>& confs,SELFCALIBMETHOD method);
int extractKfromQLAPACK(const CvMat* pQ,const CvMat* pPnormalized,CvMat* pK);
int  estimateFocalLengthsPollefey(const CvMat* pF, int width1, int height1,int width2, int height2,double& foc1,double& foc2);
int  estimateFocalLengthsPollefeyVisual(const CvMat* pF, int width1, int height1,int width2, int height2,CvMat* K1,CvMat* K2);
int estimateFocalLengthsHartley(const CvMat* pF, int width1, int height1,int width2, int height2,double& foc1,double& foc2);
int  estimateFocalLengthStrum(const CvMat* pF,int width, int height, double& foc);
int solveFfromUVWHartleyMAPLE(double& F1, double& F2, const CvMat* pU,const CvMat* pV,const CvMat* pW);
int createPseudoFundMatrix(const CvMat* pF,CvMat* pG,int width, int height);

int solveFfromUVW(double& F1, double& F2, const CvMat* pU,const CvMat* pV,const CvMat* pW);
int  solveFfromUVWLQMAPLE(double& F1, double& F2, const CvMat* pU,const CvMat* pV,const CvMat* pW);

int solveFfromUVWHoumanMAPLE(double& F1, double& F2, const CvMat* pU,const CvMat* pV,const CvMat* pW);
int  solveFfromUVWL1(double& F1, double& F2, const CvMat* pU,const CvMat* pV,const CvMat* pW);
int  solveFfromUVWL2(double& F1, double& F2, const CvMat* pU,const CvMat* pV,const CvMat* pW);
int  solveFfromUVWLQ(double& F1, double& F2, const CvMat* pU,const CvMat* pV,const CvMat* pW);
#endif
