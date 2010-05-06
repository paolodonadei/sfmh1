#ifndef HRSTRUCTURE_HPP_INCLUDED
#define HRSTRUCTURE_HPP_INCLUDED

#include <vector>
#include <cxcore.h>
#include <cv.h>
#include <iostream>
#include "HRImage.hpp"

using namespace std;


class HRStructure
{
public:

    HRStructure(HRImageSet* pimSet,string pdir );
    HRStructure();
    ~HRStructure();

    int initializeKeyFrames(int frame1, int frame2);
    double bundleAdjust();
    int addFrame(int framenum);
    void run();
    vector<int> sfmSequence;
    vector<CvPoint3D32f> structure;
    vector<double> structureErrors;
    vector<int> structureValid;
    void DLTUpdateStructure();
    void writeStructure(string fn);
    HRImageSet* imSet;
    string tempdir;
    int numImages;
    int initializeKeyFrames2(int frame1, int frame2);
     int printSBAstyleData();
     int sba_driver_interface();
};


#endif