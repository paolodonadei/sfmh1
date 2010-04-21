#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include <cv.h>
#include <vector>
#include <iostream>
#include <fstream>
#include <assert.h>

#include <iomanip>
#include <sstream>
#include "visiongen.h"
#include "general.h"
using namespace std;


#define CV_Error( code, msg ) cv::error( cv::Exception(code, msg, __func__, __FILE__, __LINE__) )
#define CV_Error_( code, args ) cv::error( cv::Exception(code, cv::format args, __func__, __FILE__, __LINE__) )
#define CV_Assert( expr ) { if(!(expr)) cv::error( cv::Exception(CV_StsAssert, #expr, __func__, __FILE__, __LINE__) ); }


void cvDecomposeProjectionMatrixHR( const CvMat *projMatr, CvMat *calibMatr,
                                    CvMat *rotMatr, CvMat *posVect,
                                    CvMat *rotMatrX, CvMat *rotMatrY,
                                    CvMat *rotMatrZ, CvPoint3D64f *eulerAngles)
{
  CvMat* ttemp = cvCreateMat(4,1, CV_64F);
    cvDecomposeProjectionMatrix(projMatr, calibMatr,rotMatr,ttemp,rotMatrX, rotMatrY,rotMatrZ,eulerAngles);

printf("ttemp is \n");
writeCVMatrix(cout,ttemp);

int j;
 for (j=0; j<3; j++)
    {
        cvmSet(posVect,j,0,cvmGet(ttemp,j,0)/cvmGet(ttemp,3,0)); // Set M(i,j)

    }

printf("posVect is \n");
writeCVMatrix(cout,posVect);


    cvReleaseMat(&ttemp);

}



/////////////////////////////////////////////
int ProjectiveMatFromF( const CvMat *F, CvMat *P1,CvMat *P2)
{
//to see the purpose of v and scale see page 256 in the multiple view book
    CvMat* v = cvCreateMat(3,1, CV_64F);

    cvmSet(v, 0, 0,0.0 );
    cvmSet(v, 1, 0,0.0 );
    cvmSet(v, 2, 0,0.0);

    ProjectiveMatFromF( F, P1,P2,v,1.00 );
    cvReleaseMat(&v);

}
int findEpipoles(const CvMat *F, CvMat *e1,CvMat *e2)
{
    if (checkMatrixOK(F,3,3)== false || checkMatrixOK(e1,3,1)==false || checkMatrixOK(e2,3,1)==false)
    {
        return 0;
    }

    //http://www4.ncsu.edu/~ipsen/REU09/chapter4.pdf
    CvMat* U = cvCreateMat(3,3, CV_64F);
    CvMat* V = cvCreateMat(3,3, CV_64F);
    CvMat* W = cvCreateMat(3,3, CV_64F);
    CvMat* FC;


    FC=cvCloneMat(F);

    cvSVD( FC, W,  U, V );  //change all of the below back to U

//epipole 1, left null vector
    cvmSet(e1, 0, 0,cvmGet(V,0,2) );
    cvmSet(e1, 1, 0,cvmGet(V,1,2) );
    cvmSet(e1, 2, 0,cvmGet(V,2,2));


//epipole 2, right null vector
    cvmSet(e2, 0, 0,cvmGet(U,0,2) );
    cvmSet(e2, 1, 0,cvmGet(U,1,2) );
    cvmSet(e2, 2, 0,cvmGet(U,2,2));


    normalizeMatrix(e1);
    normalizeMatrix(e2);



    printf("Debugging info for epipole estimation: \n Fundamental matrix read was\n");
    writeCVMatrix(cout,F );
    printf("Left Epipole (left null space) was\n");
    writeCVMatrix(cout,e1 );
    printf("Right Epipole (left null space) was\n");
    writeCVMatrix(cout,e2 );


    cout<<endl;








    cvReleaseMat(&FC);

    cvReleaseMat(&U);
    cvReleaseMat(&V);
    cvReleaseMat(&W);
}

