#include <time.h>
#include <fstream>
#include <iomanip>
#include <iostream>
#include "boost/filesystem.hpp"   // includes all needed Boost.Filesystem declarations
#include "boost/filesystem/operations.hpp"
#include "boost/filesystem/path.hpp"

#include "HRImage.hpp"
#include "general.h"
#include "pgmutils.hpp"
#include "sift.h"
#include "matching.hpp"
#define DEBUGLVL 0
#define TEMPDIR "tempdir"


CvScalar colors[3]={cvScalar(255,0,0), cvScalar(0,255,0), cvScalar(0,0,255)};


namespace fs = boost::filesystem;
using namespace std;

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



    filename="memimage.pgm";
    flag_valid=1;
    return 1;

}
HRImage::HRImage(const HRImage &img)
{
    siftkeyfilename="";
    pgmfilename="";
    flag_valid=img.flag_valid;

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
    for (int i=0;i<height;i++)
    {
        for (int j=0;j<width;j++)
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
    for (int i=0;i<height;i++)
    {
        for (int j=0;j<width;j++)
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
    for (int i=0;i<height;i++)
    {
        for (int j=0;j<width;j++)
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

    for (i=im1_top_left.y;i<(cv_im1->height+im1_top_left.y);i++)
    {
        for (j=im1_top_left.x;j<(cv_im1->width+im1_top_left.x);j++)
        {
            tdata_main[i*step_main+j*channels_main+0] = tdata_im1[(i-im1_top_left.y)*step_im1+(j-im1_top_left.x)*channels_im1+0];
            tdata_main[i*step_main+j*channels_main+1] = tdata_im1[(i-im1_top_left.y)*step_im1+(j-im1_top_left.x)*channels_im1+0];
            tdata_main[i*step_main+j*channels_main+2] = tdata_im1[(i-im1_top_left.y)*step_im1+(j-im1_top_left.x)*channels_im1+0];
            tdata_main[i*step_main+j*channels_main+3] = 150;

        }
    }



    for (i=im2_top_left.y;i<(cv_im2->height+im2_top_left.y);i++)
    {
        for (j=im2_top_left.x;j<(cv_im2->width+im2_top_left.x);j++)
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
}
HRImageSet::HRImageSet(string directoryName)
{
    open(directoryName);
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

int  HRImageSet::open(string directoryName)
{
    dirName=directoryName;
    numImages=0;




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
                            fs::extension(dir_itr->path())==".jpg" || fs::extension(dir_itr->path())==".tiff")
                    {
                        ++file_count;
                        cout << "currently loading file: "<<dir_itr->path().string()<<endl ;

                        HRImagePtr hr_iptr( new HRImage( dir_itr->path().string()) );
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
        ++img_iterator;
    }




    return 0;
}
/** @brief exhaustiveSIFTMatching
*
* @todo: document this function
*/
int HRImageSet::exhaustiveSIFTMatching()
{
    int i,j;
    correspondencesPairWise.resize( imageCollection.size(), vector<HRCorrespond2N> ( imageCollection.size() ) );



    for (i=0;i<imageCollection.size();i++)
    {
        for (j=0;j<i;j++)
        {
            correspondencesPairWise[i][j].indexIm1=i;
            correspondencesPairWise[i][j].indexIm2=j;

            correspondencesPairWise[i][j].hr1ptr=&(*imageCollection[i]);
            correspondencesPairWise[i][j].hr2ptr=&(*imageCollection[j]);

            int numf_found= matchTWOImagesNearestNeighbour( (*imageCollection[i]), (*imageCollection[j]),correspondencesPairWise[i][j]);
            printf("between image %d having %d features and image %d with %d features, we found %d correspondences\n",i,(*imageCollection[i]).HR2DVector.size()
                   ,j,(*imageCollection[j]).HR2DVector.size(),numf_found);

            correspondencesPairWise[i][j].findGeomtry();//remove outliers and find motion model
correspondencesPairWise[i][j].WriteMatches();
correspondencesPairWise[i][j].WriteMotion();


            drawMatchesPair((*imageCollection[i]), (*imageCollection[j]),correspondencesPairWise[i][j]);



        }
    }


}


int HRImageSet::createFeatureTrackMatrix()
{


    int i, j, k,q;

    for (i=0;i<imageCollection.size();i++)
    {
        for (j=0;j<i;j++)
        {
            for (k=0;k<correspondencesPairWise[i][j].imIndices.size();k++)
            {
                //cout<<"i is "<<i <<" and j is "<<j<<endl;
               myTracks.processPairMatchinTrack( correspondencesPairWise[i][j], k,imageCollection.size());

            }

        }
    }

    cout<<"number of feature tracks is ---------- "<<myTracks.trackMatrix.size()<<endl;
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


    for (i=0;i<imCollection.size();i++)
    {

        IplImage* imgTemptemp=concatImagesVertical(imgTemp,(*imCollection[i]).cv_img);

        cvReleaseImage( &imgTemp );
        imgTemp=imgTemptemp;
    }

    if (SINGLEMATCHPRINT==1)
    {
        imgTempcopy=cvCloneImage(imgTemp);

    }

    int matchCountr=0;

    for (i=0;i<trackMatrix.size(); i++)
    {
        if (SINGLEMATCHPRINT==1)
        {

            imgTemptempcopy=cvCloneImage(imgTempcopy);
        }
        matchCountr=0;
        for (j=1;j<trackMatrix[i].size();j++)
        {

            if (trackMatrix[i][j]!=-1 && trackMatrix[i][j-1]!=-1)
            {
                matchCountr++;
                x0=(*imCollection[j-1]).HR2DVector[trackMatrix[i][j-1]]->location.x;
                y0=(*imCollection[j-1]).HR2DVector[trackMatrix[i][j-1]]->location.y+ ((j-1)*heightImage);
                x1=(*imCollection[j]).HR2DVector[trackMatrix[i][j]]->location.x;
                y1=(*imCollection[j]).HR2DVector[trackMatrix[i][j]]->location.y+ ((j)*heightImage);

                cvLine(imgTemp, cvPoint(x0,y0),cvPoint(x1,y1), colors[j%3], 1);

                //print correspondences 1 to 1
                if (SINGLEMATCHPRINT==1)
                {
                    cvLine(imgTemptempcopy, cvPoint(x0,y0),cvPoint(x1,y1), colors[j%3], 1);

                }
            }
        }
        if (SINGLEMATCHPRINT==1)
        {
            string tslash="/";
            string fname=TEMPDIR+tslash+string(filname+"track_i"+stringify(i)+"_d"+stringify(matchCountr)+".png");

            if (!cvSaveImage(fname.c_str(),imgTemptempcopy)) printf("Could not save: %s\n",fname.c_str());
            cvReleaseImage( &imgTemptempcopy );
        }

    }





    string tslash="/";
    string fname=TEMPDIR+tslash+filname;

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
    int i,j;
    for (i=0;i<trackMatrix.size(); i++)
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


    int numTimes=0;
    int i,j;
    int flag=0; //1 means add new row, 2 means
    int flagFound=0;
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
        for (i=0;i<rowNumsSize;i++)
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

    for (int i=0;i< indices.size();i++)
    {
        flagExists=true;
        for (int j=0;j<trackMatrix[indices[i]].size();j++)
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

    for (i=0;i<trackMatrix.size(); i++)
    {
        count=0;
        for (j=0;j<trackMatrix[i].size();j++)
        {
            for (k=0;k<j;k++)
            {
                if (trackMatrix[i][k]!=-1 && trackMatrix[i][j]!=-1)
                {
                    for (l=0;l<pairCorrespondences[j][k].imIndices.size();l++)
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
    int i,j,k,l;


    for (i=0;i<trackMatrix.size(); i++)
    {
        for (j=0;j<i; j++)
        {
            for (k=0;k<trackMatrix[i].size();k++)
            {
                if (trackMatrix[i][k]==trackMatrix[j][k] && trackMatrix[j][k]!=-1)
                {
                    if ( curScores[i]>curScores[j])
                    {
                        eraseTrackMatRow(j);
                        break;
                    }
                    else
                    {
                        eraseTrackMatRow(i);
                        break;
                    }

                }
            }
        }
    }


    return 0;
}

int FeatureTrack::eraseTrackMatRow(int index)
{
    int i,j,k,l;



    for (j=0;j<trackMatrix[index].size();j++)
    {
        trackMatrix[index][j]=-1;

    }


    curScores[index]=0;

    return 0;
}

void FeatureTrack::writeTrackMatrix(string fname)
{

    string tslash="/";
    string newfname=TEMPDIR+tslash+fname;


    int i,j,k,l;

    fstream file_op(newfname.c_str(),ios::out);

    if (curScores.size()!=trackMatrix.size())
    {
        cout<<"the sizes of the matrices are not the same, quitting"<<endl;
        return;
    }


    for (i=0;i<trackMatrix.size(); i++)
    {
        file_op<<i<<"\t\t"<<setw(10)<<curScores[i]<<setw(10);
        for (j=0;j<trackMatrix[i].size();j++)
        {

            file_op<<setw(10)<<setprecision(5)<< trackMatrix[i][j]<<"\t";
        }
        file_op<<endl;
    }


    file_op.close();

}


