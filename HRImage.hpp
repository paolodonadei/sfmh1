#ifndef HRIMAGE_HPP_INCLUDED
#define HRIMAGE_HPP_INCLUDED

#include "general.h"
#include <cv.h>




#include <vector>
#include <boost/shared_ptr.hpp>
#include "HRprimitives.h"


class HRFeature;
class HRCorrespond2N;
class FeatureTrack;

class HRImage;

typedef boost::shared_ptr<HRFeature> HRPointFeatures;
typedef boost::shared_ptr<HRImage> HRImagePtr;

struct stats
{
    double s_deviation;
    double var;
    double mean;
    double max;
    double min;
    double count;
    double accuracy;
    double median;
};
stats findStatsArray(const vector<double>& argarray);


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
    int writeImageFeatures();
    int writeFeatures();
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
    CvMat*  intrinsicMatrix;
    CvMat*  projectionMatrix;
    double confidenceSelfCalib;
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

class FeatureTrack
{
public:
    const vector<HRImagePtr>* trackImageCollection;
    FeatureTrack();
    CvPoint2D32f pointFromTrackloc(int row, int col);
    int validTrackEntry(int row, int col);
 int valueTrackEntry(int row, int col);

    int processPairMatchinTrack( HRCorrespond2N& corrs, int indexNumber, int rowsize);
    int findMatchinTrack( HRCorrespond2N& corrs, int indexNumber, vector<int>& matchedIndices);
    int pruneFeatureTrack();
    int calcFeatureTrackScores(const vector<vector<HRCorrespond2N> >& pairCorrespondences);
    vector<double> curScores;
    vector<int> inliersStates;//this is different from the pairwise inlier variable, this is for when we remove a whole track
    int eraseTrackMatRow(int index);
    void writeTrackMatrix(string fname);
    bool rowExistsinTrack(const vector<int>& indices, const vector<int>& newRow);
    int drawImageTrackMatches(const vector<HRImagePtr>& imCollection,string filname);
    bool displayTrackRow(int row);
    int getNumTracks();
    int getNumFrames();
    private:
     vector< vector<int> > trackMatrix;

};

class HRImageSet
{
public:
    vector<HRImagePtr> imageCollection;
    string dirName;
    int numImages;

    string dirStemName;
        string outdirStemName;

    int featureDetectSift();
    HRImageSet();
    HRImageSet(string directoryName,string TEMPdirectoryName);

    ~HRImageSet();
    int open(string directoryName, string TEMPdirectoryName);
    void showOneByOne();
    void showOneByOneFeature();
    vector<vector<HRCorrespond2N> > correspondencesPairWise;
    int exhaustiveSIFTMatching();
      int multipleViewEstimate();
      int writeMotions();
    FeatureTrack myTracks;
    string temporaryDir;

    int createFeatureTrackMatrix();
    void showOneByOneFeatureMotions();
    int SelfCalibrate();
    vector<double> confid;
    void drawallMatches();
    void findEssentialMatrices();

};


#endif // HRIMAGE_HPP_INCLUDED