int ProjectiveMatFromF( const CvMat *F, CvMat *P1,CvMat *P2,CvMat* v,double scale )
{
    CvMat* e1 = cvCreateMat(3,1, CV_64F);
    CvMat* e2 = cvCreateMat(3,1, CV_64F);
    CvMat* e2s = cvCreateMat(3,3, CV_64F);

    CvMat* left1= cvCreateMat(3,3, CV_64F);
    CvMat* left2 = cvCreateMat(3,3, CV_64F);
    CvMat* left3 = cvCreateMat(3,3, CV_64F);
    CvMat* right = cvCreateMat(3,1, CV_64F);
    CvMat* vT = cvCreateMat(1,3, CV_64F);

    findEpipoles(F, e1,e2);
    skewSymmetrify(e2, e2s);

    cvMatMul(e2s, F, left1);   // Ma*Mb   -> Mc


    cvTranspose(v, vT);
    cvMatMul(e2, vT, left2);   // Ma*Mb   -> Mc

    cvAdd(left1, left2, left3);      // Ma+Mb   -> Mc

    int i,j;
    cvSetZero(P1);
    for (j=0; j<=2; j++)
    {
        cvmSet(P1,j,j,((double)1.00)); // Set M(i,j)

    }




    for (i=0; i<=2; i++)
    {
        for (j=0; j<=2; j++)
        {
            cvmSet(P2,i,j,cvmGet(left3,i,j)); // Set M(i,j)

        }

    }

    for (i=0; i<=2; i++)
    {

        cvmSet(P2,i,3,scale*cvmGet(e2,i,0)); // Set M(i,j)
    }

//debugging part
    {
//        CvMat* FDebug= cvCreateMat(3,3, CV_64F);
//        FfromProjectionMatrices(P1,P2,FDebug);
//
//
//        printf("Original F was: \nF:\n");
//        writeCVMatrix(cout,F );
//        printf("estimated projection matrices are: \nP1:\n");
//        writeCVMatrix(cout,P1 );
//        printf("\nP2:\n");
//        writeCVMatrix(cout,P2 );
//        printf("estimated F was: \nF:\n");
//        writeCVMatrix(cout,FDebug );
//        cvReleaseMat(&FDebug);

    }




    cvReleaseMat(&e1);
    cvReleaseMat(&e2);
    cvReleaseMat(&e2s);
    cvReleaseMat(&left1);
    cvReleaseMat(&left2);
    cvReleaseMat(&left3);
    cvReleaseMat(&right);
    cvReleaseMat(&vT);
}


int skewSymmetrify(const CvMat *in, CvMat *out)
{
    if (checkMatrixOK(in,3,1)== false || checkMatrixOK(out,3,3)==false)
    {
        return 0;
    }
    cvSetZero( out);

    cvmSet(out, 0, 1,-1.0*cvmGet(in,2,0));
    cvmSet(out, 0, 2,cvmGet(in,1,0));

    cvmSet(out, 1, 0,cvmGet(in,2,0));
    cvmSet(out, 1, 2,-1.0*cvmGet(in,0,0));

    cvmSet(out, 2, 0,-1.0*cvmGet(in,1,0));
    cvmSet(out, 2, 1,cvmGet(in,0,0));


//    printf("Debugging info for skew symmetric estimation, input: \n");
//    writeCVMatrix(cout,in );
//    printf("output\n");
//    writeCVMatrix(cout,out );

    return true;

}
bool checkMatrixOK(const CvMat *in,int h,int w)
{
    if (in == 0)
    {
        printf("parameters is a NULL pointer!");
        return false;
    }

    if (!CV_IS_MAT(in))
    {
        printf("Input parameter must be a matrix!");
        return false;
    }

    if (in->cols != w ||in->rows != h)
    {

        printf( "Size of  matrix must be %d x %d but it is %d X %d!\n",h,w,in->rows,in->cols);
        assert(0);
        return false;
    }

    return true;
}
int FfromProjectionMatrices(const CvMat* P1,const CvMat* P2, CvMat* F)
{
    CvMat* P1pseudo = cvCreateMat(4,3, CV_64F);
    CvMat* temp1 = cvCreateMat(3,4, CV_64F);
    CvMat* C = cvCreateMat(4,1, CV_64F);
    CvMat* e = cvCreateMat(3,1, CV_64F);
    CvMat* e_skew = cvCreateMat(3,3, CV_64F);


    findCameraCenter(P1, C);

    findPseudoInverse_3x4(P1,P1pseudo);



    cvMatMul(P2, C, e);
    skewSymmetrify(e,e_skew);
    cvMatMul(e_skew,P2, temp1);
    cvMatMul(temp1,P1pseudo, F);
    normalizeMatrix(F);

    cvReleaseMat(&temp1);
    cvReleaseMat(&P1pseudo);
    cvReleaseMat(&C);
    cvReleaseMat(&e);
    cvReleaseMat(&e_skew );
}


