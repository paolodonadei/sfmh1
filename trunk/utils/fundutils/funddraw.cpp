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
#include "funddrawutils.h"
using namespace std;



IplImage* img1;
IplImage* img2 ;
IplImage* img3 ;
CvMat* MotionModel_temp_temp;
CvMat* MotionModel;
CvMat* MotionModel_tpose;



char* fil_name1;
char* fil_name2;

MotionT MotionType;

int main(int argc, char *argv[])
{



    img1 = 0;
    img2 = 0;
    IplImage* imgtemp=NULL ;

    IplImage* img1clone=NULL ;
    IplImage* img2clone=NULL ;

    MotionModel_tpose=cvCreateMat(3,3,CV_32FC1);
    MotionModel=cvCreateMat(3,3,CV_32FC1);


    int height,width,step,channels;
    uchar *data;
    int i,j,k;

    if (argc<4)
    {
        printf("Usage: main <image-file-name1> <image-file-name2> <fundamental matrix or homography> <motion type,1= F or 2=H>\n\7");
        exit(0);
    }

    // load an image
    img1=cvLoadImage(argv[2]);
    img2=cvLoadImage(argv[1]);


    fil_name1= argv[2] ;
    fil_name2= argv[1] ;


    if (!img1 || !img2)
    {
        printf("Could not load images file: %s\n",argv[1]);
        exit(0);
    }

    imgtemp=cvCreateImage(cvSize(img1->width,img1->height),IPL_DEPTH_8U,3);
    cvConvertImage(img1, imgtemp, CV_GRAY2RGB);
    cvReleaseImage( &img1 );
    img1=imgtemp;

    imgtemp=cvCreateImage(cvSize(img2->width,img2->height),IPL_DEPTH_8U,3);
    cvConvertImage(img2, imgtemp, CV_GRAY2RGB);
    cvReleaseImage( &img2 );
    img2=imgtemp;


    img1clone=cvCloneImage(img1);
    img2clone=cvCloneImage(img2);

    readFfromfile(&MotionModel,string(argv[3]));



    if(atoi(argv[4])==1)
    {
        MotionType=DFUNDAMENTAL;
        cvTranspose( MotionModel, MotionModel_tpose );
    }
    else
    {
        MotionType=DHOMOGRAPHY;
        cvInvert( MotionModel, MotionModel_tpose );
    }


    printf("the motion matrix %s is:\n",(atoi(argv[4])==1)?"Fundamental":"homography" );

    for (i=0; i<9; i++)
        cout<<" \t"<<MotionModel->data.fl[i];
    cout<<endl;

    // create a window
    cvNamedWindow(fil_name1, CV_WINDOW_AUTOSIZE);
    cvMoveWindow(fil_name1, 100, 100);

    cvNamedWindow( fil_name2, CV_WINDOW_AUTOSIZE);
    cvMoveWindow( fil_name2, 600, 100);

    // show the image
    cvShowImage(fil_name1, img1 );
    cvShowImage( fil_name2, img2 );

    int forward=1;
    int backward=2;

    struct voidparam
    {
        IplImage* im1;
        IplImage* im2;
        int mydir;
        char* pfil_name1;
        char* pfil_name2;
        CvMat* pMotionModel;
        CvMat* pMotionModel_tpose;
        MotionT pMotionType;
    };
    voidparam myparms_f;
    voidparam myparms_b;

    myparms_f.im1=img1;
    myparms_f.im2=img2;
    myparms_f.mydir=forward;
    myparms_f.pfil_name1=fil_name1;
    myparms_f.pfil_name2=fil_name2;
    myparms_f.pMotionModel=MotionModel;
    myparms_f.pMotionModel_tpose=MotionModel_tpose;
    myparms_f.pMotionType=MotionType;

    myparms_b=myparms_f;
    myparms_b.mydir=backward;

    cvSetMouseCallback(fil_name1,imgclick,&myparms_f);
    cvSetMouseCallback(fil_name2,imgclick,&myparms_b);
    // wait for a key
    int key;
    while (1)
    {
        key=cvWaitKey(5);

        if (key==27) break;
        if (key==33) break;

        switch (key)
        {
        case 't':
            img3= img1;
            img1=img2  ;
            img2 =img3;
            cvShowImage(fil_name1, img1 );
            cvShowImage(fil_name2, img2 );
            break;
        case 'c':

            cvReleaseImage( &img1 );
            cvReleaseImage( &img2 );
            img1=cvCloneImage(img1clone);
            img2=cvCloneImage(img2clone);
            cvShowImage(fil_name1, img1 );
            cvShowImage(fil_name2, img2 );
            break;
        }
    }

    // release the image
    cvReleaseImage(&img1 );
    cvReleaseImage(&img2 );
    if ( MotionModel!=NULL)
    {
        cvReleaseMat(&MotionModel);
    }




    return 0;
}


