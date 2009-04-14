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

using namespace std;

IplImage* img1;
IplImage* img2 ;

CvMat* MotionModel_temp_temp;
CvMat* MotionModel;
CvMat* MotionModel_tpose;
CvMat* points1;
CvMat* lines;

char* fil_name1;
char* fil_name2;

bool inBound(CvPoint2D32f inter, double width, double height)
{


    double x=inter.x ;
    double y=inter.y ;

    if (x>=0 && x<width && y>=0 && y<height)
        return true;
    else
        return false;

}

double givenXfindY(double a, double b, double c, double X)
{
    double y=(-c-(a*X));
	y/=b;
    return y;

}


double givenYfindX(double a, double b, double c, double Y)
{
    double x=(-c-(b*Y));
x/=a;
    return x;

}


void  draw_cross(CvPoint2D32f center, CvScalar color, int d,IplImage* img )
{
    if (img!=NULL)
    {
        cvLine( img, cvPoint( center.x - d, center.y - d ),
                cvPoint( center.x + d, center.y + d ), color, 1, 0 );
        cvLine( img, cvPoint( center.x + d, center.y - d ),
                cvPoint( center.x - d, center.y + d ), color, 1, 0 );
    }
}


void imgclick(int event, int x, int y, int flags, void* param)
{

    if (event== CV_EVENT_LBUTTONDOWN)
    {
        CvPoint2D32f location;
        location.x= x;
        location.y= y;
        int* direction=(int*)param;

        draw_cross(location, CV_RGB(255,0,0),4,(*direction==1)?img1:img2 );


     //   printf("clicked location %d , %d \n",x,y);
        points1->data.fl[0]=x;
        points1->data.fl[1]=y;

        cvComputeCorrespondEpilines( points1, (*direction)+1,MotionModel,lines);

        CvPoint2D32f intersects[4];
        int ptindex[3];
        CvPoint2D32f start;
        CvPoint2D32f end;

        intersects[0].x=0;
        intersects[0].y=givenXfindY(lines->data.fl[0],lines->data.fl[1], lines->data.fl[2], intersects[0].x);

        intersects[1].x=((*direction==1)?img2:img1)->width-1;
        intersects[1].y=givenXfindY(lines->data.fl[0],lines->data.fl[1], lines->data.fl[2], intersects[1].x);

        intersects[2].y=0;
        intersects[2].x=givenYfindX(lines->data.fl[0],lines->data.fl[1], lines->data.fl[2], intersects[2].y);


        intersects[3].y=((*direction==1)?img2:img1)->height-1;
        intersects[3].x=givenYfindX(lines->data.fl[0],lines->data.fl[1], lines->data.fl[2], intersects[3].y);

        int j=0;
        for (int i=0;i<4;i++)
        {
            if (inBound(intersects[i], ((*direction==1)?img2:img1)->width, ((*direction==1)?img2:img1)->height))
            {
                ptindex[j++]=i;
            }
            if (j==2) break;
        }

        if (j!=2) cout<<"not enough points clipped"<<endl;



        start=intersects[ptindex[0]];
        end=intersects[ptindex[1]];


       // printf(" for point x=%f and y=%f calculated line with parameters a=%f b=%f c=%f and the intersections ended up being start.x=%f start.y=%f and end.x=%f end.y=%f \n", points1->data.fl[0],points1->data.fl[1],lines->data.fl[0],lines->data.fl[1],lines->data.fl[2],start.x, start.y,end.x,end.y);




        cvLine( ((*direction==1)?img2:img1),cvPoint( start.x , start.y ), cvPoint( end.x , end.y  ), CV_RGB(0,255,0), 1, 0 );

        cvShowImage(fil_name1, img1 );
        cvShowImage(fil_name2, img2 );

    }


}


template <class T> T from_string(const std::string& s,std::ios_base& (*f)(std::ios_base&))
{
    T t;



    std::istringstream iss(s);
    if ((iss >> f >> t).fail())
        return -1;
    else
        return t;
}




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
    points1  = cvCreateMat(1,1,CV_32FC2);
    lines= cvCreateMat(1,1,CV_32FC3);
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
        printf("Usage: main <image-file-name1> <image-file-name2> <fundamental matrix>\n\7");
        exit(0);
    }

    // load an image
    img1=cvLoadImage(argv[2]);
    img2=cvLoadImage(argv[1]);


 fil_name1= argv[2] ;
 fil_name2= argv[1]  ;


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
    cvTranspose( MotionModel, MotionModel_tpose );

    printf("the fund matrix is:\n");

    for (i=0;i<9;i++)
        cout<<" \t"<<MotionModel->data.fl[i];
    cout<<endl;

    // create a window
    cvNamedWindow(fil_name1, CV_WINDOW_AUTOSIZE);
    cvMoveWindow(fil_name1, 100, 100);

    cvNamedWindow( fil_name2, CV_WINDOW_AUTOSIZE);
    cvMoveWindow( fil_name2, 200, 500);

    // show the image
    cvShowImage(fil_name1, img1 );
    cvShowImage( fil_name2, img2 );

    int forward=1;
    int backward=2;

    cvSetMouseCallback(fil_name1,imgclick,&forward);
    cvSetMouseCallback(fil_name2,imgclick,&backward);
    // wait for a key
    int key;
    while (1)
    {
        key=cvWaitKey(0);
        printf("typed %d \n",key);
        if (key==27) break;
        if (key==33) break;

        switch (key)
        {
        case 't':

            MotionModel_temp_temp=MotionModel;
            MotionModel=MotionModel_tpose;
            MotionModel_tpose=MotionModel_temp_temp;

            printf("the fund matrix is:\n");

            for (i=0;i<9;i++)
                cout<<" \t"<<MotionModel->data.fl[i];
            cout<<endl;


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

    cvReleaseMat(&points1);
    cvReleaseMat(&lines);

    return 0;
}


