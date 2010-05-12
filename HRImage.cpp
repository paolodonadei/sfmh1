#include <time.h>

#include <cmath>

#include <iostream>
#include "boost/filesystem.hpp"   // includes all needed Boost.Filesystem declarations
#include "boost/filesystem/operations.hpp"
#include "boost/filesystem/path.hpp"
#include "focallength.h"
#include "HRImage.hpp"
#include "highgui.h"
#include "pgmutils.hpp"
#include "sift.h"
#include "matching.hpp"
#include "argproc.h"
#define DEBUGLVL 0
#include "general.h"
#include "visiongen.h"
#if defined(_WIN32) || defined(WIN32) || defined(__CYGWIN__) || defined(__MINGW32__) || defined(__BORLANDC__)
#define OS_WIN
#endif

const char* TEMPDIR;

CvScalar colors[2]= {cvScalar(255,0,0), cvScalar(0,255,0)};


namespace fs = boost::filesystem;




HRImage::HRImage()
{

    siftkeyfilename="";
    pgmfilename="";
    cv_img=NULL;
    flag_valid=0;//object not redy yet
    cv_img=NULL;
    width=height=0;
    data=NULL;
}
int HRImage::openim(string fname)
{



    camPose.tm=cvCreateMat(3,1,CV_64F);
    camPose.Rm=cvCreateMat(3,3,CV_64F);

    distortion=cvCreateMat(5,1,CV_64F);
    cvSetZero(distortion);
    cvSetZero(camPose.tm);
    cvSetIdentity(camPose.Rm);

    intrinsicMatrix=cvCreateMat(3,3, CV_64F);
    projectionMatrix=cvCreateMat(3,4, CV_64F);
    siftkeyfilename="";
    pgmfilename="";
    cv_img=NULL;
    if ( !boost::filesystem::exists( fname) )
    {
        cout << "file: "<<fname <<" does not exist, exiting"<<endl;

        exit(EXIT_FAILURE);
    }

    if (flag_valid!=0)
    {
        //release previous image
        cvReleaseImage(&cv_img );
    }

    filename=fname;


    // load an image
    cv_img=cvLoadImage(fname.c_str());
    if (!cv_img)
    {
        cout<<"could not load: "<<fname<<endl;
        exit(0);
    }


    if (cv_img->nChannels!=1)
    {
        if (DEBUGLVL>0)  cout<<" image is not grey, converting"<<endl;
        IplImage* tempImage = cvCreateImage(cvGetSize(cv_img), IPL_DEPTH_8U, 1);
        cvCvtColor(cv_img, tempImage, CV_BGR2GRAY);
        cvReleaseImage(&cv_img );
        cv_img= tempImage; //i might have ot be careful about releasing this

    }

    updateImageInfo();


    flag_valid=2;//if image was read successfully set the valid_flag to 2 or 1 (2 for file image, 1 for mem image)


    return 1;
}
int HRImage::openim(int pheight, int pwidth,int initial)
{
    siftkeyfilename="";
    pgmfilename="";
    cv_img=NULL;
    //use this to construct an empty image, not associated with any files, this is a one channel image
    if (flag_valid!=0)
    {
        cout<<"image being rewritten\n"<<endl;
        cvReleaseImage(&cv_img );
    }
    cv_img=cvCreateImage(cvSize(pwidth,pheight),IPL_DEPTH_8U,1);
    updateImageInfo();


    camPose.tm=cvCreateMat(3,1,CV_64F);
    camPose.Rm=cvCreateMat(3,3,CV_64F);

    distortion=cvCreateMat(5,1,CV_64F);
       cvSetZero(distortion);
    cvSetZero(camPose.tm);
    cvSetIdentity(camPose.Rm);

    intrinsicMatrix=cvCreateMat(3,3, CV_64F);
    projectionMatrix=cvCreateMat(3,4, CV_64F);
    filename="memimage.pgm";
    flag_valid=1;
    return 1;

}
HRImage::HRImage(const HRImage &img)
{
    siftkeyfilename="";
    pgmfilename="";
    flag_valid=img.flag_valid;


    camPose.tm=cvCreateMat(3,1,CV_64F);
    camPose.Rm=cvCreateMat(3,3,CV_64F);

    distortion=cvCreateMat(5,1,CV_64F);
       cvSetZero(distortion);
    cvSetZero(camPose.tm);
    cvSetIdentity(camPose.Rm);

    intrinsicMatrix=cvCreateMat(3,3, CV_64F);
    projectionMatrix=cvCreateMat(3,4, CV_64F);
    cv_img=cvCloneImage(img.cv_img);
    updateImageInfo();

    if (DEBUGLVL>0) cout<<"copy constructor being called on :"<<filename<<endl;


}
int HRImage::displayImage()
{
    if (flag_valid==0)
    {
        cout<<"image not loaded\n"<<endl;
        return 0;
    }
    cvNamedWindow(  filename.c_str(), CV_WINDOW_AUTOSIZE);
    cvMoveWindow(filename.c_str(), 100, 100);
    // show the image
    cvShowImage(filename.c_str(), cv_img );

//    wait for a key
    cvWaitKey(0);

    return 1;

}


HRImage::HRImage(string fname)
{

    cv_img=NULL;
    flag_valid=0;//object not redy yet
    openim(fname);

}


HRImage::HRImage(char* fname)
{

    flag_valid=0;//object not redy yet
    filename=fname;
    string sfname=fname;
    openim(sfname);

}

HRImage::HRImage(int pheight, int pwidth,int initial)
{
    flag_valid=0;//object not redy yet
    openim(pheight,pwidth,initial);
}



int HRImage::writeImage(string fname)
{
    if (flag_valid==0)
    {
        cout<<"image not loaded\n"<<endl;
        return 0;
    }

    if (!cvSaveImage(fname.c_str(),cv_img))
    {

        cout<<"image "<<fname <<" not saved\n"<<endl;
        return 0;
    }
    return 1;
}

HRImage::~HRImage()
{

    // cout<<"destructor is being called on "<<filename<<endl;
    close();

}

