#include "general.h"
#include "boost/filesystem.hpp"   // includes all needed Boost.Filesystem declarations
#include "boost/filesystem/operations.hpp"
#include "boost/filesystem/path.hpp"

#define DEBUGLVL 0
#define TEMPDIR "tempdir"
namespace fs = boost::filesystem;
using namespace std;

void  draw_cross(CvPoint2D32f center, CvScalar color, int d,IplImage* img )
{
    cvLine( img, cvPoint( center.x - d, center.y - d ),
            cvPoint( center.x + d, center.y + d ), color, 1, 0 );
    cvLine( img, cvPoint( center.x + d, center.y - d ),
            cvPoint( center.x - d, center.y + d ), color, 1, 0 );
}


Image HRImagetoDLImage(HRImage& hr_im)
{
    Image image;
    image = CreateImage(hr_im.height, hr_im.width);
    for (int r = 0; r < hr_im.height; r++)
        for (int c = 0; c < hr_im.width; c++)
            image->pixels[r][c] = ((float) hr_im[r][c]) / 255.0;


    return image;

}

bool checkTempPath()
{
    fs::path temp_path = fs::system_complete( fs::path( TEMPDIR, fs::native ) );

    if ( !fs::exists( temp_path ) )
    {
        create_directory( temp_path );
        if ( !fs::exists( temp_path ) )
        {
            cout<<"cant create directory, exiting"<<endl;
            return false;
        }
    }
    return true;
}

string combineFnames(const string& f1,const string& f2,string extension)
{

    fs::path p1( f1, fs::native );
    fs::path p2( f2, fs::native );

    string fname=fs::basename(p1)+string("_")+fs::basename(p2)+extension;
    return fname;




}

//this takes two single channel images and produces a 3 channel output image
IplImage* concatImagesVertical(IplImage* im1,IplImage* im2)
{
    if (im1->width != im2->width || im1->height!=im2->height)
    {
        cout<<"images to be concatenated are not the same size"<<endl;
        return NULL;

    }


    IplImage* imgTemp=cvCreateImage(cvSize(im1->width,im1->height*2),IPL_DEPTH_8U,3);


    int i,j;


    int step_main       =  imgTemp->widthStep/sizeof(uchar);
    int channels_main   =  imgTemp->nChannels;
    uchar* tdata_main    = (uchar *)imgTemp->imageData;

    int step_im1      =  im1->widthStep/sizeof(uchar);
    int channels_im1   =  im1->nChannels;
    uchar* tdata_im1    = (uchar *)im1->imageData;

    int step_im2      =  im2->widthStep/sizeof(uchar);
    int channels_im2   =  im2->nChannels;
    uchar* tdata_im2    = (uchar *)im2->imageData;

    for (i=0;i<im1->height;i++)
    {
        for (j=0;j<im1->width;j++)
        {
            tdata_main[i*step_main+j*channels_main+0] = tdata_im1[(i)*step_im1+(j)*channels_im1+0];
            tdata_main[i*step_main+j*channels_main+1] = tdata_im1[(i)*step_im1+(j)*channels_im1+0];
            tdata_main[i*step_main+j*channels_main+2] = tdata_im1[(i)*step_im1+(j)*channels_im1+0];


        }
    }



    for (i=im1->height;i<(im1->height*2);i++)
    {
        for (j=0;j<im1->width;j++)
        {
            tdata_main[i*step_main+j*channels_main+0] = tdata_im2[(i-im1->height)*step_im2+(j)*channels_im2+0];
            tdata_main[i*step_main+j*channels_main+1] = tdata_im2[(i-im1->height)*step_im2+(j)*channels_im2+0];
            tdata_main[i*step_main+j*channels_main+2] = tdata_im2[(i-im1->height)*step_im2+(j)*channels_im2+0];


        }
    }


    return imgTemp;
}

//this function will concatenate two images vertically and draw the correspoindence indicated b yp1 and p2, the index is used to create the filename
//and denotes the number of the correspondence
int printLine(const HRImage& im1,const HRImage& im2, CvPoint p1, CvPoint p2, int indexname)
{
    IplImage* imglineTemp=concatImagesVertical(im1.cv_img,im2.cv_img);
    cvLine(imglineTemp, p1,p2, cvScalar(255,0,0), 1);
    string tslash="/";
    string fname=TEMPDIR+tslash+combineFnames(im1.filename,im2.filename,string(stringify(indexname)+".jpg"));
    if (!cvSaveImage(fname.c_str(),imglineTemp)) printf("Could not save: %s\n",fname.c_str());
    cvReleaseImage( &imglineTemp );



}




