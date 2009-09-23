
#include <fstream>
#include <iomanip>
#include <sstream>
#include "general.h"
#include "boost/filesystem.hpp"   // includes all needed Boost.Filesystem declarations
#include "boost/filesystem/operations.hpp"
#include "boost/filesystem/path.hpp"
#include "argproc.h"
#define DEBUGLVL 0

extern const char* TEMPDIR;
namespace fs = boost::filesystem;
using namespace boost::filesystem;
using namespace boost;
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

    if (im1==NULL && im2==NULL)
    {
        cout<<"images are empty"<<endl;
        return NULL;

    }

    if (im1==NULL)
    {
        IplImage* imgTemp=cvCloneImage(im2);
        return imgTemp;
    }

    if (im2==NULL)
    {
        IplImage* imgTemp=cvCloneImage(im1);
        return imgTemp;
    }

    if (im1->width != im2->width)
    {
        cout<<"images to be concatenated are not the same size"<<endl;
        return NULL;

    }


    IplImage* imgTemp=cvCreateImage(cvSize(im1->width,im1->height+im2->height),IPL_DEPTH_8U,3);


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



    for (i=im1->height;i<(im1->height+im2->height);i++)
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

    fs::path tempath( TEMPDIR, fs::native );
    string fname=combineFnames(im1.filename,im2.filename,string(stringify(indexname)+".jpg"));
    tempath/=fname;
    fname=tempath.string();



    if (!cvSaveImage(fname.c_str(),imglineTemp)) printf("Could not save: %s\n",fname.c_str());
    cvReleaseImage( &imglineTemp );



}
string findSeedDirName(const vector<string>& oArray)
{
    if (oArray.size()<1) return string("emptydir");

    int i,j;

    path pb(oArray[0]);

    pb.remove_leaf();
    pb.remove_leaf();
    path pathname(pb.leaf());

    if (pathname.string()!="reduced")
    {
        return pathname.string();

    }






    vector<string> sArray;
    for (i=0;i<oArray.size();i++)
    {

        fs::path p( oArray[i], fs::native );

        sArray.push_back(fs::basename(p));

    }



    if (sArray.size()<1)
    {
        cout<<"number of file names is zero"<<endl;
        return string("emptydir");

    }

    string dirname="";

    string firstfName=sArray[0];


    bool keepgoing=true;
    for (j = 0; j < sArray[0].length(); j++)
    {
        sArray[0][j];
        for (i=1;i<sArray.size();i++)
        {
            if (j>=sArray[i].size() || sArray[0][j]!=sArray[i][j])
            {
                keepgoing=false;
                break;
            }

        }
        if (keepgoing==false)
        {
            break;
        }
        else
        {
            if (isalnum(sArray[0][j]) && !isdigit(sArray[0][j]))
                dirname+=sArray[0][j];

        }
    }


    if (dirname.length()==0)
        dirname=string("emptyd");



    //cout<<"returned : "<<dirname<<endl;



    return dirname;
}



void writeCVMatrix(ostream &stream,const CvMat* M)
{

    if (M==NULL)
    {
        printf("****matrix is null\n");
        return;

    }

    if ( M->rows==0 || M->cols==0 )
    {
        stream<<"EMPTY"<<endl;
        return;
    }

    int n_rows = M->rows;

    int n_cols = M->cols;


    printf("\nnumber of rows is %d and number of cols is %d \n",n_rows,n_cols);





    for (int i = 0; i < n_rows; ++i)
    {
        for (int j = 0; j < n_cols; ++j)
        {


            stream<< cvmGet( M,i,j ) << "\t";
        }
        stream << "\n";

    }


}
//
//
//bool FileExists(string strFilename)
//{
//    struct stat stFileInfo;
//    bool blnReturn;
//    int intStat;
//
//    // Attempt to get the file attributes
//    intStat = stat(strFilename.c_str(),&stFileInfo);
//    if (intStat == 0)
//    {
//        // We were able to get the file attributes
//        // so the file obviously exists.
//        blnReturn = true;
//    }
//    else
//    {
//        // We were not able to get the file attributes.
//        // This may mean that we don't have permission to
//        // access the folder which contains this file. If you
//        // need to do that level of checking, lookup the
//        // return values of stat which will give you
//        // more details on why stat failed.
//        blnReturn = false;
//    }
//
//    return(blnReturn);
//}

void readCvMatFfromfile(CvMat** tmodel,const string& mfname)
{


    if ((*tmodel)==NULL)
    {
        printf("****matrix is null\n");
        return;

    }

    if ( (*tmodel)->rows==0 || (*tmodel)->cols==0 )
    {
        cout<<"EMPTY"<<endl;
        return;
    }

    int n_rows = (*tmodel)->rows;

    int n_cols = (*tmodel)->cols;


    printf("\nnumber of rows is %d and number of cols is %d and reading from file %s \n",n_rows,n_cols,mfname.c_str());
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


    while (!file_cm.eof() && j<n_rows) //second check is redundant, being safe
    {
        file_cm.getline(str,2000);

        string s(str);
        string out;
        istringstream ss;
        ss.str(s);

        for (i=0;i<n_cols;i++)
        {
            ss>>out;
            cvmSet((*tmodel),j,i, from_string<float>(out, std::dec));
        }

        j++;


    }

}


