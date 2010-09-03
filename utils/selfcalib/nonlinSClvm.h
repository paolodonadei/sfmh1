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

struct intrinsicFamily
{
    vector<double> fx;
    vector<double> fy;
    vector<double> s;
    vector<double> ux;
    vector<double> uy;
    vector<double> score;

    vector<int> index_left;
    vector<int> index_right;
    vector<int> F_index; //this is assigning an integer id to every single f matrix
};

struct SCinputs
{
    int width;
    int height;
    vector< vector<CvMat*> > const *funds;
    vector<CvMat* > *intrin;  //this is temporary storage
    vector<CvMat* > *tempMat;  //more temporary storage
    vector<double>* confidences;  //confidence in a single frame
     vector<vector<double > >* Weights;  //weight or confidence in a single fundamental matrix
    int numFrames;
    int numParams;
    int numunknownframes;

};
double findSVDerrorSequence(vector< vector<CvMat*> > const &FV,  vector<CvMat*>  &KV );
int cvCholesky(CvMat* inM,CvMat* outM);
int findClusWinner2(intrinsicFamily& datafamily , CvMat* clusterlabels,int numClusts,CvMat* clusterscenters , double* compact,int numframes);
double findDistanceClosestVectorPt(intrinsicFamily myvec ,intrinsicFamily val,int numValus);
void errnonLinFunctionSelfCalibmestimator(double *p, double *hx, int m, int n, void *adata);
double  HRSelfCalibtwoFrameNonlin(vector< vector<CvMat*> > const &FV,  vector<CvMat*>  &KV ,int width, int height,vector<double>& confs);
double findSVDerror(CvMat* k1,CvMat* k2,CvMat* F,vector<CvMat* > *tempMat);
double HRSelfCalibtwoFrameNonlinInitGuess(vector< vector<CvMat*> > const &FV,  vector<CvMat*>  &KV ,int width, int height,vector<double>& confs,vector< vector<double> >& Weights,void (*func)(double *p, double *hx, int m, int n, void *adata)=errnonLinFunctionSelfCalibmestimator,double* covtr=NULL);
double HRSelfCalibtwoFrameNonlinMULTIStep(vector< vector<CvMat*> > const &FV,  vector<CvMat*>  &KV ,int width, int height,vector<double>& confs);
double HRSelfCalibtwoFrameNonlinMULTIStep2(vector< vector<CvMat*> > const &FV,  vector<CvMat*>  &KV ,int width, int height,vector<double>& confs);
void DrawEnergyf(vector< vector<CvMat*> > const &FV,  vector<CvMat*>  &KV ,int width, int height,vector<double>& confs);
void transferIntrinsicBufferToMatrices(SCinputs* mySCinputs,double *p);
double HRSelfCalibtwoFrameNonlinMEstimator(vector< vector<CvMat*> > const &FV,  vector<CvMat*>  &KV ,int width, int height,vector<double>& confs);
double HRSelfCalibtwoFrameNonlinCluster(vector< vector<CvMat*> > const &FV,  vector<CvMat*>  &KV ,int width, int height,vector<double>& confs);
int findClusWinner(CvMat* myclusterlabels,int numClusts);
#endif
