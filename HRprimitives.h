#ifndef HRPRIMITIVES_HPP_INCLUDED
#define HRPRIMITIVES_HPP_INCLUDED
#include <string>
#include <cv.h>
#include <highgui.h>
#include <cxcore.h>
#include <vector>
#include <boost/shared_ptr.hpp>
#include "HRImage.hpp"
#include "general.h"


class HRImage;

using namespace std;
enum MotionType { FUNDAMENTAL,HOMOGRAPHY, AFFINE };

struct matchIndex
{
    int imindex1;
    int imindex2;
    double score; //higher is better
    double motionError;
    int inlier; //we dont delete matches, we will just tag them outliers, so 1 would mean inlier
} ;


class HRFeature
{
public:
    HRFeature();
    ~HRFeature();
    HRFeature(int x,int y);
    CvPoint2D32f location;
    vector<double> descriptor;
    double pvi;
    bool valid;
    float scale, ori;           /* Scale and orientation for sift (range [-PI,PI]) */

};

typedef boost::shared_ptr<HRFeature> HRPointFeatures;

class MotionGeometry
{
public:
    MotionGeometry();
    ~MotionGeometry();

    string filenameH;
    string filenameF;

    double calculateError();


    double getMotionElement(int i,int j,MotionType mtype=FUNDAMENTAL) const;
    int findMotionModels(const vector<HRPointFeatures>& hr1vec,const vector<HRPointFeatures>&  hr2vec,  vector<matchIndex>& indices ,MotionType mtype=FUNDAMENTAL);
    int findFMatrix(const vector<HRPointFeatures>& hr1vec,const vector<HRPointFeatures>&  hr2vec,  vector<matchIndex>& indices );
    int findHMatrix(const vector<HRPointFeatures>& hr1vec,const vector<HRPointFeatures>& hr2vec,  vector<matchIndex>& indices );
    double computeReprojErrorF( const CvMat* _m1, const CvMat* _m2, const CvMat* model, CvMat* _err , const CvMat* status );
    double computeReprojErrorH( const CvMat* m1, const CvMat* m2, const CvMat* model, CvMat* _err , double thresh,int* numinliers);
    double computeReprojErrorFfromEpipolars( const CvMat* _m1, const CvMat* _m2, const CvMat* model, CvMat* _err , const CvMat* status );
    void writeMatrix(ostream &stream,MotionType mtype=FUNDAMENTAL);

    MotionGeometry(const MotionGeometry & rec);
    MotionGeometry & operator=(const MotionGeometry & rhs) throw();
    CvMat* MotionModel_F;
    CvMat* MotionModel_H;
    double motionError_F;//in pixels
    int numOutlier_F;
    int valid;
    int numInliers_F;

    double motionError_H;//in pixels
    int numOutlier_H;

    int numInliers_H;

};

class HRCorrespond2N
{
public:
    int findGeomtry();
    void writeIndices();
    friend ostream &operator<<(ostream &stream, const HRCorrespond2N& ob);
    HRCorrespond2N();
    int removeOutliers(const CvMat* tstatus);
    MotionGeometry motion;
    ~HRCorrespond2N();
    void WriteMatches();
    void WriteMotion();

    int indexIm1;
    int indexIm2;
    HRImage* hr1ptr;
    HRImage* hr2ptr;
    vector<matchIndex> imIndices;

    int readMatches(string filename);
    HRCorrespond2N(const HRCorrespond2N& rec);
    HRCorrespond2N& operator=(const HRCorrespond2N & rhs) throw();


};


#endif //HRPRIMITIVES_HPP_INCLUDED
