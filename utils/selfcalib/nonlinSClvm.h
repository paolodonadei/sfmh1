#include <vector>
#include <cxcore.h>
extern "C" {
#include "f2c.h"
#include "clapack.h"
#include "blaswrap.h"
}


using namespace std;


void errnonLinFunctionSelfCalib(double *p, double *hx, int m, int n, void *adata);
int HRSelfCalibtwoFrameNonlin(vector< vector<CvMat*> >const &FV,  vector<CvMat*> const &KV ,int width, int height);
