
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
#include "projdecompose.h"
#include "general.h"
using namespace std;


#define CV_Error( code, msg ) cv::error( cv::Exception(code, msg, __func__, __FILE__, __LINE__) )
#define CV_Error_( code, args ) cv::error( cv::Exception(code, cv::format args, __func__, __FILE__, __LINE__) )
#define CV_Assert( expr ) { if(!(expr)) cv::error( cv::Exception(CV_StsAssert, #expr, __func__, __FILE__, __LINE__) ); }

void cvRQDecomp3x3( const CvMat *matrixM, CvMat *matrixR, CvMat *matrixQ, CvMat *matrixQx, CvMat *matrixQy, CvMat *matrixQz, CvPoint3D64f *eulerAngles)
{

    double _M[3][3], _R[3][3], _Q[3][3];
    CvMat M = cvMat(3, 3, CV_64F, _M);
    CvMat R = cvMat(3, 3, CV_64F, _R);
    CvMat Q = cvMat(3, 3, CV_64F, _Q);
    double z, c, s;

    /* Validate parameters. */
    if ( CV_IS_MAT(matrixM) && CV_IS_MAT(matrixR) && CV_IS_MAT(matrixQ) &&
            matrixM->cols == 3 && matrixM->rows == 3 &&
            CV_ARE_SIZES_EQ(matrixM, matrixR) && CV_ARE_SIZES_EQ(matrixM, matrixQ));
    else
    {
        printf("\n\nWRONG PARMS!\n\n");
    }

    cvConvert(matrixM, &M);

    {
        /* Find Givens rotation Q_x for x axis (left multiplication). */
        /*
             ( 1  0  0 )
        Qx = ( 0  c  s ), c = m33/sqrt(m32^2 + m33^2), s = m32/sqrt(m32^2 + m33^2)
             ( 0 -s  c )
        */
        s = _M[2][1];
        c = _M[2][2];
        z = 1./sqrt(c * c + s * s + DBL_EPSILON);
        c *= z;
        s *= z;

        double _Qx[3][3] = { {1, 0, 0}, {0, c, s}, {0, -s, c} };
        CvMat Qx = cvMat(3, 3, CV_64F, _Qx);

        cvMatMul(&M, &Qx, &R);
        assert(fabs(_R[2][1]) < FLT_EPSILON);
        _R[2][1] = 0;

        /* Find Givens rotation for y axis. */
        /*
             ( c  0  s )
        Qy = ( 0  1  0 ), c = m33/sqrt(m31^2 + m33^2), s = m31/sqrt(m31^2 + m33^2)
             (-s  0  c )
        */
        s = _R[2][0];
        c = _R[2][2];
        z = 1./sqrt(c * c + s * s + DBL_EPSILON);
        c *= z;
        s *= z;

        double _Qy[3][3] = { {c, 0, s}, {0, 1, 0}, {-s, 0, c} };
        CvMat Qy = cvMat(3, 3, CV_64F, _Qy);
        cvMatMul(&R, &Qy, &M);

        assert(fabs(_M[2][0]) < FLT_EPSILON);
        _M[2][0] = 0;

        /* Find Givens rotation for z axis. */
        /*
             ( c  s  0 )
        Qz = (-s  c  0 ), c = m22/sqrt(m21^2 + m22^2), s = m21/sqrt(m21^2 + m22^2)
             ( 0  0  1 )
        */

        s = _M[1][0];
        c = _M[1][1];
        z = 1./sqrt(c * c + s * s + DBL_EPSILON);
        c *= z;
        s *= z;

        double _Qz[3][3] = { {c, s, 0}, {-s, c, 0}, {0, 0, 1} };
        CvMat Qz = cvMat(3, 3, CV_64F, _Qz);

        cvMatMul(&M, &Qz, &R);
        assert(fabs(_R[1][0]) < FLT_EPSILON);
        _R[1][0] = 0;

        // Solve the decomposition ambiguity.
        // Diagonal entries of R, except the last one, shall be positive.
        // Further rotate R by 180 degree if necessary
        if ( _R[0][0] < 0 )
        {
            if ( _R[1][1] < 0 )
            {
                // rotate around z for 180 degree, i.e. a rotation matrix of
                // [-1,  0,  0],
                // [ 0, -1,  0],
                // [ 0,  0,  1]
                _R[0][0] *= -1;
                _R[0][1] *= -1;
                _R[1][1] *= -1;

                _Qz[0][0] *= -1;
                _Qz[0][1] *= -1;
                _Qz[1][0] *= -1;
                _Qz[1][1] *= -1;
            }
            else
            {
                // rotate around y for 180 degree, i.e. a rotation matrix of
                // [-1,  0,  0],
                // [ 0,  1,  0],
                // [ 0,  0, -1]
                _R[0][0] *= -1;
                _R[0][2] *= -1;
                _R[1][2] *= -1;
                _R[2][2] *= -1;

                cvTranspose( &Qz, &Qz );

                _Qy[0][0] *= -1;
                _Qy[0][2] *= -1;
                _Qy[2][0] *= -1;
                _Qy[2][2] *= -1;
            }
        }
        else if ( _R[1][1] < 0 )
        {
            // ??? for some reason, we never get here ???

            // rotate around x for 180 degree, i.e. a rotation matrix of
            // [ 1,  0,  0],
            // [ 0, -1,  0],
            // [ 0,  0, -1]
            _R[0][1] *= -1;
            _R[0][2] *= -1;
            _R[1][1] *= -1;
            _R[1][2] *= -1;
            _R[2][2] *= -1;

            cvTranspose( &Qz, &Qz );
            cvTranspose( &Qy, &Qy );

            _Qx[1][1] *= -1;
            _Qx[1][2] *= -1;
            _Qx[2][1] *= -1;
            _Qx[2][2] *= -1;
        }

        // calculate the euler angle
        if ( eulerAngles )
        {
            eulerAngles->x = acos(_Qx[1][1]) * (_Qx[1][2] >= 0 ? 1 : -1) * (180.0 / CV_PI);
            eulerAngles->y = acos(_Qy[0][0]) * (_Qy[0][2] >= 0 ? 1 : -1) * (180.0 / CV_PI);
            eulerAngles->z = acos(_Qz[0][0]) * (_Qz[0][1] >= 0 ? 1 : -1) * (180.0 / CV_PI);
        }

        /* Calulate orthogonal matrix. */
        /*
        Q = QzT * QyT * QxT
        */
        cvGEMM( &Qz, &Qy, 1, 0, 0, &M, CV_GEMM_A_T + CV_GEMM_B_T );
        cvGEMM( &M, &Qx, 1, 0, 0, &Q, CV_GEMM_B_T );

        /* Save R and Q matrices. */
        cvConvert( &R, matrixR );
        cvConvert( &Q, matrixQ );

        if ( matrixQx )
            cvConvert(&Qx, matrixQx);
        if ( matrixQy )
            cvConvert(&Qy, matrixQy);
        if ( matrixQz )
            cvConvert(&Qz, matrixQz);
    }


}

