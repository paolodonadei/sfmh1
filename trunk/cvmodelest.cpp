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

#include <cv.h>
#include <highgui.h>
#include "_cvmodelest.h"
#include <stdio.h>

////////////////////////////////opencv stuff
class CvFMEstimator : public CvModelEstimator2
{
public:
    CvFMEstimator( int _modelPoints );

    virtual int runKernel( const CvMat* m1, const CvMat* m2, CvMat* model );
    virtual int run7Point( const CvMat* m1, const CvMat* m2, CvMat* model );
    virtual int run8Point( const CvMat* m1, const CvMat* m2, CvMat* model );
protected:
    virtual void computeReprojError( const CvMat* m1, const CvMat* m2,
                                     const CvMat* model, CvMat* error );
};




template<typename T> int icvCompressPoints( T* ptr, const uchar* mask, int mstep, int count )
{
    int i, j;
    for( i = j = 0; i < count; i++ )
        if( mask[i*mstep] )
        {
            if( i > j )
                ptr[j] = ptr[i];
            j++;
        }
    return j;
}

int cvFindFundamentalMat( const CvMat* points1, const CvMat* points2,
                      CvMat* fmatrix, int method,
                      double param1, double param2, CvMat* mask )
{
    int result = 0;
    CvMat *m1 = 0, *m2 = 0, *tempMask = 0;

    CV_FUNCNAME( "cvFindFundamentalMat" );

    __BEGIN__;

    double F[3*9];
    CvMat _F3x3 = cvMat( 3, 3, CV_64FC1, F ), _F9x3 = cvMat( 9, 3, CV_64FC1, F );
    int count;

    CV_ASSERT( CV_IS_MAT(points1) && CV_IS_MAT(points2) && CV_ARE_SIZES_EQ(points1, points2) );
    CV_ASSERT( CV_IS_MAT(fmatrix) && fmatrix->cols == 3 &&
        (fmatrix->rows == 3 || (fmatrix->rows == 9 && method == CV_FM_7POINT)) );

    count = MAX(points1->cols, points1->rows);
    if( count < 7 )
        EXIT;

    m1 = cvCreateMat( 1, count, CV_64FC2 );
    cvConvertPointsHomogenious( points1, m1 );

    m2 = cvCreateMat( 1, count, CV_64FC2 );
    cvConvertPointsHomogenious( points2, m2 );

    if( mask )
    {
        CV_ASSERT( CV_IS_MASK_ARR(mask) && CV_IS_MAT_CONT(mask->type) &&
            (mask->rows == 1 || mask->cols == 1) &&
            mask->rows*mask->cols == count );
        tempMask = mask;
    }
    else if( count > 8 )
        tempMask = cvCreateMat( 1, count, CV_8U );
    if( tempMask )
        cvSet( tempMask, cvScalarAll(1.) );

    {
    CvFMEstimator estimator( MIN(count, (method & 3) == CV_FM_7POINT ? 7 : 8) );
    if( count == 7 )
        result = estimator.run7Point(m1, m2, &_F9x3);
    else if( count == 8 || method == CV_FM_8POINT )
        result = estimator.run8Point(m1, m2, &_F3x3);
    else if( count > 8 )
    {
        if( param1 <= 0 )
            param1 = 3;
        if( param2 < DBL_EPSILON || param2 > 1 - DBL_EPSILON )
            param2 = 0.99;

        if( (method & ~3) == CV_RANSAC )
            result = estimator.runRANSAC(m1, m2, &_F3x3, tempMask, param1, param2 );
        else
            printf("LMDS is not implemented \n");
        if( result <= 0 )
            EXIT;
        icvCompressPoints( (CvPoint2D64f*)m1->data.ptr, tempMask->data.ptr, 1, count );
        count = icvCompressPoints( (CvPoint2D64f*)m2->data.ptr, tempMask->data.ptr, 1, count );
        assert( count >= 8 );
        m1->cols = m2->cols = count;
        estimator.run8Point(m1, m2, &_F3x3);
    }
    }

    if( result )
        cvConvert( fmatrix->rows == 3 ? &_F3x3 : &_F9x3, fmatrix );

    __END__;

    cvReleaseMat( &m1 );
    cvReleaseMat( &m2 );
    if( tempMask != mask )
        cvReleaseMat( &tempMask );

    return result;
}



CvModelEstimator2::CvModelEstimator2(int _modelPoints, CvSize _modelSize, int _maxBasicSolutions)
{
    modelPoints = _modelPoints;
    modelSize = _modelSize;
    maxBasicSolutions = _maxBasicSolutions;
    checkPartialSubsets = true;
    rng = cvRNG(-1);
}

CvModelEstimator2::~CvModelEstimator2()
{
}

void CvModelEstimator2::setSeed( int64 seed )
{
    rng = cvRNG(seed);
}


