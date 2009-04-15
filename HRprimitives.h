#ifndef HRPRIMITIVES_HPP_INCLUDED
#define HRPRIMITIVES_HPP_INCLUDED
#include <string>
#include <cv.h>
#include <highgui.h>
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



class MotionGeometry
{
    public:
    MotionGeometry();
    ~MotionGeometry();

string filename;

    void writeMotionMatrix();
    double calculateError();


    double getMotionElement(int i,int j) const;
    int findMotionModel(const HRImage* hr1,const HRImage* hr2,  vector<matchIndex>& indices ,MotionType mtype=FUNDAMENTAL);
    int findFMatrix(const HRImage* hr1,const HRImage* hr2,  vector<matchIndex>& indices );

double computeReprojErrorF( const CvMat* _m1, const CvMat* _m2, const CvMat* model, CvMat* _err , const CvMat* status );
double computeReprojErrorFfromEpipolars( const CvMat* _m1, const CvMat* _m2, const CvMat* model, CvMat* _err , const CvMat* status );
friend ostream &operator<<(ostream &stream,  const MotionGeometry& ob);

MotionGeometry(const MotionGeometry & rec);
MotionGeometry & operator=(const MotionGeometry & rhs) throw();
    CvMat* MotionModel;
 double motionError;//in pixels
    int numOutlier;
     int valid;
     int numInliers;

};

class HRCorrespond2N
{
public:
   int findGeomtry(MotionType mtype=FUNDAMENTAL);
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


#endif //HRPRIMITIVES_HPP_INCLUDED