void ProjectiveMatFromF( const CvMat *F, CvMat *P1,CvMat *P2)
{
//to see the purpose of v and scale see page 256 in the multiple view book
    CvMat* v = cvCreateMat(3,1, CV_64F);

    cvmSet(v, 0, 0,0.0 );
    cvmSet(v, 1, 0,0.0 );
    cvmSet(v, 2, 0,0.0);

    ProjectiveMatFromF( F, P1,P2,v,1.00 );
    cvReleaseMat(&v);

}
void findEpipoles(const CvMat *F, CvMat *e1,CvMat *e2)
{

http://www4.ncsu.edu/~ipsen/REU09/chapter4.pdf


}

void ProjectiveMatFromF( const CvMat *F, CvMat *P1,CvMat *P2,CvMat v,double scale )
{




}

void cvDecomposeProjectionMatrixHR( const CvMat *projMatr, CvMat *calibMatr,
                                    CvMat *rotMatr, CvMat *posVect,
                                    CvMat *rotMatrX, CvMat *rotMatrY,
                                    CvMat *rotMatrZ, CvPoint3D64f *eulerAngles)
{
//    writeCVMatrix(cout,projMatr);
    CvMat *tmpProjMatr = 0;
    CvMat *tmpMatrixD = 0;
    CvMat *tmpMatrixV = 0;
    CvMat *tmpMatrixM = 0;

    ///CV_FUNCNAME("cvDecomposeProjectionMatrixHR");


    /* Validate parameters. */
    if (projMatr == 0 || calibMatr == 0 || rotMatr == 0 || posVect == 0)
        printf("Some of parameters is a NULL pointer!");

    if (!CV_IS_MAT(projMatr) || !CV_IS_MAT(calibMatr) || !CV_IS_MAT(rotMatr) || !CV_IS_MAT(posVect))
        printf("Input parameters must be a matrices!");

    if (projMatr->cols != 4 || projMatr->rows != 3)
        printf( "Size of projection matrix must be 3x4!");

    if (calibMatr->cols != 3 || calibMatr->rows != 3 || rotMatr->cols != 3 || rotMatr->rows != 3)
        printf( "Size of calibration and rotation matrices must be 3x3!");

    if (posVect->cols != 1 || posVect->rows != 4)
        printf( "Size of position vector must be 4x1!");

    tmpProjMatr = cvCreateMat(4, 4, CV_64F);
    tmpMatrixD = cvCreateMat(4, 4, CV_64F);
    tmpMatrixV = cvCreateMat(4, 4, CV_64F);
    tmpMatrixM = cvCreateMat(3, 3, CV_64F);

    /* Compute position vector. */

    cvSetZero(tmpProjMatr); // Add zero row to make matrix square.
    int i, k;
    for (i = 0; i < 3; i++)
        for (k = 0; k < 4; k++)
            cvmSet(tmpProjMatr, i, k, cvmGet(projMatr, i, k));

    cvSVD(tmpProjMatr, tmpMatrixD, NULL, tmpMatrixV, CV_SVD_MODIFY_A + CV_SVD_V_T);

    /* Save position vector. */




    for (i = 0; i < 4; i++)
        cvmSet(posVect, i, 0, cvmGet(tmpMatrixV, 3, i)); // Solution is last row of V.

    /* Compute calibration and rotation matrices via RQ decomposition. */

    cvGetCols(projMatr, tmpMatrixM, 0, 3); // M is first square matrix of P.






    //  writeCVMatrix(cout,tmpMatrixM);
    double d = cvDet(tmpMatrixM);


//printf("\n\nthe determinent of M is %f\n",d);
    assert(d != 0.0); // So far only finite cameras could be decomposed, so M has to be nonsingular [det(M) != 0].

    cvRQDecomp3x3(tmpMatrixM, calibMatr, rotMatr, rotMatrX, rotMatrY, rotMatrZ, eulerAngles);

    double lastel=cvmGet(calibMatr, 2, 2);
    for (i = 0; i < 3; i++)
        for (k = 0; k < 3; k++)
            cvmSet(calibMatr, i, k, cvmGet(calibMatr, i, k)/lastel);


    cvReleaseMat(&tmpProjMatr);
    cvReleaseMat(&tmpMatrixD);
    cvReleaseMat(&tmpMatrixV);
    cvReleaseMat(&tmpMatrixM);
}