int CvModelEstimator2::findInliers( const CvMat* m1, const CvMat* m2,
                                    const CvMat* model, CvMat* _err,
                                    CvMat* _mask, double threshold )
{
    int i, count = _err->rows*_err->cols, goodCount = 0;
    const float* err = _err->data.fl;
    uchar* mask = _mask->data.ptr;

    computeReprojError( m1, m2, model, _err );
    threshold *= threshold;
    for( i = 0; i < count; i++ )
        goodCount += mask[i] = err[i] <= threshold;
    return goodCount;
}


int cvRANSACUpdateNumIters( double p, double ep,
                        int model_points, int max_iters )
{
    int result = 0;

    CV_FUNCNAME( "cvRANSACUpdateNumIters" );

    __BEGIN__;

    double num, denom;

    if( model_points <= 0 )
        CV_ERROR( CV_StsOutOfRange, "the number of model points should be positive" );

    p = MAX(p, 0.);
    p = MIN(p, 1.);
    ep = MAX(ep, 0.);
    ep = MIN(ep, 1.);

    // avoid inf's & nan's
    num = MAX(1. - p, DBL_MIN);
    denom = 1. - pow(1. - ep,model_points);
    if( denom < DBL_MIN )
        EXIT;

    num = log(num);
    denom = log(denom);

    result = denom >= 0 || -num >= max_iters*(-denom) ?
        max_iters : cvRound(num/denom);

    __END__;

    return result;
}

bool CvModelEstimator2::runRANSAC( const CvMat* m1, const CvMat* m2, CvMat* model,
                                        CvMat* mask, double reprojThreshold,
                                        double confidence, int maxIters )
{
    bool result = false;
    CvMat* mask0 = mask, *tmask = 0, *t;
    CvMat* models = 0, *err = 0;
    CvMat *ms1 = 0, *ms2 = 0;

    CV_FUNCNAME( "CvModelEstimator2::estimateRansac" );

    __BEGIN__;

    int iter, niters = maxIters;
    int count = m1->rows*m1->cols, maxGoodCount = 0;
    CV_ASSERT( CV_ARE_SIZES_EQ(m1, m2) && CV_ARE_SIZES_EQ(m1, mask) );

    if( count < modelPoints )
        EXIT;

    models = cvCreateMat( modelSize.height*maxBasicSolutions, modelSize.width, CV_64FC1 );
    err = cvCreateMat( 1, count, CV_32FC1 );
    tmask = cvCreateMat( 1, count, CV_8UC1 );

    if( count > modelPoints )
    {
        ms1 = cvCreateMat( 1, modelPoints, m1->type );
        ms2 = cvCreateMat( 1, modelPoints, m2->type );
    }
    else
    {
        niters = 1;
        ms1 = (CvMat*)m1;
        ms2 = (CvMat*)m2;
    }

    for( iter = 0; iter < niters; iter++ )
    {
        int i, goodCount, nmodels;
        if( count > modelPoints )
        {
            bool found = getSubset( m1, m2, ms1, ms2, 300 );
            if( !found )
            {
                if( iter == 0 )
                    EXIT;
                break;
            }
        }

        nmodels = runKernel( ms1, ms2, models );
        if( nmodels <= 0 )
            continue;
        for( i = 0; i < nmodels; i++ )
        {
            CvMat model_i;
            cvGetRows( models, &model_i, i*modelSize.height, (i+1)*modelSize.height );
            goodCount = findInliers( m1, m2, &model_i, err, tmask, reprojThreshold );

            if( goodCount > MAX(maxGoodCount, modelPoints-1) )
            {
                CV_SWAP( tmask, mask, t );
                cvCopy( &model_i, model );
                maxGoodCount = goodCount;
                niters = cvRANSACUpdateNumIters( confidence,
                    (double)(count - goodCount)/count, modelPoints, niters );
            }
        }
    }

    if( maxGoodCount > 0 )
    {
        if( mask != mask0 )
        {
            CV_SWAP( tmask, mask, t );
            cvCopy( tmask, mask );
        }
        result = true;
    }

    __END__;

    if( ms1 != m1 )
        cvReleaseMat( &ms1 );
    if( ms2 != m2 )
        cvReleaseMat( &ms2 );
    cvReleaseMat( &models );
    cvReleaseMat( &err );
    cvReleaseMat( &tmask );
    return result;
}

void* cvAlignPtr( const void* ptr, int align CV_DEFAULT(32) )
{
    assert( (align & (align-1)) == 0 );
    return (void*)( ((size_t)ptr + align - 1) & ~(size_t)(align-1) );
}

