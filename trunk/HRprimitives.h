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
typedef boost::shared_ptr<double> HRFeatureDescriptor;


class HRFeature
{
    HRFeature();
    ~HRFeature();
    HRFeature(int x,int y);
    CvPoint2D32f location;
    vector<HRFeatureDescriptor> descriptor;
    double pvi;
    bool valid;
};


#endif //HRPRIMITIVES_HPP_INCLUDED