void HRImage::scale(int pscale)
{
    if (flag_valid==0)
    {
        cout<<"image not loaded\n"<<endl;
        return ;
    }

    int temppix;
    for (int i=0; i<height; i++)
    {
        for (int j=0; j<width; j++)
        {
            temppix=getPixel(i,j)*pscale;
            setPixel(i,j,(temppix >= 0 && temppix <255)? temppix:0);
        }
    }


}

void HRImage::divideAll(int val)
{
    if (flag_valid==0)
    {
        cout<<"image not loaded\n"<<endl;
        return ;
    }
    double pixel;
    for (int i=0; i<height; i++)
    {
        for (int j=0; j<width; j++)
        {
            pixel=getPixel(i,j);
            pixel/=val;
            setPixel(i,j,(int)pixel);
        }
    }

}

void  HRImage::setAll(int val)
{
    if (flag_valid==0)
    {
        cout<<"image not loaded\n"<<endl;
        return ;
    }
    for (int i=0; i<height; i++)
    {
        for (int j=0; j<width; j++)
        {
            setPixel(i,j,val);
        }
    }


}

void HRImage::close()
{
    if (DEBUGLVL>0)   cout<<"close of HRImage called "<<filename<<endl;
    //free the oneD and twoD arrays
    if (flag_valid==0 || cv_img==NULL )
    {
        cout<<"freeing nonexistent image"<<endl;
        return ;
    }
    cvReleaseMat(&intrinsicMatrix);
    cvReleaseMat(&projectionMatrix);
    cvReleaseMat(&distortion);
    cvReleaseMat(&camPose.tm);
    cvReleaseMat(&camPose.Rm);


    cvReleaseImage(&cv_img );

    flag_valid=0;
}

unsigned char* HRImage::get1d_array()
{
    return data;
}
int HRImage::getPixel(int y,int x)
{
    if (flag_valid==0)
    {
        cout<<"image not loaded\n"<<endl;
        return 0;
    }


    return data[y*step+x*channels+0];




}
void HRImage::incrementPixel(int y,int x)
{

    if (flag_valid==0)
    {
        cout<<"image not loaded\n"<<endl;
        return ;
    }

    if (y<0 || y>=height || x<0 || x>=width)
    {
        cout<<"incorrect pixel is incremented"<<endl;
        return ;
    }

    if ((*this)[y][x]<255)
        (*this)[y][x]++;


}
void HRImage::setPixel(int y,int x,int pixValue)
{

    if (flag_valid==0)
    {
        cout<<"image not loaded\n"<<endl;
        return ;
    }

    if (y<0 || y>=height || x<0 || x>=width)
    {
        cout<<"incorrect pixel is set"<<endl;
        return ;
    }
    if (pixValue<256 && pixValue>0)
        (*this)[y][x]=(unsigned char)pixValue;
    else
        cout<<"error "<<endl;

}

HRImage& HRImage::operator= (const HRImage& param)
{

    if (param.flag_valid==0)
    {
        cout<<"equating to empty image, returning\n"<<endl;

    }
    if (flag_valid!=0)
    {
        cout<<"image being rewritten\n"<<endl;
        cvReleaseImage(&cv_img );
    }


    cv_img=cvCloneImage(param.cv_img);

    updateImageInfo();
    flag_valid=3; //image obtained from another image, flag indicates that



    return *this;


}

void HRImage::updateImageInfo()
{
// get the image data
    height    = cv_img->height;
    width     = cv_img->width;
    step      = cv_img->widthStep;
    channels  = cv_img->nChannels;
    data      = (uchar *)cv_img->imageData;
    if (DEBUGLVL>0)   cout<<"Processing a "<< height<<"X"<< width <<"image with "<<  channels<<" channels, called: "<<filename<<endl;


}
int HRImage::undistortPoints()
{
    int i;


    HR2DVectorUndistorted.clear();
    CvMat* curPT=cvCreateMat(1,1,CV_32FC2);
    CvMat* curPTundist=cvCreateMat(1,1,CV_32FC2);


    if (flag_valid==0)
    {
        cout<<"image now created yet\n"<<endl;
        return 0;
    }

    HR2DVectorUndistorted.resize(HR2DVector.size());



    vector<HRPointFeatures>::iterator feature_iterator;

//cvSetZero(distortion);
    feature_iterator = HR2DVector.begin();
    int count=0;

    for(i=0; i<HR2DVector.size(); i++)
    {
        HRPointFeatures newfeature( new HRFeature);

        curPT->data.fl[0]=HR2DVector[i]->location.x;
        curPT->data.fl[1]=HR2DVector[i]->location.y;


        cvUndistortPoints(curPT, curPTundist, intrinsicMatrix, distortion, NULL, NULL);


        newfeature->location.x=(cvmGet(intrinsicMatrix, 0,0)*curPTundist->data.fl[0])+cvmGet(intrinsicMatrix, 0,2);
        newfeature->location.y=(cvmGet(intrinsicMatrix, 1,1)*curPTundist->data.fl[1])+cvmGet(intrinsicMatrix, 1,2);

        HR2DVectorUndistorted[i]=newfeature;
        //   printf("undistorted point [%f  ; %f] -> [%f  ; %f]\n", curPT->data.fl[0], curPT->data.fl[1],newfeature->location.x,newfeature->location.y );
    }

    cvReleaseMat(&curPTundist);
    cvReleaseMat(&curPT);

}
int HRImage::showParams()
{
    printf("current image is %s\n",pgmfilename.c_str());
    printf("intrinsics:\n");
    writeCVMatrix(cout,intrinsicMatrix);
    printf("distortion:\n");
    writeCVMatrix(cout,distortion);
    printf("rotation:\n");
    writeCVMatrix(cout,camPose.Rm);
    printf("translation :\n");
    writeCVMatrix(cout,camPose.tm);
    printf("projection :\n");
    writeCVMatrix(cout,projectionMatrix);

    return 0;
}
int HRImage::writeFeatures()
{

    if (flag_valid==0)
    {
        cout<<"image now created yet\n"<<endl;
        return 0;
    }
    if (HR2DVector.size()<1)
    {
        cout<<"no features available yet\n"<<endl;
        return 0;
    }

    int i;

    string tempfilename="";
    fs::path p( filename, fs::native );

    tempfilename=fs::basename(p)+"features.txt";

    tempfilename=(fs::path( TEMPDIR, fs::native )/fs::path( tempfilename, fs::native )).file_string();


    fstream fp_out;
    fp_out.open(tempfilename.c_str(), ios::out);
    if (!fp_out)
    {
        cout << "Cannot open file.\n";
        return 1;
    }

    fp_out<<"#\t\t feature number \t\t X \t\t Y"<<endl;




    for (i=0; i<HR2DVector.size(); i++)
    {
        fp_out<<i<<"  \t\t  "<<HR2DVector[i]->location.x<<"  \t\t  "<<HR2DVector[i]->location.y<<endl;


    }






    fp_out.close();

    return 1;



}
int  HRImage::writeImageFeatures()
{

    if (flag_valid==0)
    {
        cout<<"image now created yet\n"<<endl;
        return 0;
    }
    if (HR2DVector.size()<1)
    {
        cout<<"no features available yet\n"<<endl;
        return 0;
    }



    IplImage* tempImage = cvCreateImage(cvGetSize(cv_img), IPL_DEPTH_8U, 3);//png doesnt support alpha channel in opencv
    cvCvtColor(cv_img, tempImage, CV_GRAY2BGR);


    vector<HRPointFeatures>::iterator feature_iterator;


    feature_iterator = HR2DVector.begin();
    int count=0;
    while ( feature_iterator  != HR2DVector.end() )
    {
        count++;
        draw_cross((*feature_iterator)->location,CV_RGB(0,255,0),4,tempImage);
        ++feature_iterator;
    }
    printf("drew %d crosses \n",count);
    string tempfilename="";
    fs::path p( filename, fs::native );

    tempfilename=fs::basename(p)+"features.png";

    tempfilename=(fs::path( TEMPDIR, fs::native )/fs::path( tempfilename, fs::native )).file_string();


    if ( checkTempPath()==false)
        return 0;
    if (!cvSaveImage(tempfilename.c_str(),tempImage ))
    {

        cout<<"image "<<tempfilename<<" not saved\n"<<endl;
        return 0;
    }
    cvReleaseImage(&tempImage );

    return 1;

}


