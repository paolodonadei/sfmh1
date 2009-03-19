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
int matchTWOImagesNearestNeighbour(const HRImage& im1,const HRImage& im2,HRCorrespond2N& hr_correspond,bool outputimage=false)


#endif // MATCHING_HPP_INCLUDED