bool CvModelEstimator2::getSubset( const CvMat* m1, const CvMat* m2,
                                   CvMat* ms1, CvMat* ms2, int maxAttempts )
{
    int* idx = (int*)cvStackAlloc( modelPoints*sizeof(idx[0]) );
    int i, j, k, idx_i, iters = 0;
    int type = CV_MAT_TYPE(m1->type), elemSize = CV_ELEM_SIZE(type);
    const int *m1ptr = m1->data.i, *m2ptr = m2->data.i;
    int *ms1ptr = ms1->data.i, *ms2ptr = ms2->data.i;
    int count = m1->cols*m1->rows;

    assert( CV_IS_MAT_CONT(m1->type & m2->type) && (elemSize % sizeof(int) == 0) );
    elemSize /= sizeof(int);

    for(;;)
    {
        for( i = 0; i < modelPoints && iters < maxAttempts; iters++ )
        {
            idx[i] = idx_i = cvRandInt(&rng) % count;
            for( j = 0; j < i; j++ )
                if( idx_i == idx[j] )
                    break;
            if( j < i )
                continue;
            for( k = 0; k < elemSize; k++ )
            {
                ms1ptr[i*elemSize + k] = m1ptr[idx_i*elemSize + k];
                ms2ptr[i*elemSize + k] = m2ptr[idx_i*elemSize + k];
            }
            if( checkPartialSubsets && (!checkSubset( ms1, i+1 ) || !checkSubset( ms2, i+1 )))
                continue;
            i++;
            iters = 0;
        }
        if( !checkPartialSubsets && i == modelPoints &&
            (!checkSubset( ms1, i+1 ) || !checkSubset( ms2, i+1 )))
            continue;
        break;
    }

    return i == modelPoints;
}


bool CvModelEstimator2::checkSubset( const CvMat* m, int count )
{
    int j, k, i = count-1;
    CvPoint2D64f* ptr = (CvPoint2D64f*)m->data.ptr;

    assert( CV_MAT_TYPE(m->type) == CV_64FC2 );

    // check that the i-th selected point does not belong
    // to a line connecting some previously selected points
    for( j = 0; j < i; j++ )
    {
        double dx1 = ptr[j].x - ptr[i].x;
        double dy1 = ptr[j].y - ptr[i].y;
        for( k = 0; k < j; k++ )
        {
            double dx2 = ptr[k].x - ptr[i].x;
            double dy2 = ptr[k].y - ptr[i].y;
            if( fabs(dx2*dy1 - dy2*dx1) < FLT_EPSILON*(fabs(dx1) + fabs(dy1) + fabs(dx2) + fabs(dy2)))
                break;
        }
        if( k < j )
            break;
    }

    return j == i;
}
//////////////////////////////////////////////////////////////////

CvLevMarqHR::CvLevMarqHR()
{
    mask = prevParam = param = J = err = JtJ = JtJN = JtErr = JtJV = JtJW = 0;
    lambdaLg10 = 0; state = DONE;
    criteria = cvTermCriteria(0,0,0);
    iters = 0;
    completeSymmFlag = false;
}

CvLevMarqHR::CvLevMarqHR( int nparams, int nerrs, CvTermCriteria criteria0, bool _completeSymmFlag )
{
    mask = prevParam = param = J = err = JtJ = JtJN = JtErr = JtJV = JtJW = 0;
    init(nparams, nerrs, criteria0, _completeSymmFlag);
}

void CvLevMarqHR::clear()
{
    cvReleaseMat(&mask);
    cvReleaseMat(&prevParam);
    cvReleaseMat(&param);
    cvReleaseMat(&J);
    cvReleaseMat(&err);
    cvReleaseMat(&JtJ);
    cvReleaseMat(&JtJN);
    cvReleaseMat(&JtErr);
    cvReleaseMat(&JtJV);
    cvReleaseMat(&JtJW);
}

CvLevMarqHR::~CvLevMarqHR()
{
    clear();
}

void CvLevMarqHR::init( int nparams, int nerrs, CvTermCriteria criteria0, bool _completeSymmFlag )
{
    if( !param || param->rows != nparams || nerrs != (err ? err->rows : 0) )
        clear();
    mask = cvCreateMat( nparams, 1, CV_8U );
    cvSet(mask, cvScalarAll(1));
    prevParam = cvCreateMat( nparams, 1, CV_64F );
    param = cvCreateMat( nparams, 1, CV_64F );
    JtJ = cvCreateMat( nparams, nparams, CV_64F );
    JtJN = cvCreateMat( nparams, nparams, CV_64F );
    JtJV = cvCreateMat( nparams, nparams, CV_64F );
    JtJW = cvCreateMat( nparams, 1, CV_64F );
    JtErr = cvCreateMat( nparams, 1, CV_64F );
    if( nerrs > 0 )
    {
        J = cvCreateMat( nerrs, nparams, CV_64F );
        err = cvCreateMat( nerrs, 1, CV_64F );
    }
    prevErrNorm = DBL_MAX;
    lambdaLg10 = -3;
    criteria = criteria0;
    if( criteria.type & CV_TERMCRIT_ITER )
        criteria.max_iter = MIN(MAX(criteria.max_iter,1),1000);
    else
        criteria.max_iter = 30;
    if( criteria.type & CV_TERMCRIT_EPS )
        criteria.epsilon = MAX(criteria.epsilon, 0);
    else
        criteria.epsilon = DBL_EPSILON;
    state = STARTED;
    iters = 0;
    completeSymmFlag = _completeSymmFlag;
}

