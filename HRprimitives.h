#ifndef HRPRIMITIVES_HPP_INCLUDED
#define HRPRIMITIVES_HPP_INCLUDED
#include <string>
#include <cv.h>
#include <highgui.h>
#include "HRprimitives.h"
#include <vector>
#include <boost/shared_ptr.hpp>
using namespace std;
class HRCorrespondences;

class HRCorrespond2N
{
public:
 int indexIm1;
 int indexIm2;
 string im1Name;
 string im2Name;
 vector<int> im1Index;
 vector<int> im2Index;
HRCorrespond2N();
~HRCorrespond2N();
HRCorrespond2N(int index1,int index2, string f1name, string f2name);
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