/** @brief shows image with features
  * this function shows the featues superimposed on the images
  */
int HRImage::displayImageFeatures()
{

    if (flag_valid==0)
    {
        cout<<"image now created yet\n"<<endl;
        return 0;
    }
    if (HR2DVector.size()<1)
    {
        cout<<"no features available yet\n"<<endl;
        return 0;
    }



    IplImage* tempImage = cvCreateImage(cvGetSize(cv_img), IPL_DEPTH_8U, 4);
    cvCvtColor(cv_img, tempImage, CV_GRAY2BGR);


    vector<HRPointFeatures>::iterator feature_iterator;


    feature_iterator = HR2DVector.begin();
    while ( feature_iterator  != HR2DVector.end() )
    {
        draw_cross((*feature_iterator)->location,CV_RGB(0,255,0),4,tempImage);
        ++feature_iterator;
    }


    cvNamedWindow(  filename.c_str(), CV_WINDOW_AUTOSIZE);
    cvMoveWindow(filename.c_str(), 100, 100);
    // show the image
    cvShowImage(filename.c_str(), tempImage );

//    wait for a key
    cvWaitKey(0);
    cvReleaseImage(&tempImage );
    cvDestroyWindow(filename.c_str());
    return 1;
}

///////////////////////////////////////////////////////////CLASS HRCORRIMAGE:
HRCORRImage::HRCORRImage()
{
    flag_valid=0;//object not redy yet
    cv_img=NULL;
    width=height=0;
    data=NULL;

    cv_im1=NULL;
    cv_im2=NULL;
}

HRCORRImage::HRCORRImage(IplImage* p_cv_im1,IplImage* p_cv_im2)
{

    init(p_cv_im1,p_cv_im2);

}

