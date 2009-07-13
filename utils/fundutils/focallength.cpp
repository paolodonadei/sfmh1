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
#define typicalF 7000

using namespace std;


int  estimateFocalLengthStrum(const CvMat* pF,int width, int height, double& foc1, double& foc2)
{

    foc1=foc2=0;
    CvMat* G = cvCreateMat(3,3, CV_64F);
    CvMat* U = cvCreateMat(3,3, CV_64F);
    CvMat* V = cvCreateMat(3,3, CV_64F);
    CvMat* W = cvCreateMat(3,3, CV_64F);


    if(pF==NULL)
    {
     printf("****matrix is null\n");
        return -1;

    }

    if ( pF->rows!=3 || pF->cols!=3 )
    {
        cout<<"fundamental matrix is the wrong size dudes"<<endl;
        return -2;
    }

    int status=0;

     status = createPseudoFundMatrix(pF,G,width, height);

     cvSVD( G, W,  U, V );
     status= solveFfromUVW(foc1, foc2,U,V,W);



     //undoing the efffects of the multiplication by the typical f
     foc1=*typicalF;
     foc2=*typicalF;


cvReleaseMat(&G);
cvReleaseMat(&U);
cvReleaseMat(&V);
cvReleaseMat(&W);

    return 0;
}

int createPseudoFundMatrix(const CvMat* pF,CvMat* pG,int width, int height)
{
    CvMat* Gtemp1 = cvCreateMat(3,3, CV_64F);
    CvMat* Gtemp2 = cvCreateMat(3,3, CV_64F);
    CvMat* standMatrix = cvCreateMat(3,3, CV_64F);
    CvMat* leftMatr = cvCreateMat(3,3, CV_64F);
    CvMat* rightMatr = cvCreateMat(3,3, CV_64F);


    
    double ux, vy, skew;

//determinig skew and camera center (this should be known, but here we simply them)
    ux=width;
    ux/=((double)2.0);
    vy=height;
    vy/=((double)2.0);
    skew=((double)1.0);

    cvSetZero(leftMatr); 
    cvSetZero(rightMatr); 
    cvSetZero(Gtemp1); 
    cvSetZero(Gtemp2); 
    cvSetZero(standMatrix);

//left one
   cvmSet(leftMatr, 0, 0,skew );
   cvmSet(leftMatr, 1,1,1.0 );
   cvmSet(leftMatr, 2, 2, 1.0);
   cvmSet(leftMatr, 2, 0,ux );
   cvmSet(leftMatr, 2, 1,vy );


//right one
   cvmSet(rightMatr, 0, 0,skew );
   cvmSet(rightMatr, 1,1,1.0 );
   cvmSet(rightMatr, 2, 2, 1.0);
   cvmSet(rightMatr, 0, 2,ux );
   cvmSet(rightMatr, 1, 2,vy );


cvMatMul(leftMatr,pF , Gtemp1);   // Ma*Mb   -> Mc
cvMatMul(Gtemp1,rightMatr , Gtemp2);   // Ma*Mb   -> Mc

//now multiply by some random F value

cvSetIdentity(rightMatr);

   cvmSet(rightMatr, 0, 0,typicalF );
   cvmSet(rightMatr, 1, 1,typicalF);
   



cvMatMul(Gtemp2,rightMatr , Gtemp1);   // Ma*Mb   -> Mc
cvMatMul(rightMatr,Gtemp1 , Gtemp2);   // Ma*Mb   -> Mc



//normalizing the forbenius norm
double norm = cvNorm( Gtemp2, 0, CV_L2 );
norm = ((double)1.0)/norm;
cvConvertScale( Gtemp2,Gtemp1,norm );



    cvReleaseMat(&leftMatr);
    cvReleaseMat(&rightMatr);
    cvReleaseMat(&Gtemp1);
    cvReleaseMat(&Gtemp2);
    cvReleaseMat(&standMatrix);
    return 0;
}

int solveFfromUVW(double& F1, double& F2, const CvMat* pU,const CvMat* pV,const CvMat* pW)
{





}
