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

template <class T> T from_string(const std::string& s,std::ios_base& (*f)(std::ios_base&))
{
    T t;



    std::istringstream iss(s);
    if ((iss >> f >> t).fail())
        return -1;
    else
        return t;
}



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
    CvMat* points1;
    points1  = cvCreateMat(1,1,CV_32FC2);
    CvMat* lines;

    lines= cvCreateMat(1,1,CV_32FC3);
    if (event== CV_EVENT_LBUTTONDOWN)
    {
        CvPoint2D32f location;
        location.x= x;
        location.y= y;



        int* direction=&((voidparam*)param)->mydir;
        IplImage* pimg1=((voidparam*)param)->im1;
        IplImage* pimg2=((voidparam*)param)->im2;
        CvMat* MotionModel=((voidparam*)param)->pMotionModel;
        CvMat* MotionModel_tpose=((voidparam*)param)->pMotionModel_tpose;
        char* fil_name1=((voidparam*)param)->pfil_name1;
        char* fil_name2=((voidparam*)param)->pfil_name2;
        MotionT MotionType=((voidparam*)param)->pMotionType;


        draw_cross(location, CV_RGB(255,0,0),4,(*direction==1)?pimg1:pimg2 );
        points1->data.fl[0]=x;
        points1->data.fl[1]=y;



        if(MotionType==DFUNDAMENTAL)
        {
            //   printf("clicked location %d , %d \n",x,y);


            cvComputeCorrespondEpilines( points1, (*direction)+1,MotionModel,lines);

            CvPoint2D32f intersects[4];
            int ptindex[3];
            CvPoint2D32f start;
            CvPoint2D32f end;

            intersects[0].x=0;
            intersects[0].y=givenXfindY(lines->data.fl[0],lines->data.fl[1], lines->data.fl[2], intersects[0].x);

            intersects[1].x=((*direction==1)?pimg2:pimg1)->width-1;
            intersects[1].y=givenXfindY(lines->data.fl[0],lines->data.fl[1], lines->data.fl[2], intersects[1].x);

            intersects[2].y=0;
            intersects[2].x=givenYfindX(lines->data.fl[0],lines->data.fl[1], lines->data.fl[2], intersects[2].y);


            intersects[3].y=((*direction==1)?pimg2:pimg1)->height-1;
            intersects[3].x=givenYfindX(lines->data.fl[0],lines->data.fl[1], lines->data.fl[2], intersects[3].y);

            int j=0;
            for (int i=0; i<4; i++)
            {
                if (inBound(intersects[i], ((*direction==1)?pimg2:pimg1)->width, ((*direction==1)?pimg2:pimg1)->height))
                {
                    ptindex[j++]=i;
                }
                if (j==2) break;
            }

            if (j!=2) cout<<"not enough points clipped"<<endl;



            start=intersects[ptindex[0]];
            end=intersects[ptindex[1]];


            // printf(" for point x=%f and y=%f calculated line with parameters a=%f b=%f c=%f and the intersections ended up being start.x=%f start.y=%f and end.x=%f end.y=%f \n", points1->data.fl[0],points1->data.fl[1],lines->data.fl[0],lines->data.fl[1],lines->data.fl[2],start.x, start.y,end.x,end.y);




            cvLine( ((*direction==1)?pimg2:pimg1),cvPoint( start.x , start.y ), cvPoint( end.x , end.y  ), CV_RGB(0,255,0), 1, 0 );
        }
        else if (MotionType==DHOMOGRAPHY)
        {
            const float* H = (*direction==1)?MotionModel_tpose->data.fl:MotionModel->data.fl;

            double ww = 1./(H[6]*location.x + H[7]*location.y + 1.);
            double dx = (H[0]*location.x + H[1]*location.y + H[2])*ww ;
            double dy = (H[3]*location.x + H[4]*location.y + H[5])*ww ;


            CvPoint2D32f location2;
            location2.x= dx ;
            location2.y= dy ;
            draw_cross(location2, CV_RGB(255,0,0),4,(*direction==1)?pimg2:pimg1 );





        }
        cvShowImage(fil_name1, pimg1 );
        cvShowImage(fil_name2, pimg2 );

    }
    cvReleaseMat(&points1);
    cvReleaseMat(&lines);
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




//this uses the ax+by+c=0 representation of a line to draw
void drawLineonIMG(IplImage* img,CvMat* line,CvScalar color )
{
CvPoint2D32f intersects[4];
    int ptindex[3];
    CvPoint2D32f start;
    CvPoint2D32f end;

    intersects[0].x=0;
    intersects[0].y=givenXfindY(line->data.fl[0],line->data.fl[1], line->data.fl[2], intersects[0].x);

    intersects[1].x=img->width-1;
    intersects[1].y=givenXfindY(line->data.fl[0],line->data.fl[1], line->data.fl[2], intersects[1].x);

    intersects[2].y=0;
    intersects[2].x=givenYfindX(line->data.fl[0],line->data.fl[1], line->data.fl[2], intersects[2].y);


    intersects[3].y=img->height-1;
    intersects[3].x=givenYfindX(line->data.fl[0],line->data.fl[1], line->data.fl[2], intersects[3].y);

    int j=0;
    for (int i=0; i<4; i++)
    {
        if (inBound(intersects[i], img->width,img->height))
        {
            ptindex[j++]=i;
        }
        if (j==2) break;
    }

    if (j!=2) cout<<"not enough points clipped"<<endl;



    start=intersects[ptindex[0]];
    end=intersects[ptindex[1]];


    // printf(" for point x=%f and y=%f calculated line with parameters a=%f b=%f c=%f and the intersections ended up being start.x=%f start.y=%f and end.x=%f end.y=%f \n", points1->data.fl[0],points1->data.fl[1],line->data.fl[0],line->data.fl[1],line->data.fl[2],start.x, start.y,end.x,end.y);




    cvLine(img,cvPoint( start.x , start.y ), cvPoint( end.x , end.y  ),  color, 1, 0 );



    return;
}
void drawLineonIMG(IplImage* img,CvMat* line)
{
 drawLineonIMG(img, line,CV_RGB(0,255,0) );
}


