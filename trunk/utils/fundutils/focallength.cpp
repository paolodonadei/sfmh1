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
#include "general.h"
#include "HRprimitives.h"
#include "projdecompose.h"
#include "focallength.h"
using namespace std;


int  estimateFocalLengthStrum(const CvMat* pF, double& foc1, double& foc2)
{

    foc1=foc2=0;
    CvMat* G = cvCreateMat(3,3, CV_64F);
    CvMat* U = cvCreateMat(3,3, CV_64F);
    CvMat* V = cvCreateMat(3,3, CV_64F);
    CvMat* W = cvCreateMat(3,3, CV_64F);


    if(pF==NULL)
    {
     printf("****matrix is null\n");
        return;

    }

    if ( pF->rows!=3 || pF->cols!=3 )
    {
        cout<<"fundamental matrix is the wrong size dudes"<<endl;
        return;
    }

    int status=0;

     status = createPseudoFundMatrix(F,G);

     cvSVD( G, W,  U, V, int flags=0 );
     status= solveFfromUVW(foc1, foc2,U,V,W)

cvReleaseMat(&G);
cvReleaseMat(&U);
cvReleaseMat(&V);
cvReleaseMat(&W);

    return 0;
}

int createPseudoFundMatrix(const CvMat* pF,CvMat* pG)
{



    return 0;
}

int solveFfromUVW(double& F1, double& F2, const CvMat* pU,const CvMat* pV,const CvMat* pW)
{





}
