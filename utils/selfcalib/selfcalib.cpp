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
#include "visiongen.h"
#include "focallength.h"
using namespace std;



char* fil_name1;


MotionType MotionType;



int main(int argc, char *argv[])
{

    CvMat* K1 = cvCreateMat(3,3, CV_64F);
    CvMat* K2 = cvCreateMat(3,3, CV_64F);

    CvMat* F = cvCreateMat(3,3, CV_64F);

    double width,height;

    height=width=256.0*2.0;


    int i,j,k;

    if (argc<2)
    {
        printf("Usage: main <fundamental matrix> [width] [height]\n\7");
        exit(0);
    }





    fil_name1= argv[1] ;
    readCvMatFfromfile(&F,fil_name1);

    printf("Fundamental matrix read was\n");
    writeCVMatrix(cout,F );
    cout<<endl;

    if (argc>2)
    {

        width=  atof(argv[2] );
        height=  atof(argv[3] );

    }



    double foc;
    // estimateFocalLengthStrum(F,width,height,foc);

    HRSelfCalibtwoFrame(F,width,height, width,height,K1, K2,HARTLEY);

    cout<<" According to Hartley F1= "<<  cvmGet( K1,0,0 )  << " and F2=  "<<  cvmGet( K2,0,0 )  <<endl;

    HRSelfCalibtwoFrame(F,width,height, width,height,K1, K2,STRUM);
    cout<<" According to STRUM F1= "<<  cvmGet( K1,0,0 )  << " and F2=  "<<  cvmGet( K2,0,0 )  <<endl;

    cvReleaseMat(&F);
    cvReleaseMat(&K1);
    cvReleaseMat(&K2);
    return 0;
}


