#ifndef HRIMAGE_HPP_INCLUDED
#define HRIMAGE_HPP_INCLUDED


#include <cv.h>
#include <highgui.h>
#include "HRprimitives.h"
#include <vector>
#include <boost/shared_ptr.hpp>
#include "HRprimitives.h"


using namespace std;

class HRFeature;
typedef boost::shared_ptr<HRFeature> HRPointFeatures;


class HRImage
{

public:
    HRImage();
    HRImage(string fname);
    HRImage(char* fname);
    HRImage(int pheight, int pwidth,int initial=0);
    HRImage(const HRImage &img);
    ~HRImage();
    int openim(string fname);
    int openim(int pheight,int pwidth,int initial=0);
    HRImage& operator= (const HRImage& param);
    void scale(int pscale);
    void close();
    void divideAll(int val);
    int height,width,step,channels;
    uchar *data;
    int flag_valid;
    string filename;
    IplImage* cv_img;
    int writeImage(string fname);
    void  setAll(int val);
    int  getPixel(int y,int x) ;
    void setPixel(int y,int x,int pixValue);
    int displayImage();
    int displayImageFeatures();
    unsigned char* get1d_array();
//copies this technique from http://www.cs.iit.edu/~agam/cs512/lect-notes/opencv-intro/opencv-intro.html#SECTION00053000000000000000
    inline unsigned char* operator[](const int rowIndx)
    {
        return ((unsigned char *)(cv_img->imageData + rowIndx*cv_img->widthStep));
    }
    void updateImageInfo();
    void incrementPixel(int y,int x);
    string pgmfilename;//this is used for some external programs that need a pgm format, i.e. sift
    string siftkeyfilename;
    vector<HRPointFeatures> HR2DVector;
private:

    enum {BLACK=0,WHITE=255};



};


//i need to create a function that does this: void match(hrimage 1, hirimage 2, hirimage final, method, correspondences)
///////left off here i have to painin the two images, motion vectors with an alpha field, and be able to save this shape with the alpha field into eps
class HRCORRImage: public HRImage
{
    IplImage* cv_im1;
    IplImage* cv_im2;
    double whitespace_ratio;
    CvPoint im1_top_left;
    CvPoint im2_top_left;

public:
    // HRCORRImage(IplImage* p_cv_im1,IplImage* p_cv_im2,HRCorrespondences *p_img_corrs);
    HRCORRImage();
    HRCORRImage(IplImage* p_cv_im1,IplImage* p_cv_im2);
    ~HRCORRImage();
    // int init(IplImage* p_cv_im1,IplImage* p_cv_im2,HRCorrespondences *p_img_corrs);
    int init(IplImage* p_cv_im1,IplImage* p_cv_im2);
    int init(HRImage* p_hr_im1,HRImage* p_hr_im2);
    void close();
};

typedef boost::shared_ptr<HRImage> HRImagePtr;
class HRImageSet
{

    vector<HRImagePtr> imageCollection;
    string dirName;
    int numImages;
public:
    int featureDetectSift();
    HRImageSet();
    HRImageSet(string directoryName);
    ~HRImageSet();
    int open(string directoryName);
    void showOneByOne();
    void showOneByOneFeature();

vector< vector<int> > trackMatrix;
vector<vector<HRCorrespond2N> > correspondencesPairWise;
int createFeatureTrackMatrix();

    int exhaustiveSIFTMatching();
    bool TrackMatrixContains(vector< vector<int> >& tMatrix);
int processPairMatchinTrack(vector< vector<int> >& tMatrix, HRCorrespond2N& corrs, int indexNumber);
int pruneFeatureTrack(vector< vector<int> >& tMatrix);
int calcFeatureTrackScores(vector< vector<int> >& tMatrix);
    vector<double> curScores;
    int eraseTrackMatRow(int index);
};
#endif // HRIMAGE_HPP_INCLUDED
