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
     int printSBAstyleData(string camFname, string ptFname);
     int sba_driver_interface();
     double findReconstructionError();
     int decomposeEssential(CvMat* E, CvPoint2D32f p1,CvPoint2D32f p2,CvMat* K1, CvMat* K2, CvMat* R,CvMat* t);
};


#endif
