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

#include <boost/shared_ptr.hpp>

typedef boost::shared_ptr<HRImage> HRImagePtr;
i left here, immplement the matrix stuff , invert matrices, imeplment method for self calibration
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
        printf("Usage: main [<fundamental matrix>|<index fundmatrices>] [width] [height] [mode]\n");
        printf("<fundamental matrix> name of the file containing the fundamental matrix OR\n");
        printf("<index fundmatrices> an index pointing to the set of fundamental matrices, see [mode] option\n");
        printf("[width] [height] size of the images\n");
        printf("[mode] 1 would indicate first option is a single fundamental matrix, 2 means its an index\n");
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
    cvSetZero(K1);
    cvSetZero(K2);
    HRSelfCalibtwoFrame(F,width,height, width,height,K1, K2,HARTLEY);
    cout<<" According to Hartley F1= "<<  cvmGet( K1,0,0 )  << " and F2=  "<<  cvmGet( K2,0,0 )  <<endl;

    cvSetZero(K1);
    cvSetZero(K2);
    HRSelfCalibtwoFrame(F,width,height, width,height,K1, K2,STRUM);
    cout<<" According to STRUM F1= "<<  cvmGet( K1,0,0 )  << " and F2=  "<<  cvmGet( K2,0,0 )  <<endl;

    cvSetZero(K1);
    cvSetZero(K2);
    HRSelfCalibtwoFrame(F,width,height, width,height,K1, K2,POLLEFEYVISUAL);
    cout<<" According to POLLEFEYVISUAL F1= "<<  cvmGet( K1,0,0 )  << " and F2=  "<<  cvmGet( K2,0,0 )  <<endl;

    cvReleaseMat(&F);
    cvReleaseMat(&K1);
    cvReleaseMat(&K2);
    return 0;
}


