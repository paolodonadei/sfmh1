#ifndef NONLINSCLVM_HPP_INCLUDED
#define NONLINSCLVM_HPP_INCLUDED

#include <vector>

#include <cxcore.h>
extern "C" {
#include "f2c.h"
#include "clapack.h"
#include "blaswrap.h"
}


using namespace std;


void errnonLinFunctionSelfCalib(double *p, double *hx, int m, int n, void *adata);

int HRSelfCalibtwoFrameNonlin(vector< vector<CvMat*> > const &FV,  vector<CvMat*>  &KV ,int width, int height,vector<double>& confs);
double findSVDerror(CvMat* k1,CvMat* k2,CvMat* F,vector<CvMat* > *tempMat);

struct SCinputs
{
   int width;
   int height;
   vector< vector<CvMat*> > const *funds;
    vector<CvMat* > *intrin;  //this is temporary storage
    vector<CvMat* > *tempMat;  //more temporary storage
    int numFrames;
    int numParams;
     int numunknownframes;

};

#endif
