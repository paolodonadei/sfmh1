#ifndef HRSTRUCTURE_HPP_INCLUDED
#define HRSTRUCTURE_HPP_INCLUDED

#include <vector>
#include <cxcore.h>
#include <cv.h>
#include <iostream>
#include "HRImage.hpp"

using namespace std;

struct pose
{
CvMat* Rm;
CvMat* tm;
int frame1;
int frame2;
};

class HRStructure
{
public:
vector<pose> poses;
    HRStructure(HRImageSet* pimSet );
    HRStructure();
    ~HRStructure();
    vector<CvPoint3D32f> strucPoints;
    int initializeKeyFrames(int frame1, int frame2);
    double bundleAdjust();
    int addFrame(int framenum);
    void run();
    vector<int> sfmSequence;
HRImageSet* imSet;
int numImages;
};


#endif
