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

    while (!file_cm.eof() && i<9) //second check is redundant, being safe
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




    }

}

int main(int argc, char *argv[])
{

  

    int i,j,k;

    if (argc<2)
    {
        printf("Usage: main <projection matrix>\n\7");
        exit(0);
    }

 


 fil_name1= argv[1] ;


    
   
MotionGeometry mymotion;
readFfromfile(&mymotion.MotionModel_F,fil_name1);

    for (i=0;i<9;i++)
        cout<<" \t"<<mymotion.MotionModel_F->data.fl[i];
    cout<<endl;


    return 0;
}