int   HRCORRImage::init(HRImage* p_hr_im1,HRImage* p_hr_im2)
{

    init(p_hr_im1->cv_img,p_hr_im2->cv_img);
}
int HRCORRImage::init(IplImage* p_cv_im1,IplImage* p_cv_im2)
{
    whitespace_ratio=0.1;
    //use this to construct an empty image, not associated with any files, this is a one channel image
    if (flag_valid!=0)
    {
        cout<<"image being rewritten\n"<<endl;
        cvReleaseImage(&cv_img );
        cvReleaseImage(&cv_im1 );
        cvReleaseImage(&cv_im2 );
    }
    flag_valid=1;
    if (p_cv_im1->height!=p_cv_im2->height || p_cv_im1->width!=p_cv_im2->width)
    {
        cout<<"images dont have the same size, returning\n"<<endl;
        return 0;
    }
    height    = p_cv_im1->height;
    width     = (2*p_cv_im1->width)+(int)(rint(((double)whitespace_ratio*p_cv_im1->width)))+2;//extra margin to be sure


    cv_img=cvCreateImage(cvSize(width,height),IPL_DEPTH_8U,4);
    updateImageInfo();


    cv_im1=cvCloneImage(p_cv_im1);
    cv_im2=cvCloneImage(p_cv_im2);


    filename="memimagecorrs.pgm";

    im1_top_left.x= 0;
    im1_top_left.y=0 ;


    im2_top_left.x=cv_im1->width+ (int)(rint(((double)whitespace_ratio*cv_im1->width))) ;
    im2_top_left.y=0;

    flag_valid=1;

//setting the image



    int i,j;


    int step_main       =  cv_img->widthStep/sizeof(uchar);
    int channels_main   =  cv_img->nChannels;
    uchar* tdata_main    = (uchar *)cv_img->imageData;

    int step_im1      =  cv_im1->widthStep/sizeof(uchar);
    int channels_im1   =  cv_im1->nChannels;
    uchar* tdata_im1    = (uchar *)cv_im1->imageData;

    int step_im2      =  cv_im1->widthStep/sizeof(uchar);
    int channels_im2   =  cv_im1->nChannels;
    uchar* tdata_im2    = (uchar *)cv_im1->imageData;

    for (i=im1_top_left.y; i<(cv_im1->height+im1_top_left.y); i++)
    {
        for (j=im1_top_left.x; j<(cv_im1->width+im1_top_left.x); j++)
        {
            tdata_main[i*step_main+j*channels_main+0] = tdata_im1[(i-im1_top_left.y)*step_im1+(j-im1_top_left.x)*channels_im1+0];
            tdata_main[i*step_main+j*channels_main+1] = tdata_im1[(i-im1_top_left.y)*step_im1+(j-im1_top_left.x)*channels_im1+0];
            tdata_main[i*step_main+j*channels_main+2] = tdata_im1[(i-im1_top_left.y)*step_im1+(j-im1_top_left.x)*channels_im1+0];
            tdata_main[i*step_main+j*channels_main+3] = 150;

        }
    }



    for (i=im2_top_left.y; i<(cv_im2->height+im2_top_left.y); i++)
    {
        for (j=im2_top_left.x; j<(cv_im2->width+im2_top_left.x); j++)
        {
            tdata_main[i*step_main+j*channels_main+0] = tdata_im2[(i-im2_top_left.y)*step_im2+(j-im2_top_left.x)*channels_im2+0];
            tdata_main[i*step_main+j*channels_main+1] = tdata_im2[(i-im2_top_left.y)*step_im2+(j-im2_top_left.x)*channels_im2+0];
            tdata_main[i*step_main+j*channels_main+2] = tdata_im2[(i-im2_top_left.y)*step_im2+(j-im2_top_left.x)*channels_im2+0];
            tdata_main[i*step_main+j*channels_main+3] = 150;

        }
    }

    return 1;


}
HRCORRImage::~HRCORRImage()
{
    close();//not sure if the baseclass's close will be called, here in this i wil close the extra two images



}
void HRCORRImage::close()
{

    if (DEBUGLVL>0)  cout<<"close of HRCORRImage"<<endl;
    //free the oneD and twoD arrays
    if (flag_valid==0)
    {
        cout<<"freeing nonexistent image"<<endl;
        return ;
    }

    cvReleaseImage(&cv_im1 );
    cvReleaseImage(&cv_im2 );

}
////////////////////////////// functions belonging to imageSet
HRImageSet::HRImageSet()
{
    numImages=0;
    temporaryDir="emptidir";
}
HRImageSet:: HRImageSet(string directoryName,string TEMPdirectoryName)
{
    open(directoryName,TEMPdirectoryName);
}
HRImageSet::~HRImageSet()
{

}
void HRImageSet::showOneByOne()
{
    if (DEBUGLVL>0)   cout<<"displaying images size of the vector is "<<imageCollection.size()<<endl;
    vector<HRImagePtr>::iterator img_iterator;


    img_iterator = imageCollection.begin();
    while ( img_iterator  != imageCollection.end() )
    {
        (*img_iterator)->displayImage();
        ++img_iterator;
    }

}
void HRImageSet::showOneByOneFeatureMotions()
{

#ifdef OS_WIN
    string funddrawname=(fs::path( string("utils"), fs::native )/fs::path(string("fundutils"), fs::native )/fs::path(string("funddraw.exe"), fs::native )).file_string();

#else
    string funddrawname=(fs::path( string("utils"), fs::native )/fs::path(string("fundutils"), fs::native )/fs::path(string("funddraw"), fs::native )).file_string();
#endif


    if ( !fs::exists( funddrawname ) )
    {
        cout << "funddraw for matching is unavailable\n";
        return ;
    }

    if (system(NULL)==0)
    {
        cout<<"command processor not available , no features found"<<endl;
        return ;

    }



    int i,j;

    for (i=0; i<imageCollection.size(); i++)
    {
        for (j=0; j<i; j++)
        {
            {
                string command_run=funddrawname+string(" \"")+(*imageCollection[i]).filename+string("\" \"")+(*imageCollection[j]).filename+string("\" \"")+correspondencesPairWise[i][j].motion.filenameF+string("\" 1 ");
                cout<<command_run<<endl;
                system (command_run.c_str());

            }
            {
                string command_run=funddrawname+string(" \"")+(*imageCollection[i]).filename+string("\" \"")+(*imageCollection[j]).filename+string("\" \"")+correspondencesPairWise[i][j].motion.filenameH+string("\" 2 ");
                cout<<command_run<<endl;
                system (command_run.c_str());

            }
        }
    }





}
void HRImageSet::showOneByOneFeature()
{
    if (DEBUGLVL>0)   cout<<"displaying images size of the vector is "<<imageCollection.size()<<endl;
    vector<HRImagePtr>::iterator img_iterator;


    img_iterator = imageCollection.begin();
    while ( img_iterator  != imageCollection.end() )
    {
        (*img_iterator)->displayImageFeatures();
        ++img_iterator;
    }

}

int  HRImageSet::open(string directoryName, string TEMPdirectoryName)
{
    temporaryDir=TEMPdirectoryName;
    dirName=directoryName;
    numImages=0;

    vector<string> fnameArray;


    unsigned long file_count = 0;

    fs::path full_path( fs::initial_path<fs::path>() );


    full_path = fs::system_complete( fs::path( directoryName, fs::native ) );

    if ( !fs::exists( full_path ) )
    {
        cout << "\nNot found: " << full_path.native_file_string() << endl;
        return 1;
    }

    if ( fs::is_directory( full_path ) )
    {

        fs::directory_iterator end_iter;
        for ( fs::directory_iterator dir_itr( full_path );
                dir_itr != end_iter;
                ++dir_itr )
        {
            try
            {

                if ( fs::is_regular( dir_itr->status() ) )
                {
                    if (fs::extension(dir_itr->path())==".pgm" || fs::extension(dir_itr->path())==".bpm" ||
                            fs::extension(dir_itr->path())==".jpg" || fs::extension(dir_itr->path())==".tiff"
                            || fs::extension(dir_itr->path())==".ppm" || fs::extension(dir_itr->path())==".png")
                    {
                        ++file_count;
                        cout << "currently loading file: "<<dir_itr->path().file_string()<<endl ;

                        HRImagePtr hr_iptr( new HRImage( dir_itr->path().file_string()) );
                        fnameArray.push_back( dir_itr->path().file_string() );

                        imageCollection.push_back( hr_iptr );



                    }
                    //  std::cout<<"Press [Enter] to continue . . .";
                    //    std::cin.get();

                }

            }
            catch ( const exception & ex )
            {

                cout << dir_itr->path().leaf() << " " << ex.what() << endl;
            }

        }

    }
    else // must be a file
    {
        cout << "\nFound: " << full_path.native_file_string() << endl;
    }
    numImages=file_count;
    // cout<<"finished processing images, size of the collection is : "<<imageCollection.size()<<endl;
//cout<<"________________LOOP FINISHED______________________________"<<endl;
    dirStemName=findSeedDirName(fnameArray)+"sfmh";

    fs::path temp_path=temporaryDir;
    temp_path/=dirStemName;
    if ( !fs::exists(temp_path ) )
    {
        create_directories( temp_path );
        if ( !fs::exists( temp_path ) )
        {
            cout<<"cant create directory, exiting"<<endl;
            exit(EXIT_FAILURE);
        }
    }


    temporaryDir=temp_path.file_string();
    TEMPDIR=temporaryDir.c_str();
    outdirStemName=temporaryDir;
    return file_count;
}

