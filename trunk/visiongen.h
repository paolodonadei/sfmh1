

#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include <cv.h>

#include <vector>
#include <iostream>
#include <fstream>
#include <iomanip>
#include <sstream>

#ifdef CV_NO_FUNC_NAMES
    #define CV_FUNCNAME( Name )
    #define cvFuncName ""
#else
    #define CV_FUNCNAME( Name )  \
    static char cvFuncName[] = Name
#endif

/* Computes projection matrix decomposition */
CVAPI(void) cvDecomposeProjectionMatrixHR( const CvMat *projMatr, CvMat *calibMatr, CvMat *rotMatr, CvMat *posVect, CvMat *rotMatrX CV_DEFAULT(NULL), CvMat *rotMatrY CV_DEFAULT(NULL), CvMat *rotMatrZ CV_DEFAULT(NULL), CvPoint3D64f *eulerAngles CV_DEFAULT(NULL));

int ProjectiveMatFromF( const CvMat *F, CvMat *P1,CvMat *P2);
int ProjectiveMatFromF( const CvMat *F, CvMat *P1,CvMat *P2,CvMat* v,double scale );
int findEpipoles(const CvMat *F, CvMat *e1,CvMat *e2);
bool checkMatrixOK(const CvMat *in,int h,int w);
int skewSymmetrify(const CvMat *in, CvMat *out);
int FfromProjectionMatrices(const CvMat* P1,const CvMat* P2, CvMat* F);
int findCameraCenter(const CvMat* P, CvMat* C);
int findPseudoInverse_3x4(const CvMat* in,CvMat* out);
void scaleMatrix(CvMat* in,double s);
int normalizeMatrix( CvMat* in);
