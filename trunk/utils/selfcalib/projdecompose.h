


#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include <cv.h>
#include <highgui.h>
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

void ProjectiveMatFromF( const CvMat *F, CvMat *P1,CvMat *P2);
void ProjectiveMatFromF( const CvMat *F, CvMat *P1,CvMat *P2);