int HRImageSet::featureDetectSift()
{

    if (DEBUGLVL>0)   cout<<"SIFT FEATURE MATCHER BEGINS"<<endl;
    vector<HRImagePtr>::iterator img_iterator;


    img_iterator = imageCollection.begin();
    while ( img_iterator  != imageCollection.end() )
    {
        findSIFTfeatures(**img_iterator);
        (*img_iterator)->writeImageFeatures(); //remove this if you dont want to see the features
        (*img_iterator)->writeFeatures();
        ++img_iterator;
    }




    return 0;
}
/** @brief exhaustiveSIFTMatching
*
* @todo: document this function
*/
int HRImageSet::multipleViewEstimate()
{
    int i,j;

    for (i=0; i<imageCollection.size(); i++)
    {
        for (j=0; j<i; j++)
        {
            correspondencesPairWise[i][j].findGeomtry();//remove outliers and find motion model
            correspondencesPairWise[i][j].WriteMatches();



        }
    }




}
int HRImageSet::writeMotions()
{
    int i,j;

    for (i=0; i<imageCollection.size(); i++)
    {
        for (j=0; j<i; j++)
        {

            correspondencesPairWise[i][j].WriteMotion();


        }
    }




}

int HRImageSet::exhaustiveSIFTMatching()
{
    int i,j;
    double f1,f2;

    correspondencesPairWise.resize( imageCollection.size(), vector<HRCorrespond2N> ( imageCollection.size() ) );



//deleting the index files, i should propbablyt find a better place for this
    string fname1=TEMPDIR+string("/")+string("F_")+string(INDEXFNAME);
    if( remove( fname1.c_str() ) != 0 )
        perror( "ignore this error - Error deleting file" );
    else
        puts( "File successfully deleted" );
    string fname2=TEMPDIR+string("/")+string("H_")+string(INDEXFNAME);
    if( remove( fname2.c_str() ) != 0 )
        perror( "ignore this error - Error deleting file" );
    else
        puts( "File successfully deleted" );
//end of deleting index files, these files were necessary for self calibration


    for (i=0; i<imageCollection.size(); i++)
    {
        for (j=0; j<i; j++)
        {
            correspondencesPairWise[i][j].indexIm1=i;
            correspondencesPairWise[i][j].indexIm2=j;

            correspondencesPairWise[i][j].hr1ptr=&(*imageCollection[i]);
            correspondencesPairWise[i][j].hr2ptr=&(*imageCollection[j]);

            int numf_found= matchTWOImagesNearestNeighbour( (*imageCollection[i]), (*imageCollection[j]),correspondencesPairWise[i][j]);

            printf("between image %s having %d features and image %s with %d features, we found %d correspondences\n",(*imageCollection[i]).pgmfilename.c_str(),(*imageCollection[i]).HR2DVector.size()
                   ,(*imageCollection[j]).pgmfilename.c_str(),(*imageCollection[j]).HR2DVector.size(),numf_found);




        }
    }


}
void HRImageSet::drawallMatches()
{
    int i,j;

    for (i=0; i<imageCollection.size(); i++)
    {
        for (j=0; j<i; j++)
        {

            drawMatchesPair((*imageCollection[i]), (*imageCollection[j]),correspondencesPairWise[i][j]);
            drawMatchesSingle((*imageCollection[i]), (*imageCollection[j]),correspondencesPairWise[i][j]);


        }
    }




}
int HRImageSet::SelfCalibrate()
{
    int width =(*imageCollection[0]).width;
    int height =(*imageCollection[0]).height;



    int numFrames=imageCollection.size();

    vector<CvMat* > intrinMatrix;
    vector< vector<CvMat*> > funMatrix;

    intrinMatrix.resize(numFrames);
    funMatrix.resize(numFrames);


    for (int i = 0; i < numFrames; ++i)
    {
        funMatrix[i].resize(numFrames);

    }

    for (int i = 0; i < numFrames; ++i)
    {
        intrinMatrix[i]=(*imageCollection[i]).intrinsicMatrix;
        for (int j = 0; j < numFrames; ++j)
        {
            funMatrix[i][j]=NULL;

        }
    }



    //filling in the rest of the Fs
    for (int i = 0; i < numFrames; ++i)
    {

        for (int j = i+1; j < numFrames; ++j)
        {
            cvTranspose((correspondencesPairWise[j][i]).motion.MotionModel_F,(correspondencesPairWise[i][j]).motion.MotionModel_F);


        }
    }

    for (int i = 0; i < numFrames; ++i)
    {
        intrinMatrix[i]=(*imageCollection[i]).intrinsicMatrix;
        for (int j = 0; j < numFrames; ++j)
        {
            if(i!=j)
            {


                funMatrix[i][j]=(correspondencesPairWise[i][j]).motion.MotionModel_F;
            }
        }
    }



    HRSelfCalibtwoFrame(funMatrix, intrinMatrix, width, height,confid, HARTLEY);


    cout<<" According to Hartley :"<<endl;

    for (int i = 0; i < numFrames; ++i)
    {
        printf("confidence for K %d is %f\n",i,confid[i]);
        writeCVMatrix(cout,intrinMatrix[i]);
    }



    HRSelfCalibtwoFrame(funMatrix, intrinMatrix, width, height, confid,STRUM);

    cout<<" According to Sturm :"<<endl;

    for (int i = 0; i < numFrames; ++i)
    {
        printf("confidence for K %d is %f\n",i,confid[i]);
        writeCVMatrix(cout,intrinMatrix[i]);
    }



    HRSelfCalibtwoFrame(funMatrix, intrinMatrix, width, height, confid,NONLINSIMPLE);

    cout<<" According to NONLINSIMPLE :"<<endl;

    for (int i = 0; i < numFrames; ++i)
    {
        printf("confidence for K %d is %f\n",i,confid[i]);
        writeCVMatrix(cout,intrinMatrix[i]);
    }



}
void HRImageSet::findEssentialMatrices()
{
    int i, j, k,q;
    CvMat* tempmat1 = cvCreateMat(3,3, CV_64F);
    CvMat* tempmat2 = cvCreateMat(3,3, CV_64F);


    CvMat* temp2 = cvCreateMat(3,3, CV_64F);
    CvMat* temp3 = cvCreateMat(3,3, CV_64F);
    CvMat* temp4 = cvCreateMat(3,3, CV_64F);



    for (i=0; i<imageCollection.size(); i++)
    {
        for (j=0; j<imageCollection.size(); j++)
        {
            if(i!=j)
            {
//                printf("__________________________________________________\n");
//                printf("calculating essential %d -> %d \n",i,j);
//                printf("intrinsic %d\n:",i);
//                writeCVMatrix(cout,(*imageCollection[i]).intrinsicMatrix);
//
//                printf("intrinsic %d\n:",j);
//                writeCVMatrix(cout,(*imageCollection[j]).intrinsicMatrix);
//                printf("fundamental %d -> %d \n",i,j);
//                writeCVMatrix(cout,(correspondencesPairWise[i][j]).motion.MotionModel_F);


                cvTranspose((*imageCollection[j]).intrinsicMatrix, tempmat1);
                cvMatMul(tempmat1, (correspondencesPairWise[i][j]).motion.MotionModel_F, tempmat2);
                cvMatMul(tempmat2, (*imageCollection[i]).intrinsicMatrix, (correspondencesPairWise[i][j]).motion.MotionModel_E);



                normalizeMatrix((correspondencesPairWise[i][j]).motion.MotionModel_E);

//                printf("essential %d -> %d \n",i,j);
//                writeCVMatrix(cout,(correspondencesPairWise[i][j]).motion.MotionModel_E);
                //    printf("__________________________________________________\n");
//                cvSVD( (correspondencesPairWise[i][j]).motion.MotionModel_E, temp2,  temp3, temp4,CV_SVD_U_T |CV_SVD_V_T );  //change all of the below back to U
//
//
//
//                double err=cvmGet(temp2,0,0)-cvmGet(temp2,1,1);
//                printf("svd error %d and %d was %f \n",i,j,err);
            }
        }
    }

    cvReleaseMat(&tempmat1);
    cvReleaseMat(&tempmat2);


    cvReleaseMat(&temp2);
    cvReleaseMat(&temp3);
    cvReleaseMat(&temp4);

}
void HRImageSet::showparamsALL()
{


    for (int i=0; i<imageCollection.size(); i++)
    {
        printf("image %d\n",i);

        (*imageCollection[i]).showParams();

        printf("___________________________________\n",i);
    }


}
void HRImageSet::undistortall()
{

    for (int i=0; i<imageCollection.size(); i++)
    {


        (*imageCollection[i]).undistortPoints();


    }



}