bool CvLevMarqHR::update( const CvMat*& _param, CvMat*& _J, CvMat*& _err )
{
    double change;

    _J = _err = 0;

    assert( err != 0 );
    if( state == DONE )
    {
        _param = param;
        return false;
    }

    if( state == STARTED )
    {
        _param = param;
        cvZero( J );
        cvZero( err );
        _J = J;
        _err = err;
        state = CALC_J;
        return true;
    }

    if( state == CALC_J )
    {
        cvMulTransposed( J, JtJ, 1 );
        cvGEMM( J, err, 1, 0, 0, JtErr, CV_GEMM_A_T );
        cvCopy( param, prevParam );
        step();
        if( iters == 0 )
            prevErrNorm = cvNorm(err, 0, CV_L2);
        _param = param;
        cvZero( err );
        _err = err;
        state = CHECK_ERR;
        return true;
    }

    assert( state == CHECK_ERR );
    errNorm = cvNorm( err, 0, CV_L2 );
    if( errNorm > prevErrNorm )
    {
        lambdaLg10++;
        step();
        _param = param;
        cvZero( err );
        _err = err;
        state = CHECK_ERR;
        return true;
    }

    lambdaLg10 = MAX(lambdaLg10-1, -16);
    if( ++iters >= criteria.max_iter ||
        (change = cvNorm(param, prevParam, CV_RELATIVE_L2)) < criteria.epsilon )
    {
        _param = param;
        state = DONE;
        return true;
    }

    prevErrNorm = errNorm;
    _param = param;
    cvZero(J);
    _J = J;
    _err = err;
    state = CALC_J;
    return true;
}


bool CvLevMarqHR::updateAlt( const CvMat*& _param, CvMat*& _JtJ, CvMat*& _JtErr, double*& _errNorm )
{
    double change;

    assert( err == 0 );
    if( state == DONE )
    {
        _param = param;
        return false;
    }

    if( state == STARTED )
    {
        _param = param;
        cvZero( JtJ );
        cvZero( JtErr );
        errNorm = 0;
        _JtJ = JtJ;
        _JtErr = JtErr;
        _errNorm = &errNorm;
        state = CALC_J;
        return true;
    }

    if( state == CALC_J )
    {
        cvCopy( param, prevParam );
        step();
        _param = param;
        prevErrNorm = errNorm;
        errNorm = 0;
        _errNorm = &errNorm;
        state = CHECK_ERR;
        return true;
    }

    assert( state == CHECK_ERR );
    if( errNorm > prevErrNorm )
    {
        lambdaLg10++;
        step();
        _param = param;
        errNorm = 0;
        _errNorm = &errNorm;
        state = CHECK_ERR;
        return true;
    }

    lambdaLg10 = MAX(lambdaLg10-1, -16);
    if( ++iters >= criteria.max_iter ||
        (change = cvNorm(param, prevParam, CV_RELATIVE_L2)) < criteria.epsilon )
    {
        _param = param;
        state = DONE;
        return false;
    }

    prevErrNorm = errNorm;
    cvZero( JtJ );
    cvZero( JtErr );
    _param = param;
    _JtJ = JtJ;
    _JtErr = JtErr;
    state = CALC_J;
    return true;
}

void CvLevMarqHR::step()
{
    const double LOG10 = log(10.);
    double lambda = exp(lambdaLg10*LOG10);
    int i, j, nparams = param->rows;

    for( i = 0; i < nparams; i++ )
        if( mask->data.ptr[i] == 0 )
        {
            double *row = JtJ->data.db + i*nparams, *col = JtJ->data.db + i;
            for( j = 0; j < nparams; j++ )
                row[j] = col[j*nparams] = 0;
            JtErr->data.db[i] = 0;
        }

    if( !err )
        cvCompleteSymm( JtJ, completeSymmFlag );
#if 1
    cvCopy( JtJ, JtJN );
    for( i = 0; i < nparams; i++ )
        JtJN->data.db[(nparams+1)*i] *= 1. + lambda;
#else
    cvSetIdentity(JtJN, cvRealScalar(lambda));
    cvAdd( JtJ, JtJN, JtJN );
#endif
    cvSVD( JtJN, JtJW, 0, JtJV, CV_SVD_MODIFY_A + CV_SVD_U_T + CV_SVD_V_T );
    cvSVBkSb( JtJW, JtJV, JtJV, JtErr, param, CV_SVD_U_T + CV_SVD_V_T );
    for( i = 0; i < nparams; i++ )
        param->data.db[i] = prevParam->data.db[i] - (mask->data.ptr[i] ? param->data.db[i] : 0);
}

// reimplementation of dAB.m
 void
