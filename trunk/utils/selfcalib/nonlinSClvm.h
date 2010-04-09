#include <vector>

#include <cxcore.h>
extern "C" {
#include "f2c.h"
#include "clapack.h"
#include "blaswrap.h"
}


using namespace std;


void errnonLinFunctionSelfCalib(double *p, double *hx, int m, int n, void *adata);

int HRSelfCalibtwoFrameNonlin(vector< vector<CvMat*> > const &FV,  vector<CvMat*>  &KV ,int width, int height);
int CvMat2D_to_buff(vector< vector<CvMat*> >const &Mat,double** pbuffer);
int buff_to_CvMat2D(double* pbuffer,vector< vector<CvMat*> >const &Mat);
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
