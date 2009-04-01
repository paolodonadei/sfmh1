#ifndef MATCHING_HPP_INCLUDED
#define MATCHING_HPP_INCLUDED

#include <stdlib.h>
#include <math.h>
#include <assert.h>
#include <stdio.h>
#include <string.h>
#include <string>
#include <iostream>
#include <cv.h>
#include <highgui.h>
#include "HRImage.hpp"
using namespace std;
int findSIFTfeatures( HRImage& image);
int readSIFTfile(vector<HRPointFeatures>& siftVector,string filename);
int matchTWOImagesNearestNeighbour( HRImage& im1, HRImage& im2,HRCorrespond2N& hr_correspond);
int CheckForMatch(const HRPointFeatures& key, const vector<HRPointFeatures>& HR2Dfeatures, int& winKey, double& score);
int DistSquared(const HRPointFeatures& k1, const HRPointFeatures& k2);
int Dist(const HRPointFeatures& k1, const HRPointFeatures& k2);
int drawMatchesPair(HRImage& im1, HRImage& im2,HRCorrespond2N& hr_correspond);
int drawImageTrackMatches(const vector< vector<int> >& tMatrix,const vector<HRImagePtr>& imCollection,string filname);
#endif // MATCHING_HPP_INCLUDED