cvCalcMatMulDeriv( const CvMat* A, const CvMat* B, CvMat* dABdA, CvMat* dABdB )
{
    CV_FUNCNAME( "cvCalcMatMulDeriv" );

    __BEGIN__;

    int i, j, M, N, L;
    int bstep;

    CV_ASSERT( CV_IS_MAT(A) && CV_IS_MAT(B) );
    CV_ASSERT( CV_ARE_TYPES_EQ(A, B) &&
        (CV_MAT_TYPE(A->type) == CV_32F || CV_MAT_TYPE(A->type) == CV_64F) );
    CV_ASSERT( A->cols == B->rows );

    M = A->rows;
    L = A->cols;
    N = B->cols;
    bstep = B->step/CV_ELEM_SIZE(B->type);

    if( dABdA )
    {
        CV_ASSERT( CV_ARE_TYPES_EQ(A, dABdA) &&
            dABdA->rows == A->rows*B->cols && dABdA->cols == A->rows*A->cols );
    }

    if( dABdB )
    {
        CV_ASSERT( CV_ARE_TYPES_EQ(A, dABdB) &&
            dABdB->rows == A->rows*B->cols && dABdB->cols == B->rows*B->cols );
    }

    if( CV_MAT_TYPE(A->type) == CV_32F )
    {
        for( i = 0; i < M*N; i++ )
        {
            int i1 = i / N,  i2 = i % N;

            if( dABdA )
            {
                float* dcda = (float*)(dABdA->data.ptr + dABdA->step*i);
                const float* b = (const float*)B->data.ptr + i2;

                for( j = 0; j < M*L; j++ )
                    dcda[j] = 0;
                for( j = 0; j < L; j++ )
                    dcda[i1*L + j] = b[j*bstep];
            }

            if( dABdB )
            {
                float* dcdb = (float*)(dABdB->data.ptr + dABdB->step*i);
                const float* a = (const float*)(A->data.ptr + A->step*i1);

                for( j = 0; j < L*N; j++ )
                    dcdb[j] = 0;
                for( j = 0; j < L; j++ )
                    dcdb[j*N + i2] = a[j];
            }
        }
    }
    else
    {
        for( i = 0; i < M*N; i++ )
        {
            int i1 = i / N,  i2 = i % N;

            if( dABdA )
            {
                double* dcda = (double*)(dABdA->data.ptr + dABdA->step*i);
                const double* b = (const double*)B->data.ptr + i2;

                for( j = 0; j < M*L; j++ )
                    dcda[j] = 0;
                for( j = 0; j < L; j++ )
                    dcda[i1*L + j] = b[j*bstep];
            }

            if( dABdB )
            {
                double* dcdb = (double*)(dABdB->data.ptr + dABdB->step*i);
                const double* a = (const double*)(A->data.ptr + A->step*i1);

                for( j = 0; j < L*N; j++ )
                    dcdb[j] = 0;
                for( j = 0; j < L; j++ )
                    dcdb[j*N + i2] = a[j];
            }
        }
    }

    __END__;
}

// reimplementation of compose_motion.m
 void
