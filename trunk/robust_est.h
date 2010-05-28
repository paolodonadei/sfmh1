#ifndef ROBUST_EST_H
#define ROBUST_EST_H

#include <vector>
#include <cxcore.h>
#include <cv.h>
#include <iostream>

using namespace std;

int ROBUST_EST(const CvMat* data,vector<double>& aprioris,int  (*fitFunctionPtr)(const CvMat* ,vector<CvMat*> models ),
               double (*distanceFunctionPTR)(const CvMat* , const vector<CvMat*> ,vector<double>&), bool (*degenFunctionPTR)(CvMat*),
               int s, double t, vector<bool>& inliers,CvMat* model,int maxTrials, int maxDataTrials);



int findTrialCount(int score,int size,int s,double p);


int countTrues(const vector<bool>& vec);

int extractCopySamples(vector<int>& rndSamples,CvMat* rndSampleData,const CvMat* data);
int  drawRandSampleMonteCarlo(vector<int>& rndSample, const vector<double>& pvis,bool MCS);


#endif
