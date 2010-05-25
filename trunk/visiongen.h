#ifndef VISIONGEN_HPP_INCLUDED
#define VISIONGEN_HPP_INCLUDED


#undef complex
#undef real

#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include <cv.h>

#include <vector>
#include <iostream>
#include <fstream>
#include <iomanip>
#include <sstream>
#include "HRImage.hpp"

using namespace std;

/* Computes projection matrix decomposition */
CVAPI(void) cvDecomposeProjectionMatrixHR( const CvMat *projMatr, CvMat *calibMatr, CvMat *rotMatr, CvMat *posVect, CvMat *rotMatrX CV_DEFAULT(NULL), CvMat *rotMatrY CV_DEFAULT(NULL), CvMat *rotMatrZ CV_DEFAULT(NULL), CvPoint3D64f *eulerAngles CV_DEFAULT(NULL));
int findProjfromcompon(HRImage& img);
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
int findProjfromcompon(CvMat* P,CvMat* R,CvMat* t,CvMat* K);
double cvTriangulatePointsNframs(int numframes, vector<CvMat*>& projMatrs,vector<CvPoint2D32f>& projPoints,CvPoint3D32f& spPoint);
double findDepth(CvMat* P,CvPoint3D32f S);
void  draw_cross(CvPoint2D32f center, CvScalar color, int d,IplImage* img );
void showMatchAcross(vector<string>& fnames,const  vector< vector< CvPoint2D32f> >& projPoints);
CvPoint2D32f project3DPoint(CvMat* P, CvPoint3D32f S);
double projectionErrorSquared(CvMat* P, CvPoint3D32f s_pt,CvPoint2D32f im_pt);
#endif