int HRImageSet::createFeatureTrackMatrix()
{
    myTracks.trackImageCollection=&imageCollection;

    int i, j, k,q;

    for (i=0; i<imageCollection.size(); i++)
    {
        for (j=0; j<i; j++)
        {
            for (k=0; k<correspondencesPairWise[i][j].imIndices.size(); k++)
            {
                //cout<<"i is "<<i <<" and j is "<<j<<endl;
                if (correspondencesPairWise[i][j].imIndices[k].inlier==1)
                {
                    myTracks.processPairMatchinTrack( correspondencesPairWise[i][j], k,imageCollection.size());
                }
            }

        }
    }

    cout<<"number of feature tracks is ---------- "<<myTracks.getNumTracks()<<endl;
    myTracks.calcFeatureTrackScores(correspondencesPairWise);

    myTracks.writeTrackMatrix("trackbefore.txt");
    myTracks.drawImageTrackMatches(imageCollection,"beforeprunetracks.png");
    myTracks.pruneFeatureTrack(  );
    myTracks.writeTrackMatrix("trackafter.txt");
    myTracks.drawImageTrackMatches(imageCollection,"afterprunetracks.png");
}





FeatureTrack::FeatureTrack()
{


}
int FeatureTrack::drawImageTrackMatches(const vector<HRImagePtr>& imCollection,string filname)
{
#define SINGLEMATCHPRINT 0

    int i, j, k;
    int x0,y0,x1,y1;
    IplImage* imgTemp=NULL;
    IplImage* imgTempcopy=NULL;
    IplImage* imgTemptempcopy=NULL;
    if ( checkTempPath()==false)
        return 0;

    int heightImage=0;

    if (imCollection.size()==0)
    {
        cout<<"no images exist in the database, quitting"<<endl;
        return 0;

    }
    heightImage=(*imCollection[0]).height;


    for (i=0; i<imCollection.size(); i++)
    {

        IplImage* imgTemptemp=concatImagesVertical(imgTemp,(*imCollection[i]).cv_img);

        cvReleaseImage( &imgTemp );
        imgTemp=imgTemptemp;
    }

    if (SINGLEMATCHPRINT==1)
    {
        imgTempcopy=cvCloneImage(imgTemp);

    }



    for (i=0; i<trackMatrix.size(); i++)
    {
        if (SINGLEMATCHPRINT==1)
        {

            imgTemptempcopy=cvCloneImage(imgTempcopy);
        }

        for (j=1; j<trackMatrix[i].size(); j++)
        {

            if (trackMatrix[i][j]!=-1 && trackMatrix[i][j-1]!=-1)
            {

                x0=(*imCollection[j-1]).HR2DVector[trackMatrix[i][j-1]]->location.x;
                y0=(*imCollection[j-1]).HR2DVector[trackMatrix[i][j-1]]->location.y+ ((j-1)*heightImage);
                x1=(*imCollection[j]).HR2DVector[trackMatrix[i][j]]->location.x;
                y1=(*imCollection[j]).HR2DVector[trackMatrix[i][j]]->location.y+ ((j)*heightImage);

                cvLine(imgTemp, cvPoint(x0,y0),cvPoint(x1,y1), (inliersStates[i])?colors[j%2]:cvScalar(0,0,255), 1);

                //print correspondences 1 to 1
                if (SINGLEMATCHPRINT==1)
                {
                    cvLine(imgTemptempcopy, cvPoint(x0,y0),cvPoint(x1,y1), colors[j%3], 1);

                }
            }
        }
        if (SINGLEMATCHPRINT==1)
        {

            string  fname=(fs::path( TEMPDIR, fs::native )/fs::path( fname, fs::native )).file_string();



            if (!cvSaveImage(fname.c_str(),imgTemptempcopy)) printf("Could not save: %s\n",fname.c_str());
            cvReleaseImage( &imgTemptempcopy );
        }

    }



    fs::path tempath( TEMPDIR, fs::native );
    string fname=filname;
    tempath/=fname;
    fname=tempath.file_string();



    if (!cvSaveImage(fname.c_str(),imgTemp)) printf("Could not save: %s\n",fname.c_str());


    cvReleaseImage( &imgTemp );

    if (SINGLEMATCHPRINT==1)
    {
        cvReleaseImage( &imgTempcopy );


    }


    return 0;






}

