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




void readFfromfile(CvMat** tmodel,const string& mfname)
{
//this function is not very error resilient, it will crash and burn if the data file is not the right format
    string fname(mfname);


    fstream file_cm(fname.c_str(),ios::in);
    if (!file_cm.is_open())
    {
        cout << "File +" <<  fname << "+ does not exist" << endl;
        exit(1);
    }

    char str[2000];

    int i=0;
    int j=0;


    while (!file_cm.eof() && j<3) //second check is redundant, being safe
    {
        file_cm.getline(str,2000);

        string s(str);
        string out;
        istringstream ss;
        ss.str(s);

        ss>>out;
	cvmSet((*tmodel),j,i++, from_string<float>(out, std::dec));

        ss>>out;
	cvmSet((*tmodel),j,i++, from_string<float>(out, std::dec));

        ss>>out;
	cvmSet((*tmodel),j,i++, from_string<float>(out, std::dec));


        ss>>out;
	cvmSet((*tmodel),j,i++, from_string<float>(out, std::dec));

	j++;
	i=0;

    }

}

int main(int argc, char *argv[])
{
  
    CvMat* T = cvCreateMat(4, 1, CV_64F);
CvMat* R = cvCreateMat(3,3, CV_64F);
CvMat* K = cvCreateMat(3,3, CV_64F);


    int i,j,k;

    if (argc<2)
    {
        printf("Usage: main <projection matrix>\n\7");
        exit(0);
    }

 


 fil_name1= argv[1] ;

   
MotionGeometry mymotion;
readFfromfile(&mymotion.MotionModel_P1,fil_name1);


printf("projection matrix read was\n");
writeCVMatrix(cout,mymotion.MotionModel_P1 );
cout<<endl;


cvDecomposeProjectionMatrix(mymotion.MotionModel_P1, K, R,T, 0, 0, 0, 0);
  


printf("rotation matrix found was:\n");
writeCVMatrix(cout,R );
cout<<endl;



printf("translation matrix found was:\n");
writeCVMatrix(cout,T );
cout<<endl;




printf("intrinsics matrix found was:\n");
writeCVMatrix(cout,K );
cout<<endl;

cvReleaseMat(&T);
cvReleaseMat(&R);
cvReleaseMat(&K);
    return 0;
}




//     writeMatrixRaw(translationMatrix2,3,1,"trans_synth_data.txt");
//     writeMatrixRaw(rotationMatrix2,3,3,"rotation_synth_data.txt");
//     writeMatrixRaw(projectionCamPrev,3,4,"proj1_synth_data.txt");
//     writeMatrixRaw(projectionCamCur,3,4,"proj2_synth_data.txt");
//     writeMatrixRaw(intrinsicMatrix,3,3,"intrinsic_synth_data.txt");
