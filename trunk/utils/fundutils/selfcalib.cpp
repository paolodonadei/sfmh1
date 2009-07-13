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

    while (!file_cm.eof() && i<12) //second check is redundant, being safe
    {
        file_cm.getline(str,2000);

        string s(str);
        string out;
        istringstream ss;
        ss.str(s);

        ss>>out;

        (*tmodel)->data.fl[i++]=from_string<float>(out, std::dec);

        ss>>out;
        (*tmodel)->data.fl[i++]=from_string<float>(out, std::dec);

        ss>>out;
        (*tmodel)->data.fl[i++]=from_string<float>(out, std::dec);


        ss>>out;
        (*tmodel)->data.fl[i++]=from_string<float>(out, std::dec);



    }

}

int main(int argc, char *argv[])
{
    CvMat TT = cvMat(4, 1, CV_64F);
    CvMat R = cvMat(3, 3, CV_64F);
    CvMat K = cvMat(3, 3, CV_64F);
  

    int i,j,k;

    if (argc<2)
    {
        printf("Usage: main <projection matrix>\n\7");
        exit(0);
    }

 


 fil_name1= argv[1] ;


for(i = 0; i < 4; i++)
 cvmSet(&TT, i, 0, 1); // Solution is last row of V.
   
MotionGeometry mymotion;
readFfromfile(&mymotion.MotionModel_P1,fil_name1);


printf("projection matrix read was\n");
    for (i=0;i<12;i++)
        cout<<" \t"<<mymotion.MotionModel_P1->data.fl[i];
    cout<<endl;

printf("finished reading\n");

    cvDecomposeProjectionMatrix(mymotion.MotionModel_P1, &K, &R,&TT, 0, 0, 0, 0);
  


    return 0;
}




//     writeMatrixRaw(translationMatrix2,3,1,"trans_synth_data.txt");
//     writeMatrixRaw(rotationMatrix2,3,3,"rotation_synth_data.txt");
//     writeMatrixRaw(projectionCamPrev,3,4,"proj1_synth_data.txt");
//     writeMatrixRaw(projectionCamCur,3,4,"proj2_synth_data.txt");
//     writeMatrixRaw(intrinsicMatrix,3,3,"intrinsic_synth_data.txt");