int FeatureTrack::findMatchinTrack( HRCorrespond2N& corrs, int indexNumber, vector<int>& matchedIndices)
{
    int i;
    for (i=0; i<trackMatrix.size(); i++)
    {

        if (trackMatrix[i][corrs.indexIm1]==corrs.imIndices[indexNumber].imindex1 || trackMatrix[i][corrs.indexIm2]==corrs.imIndices[indexNumber].imindex2)
        {
            //dont return this if they are tehre exactly
            matchedIndices.push_back(i);
        }


    }
    return 1;
}

int FeatureTrack::processPairMatchinTrack( HRCorrespond2N& corrs, int indexNumber, int rowsize)
{
    //this function behaves as such, first it takes a single match between two images, so a single correspondence
//   then it looks into the track matrix for the presence of any one of those correesponence points, if one exists, it looks at the correponding slot for the other image
//if its empty then it populates it and adds in the score, if its none empty and its the same value that it would have been it does the same, but if its empty and has a dfferent
//value for the other image then it clones that row with the new correspondence



    int i;


    vector<int> matchedIndex(0);
    int rowNumsSize=0;

    findMatchinTrack( corrs,indexNumber,matchedIndex);


    if (matchedIndex.size()==0) //add new row
    {
        vector<int> newRow(rowsize,-1);
        newRow[corrs.indexIm2]=corrs.imIndices[indexNumber].imindex2;
        newRow[corrs.indexIm1]=corrs.imIndices[indexNumber].imindex1;

        trackMatrix.push_back(newRow);

    }
    else
    {
        rowNumsSize=matchedIndex.size() ;//this trick is done to make sure we dont keep readding same rows
        for (i=0; i<rowNumsSize; i++)
        {
            if (trackMatrix[matchedIndex[i]][corrs.indexIm1]==corrs.imIndices[indexNumber].imindex1 && trackMatrix[matchedIndex[i]][corrs.indexIm2]==corrs.imIndices[indexNumber].imindex2)
            {
                continue;
            }
            else if (trackMatrix[matchedIndex[i]][corrs.indexIm2]==-1 || trackMatrix[matchedIndex[i]][corrs.indexIm1]==-1)
            {
                trackMatrix[matchedIndex[i]][corrs.indexIm2]=corrs.imIndices[indexNumber].imindex2;
                trackMatrix[matchedIndex[i]][corrs.indexIm1]=corrs.imIndices[indexNumber].imindex1;

            }
            else
            {
                vector<int> newRow(trackMatrix[matchedIndex[i]]);
                newRow[corrs.indexIm2]=corrs.imIndices[indexNumber].imindex2;
                newRow[corrs.indexIm1]=corrs.imIndices[indexNumber].imindex1;



                if (rowExistsinTrack(matchedIndex, newRow)==false)
                {
                    trackMatrix.push_back(newRow);
                    matchedIndex.push_back(trackMatrix.size()-1);


                }


            }

        }
    }

}

bool FeatureTrack::rowExistsinTrack(const vector<int>& indices, const vector<int>& newRow)
{
    bool flagExists=true;

    for (int i=0; i< indices.size(); i++)
    {
        flagExists=true;
        for (int j=0; j<trackMatrix[indices[i]].size(); j++)
        {

            if (trackMatrix[indices[i]][j]!=newRow[j])
            {
                flagExists=false;
                break;
            }

        }
        if (flagExists==true) return true;


    }


    return flagExists;
}
int FeatureTrack::calcFeatureTrackScores(const vector<vector<HRCorrespond2N> >& pairCorrespondences)
{
    int i,j,k,l;
    int count=0;
    curScores.resize(trackMatrix.size(),0);
    inliersStates.resize(trackMatrix.size(),1);//all start out as inliers
    for (i=0; i<trackMatrix.size(); i++)
    {
        count=0;
        for (j=0; j<trackMatrix[i].size(); j++)
        {
            for (k=0; k<j; k++)
            {
                if (trackMatrix[i][k]!=-1 && trackMatrix[i][j]!=-1)
                {
                    for (l=0; l<pairCorrespondences[j][k].imIndices.size(); l++)
                    {
                        if (pairCorrespondences[j][k].imIndices[l].imindex1==trackMatrix[i][j] && pairCorrespondences[j][k].imIndices[l].imindex2==trackMatrix[i][k]) // zzz indices might be wrong, how do you knwo which is which, check the previous ones too
                        {
                            curScores[i]+=pairCorrespondences[j][k].imIndices[l].score;

                            count++;
                            break;
                        }
                    }
                }
            }
        }
        if (count!=0)
            curScores[i]=curScores[i]/(double)count;


    }
}
int FeatureTrack::pruneFeatureTrack()
{
    int i,j,k;
    int indexRemove=0;


    for (i=0; i<trackMatrix.size(); i++)
    {
        for (j=0; j<i; j++)
        {
            if (inliersStates[i]==1 && inliersStates[j]==1)
            {
                for (k=0; k<trackMatrix[i].size(); k++)
                {
                    if (trackMatrix[i][k]==trackMatrix[j][k] && trackMatrix[j][k]!=-1)
                    {
                        indexRemove=(curScores[i]>curScores[j])?j:i;
                        eraseTrackMatRow(indexRemove);
//                        cout<<"removing: "<<endl;
//                        displayTrackRow((curScores[i]>curScores[j])?j:i);
//                        cout<<"keeping: "<<endl;
//                        displayTrackRow((curScores[i]<=curScores[j])?j:i);

                    }
                }
            }
        }
    }


    return 0;
}