cvComposeRT( const CvMat* _rvec1, const CvMat* _tvec1,
             const CvMat* _rvec2, const CvMat* _tvec2,
             CvMat* _rvec3, CvMat* _tvec3,
             CvMat* dr3dr1, CvMat* dr3dt1,
             CvMat* dr3dr2, CvMat* dr3dt2,
             CvMat* dt3dr1, CvMat* dt3dt1,
             CvMat* dt3dr2, CvMat* dt3dt2 )
{
    CV_FUNCNAME( "cvComposeRT" );

    __BEGIN__;

    double _r1[3], _r2[3];
    double _R1[9], _d1[9*3], _R2[9], _d2[9*3];
    CvMat r1 = cvMat(3,1,CV_64F,_r1), r2 = cvMat(3,1,CV_64F,_r2);
    CvMat R1 = cvMat(3,3,CV_64F,_R1), R2 = cvMat(3,3,CV_64F,_R2);
    CvMat dR1dr1 = cvMat(9,3,CV_64F,_d1), dR2dr2 = cvMat(9,3,CV_64F,_d2);

    CV_ASSERT( CV_IS_MAT(_rvec1) && CV_IS_MAT(_rvec2) );

    CV_ASSERT( CV_MAT_TYPE(_rvec1->type) == CV_32F ||
               CV_MAT_TYPE(_rvec1->type) == CV_64F );

    CV_ASSERT( _rvec1->rows == 3 && _rvec1->cols == 1 && CV_ARE_SIZES_EQ(_rvec1, _rvec2) );

    cvConvert( _rvec1, &r1 );
    cvConvert( _rvec2, &r2 );

    cvRodrigues2( &r1, &R1, &dR1dr1 );
    cvRodrigues2( &r2, &R2, &dR2dr2 );

    if( _rvec3 || dr3dr1 || dr3dr1 )
    {
        double _r3[3], _R3[9], _dR3dR1[9*9], _dR3dR2[9*9], _dr3dR3[9*3];
        double _W1[9*3], _W2[3*3];
        CvMat r3 = cvMat(3,1,CV_64F,_r3), R3 = cvMat(3,3,CV_64F,_R3);
        CvMat dR3dR1 = cvMat(9,9,CV_64F,_dR3dR1), dR3dR2 = cvMat(9,9,CV_64F,_dR3dR2);
        CvMat dr3dR3 = cvMat(3,9,CV_64F,_dr3dR3);
        CvMat W1 = cvMat(3,9,CV_64F,_W1), W2 = cvMat(3,3,CV_64F,_W2);

        cvMatMul( &R2, &R1, &R3 );
        cvCalcMatMulDeriv( &R2, &R1, &dR3dR2, &dR3dR1 );

        cvRodrigues2( &R3, &r3, &dr3dR3 );

        if( _rvec3 )
            cvConvert( &r3, _rvec3 );

        if( dr3dr1 )
        {
            cvMatMul( &dr3dR3, &dR3dR1, &W1 );
            cvMatMul( &W1, &dR1dr1, &W2 );
            cvConvert( &W2, dr3dr1 );
        }

        if( dr3dr2 )
        {
            cvMatMul( &dr3dR3, &dR3dR2, &W1 );
            cvMatMul( &W1, &dR2dr2, &W2 );
            cvConvert( &W2, dr3dr2 );
        }
    }

    if( dr3dt1 )
        cvZero( dr3dt1 );
    if( dr3dt2 )
        cvZero( dr3dt2 );

    if( _tvec3 || dt3dr2 || dt3dt1 )
    {
        double _t1[3], _t2[3], _t3[3], _dxdR2[3*9], _dxdt1[3*3], _W3[3*3];
        CvMat t1 = cvMat(3,1,CV_64F,_t1), t2 = cvMat(3,1,CV_64F,_t2);
        CvMat t3 = cvMat(3,1,CV_64F,_t3);
        CvMat dxdR2 = cvMat(3, 9, CV_64F, _dxdR2);
        CvMat dxdt1 = cvMat(3, 3, CV_64F, _dxdt1);
        CvMat W3 = cvMat(3, 3, CV_64F, _W3);

        CV_ASSERT( CV_IS_MAT(_tvec1) && CV_IS_MAT(_tvec2) );
        CV_ASSERT( CV_ARE_SIZES_EQ(_tvec1, _tvec2) && CV_ARE_SIZES_EQ(_tvec1, _rvec1) );

        cvConvert( _tvec1, &t1 );
        cvConvert( _tvec2, &t2 );
        cvMatMulAdd( &R2, &t1, &t2, &t3 );

        if( _tvec3 )
            cvConvert( &t3, _tvec3 );

        if( dt3dr2 || dt3dt1 )
        {
            cvCalcMatMulDeriv( &R2, &t1, &dxdR2, &dxdt1 );
            if( dt3dr2 )
            {
                cvMatMul( &dxdR2, &dR2dr2, &W3 );
                cvConvert( &W3, dt3dr2 );
            }
            if( dt3dt1 )
                cvConvert( &dxdt1, dt3dt1 );
        }
    }

    if( dt3dt2 )
        cvSetIdentity( dt3dt2 );
    if( dt3dr1 )
        cvZero( dt3dr1 );

    __END__;
}

 int
