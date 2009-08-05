/*M///////////////////////////////////////////////////////////////////////////////////////
//
//  IMPORTANT: READ BEFORE DOWNLOADING, COPYING, INSTALLING OR USING.
//
//  By downloading, copying, installing or using the software you agree to this license.
//  If you do not agree to this license, do not download, install,
//  copy or use the software.
//
//
//                        Intel License Agreement
//                For Open Source Computer Vision Library
//
// Copyright (C) 2000, Intel Corporation, all rights reserved.
// Third party copyrights are property of their respective owners.
//
// Redistribution and use in source and binary forms, with or without modification,
// are permitted provided that the following conditions are met:
//
//   * Redistribution's of source code must retain the above copyright notice,
//     this list of conditions and the following disclaimer.
//
//   * Redistribution's in binary form must reproduce the above copyright notice,
//     this list of conditions and the following disclaimer in the documentation
//     and/or other materials provided with the distribution.
//
//   * The name of Intel Corporation may not be used to endorse or promote products
//     derived from this software without specific prior written permission.
//
// This software is provided by the copyright holders and contributors "as is" and
// any express or implied warranties, including, but not limited to, the implied
// warranties of merchantability and fitness for a particular purpose are disclaimed.
// In no event shall the Intel Corporation or contributors be liable for any direct,
// indirect, incidental, special, exemplary, or consequential damages
// (including, but not limited to, procurement of substitute goods or services;
// loss of use, data, or profits; or business interruption) however caused
// and on any theory of liability, whether in contract, strict liability,
// or tort (including negligence or otherwise) arising in any way out of
// the use of this software, even if advised of the possibility of such damage.
//
//M*/


#define CV_LMEDS 4
#define CV_RANSAC 8
#ifndef _CV_MODEL_EST_H_
#define _CV_MODEL_EST_H_

#include <cv.h>
#include <highgui.h>

#ifdef __GNUC__
    #undef alloca
    #define alloca __builtin_alloca
#elif defined WIN32 || defined WIN64
    #if defined _MSC_VER || defined __BORLANDC__
        #include <malloc.h>
    #endif
#elif defined HAVE_ALLOCA_H
    #include <alloca.h>
#elif defined HAVE_ALLOCA
    #include <stdlib.h>
#elif
    #error
#endif

#define  CV_MALLOC_ALIGN    32
#define cvStackAlloc(size) cvAlignPtr( alloca((size) + CV_MALLOC_ALIGN), CV_MALLOC_ALIGN )
void cvConvertPointsHomogeneous( const CvMat* src, CvMat* dst );

CVAPI(void)  cvCompleteSymm( CvMat* matrix, int LtoR );
struct CV_EXPORTS CvLevMarqHR
{
    CvLevMarqHR();
    CvLevMarqHR( int nparams, int nerrs, CvTermCriteria criteria=
        cvTermCriteria(CV_TERMCRIT_EPS+CV_TERMCRIT_ITER,30,DBL_EPSILON),
        bool completeSymmFlag=false );
    ~CvLevMarqHR();
    void init( int nparams, int nerrs, CvTermCriteria criteria=
        cvTermCriteria(CV_TERMCRIT_EPS+CV_TERMCRIT_ITER,30,DBL_EPSILON),
        bool completeSymmFlag=false );
    bool update( const CvMat*& param, CvMat*& J, CvMat*& err );
    bool updateAlt( const CvMat*& param, CvMat*& JtJ, CvMat*& JtErr, double*& errNorm );

    void clear();
    void step();
    enum { DONE=0, STARTED=1, CALC_J=2, CHECK_ERR=3 };

    CvMat* mask;
    CvMat* prevParam;
    CvMat* param;
    CvMat* J;
    CvMat* err;
    CvMat* JtJ;
    CvMat* JtJN;
    CvMat* JtErr;
    CvMat* JtJV;
    CvMat* JtJW;
    double prevErrNorm, errNorm;
    int lambdaLg10;
    CvTermCriteria criteria;
    int state;
    int iters;
    bool completeSymmFlag;
};


int cvFindHomography2( const CvMat* objectPoints, const CvMat* imagePoints, CvMat* __H, int method, double ransacReprojThreshold, CvMat* mask );
class CvModelEstimator2
{
public:
    CvModelEstimator2(int _modelPoints, CvSize _modelSize, int _maxBasicSolutions);
    virtual ~CvModelEstimator2();

    virtual int runKernel( const CvMat* m1, const CvMat* m2, CvMat* model )=0;

    virtual bool runRANSAC( const CvMat* m1, const CvMat* m2, CvMat* model,
                            CvMat* mask, double threshold,
                            double confidence=0.99, int maxIters=1000 );
    virtual bool refine( const CvMat*, const CvMat*, CvMat*, int ) { return true; }
    virtual void setSeed( int64 seed );

protected:
    virtual void computeReprojError( const CvMat* m1, const CvMat* m2,
                                     const CvMat* model, CvMat* error ) = 0;
    virtual int findInliers( const CvMat* m1, const CvMat* m2,
                             const CvMat* model, CvMat* error,
                             CvMat* mask, double threshold );
    virtual bool getSubset( const CvMat* m1, const CvMat* m2,
                            CvMat* ms1, CvMat* ms2, int maxAttempts=1000 );
    virtual bool checkSubset( const CvMat* ms1, int count );

    CvRNG rng;
    int modelPoints;
    CvSize modelSize;
    int maxBasicSolutions;
    bool checkPartialSubsets;
};

#endif // _CV_MODEL_EST_H_