int findCameraCenter(const CvMat* P, CvMat* C)
{

    checkMatrixOK(P,3,4);
    checkMatrixOK(C,4,1);

    //http://www4.ncsu.edu/~ipsen/REU09/chapter4.pdf
    CvMat* U = cvCreateMat(4,4, CV_64F);
    CvMat* V = cvCreateMat(4,4, CV_64F);
    CvMat* W = cvCreateMat(4,4, CV_64F);
    CvMat* PCl= cvCreateMat(4,4, CV_64F);

    cvSetZero( PCl);
    int i,j;


    for (i=0; i<=2; i++)
    {
        for (j=0; j<=3; j++)
        {
            cvmSet(PCl,i,j,cvmGet(P,i,j)); // Set M(i,j)

        }

    }

    cvSVD( PCl, W,  U, V );  //change all of the below back to U


    for (j=0; j<=3; j++)
    {
        cvmSet(C,j,0,cvmGet(V,j,3)); // Set M(i,j)

    }



    normalizeMatrix(C);
    cvReleaseMat(&PCl);
    cvReleaseMat(&U);
    cvReleaseMat(&V);
    cvReleaseMat(&W);
}

int findPseudoInverse_3x4(const CvMat* in,CvMat* out)
{
    checkMatrixOK(in,3,4);
    checkMatrixOK(out,4,3);


    CvMat* U = cvCreateMat(4,4, CV_64F);
    CvMat* UT = cvCreateMat(4,4, CV_64F);
    CvMat* V = cvCreateMat(4,4, CV_64F);
    CvMat* W = cvCreateMat(4,4, CV_64F);
    CvMat* inCl= cvCreateMat(4,4, CV_64F);
    CvMat* outful= cvCreateMat(4,4, CV_64F);


    cvSetZero( inCl);
    int i,j;


    for (i=0; i<=2; i++)
    {
        for (j=0; j<=3; j++)
        {
            cvmSet(inCl,i,j,cvmGet(in,i,j)); // Set M(i,j)

        }

    }

    cvSVD( inCl, W,  U, V );  //change all of the below back to U


    for (i=0; i<=3; i++)
    {
        double t=cvmGet(W,i,i);
        cvmSet(W,i,i,(t<0.00000000001)?0:(((double)1.0)/t)); //inverse
    }

    cvTranspose(U, UT);

    cvMatMul(V, W, inCl);
    cvMatMul(inCl, UT, outful);



    for (i=0; i<=3; i++)
    {
        for (j=0; j<=2; j++)
        {
            cvmSet(out,i,j,cvmGet(outful,i,j)); // Set M(i,j)

        }

    }

    cvReleaseMat(&outful);
    cvReleaseMat(&inCl);
    cvReleaseMat(&U);
    cvReleaseMat(&UT);
    cvReleaseMat(&V);
    cvReleaseMat(&W);
}

int normalizeMatrix( CvMat* in)
{
    if (in == 0)
    {
        printf("parameters is a NULL pointer!");
        return false;
    }

    if (!CV_IS_MAT(in))
    {
        printf("Input parameter must be a matrix!");
        return false;
    }

    int h = in->rows;

    int w = in->cols;
    double s=cvmGet(in,h-1,w-1);
    if (fabs(s)<=0.00000000001)
    {
        printf("cant normalize matridx if last element is zero\n");

    }
    else
    {
        scaleMatrix( in, ((double)1.00)/s);
    }

}


void scaleMatrix(CvMat* in,double s)
{
    if (in == 0)
    {
        printf("parameters is a NULL pointer!");
        return ;
    }

    if (!CV_IS_MAT(in))
    {
        printf("Input parameter must be a matrix!");
        return ;
    }

    int h = in->rows;

    int w = in->cols;

    int i,j;



    for (i=0; i<h; i++)
    {
        for (j=0; j<w; j++)
        {
            double t=cvmGet(in,i,j);
            cvmSet(in,i,j,t*s);

        }

    }

}

int findProjfromcompon(CvMat* P,CvMat* R,CvMat* t,CvMat* K)
{
    int i,j;
    CvMat* Ptemp=cvCreateMat(3,4,CV_64F);
   CvMat* Ttemp=cvCreateMat(3,1,CV_64F);

    checkMatrixOK(P,3,4);
    checkMatrixOK(R,3,3);
    checkMatrixOK(K,3,3);
    checkMatrixOK(t,3,1);

cvMatMul(R, t, Ttemp);
scaleMatrix(Ttemp,-1);


    for (i=0; i<3; i++)
    {
        for (j=0; j<3; j++)
        {

            cvmSet(P,i,j,cvmGet(R,i,j));

        }

    }

    for (j=0; j<3; j++)
    {

        cvmSet(P,j,3,cvmGet(Ttemp,j,0));

    }

    cvMatMul(K, P, Ptemp);

    for (i=0; i<3; i++)
    {
        for (j=0; j<4; j++)
        {

            cvmSet(P,i,j,cvmGet(Ptemp,i,j));

        }

    }


    cvReleaseMat(&Ptemp);
    cvReleaseMat(&Ttemp);
}