cvRodrigues2( const CvMat* src, CvMat* dst, CvMat* jacobian )
{
    int result = 0;

    CV_FUNCNAME( "cvRogrigues2" );

    __BEGIN__;

    int depth, elem_size;
    int i, k;
    double J[27];
    CvMat _J = cvMat( 3, 9, CV_64F, J );

    if( !CV_IS_MAT(src) )
        CV_ERROR( !src ? CV_StsNullPtr : CV_StsBadArg, "Input argument is not a valid matrix" );

    if( !CV_IS_MAT(dst) )
        CV_ERROR( !dst ? CV_StsNullPtr : CV_StsBadArg,
        "The first output argument is not a valid matrix" );

    depth = CV_MAT_DEPTH(src->type);
    elem_size = CV_ELEM_SIZE(depth);

    if( depth != CV_32F && depth != CV_64F )
        CV_ERROR( CV_StsUnsupportedFormat, "The matrices must have 32f or 64f data type" );

    if( !CV_ARE_DEPTHS_EQ(src, dst) )
        CV_ERROR( CV_StsUnmatchedFormats, "All the matrices must have the same data type" );

    if( jacobian )
    {
        if( !CV_IS_MAT(jacobian) )
            CV_ERROR( CV_StsBadArg, "Jacobian is not a valid matrix" );

        if( !CV_ARE_DEPTHS_EQ(src, jacobian) || CV_MAT_CN(jacobian->type) != 1 )
            CV_ERROR( CV_StsUnmatchedFormats, "Jacobian must have 32fC1 or 64fC1 datatype" );

        if( (jacobian->rows != 9 || jacobian->cols != 3) &&
            (jacobian->rows != 3 || jacobian->cols != 9))
            CV_ERROR( CV_StsBadSize, "Jacobian must be 3x9 or 9x3" );
    }

    if( src->cols == 1 || src->rows == 1 )
    {
        double rx, ry, rz, theta;
        int step = src->rows > 1 ? src->step / elem_size : 1;

        if( src->rows + src->cols*CV_MAT_CN(src->type) - 1 != 3 )
            CV_ERROR( CV_StsBadSize, "Input matrix must be 1x3, 3x1 or 3x3" );

        if( dst->rows != 3 || dst->cols != 3 || CV_MAT_CN(dst->type) != 1 )
            CV_ERROR( CV_StsBadSize, "Output matrix must be 3x3, single-channel floating point matrix" );

        if( depth == CV_32F )
        {
            rx = src->data.fl[0];
            ry = src->data.fl[step];
            rz = src->data.fl[step*2];
        }
        else
        {
            rx = src->data.db[0];
            ry = src->data.db[step];
            rz = src->data.db[step*2];
        }
        theta = sqrt(rx*rx + ry*ry + rz*rz);

        if( theta < DBL_EPSILON )
        {
            cvSetIdentity( dst );

            if( jacobian )
            {
                memset( J, 0, sizeof(J) );
                J[5] = J[15] = J[19] = -1;
                J[7] = J[11] = J[21] = 1;
            }
        }
        else
        {
            const double I[] = { 1, 0, 0, 0, 1, 0, 0, 0, 1 };

            double c = cos(theta);
            double s = sin(theta);
            double c1 = 1. - c;
            double itheta = theta ? 1./theta : 0.;

            rx *= itheta; ry *= itheta; rz *= itheta;

            double rrt[] = { rx*rx, rx*ry, rx*rz, rx*ry, ry*ry, ry*rz, rx*rz, ry*rz, rz*rz };
            double _r_x_[] = { 0, -rz, ry, rz, 0, -rx, -ry, rx, 0 };
            double R[9];
            CvMat _R = cvMat( 3, 3, CV_64F, R );

            // R = cos(theta)*I + (1 - cos(theta))*r*rT + sin(theta)*[r_x]
            // where [r_x] is [0 -rz ry; rz 0 -rx; -ry rx 0]
            for( k = 0; k < 9; k++ )
                R[k] = c*I[k] + c1*rrt[k] + s*_r_x_[k];

            cvConvert( &_R, dst );

            if( jacobian )
            {
                double drrt[] = { rx+rx, ry, rz, ry, 0, 0, rz, 0, 0,
                                  0, rx, 0, rx, ry+ry, rz, 0, rz, 0,
                                  0, 0, rx, 0, 0, ry, rx, ry, rz+rz };
                double d_r_x_[] = { 0, 0, 0, 0, 0, -1, 0, 1, 0,
                                    0, 0, 1, 0, 0, 0, -1, 0, 0,
                                    0, -1, 0, 1, 0, 0, 0, 0, 0 };
                for( i = 0; i < 3; i++ )
                {
                    double ri = i == 0 ? rx : i == 1 ? ry : rz;
                    double a0 = -s*ri, a1 = (s - 2*c1*itheta)*ri, a2 = c1*itheta;
                    double a3 = (c - s*itheta)*ri, a4 = s*itheta;
                    for( k = 0; k < 9; k++ )
                        J[i*9+k] = a0*I[k] + a1*rrt[k] + a2*drrt[i*9+k] +
                                   a3*_r_x_[k] + a4*d_r_x_[i*9+k];
                }
            }
        }
    }
    else if( src->cols == 3 && src->rows == 3 )
    {
        double R[9], U[9], V[9], W[3], rx, ry, rz;
        CvMat _R = cvMat( 3, 3, CV_64F, R );
        CvMat _U = cvMat( 3, 3, CV_64F, U );
        CvMat _V = cvMat( 3, 3, CV_64F, V );
        CvMat _W = cvMat( 3, 1, CV_64F, W );
        double theta, s, c;
        int step = dst->rows > 1 ? dst->step / elem_size : 1;

        if( (dst->rows != 1 || dst->cols*CV_MAT_CN(dst->type) != 3) &&
            (dst->rows != 3 || dst->cols != 1 || CV_MAT_CN(dst->type) != 1))
            CV_ERROR( CV_StsBadSize, "Output matrix must be 1x3 or 3x1" );

        cvConvert( src, &_R );
        if( !cvCheckArr( &_R, CV_CHECK_RANGE+CV_CHECK_QUIET, -100, 100 ) )
        {
            cvZero(dst);
            if( jacobian )
                cvZero(jacobian);
            EXIT;
        }

        cvSVD( &_R, &_W, &_U, &_V, CV_SVD_MODIFY_A + CV_SVD_U_T + CV_SVD_V_T );
        cvGEMM( &_U, &_V, 1, 0, 0, &_R, CV_GEMM_A_T );

        rx = R[7] - R[5];
        ry = R[2] - R[6];
        rz = R[3] - R[1];

        s = sqrt((rx*rx + ry*ry + rz*rz)*0.25);
        c = (R[0] + R[4] + R[8] - 1)*0.5;
        c = c > 1. ? 1. : c < -1. ? -1. : c;
        theta = acos(c);

        if( s < 1e-5 )
        {
            double t;

            if( c > 0 )
                rx = ry = rz = 0;
            else
            {
                t = (R[0] + 1)*0.5;
                rx = sqrt(MAX(t,0.));
                t = (R[4] + 1)*0.5;
                ry = sqrt(MAX(t,0.))*(R[1] < 0 ? -1. : 1.);
                t = (R[8] + 1)*0.5;
                rz = sqrt(MAX(t,0.))*(R[2] < 0 ? -1. : 1.);
                if( fabs(rx) < fabs(ry) && fabs(rx) < fabs(rz) && (R[5] > 0) != (ry*rz > 0) )
                    rz = -rz;
                theta /= sqrt(rx*rx + ry*ry + rz*rz);
                rx *= theta;
                ry *= theta;
                rz *= theta;
            }

            if( jacobian )
            {
                memset( J, 0, sizeof(J) );
                if( c > 0 )
                {
                    J[5] = J[15] = J[19] = -0.5;
                    J[7] = J[11] = J[21] = 0.5;
                }
            }
        }
        else
        {
            double vth = 1/(2*s);

            if( jacobian )
            {
                double t, dtheta_dtr = -1./s;
                // var1 = [vth;theta]
                // var = [om1;var1] = [om1;vth;theta]
                double dvth_dtheta = -vth*c/s;
                double d1 = 0.5*dvth_dtheta*dtheta_dtr;
                double d2 = 0.5*dtheta_dtr;
                // dvar1/dR = dvar1/dtheta*dtheta/dR = [dvth/dtheta; 1] * dtheta/dtr * dtr/dR
                double dvardR[5*9] =
                {
                    0, 0, 0, 0, 0, 1, 0, -1, 0,
                    0, 0, -1, 0, 0, 0, 1, 0, 0,
                    0, 1, 0, -1, 0, 0, 0, 0, 0,
                    d1, 0, 0, 0, d1, 0, 0, 0, d1,
                    d2, 0, 0, 0, d2, 0, 0, 0, d2
                };
                // var2 = [om;theta]
                double dvar2dvar[] =
                {
                    vth, 0, 0, rx, 0,
                    0, vth, 0, ry, 0,
                    0, 0, vth, rz, 0,
                    0, 0, 0, 0, 1
                };
                double domegadvar2[] =
                {
                    theta, 0, 0, rx*vth,
                    0, theta, 0, ry*vth,
                    0, 0, theta, rz*vth
                };

                CvMat _dvardR = cvMat( 5, 9, CV_64FC1, dvardR );
                CvMat _dvar2dvar = cvMat( 4, 5, CV_64FC1, dvar2dvar );
                CvMat _domegadvar2 = cvMat( 3, 4, CV_64FC1, domegadvar2 );
                double t0[3*5];
                CvMat _t0 = cvMat( 3, 5, CV_64FC1, t0 );

                cvMatMul( &_domegadvar2, &_dvar2dvar, &_t0 );
                cvMatMul( &_t0, &_dvardR, &_J );

                // transpose every row of _J (treat the rows as 3x3 matrices)
                CV_SWAP(J[1], J[3], t); CV_SWAP(J[2], J[6], t); CV_SWAP(J[5], J[7], t);
                CV_SWAP(J[10], J[12], t); CV_SWAP(J[11], J[15], t); CV_SWAP(J[14], J[16], t);
                CV_SWAP(J[19], J[21], t); CV_SWAP(J[20], J[24], t); CV_SWAP(J[23], J[25], t);
            }

            vth *= theta;
            rx *= vth; ry *= vth; rz *= vth;
        }

        if( depth == CV_32F )
        {
            dst->data.fl[0] = (float)rx;
            dst->data.fl[step] = (float)ry;
            dst->data.fl[step*2] = (float)rz;
        }
        else
        {
            dst->data.db[0] = rx;
            dst->data.db[step] = ry;
            dst->data.db[step*2] = rz;
        }
    }

    if( jacobian )
    {
        if( depth == CV_32F )
        {
            if( jacobian->rows == _J.rows )
                cvConvert( &_J, jacobian );
            else
            {
                float Jf[3*9];
                CvMat _Jf = cvMat( _J.rows, _J.cols, CV_32FC1, Jf );
                cvConvert( &_J, &_Jf );
                cvTranspose( &_Jf, jacobian );
            }
        }
        else if( jacobian->rows == _J.rows )
            cvCopy( &_J, jacobian );
        else
            cvTranspose( &_J, jacobian );
    }

    result = 1;

    __END__;

    return result;
}

