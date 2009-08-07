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

IplImage* img1;
IplImage* img2 ;
IplImage* img3 ;
CvMat* MotionModel_temp_temp;
CvMat* MotionModel;
CvMat* MotionModel_tpose;
CvMat* points1;
CvMat* lines;

char* fil_name1;


MotionType MotionType;



int main(int argc, char *argv[])
{

    CvMat* T = cvCreateMat(4, 1, CV_64F);
CvMat* R = cvCreateMat(3,3, CV_64F);
CvMat* K = cvCreateMat(3,3, CV_64F);
CvMat* P1 = cvCreateMat(3,4, CV_64F);
CvMat* F = cvCreateMat(3,3, CV_64F);

double width,height;

height=width=256.0*2.0;


int i,j,k;

    if (argc<2)
    {
        printf("Usage: main <projection matrix> [fundamental matrix] [width] [height]\n\7");
        exit(0);
    }




    fil_name1= argv[1] ;
    readCvMatFfromfile(&P1,fil_name1);


    if (argc>2)
    {

    fil_name1= argv[2] ;
    readCvMatFfromfile(&F,fil_name1);

    printf("Fundamental matrix read was\n");
    writeCVMatrix(cout,F );
    cout<<endl;

    }

if (argc>3)
{

width=  atof(argv[3] );
height=  atof(argv[4] );

}

printf("projection matrix read was\n");
writeCVMatrix(cout,P1 );
cout<<endl;


cvDecomposeProjectionMatrixHR(P1, K, R,T, 0, 0, 0, 0);



printf("rotation matrix found was:\n");
writeCVMatrix(cout,R );
cout<<endl;



printf("translation matrix found was:\n");
writeCVMatrix(cout,T );
cout<<endl;




printf("intrinsics matrix found was:\n");
writeCVMatrix(cout,K );
cout<<endl;


double foc;
estimateFocalLengthStrum(F,width,height,foc);

cvReleaseMat(&T);
cvReleaseMat(&R);
cvReleaseMat(&K);
cvReleaseMat(&P1);
cvReleaseMat(&F);
    return 0;
}




//     writeMatrixRaw(translationMatrix2,3,1,"trans_synth_data.txt");
//     writeMatrixRaw(rotationMatrix2,3,3,"rotation_synth_data.txt");
//     writeMatrixRaw(projectionCamPrev,3,4,"proj1_synth_data.txt");
//     writeMatrixRaw(projectionCamCur,3,4,"proj2_synth_data.txt");
//     writeMatrixRaw(intrinsicMatrix,3,3,"intrinsic_synth_data.txt");