double cvTriangulatePointsNframs(int numframes, vector<CvMat*>& projMatrs,vector<CvPoint2D32f>& projPoints,CvPoint3D32f& spPoint)
{

    int i,j;

    if(projMatrs.size()!=numframes || projPoints.size()!=numframes  )
    {
        printf("wrong size of vectors in triangulation\n");
        printf("number of projection matrices was %d and number of points was %d and the unmfram was %d\n",projMatrs.size() , projPoints.size() , numframes);

    }


    for(i=0; i<numframes; i++)
    {

        if( projMatrs[i] == 0)
            CV_Error( CV_StsNullPtr, "Some of parameters is a NULL pointer" );

        if( !CV_IS_MAT(projMatrs[i])  )
            CV_Error( CV_StsUnsupportedFormat, "Input parameters must be matrices" );

        if( projMatrs[i]->cols != 4 || projMatrs[i]->rows != 3 )
            CV_Error( CV_StsUnmatchedSizes, "Size of projection matrices must be 3x4" );

    }







    CvMat matrA;
    double matrA_dat[numframes*3*4];
    matrA = cvMat(numframes*3,4,CV_64F,matrA_dat);

    //CvMat matrU;
    CvMat matrW;
    CvMat matrV;
    //double matrU_dat[9*9];
    double matrW_dat[numframes*3*4];
    double matrV_dat[4*4];

    //matrU = cvMat(6,6,CV_64F,matrU_dat);
    matrW = cvMat(numframes*3,4,CV_64F,matrW_dat);
    matrV = cvMat(4,4,CV_64F,matrV_dat);


    /* Solve system for each point */

    for( j = 0; j < numframes; j++ )/* For each view */
    {
        double x,y;
        x = projPoints[j].x;
        y = projPoints[j].y;
        for( int k = 0; k < 4; k++ )
        {
            cvmSet(&matrA, j*3+0, k, x * cvmGet(projMatrs[j],2,k) -     cvmGet(projMatrs[j],0,k) );
            cvmSet(&matrA, j*3+1, k, y * cvmGet(projMatrs[j],2,k) -     cvmGet(projMatrs[j],1,k) );
            cvmSet(&matrA, j*3+2, k, x * cvmGet(projMatrs[j],1,k) - y * cvmGet(projMatrs[j],0,k) );
        }
    }


    cvSVD(&matrA,&matrW,0,&matrV,CV_SVD_V_T);

    /* Copy computed point */
    spPoint.x=    cvmGet(&matrV,3,0)/cvmGet(&matrV,3,3);/* X */
    spPoint.y=    cvmGet(&matrV,3,1)/cvmGet(&matrV,3,3);/* Y */
    spPoint.z=    cvmGet(&matrV,3,2)/cvmGet(&matrV,3,3);/* Z */


double    rep_error=0;


    CvMat point3D;
    double point3D_dat[4];
    point3D = cvMat(4,1,CV_64F,point3D_dat);

    CvMat point2D;
    double point2D_dat[3];
    point2D = cvMat(3,1,CV_64F,point2D_dat);



    point3D_dat[0] = spPoint.x;
    point3D_dat[1] = spPoint.y;
    point3D_dat[2] = spPoint.z;
    point3D_dat[3] = 1;

    /* !!! Project this point for each camera */
    for( int currCamera = 0; currCamera < numframes; currCamera++ )
    {
        cvmMul(projMatrs[currCamera], &point3D, &point2D);

        double x,y;
        double xr,yr,wr;
        x = projPoints[currCamera].x;
        y = projPoints[currCamera].y;

        wr = (double)point2D_dat[2];
        xr = (double)(point2D_dat[0]/wr);
        yr = (double)(point2D_dat[1]/wr);

        double deltaX,deltaY;
        deltaX = (double)fabs(x-xr);
        deltaY = (double)fabs(y-yr);
        rep_error+=(sqrt(deltaX*deltaX+deltaY*deltaY));
    }
rep_error=rep_error/((double)numframes );

return rep_error;




}
