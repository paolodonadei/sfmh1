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

int HRFindFfromP( const CvMat *projMatr1,const CvMat *projMatr2,CvMat *C1,CvMat *C2,CvMat F);
int HRFindEfromP( const CvMat *projMatr1,const CvMat *projMatr2,const CvMat *K1,const CvMat *K2,CvMat *E);
int HRFindRTFromE(const CvMat *E,CvMat *R,CvMat *T);




MotionType MotionType;



int main(int argc, char *argv[])
{
char* fil_name1;
char* fil_name2;
CvMat* T = cvCreateMat(4, 1, CV_64F);
CvMat* R = cvCreateMat(3,3, CV_64F);
CvMat* T1 = cvCreateMat(4, 1, CV_64F);
CvMat* R1 = cvCreateMat(3,3, CV_64F);
CvMat* T2 = cvCreateMat(4, 1, CV_64F);
CvMat* R2 = cvCreateMat(3,3, CV_64F);
CvMat* K1 = cvCreateMat(3,3, CV_64F);
CvMat* K2 = cvCreateMat(3,3, CV_64F);
CvMat* P1 = cvCreateMat(3,4, CV_64F);
CvMat* P2 = cvCreateMat(3,4, CV_64F);
CvMat* F1 = cvCreateMat(3,3, CV_64F);
CvMat* F2 = cvCreateMat(3,3, CV_64F);
CvMat* E1 = cvCreateMat(3,3, CV_64F);
CvMat* E2 = cvCreateMat(3,3, CV_64F);
CvMat* W1 = cvCreateMat(3,3, CV_64F);
CvMat* W2 = cvCreateMat(3,3, CV_64F);



int i,j,k;
int numP=0;
    if (argc<2)
    {
        printf("Usage: main <projection matrix1> [projection matrix2]\n\7");
        exit(0);
    }

if (argc>2)
numP=2;
else
numP=1;


    fil_name1= argv[1] ;
    readCvMatFfromfile(&P1,fil_name1);
	printf("P1 read was\n");
    writeCVMatrix(cout,P1 );
    cout<<endl;

    if (numP==2)
    {

    fil_name2= argv[2] ;
    readCvMatFfromfile(&P2,fil_name2);
    printf("P2 read was\n");
    writeCVMatrix(cout,P2 );
    cout<<endl;

    }


cvDecomposeProjectionMatrixHR(P1, K1, R1,T1, 0, 0, 0, 0);

printf("rotation matrix found for P1 was:\n");
writeCVMatrix(cout,R1 );
cout<<endl;

printf("translation matrix found for P1 was:\n");
writeCVMatrix(cout,T1);
cout<<endl;

printf("intrinsics matrix found for P1 was:\n");
writeCVMatrix(cout,K1 );
cout<<endl;

if (numP==2)
{
cvDecomposeProjectionMatrixHR(P2, K2, R2,T3, 0, 0, 0, 0);

printf("rotation matrix found for P2 was:\n");
writeCVMatrix(cout,R2 );
cout<<endl;

printf("translation matrix found for P2 was:\n");
writeCVMatrix(cout,T2);
cout<<endl;

printf("intrinsics matrix found for P2 was:\n");
writeCVMatrix(cout,K2 );
cout<<endl;
}

HRFindFfromP(P1,P2,C1,C2,F1);
HRFindEfromP(P1,P2,K1,K2,E1);
HRFindRTFromE(R,T,E1);



cvReleaseMat(&T);
cvReleaseMat(&R);
cvReleaseMat(&T1);
cvReleaseMat(&R1);
cvReleaseMat(&T2);
cvReleaseMat(&R2);
cvReleaseMat(&K1);
cvReleaseMat(&K2);
cvReleaseMat(&P1);
cvReleaseMat(&P2);
cvReleaseMat(&F1);
cvReleaseMat(&F2);
cvReleaseMat(&E1);
cvReleaseMat(&E2);
cvReleaseMat(&W1);
cvReleaseMat(&W2);
    return 0;
}


int HRFindFfromP( const CvMat *projMatr1,const CvMat *projMatr2,CvMat *C1,CvMat *C2,CvMat F);
{



}

int HRFindEfromP( const CvMat *projMatr1,const CvMat *projMatr2,const CvMat *K1,const CvMat *K2,CvMat *E);
int HRFindRTFromE(const CvMat *E,CvMat *R,CvMat *T);