bool FeatureTrack::displayTrackRow(int row)
{

    cout<<"Track index : "<<row;
    for (int j=0; j<trackMatrix[row].size(); j++)
    {
        fs::path p( (*trackImageCollection)[j]->filename, fs::native );

        string tempfilename=fs::basename(p);
        CvPoint2D32f temploc=pointFromTrackloc(row, j);
        printf("{%s ( %d, %d) }\t",tempfilename.c_str(),(int)temploc.x, (int)temploc.y);

    }
    cout<<endl;
    return true;
}
int FeatureTrack::eraseTrackMatRow(int index)
{




    if (index<0 || index>=inliersStates.size())
    {
        cout<<"wrong index used. quitting."<<endl;
        return -1;

    }
    inliersStates[index]=0;



    return 0;
}
int  FeatureTrack::getNumTracks()
{
    return trackMatrix.size();

}
int  FeatureTrack::getNumFrames()
{
    return trackMatrix[0].size();
}

int FeatureTrack::valueTrackEntry(int row, int col)
{
    if(validTrackEntry(row,  col))
    {
        return trackMatrix[row][col];

    }
    else
    {

        return -1;
    }


}
int FeatureTrack::validTrackEntry(int row, int col)
{
    if (row>=trackMatrix.size())
    {
        return 0;

    }
    if (col<0 || col>=(*trackImageCollection).size())
    {

        return 0;
    }
    if (trackMatrix[row][col]==-1)
    {
        return 0;

    }

    return 1;


}
CvPoint2D32f FeatureTrack::pointFromTrackloc(int row, int col,int undistorted)
{

    // col is the number of the image
    //row is the number of the feature
    CvPoint2D32f temp;
    temp.x=0;
    temp.y=0;


    if (row>=trackMatrix.size())
    {
        cout<<"1-calling on a nonexistent row, track this bug"<<endl;
        printf("row num was %d and col num was %d, max row was %d and max col was %d\n",row,col,trackMatrix.size(),trackMatrix[row].size());
        return temp;

    }
    if (col<0 || col>=(*trackImageCollection).size())
    {
        cout<<"2-calling on a nonexistent row, track this bug"<<endl;
        return temp;
    }
    if (trackMatrix[row][col]==-1)
    {
        return temp;

    }

    if (trackMatrix[row][col]<0 || trackMatrix[row][col]>= (*trackImageCollection)[col]->HR2DVector.size())//this is becasue some columns will be -1
    {
        cout<<"3-calling on a nonexistent row, track this bug"<<endl;
        printf("value of track matrix was %d\n",trackMatrix[row][col]);
        printf("row num was %d and col num was %d, max row was %d and max col was %d\n",row,col,trackMatrix.size(),trackMatrix[row].size());
        return temp;
    }
    if(undistorted==0 || (*trackImageCollection)[col]->HR2DVectorUndistorted.size()!=(*trackImageCollection)[col]->HR2DVector.size())
        temp=(*trackImageCollection)[col]->HR2DVector[trackMatrix[row][col]]->location;
    else
        temp=(*trackImageCollection)[col]->HR2DVectorUndistorted[trackMatrix[row][col]]->location;

    return temp;



}
void FeatureTrack::writeTrackMatrix(string fname)
{


    fs::path tempath( TEMPDIR, fs::native );
    tempath/=fname;
    string newfname=tempath.file_string();

    int i,j,k,l;

    fstream file_op(newfname.c_str(),ios::out);

    if (curScores.size()!=trackMatrix.size())
    {
        cout<<"the sizes of the matrices are not the same, quitting"<<endl;
        return;
    }
    if (inliersStates.size()!=trackMatrix.size())
    {
        cout<<"the sizes of the matrices are not the same, quitting"<<endl;
        return;
    }


    for (i=0; i<trackMatrix.size(); i++)
    {
        file_op<<i<<"\t\t"<<setw(10)<<curScores[i]<<setw(10)<<"\t\t"<<setw(10)<<inliersStates[i]<<setw(10);
        for (j=0; j<trackMatrix[i].size(); j++)
        {

            file_op<<setw(10)<<setprecision(5)<< trackMatrix[i][j]<<"\t";
        }
        file_op<<endl;
    }


    file_op.close();

}



stats findStatsArray(const vector<double>& argarray)
{

    int i;
    vector<double> array;


//printing the content
//    for (i=0; i<argarray.size(); i++)
//    {
//        printf("element %d is %f\n",i,argarray[i]);
//    }




    for (i=0; i<argarray.size(); i++)
    {
        if (isnan(argarray[i])==false)
        {
            array.push_back(argarray[i]);
        }
    }


    int size=array.size();


    stats mystats;

    mystats.s_deviation=0;
    mystats.var=0;
    mystats.mean=0;
    mystats.max=0;

    mystats.count=0;

    mystats.count=size;

    if (size==0)
    {
        return mystats;
    }

    mystats.max=mystats.min=array[0];
    double dsize=size;

    for (i=0; i<size; i++)
    {
        mystats.mean+=array[i];

        if (array[i]>mystats.max) mystats.max=array[i];
        if (array[i]<mystats.min) mystats.min=array[i];
    }

    mystats.mean/=dsize;



    for (i=0; i<size; i++)
    {
        mystats.var+=((array[i]-mystats.mean)*(array[i]-mystats.mean));
    }

    mystats.var/=dsize;

    mystats.s_deviation=sqrt(mystats.var);

    sort(array.begin(), array.end());

    if(size%2==1)
        mystats.median=array[(size+1)/2];
    else
        mystats.median=(array[(size/2)]+array[(size/2)-1]  )/2.0F;



    return mystats;


}
