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
