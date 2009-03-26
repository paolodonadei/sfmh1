#ifndef HRPRIMITIVES_HPP_INCLUDED
#define HRPRIMITIVES_HPP_INCLUDED
#include <string>
#include <cv.h>
#include <highgui.h>
#include <vector>
#include <boost/shared_ptr.hpp>
#include "HRprimitives.h"
#include "HRImage.hpp"

class HRImage;

using namespace std;


struct matchIndex
{
    int imindex1;
    int imindex2;
} ;

class HRCorrespond2N
{
public:
    int indexIm1;
    int indexIm2;
    HRImage* hr1ptr;
    HRImage* hr2ptr;
    vector<matchIndex> imIndices;
    friend ostream &operator<<(ostream &stream, HRCorrespond2N ob);
    HRCorrespond2N();
    ~HRCorrespond2N();

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
