////////////////////////////////////////////////////////////////////////
//
// hello-world.cpp
//
// This is a simple, introductory OpenCV program. The program reads an
// image from a file, inverts it, and displays the result.
//
////////////////////////////////////////////////////////////////////////
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
#include <math.h>
#include <stddef.h>



#include "focallength.h"
#include "visiongen.h"

const double typicalF= 5000.0;//change this back

using namespace std;

int HRSelfCalibtwoFrame(const CvMat* pF,int width1, int height1, int width2, int height2,double& f1,double& f2,SELFCALIBMETHOD method)
{

    CvMat* K1 = cvCreateMat(3,3, CV_64F);
    CvMat* K2 = cvCreateMat(3,3, CV_64F);

    int retval=HRSelfCalibtwoFrame( pF, width1,  height1,  width2, height2,K1, K2, method);


    f1=cvmGet( K1, 0,0 )  ;
    f2=cvmGet( K2, 0,0 )  ;

    cvReleaseMat(&K1);
    cvReleaseMat(&K2);

    return retval;

}

int HRSelfCalibtwoFrame(const CvMat* pF,int width1, int height1, int width2, int height2,CvMat* K1,CvMat* K2,SELFCALIBMETHOD method)
{
    if (pF==NULL ||K1==NULL || K2==NULL  )
    {
        printf("****matrix is null in self calib,m inintialize the input matrices first\n");
        return -1;

    }

    if ( pF->rows!=3 || pF->cols!=3 || K1->rows!=3 || K1->cols!=3  ||  K2->rows!=3 || K2->cols!=3 )
    {
        cout<<"one of the input matrices has the wrong size in self calib, check to see they are all 3X3"<<endl;
        return -1;
    }

    if (method==STRUM)
    {
        if (width1!=width2 || height1!=height2)
        {
            cout<<"using the strum method both images need to have the same size"<<endl;
        }

        double foc;
        estimateFocalLengthStrum(pF,width1,height1,foc);

        cvSetIdentity(K1);


        cvmSet(K1, 0, 0, foc);
        cvmSet(K1, 1, 1, foc);
        cvmSet(K1, 0, 2, ((double)(width1/2.00)));
        cvmSet(K1, 1, 2, ((double)(height1/2.00)));

        cvCopy(K1,K2);

    }

    if (method==POLLEFEY)
    {
        double foc1,foc2;
        estimateFocalLengthsPollefey(pF,width1,height1,width2,height2,foc1,foc2);

//        //if one of them isnan then the other should be isnan
//        if(isnan(foc1)) foc2=foc1;
//        if(isnan(foc2)) foc1=foc2;

        cvSetIdentity(K1);
        cvSetIdentity(K2);

        cvmSet(K1, 0, 0, foc1);
        cvmSet(K1, 1, 1, foc1);
        cvmSet(K1, 0, 2, ((double)(width1/2.00)));
        cvmSet(K1, 1, 2, ((double)(height1/2.00)));

        cvmSet(K2, 0, 0, foc2);
        cvmSet(K2, 1, 1, foc2);
        cvmSet(K2, 0, 2, ((double)(width2/2.00)));
        cvmSet(K2, 1, 2, ((double)(height2/2.00)));



    }
    if (method==POLLEFEYVISUAL)
    {
        double foc1,foc2;
        estimateFocalLengthsPollefeyVisual(pF,width1,height1,width2,height2,K1,K2);

    }
    if (method==HARTLEY)
    {
        double foc1,foc2;

        estimateFocalLengthsHartley(pF,width1,height1,width2,height2,foc1,foc2);


//
//          //if one of them isnan then the other should be isnan
//        if(isnan(foc1)) foc2=foc1;
//        if(isnan(foc2)) foc1=foc2;

        cvSetIdentity(K1);
        cvSetIdentity(K2);

        cvmSet(K1, 0, 0, foc1);
        cvmSet(K1, 1, 1, foc1);
        cvmSet(K1, 0, 2, ((double)(width1/2.00)));
        cvmSet(K1, 1, 2, ((double)(height1/2.00)));

        cvmSet(K2, 0, 0, foc2);
        cvmSet(K2, 1, 1, foc2);
        cvmSet(K2, 0, 2, ((double)(width2/2.00)));
        cvmSet(K2, 1, 2, ((double)(height2/2.00)));







    }


}







/* Copyright (C) 1978-1999 Ken Turkowski. <turk@computer.org>
 *
 * All rights reserved.
 *
 * Warranty Information
 *  Even though I have reviewed this software, I make no warranty
 *  or representation, either express or implied, with respect to this
 *  software, its quality, accuracy, merchantability, or fitness for a
 *  particular purpose.  As a result, this software is provided "as is,"
 *  and you, its user, are assuming the entire risk as to its quality
 *  and accuracy.
 *
 * This code may be used and freely distributed as long as it includes
 * this copyright notice and the above warranty information.
 */



#ifdef DOUBLE_PRECISION
# define FLOAT double
# define PARAMFLOAT double_t
#else   /* DOUBLE_PRECISION */
# define FLOAT float
# define PARAMFLOAT float_t
#endif  /* DOUBLE_PRECISION */



/*******************************************************************************
 * FindQuadraticRoots
 *
 *      return(2): 2 real roots
 *      return(1): 1 real, double root
 *      return(0): 2 complex roots
 *
 *  The imaginary component may be NULL if not desired.
 *******************************************************************************/

#define a (coeff[2])    /* Quadratic coefficient */
#define b (coeff[1])    /* Linear coefficient */
#define c (coeff[0])    /* Constant coefficient */

int FindQuadraticRoots(const FLOAT coeff[3], FLOAT re[2], FLOAT im[2])
{
    register double_t d = b * b - 4 * a * c;

    /* Two real, distinct roots */
    if (d > 0)
    {
        register double_t q;
        d = sqrt(d);
        q = (-b + ((b < 0) ? -d : d)) * 0.5;
        re[0] = q / a;
        re[1] = c / q;
        return (2);
    }

    /* One real double root */
    else if (d == 0)
    {
        re[0] = re[1] = -b / (2 * a);
        return (1);
    }

    /* Two complex conjugate roots */
    else                                        /* d < 0 */
    {
        re[0] = re[1] = -b / (2 * a);
        if (im != NULL)
        {
            im[0] = d / (2 * a);
            im[1] = -im[0];
        }
        return (0);
    }

}

#undef a
#undef b
#undef c
//solves:
//       axy+by+cx+d=0
//       exy+fx+gy+h=0
int solveQuadratictwoUnknowns(const FLOAT coeff1[4], const FLOAT coeff2[4], FLOAT re[2], FLOAT im[2])
{

    double a =(coeff1[3])   ;
    double b =(coeff1[2])   ;
    double c =(coeff1[1])   ;
    double d =(coeff1[0])   ;

    double e =(coeff1[3])   ;
    double f =(coeff1[2])   ;
    double g =(coeff1[1])   ;
    double h =(coeff1[0])   ;




}
int  estimateFocalLengthsPollefeyVisual(const CvMat* pF, int width1, int height1,int width2, int height2,CvMat* K1,CvMat* K2)
{
    //this is same as pollefey's method but this is a simpler version deriven from his visual modelling paper

    //estim
    int i;
    CvMat* P1 = cvCreateMat(3,4, CV_64F);
    CvMat* P2 = cvCreateMat(3,4, CV_64F);

    CvMat* P1_normalized = cvCreateMat(3,4, CV_64F);
    CvMat* P2_normalized = cvCreateMat(3,4, CV_64F);

    CvMat* A = cvCreateMat(12,10, CV_64F);
    CvMat* Q = cvCreateMat(4,4, CV_64F);

    CvMat* P2R3= cvCreateMat(1,4, CV_64F);



    ProjectiveMatFromF(pF, P1,P2);


    normalizeProjectionMatrix(P1,P1_normalized,width1,height1);
    normalizeProjectionMatrix(P2,P2_normalized,width2,height2);


//take the 3rd row of P to pass it to the absolute quadric since it needs it to find omega
// i think we need the normalized matrices
    for(i=0; i<4; i++)
    {
        cvmSet(P2R3,0,i, cvmGet(P2_normalized,2,i));
    }


    formAfromP1P2PollefeyVisual(P1_normalized,P2_normalized, A);
    absoluteQuadricfromAVisual(A,Q,P2R3);


    extractKfromQ(Q,P1_normalized,K1);
    extractKfromQ(Q,P2_normalized,K2);

//here print K and also denormalize it, if you remember we normalized the projection matrices




    cvReleaseMat(&P2R3);

    cvReleaseMat(&P1);
    cvReleaseMat(&P2);



    cvReleaseMat(&P1_normalized);
    cvReleaseMat(&P2_normalized);

    cvReleaseMat(&A);
    cvReleaseMat(&Q);



}
int extractKfromQ(const CvMat* pQ,const CvMat* pPnormalized,CvMat* pK)
{
printf(" extract intrinsic matrix, dont forget that the projection matrices were initially normalized, so we have to do some kind of denormalization\n");
}
int normalizeProjectionMatrix(const CvMat* in, CvMat* inNormalized,int width, int height)
{
//this normalization is taken from the paper:Visual modeling with a hand-held camera
    CvMat* Knorm = cvCreateMat(3,3, CV_64F);

    cvSetIdentity(Knorm);


    cvmSet(Knorm,0,0, 1.0 / (width + height));
    cvmSet(Knorm,0,1, 0);
    cvmSet(Knorm,0,2, -width / ((width + height) * 2.0));
    cvmSet(Knorm,1,0, 0);
    cvmSet(Knorm,1,1, 1.0 / (width + height));
    cvmSet(Knorm,1,2, -height / ((width + height) * 2.0));
    cvmSet(Knorm,2,0, 0);
    cvmSet(Knorm,2,1, 0);
    cvmSet(Knorm,2,2, 1);

    cvMul(Knorm, in, inNormalized);      // Ma.*Mb  -> Mc


    cvReleaseMat(&Knorm);


}
int  estimateFocalLengthsPollefey(const CvMat* pF, int width1, int height1,int width2, int height2,double& foc1,double& foc2)
{
    //estim
    CvMat* P1 = cvCreateMat(3,4, CV_64F);
    CvMat* P2 = cvCreateMat(3,4, CV_64F);


    ProjectiveMatFromF(pF, P1,P2);



    CvMat* A = cvCreateMat(8,5, CV_64F);
    CvMat* Y = cvCreateMat(8,1, CV_64F);
    CvMat* Q = cvCreateMat(4,4, CV_64F);


    formAYfromP1P2Pollefey(P1,P2, A,Y);
    absoluteQuadricfromAY(A,Y,Q);

    cvReleaseMat(&A);
    cvReleaseMat(&Y);

    cvReleaseMat(&Q);

    cvReleaseMat(&P1);
    cvReleaseMat(&P2);



}
int absoluteQuadricfromAVisual(const CvMat* pA, CvMat* Q,CvMat* P2R3)
{
    int i,j;
    double omg;
    double coeff;
    //the visual is the modified pollefey method outlined in his visual modelling paoer
    CvMat* U = cvCreateMat(3,3, CV_64F);
    CvMat* V = cvCreateMat(3,3, CV_64F);
    CvMat* W = cvCreateMat(3,3, CV_64F);
    CvMat* AC;
    CvMat* C = cvCreateMat(10,1, CV_64F);
    CvMat* P2R3_T= cvCreateMat(4,1, CV_64F);
    CvMat* P2Q_intermediate= cvCreateMat(1,4, CV_64F);
    CvMat* omegaMat= cvCreateMat(1,4, CV_64F);
    int count=0;

    cvTranspose(P2R3, P2R3_T);
    omg=((double)1.00);

    while(count<6)
    {


        AC=cvCloneMat(pA);


        coeff=0;

        for(i=0; i<10; i++)
        {
            if((i+0)%6==0 || (i+1)%6==0)
            {
                coeff=1.00/(9.0*omg);
            }
            if((i+2)%6==0 )
            {
                coeff=1.00/(0.2*omg);
            }
            if((i+3)%6==0 || (i+4)%6==0)
            {
                coeff=1.00/(0.1*omg);
            }
            if((i+5)%6==0 )
            {
                coeff=1.00/(0.01*omg);
            }
            for(j=0; j<10; j++)
            {

                cvmSet(AC,i,j,cvmGet(pA,0,j)*coeff);

            }
        }

        cvSVD( AC, W,  U, V );

//taking the solution out
        for(i=0; i<10; i++)
        {
            cvmSet(C,i,0,cvmGet(V,i,9));

        }

//putting C into Q
        int z=0;
        for(i=0; i<4; i++)
        {
            for(j=0; j<4; j++)
            {
                cvmSet(Q,i,j,cvmGet(C,z,0));
                z++;
            }
        }

//calculate new omega

        cvMul(P2R3, Q, P2Q_intermediate);      // Ma.*Mb  -> Mc
        cvMul(P2Q_intermediate, P2R3_T,omegaMat);      // Ma.*Mb  -> Mc

        omg=cvmGet(omegaMat,0,0);

        printf("the new omega is %f\n",omg);
        count++;
    }

    enforceRank3forQ(Q);

    cvReleaseMat(&AC);
    cvReleaseMat(&C);
    cvReleaseMat(&P2Q_intermediate);
    cvReleaseMat(&omegaMat);

    cvReleaseMat(&U);
    cvReleaseMat(&V);
    cvReleaseMat(&W);


    cvReleaseMat(&P2R3_T);




}

int enforceRank3forQ(const CvMat* pQ)
{
    printf("have not enforced rank 3 yet but do it, you can do it from the code for F\n");


}
int absoluteQuadricfromAY(const CvMat* pA, CvMat* pY,CvMat* Q)
{
    CvMat* U = cvCreateMat(3,3, CV_64F);
    CvMat* V = cvCreateMat(3,3, CV_64F);
    CvMat* W = cvCreateMat(3,3, CV_64F);
    CvMat* AC;

    AC=cvCloneMat(pA);


    cvSVD( AC, W,  U, V );  //change all of the below back to U

    enforceRank3forQ(Q);

    cvReleaseMat(&U);
    cvReleaseMat(&V);
    cvReleaseMat(&W);
}
int formAfromP1P2PollefeyVisual(const CvMat* pP1,const CvMat* pP2, CvMat* pA)
{

    cvmSet(pA,0,0, pow( cvmGet(pP1,0,0), 2) - pow( cvmGet(pP1,2,0), 2));
    cvmSet(pA,0,1, -2 * cvmGet(pP1,2,0)* cvmGet(pP1,2,1)+ 2 * cvmGet(pP1,0,0)* cvmGet(pP1,0,1));
    cvmSet(pA,0,2, 2 * cvmGet(pP1,0,0)* cvmGet(pP1,0,2)- 2 * cvmGet(pP1,2,0)* cvmGet(pP1,2,2));
    cvmSet(pA,0,3, 2 * cvmGet(pP1,0,0)* cvmGet(pP1,0,3)- 2 * cvmGet(pP1,2,3)* cvmGet(pP1,2,0));
    cvmSet(pA,0,4, pow( cvmGet(pP1,0,1), 2) - pow( cvmGet(pP1,2,1), 2));
    cvmSet(pA,0,5, 2 * cvmGet(pP1,0,1)* cvmGet(pP1,0,2)- 2 * cvmGet(pP1,2,2)* cvmGet(pP1,2,1));
    cvmSet(pA,0,6, 2 * cvmGet(pP1,0,3)* cvmGet(pP1,0,1)- 2 * cvmGet(pP1,2,3)* cvmGet(pP1,2,1));
    cvmSet(pA,0,7, -pow( cvmGet(pP1,2,2), 2) + pow( cvmGet(pP1,0,2), 2));
    cvmSet(pA,0,8, -2 * cvmGet(pP1,2,2)* cvmGet(pP1,2,3)+ 2 * cvmGet(pP1,0,2)* cvmGet(pP1,0,3));
    cvmSet(pA,0,9, pow( cvmGet(pP1,0,3), 2) - pow( cvmGet(pP1,2,3), 2));
    cvmSet(pA,1,0, pow( cvmGet(pP1,1,0), 2) - pow( cvmGet(pP1,2,0), 2));
    cvmSet(pA,1,1, -2 * cvmGet(pP1,2,0)* cvmGet(pP1,2,1)+ 2 * cvmGet(pP1,1,0)* cvmGet(pP1,1,1));
    cvmSet(pA,1,2, 2 * cvmGet(pP1,1,0)* cvmGet(pP1,1,2)- 2 * cvmGet(pP1,2,0)* cvmGet(pP1,2,2));
    cvmSet(pA,1,3, 2 * cvmGet(pP1,1,0)* cvmGet(pP1,1,3)- 2 * cvmGet(pP1,2,3)* cvmGet(pP1,2,0));
    cvmSet(pA,1,4, pow( cvmGet(pP1,1,1), 2) - pow( cvmGet(pP1,2,1), 2));
    cvmSet(pA,1,5, 2 * cvmGet(pP1,1,1)* cvmGet(pP1,1,2)- 2 * cvmGet(pP1,2,2)* cvmGet(pP1,2,1));
    cvmSet(pA,1,6, 2 * cvmGet(pP1,1,3)* cvmGet(pP1,1,1)- 2 * cvmGet(pP1,2,3)* cvmGet(pP1,2,1));
    cvmSet(pA,1,7, -pow( cvmGet(pP1,2,2), 2) + pow( cvmGet(pP1,1,2), 2));
    cvmSet(pA,1,8, -2 * cvmGet(pP1,2,2)* cvmGet(pP1,2,3)+ 2 * cvmGet(pP1,1,2)* cvmGet(pP1,1,3));
    cvmSet(pA,1,9, pow( cvmGet(pP1,1,3), 2) - pow( cvmGet(pP1,2,3), 2));
    cvmSet(pA,2,0, pow( cvmGet(pP1,0,0), 2) - pow( cvmGet(pP1,1,0), 2));
    cvmSet(pA,2,1, -2 * cvmGet(pP1,1,0)* cvmGet(pP1,1,1)+ 2 * cvmGet(pP1,0,0)* cvmGet(pP1,0,1));
    cvmSet(pA,2,2, 2 * cvmGet(pP1,0,0)* cvmGet(pP1,0,2)- 2 * cvmGet(pP1,1,0)* cvmGet(pP1,1,2));
    cvmSet(pA,2,3, 2 * cvmGet(pP1,0,0)* cvmGet(pP1,0,3)- 2 * cvmGet(pP1,1,0)* cvmGet(pP1,1,3));
    cvmSet(pA,2,4, pow( cvmGet(pP1,0,1), 2) - pow( cvmGet(pP1,1,1), 2));
    cvmSet(pA,2,5, 2 * cvmGet(pP1,0,1)* cvmGet(pP1,0,2)- 2 * cvmGet(pP1,1,1)* cvmGet(pP1,1,2));
    cvmSet(pA,2,6, 2 * cvmGet(pP1,0,3)* cvmGet(pP1,0,1)- 2 * cvmGet(pP1,1,3)* cvmGet(pP1,1,1));
    cvmSet(pA,2,7, -pow( cvmGet(pP1,1,2), 2) + pow( cvmGet(pP1,0,2), 2));
    cvmSet(pA,2,8, -2 * cvmGet(pP1,1,2)* cvmGet(pP1,1,3)+ 2 * cvmGet(pP1,0,2)* cvmGet(pP1,0,3));
    cvmSet(pA,2,9, pow( cvmGet(pP1,0,3), 2) - pow( cvmGet(pP1,1,3), 2));
    cvmSet(pA,3,0, cvmGet(pP1,1,0)* cvmGet(pP1,0,0));
    cvmSet(pA,3,1, cvmGet(pP1,1,1)* cvmGet(pP1,0,0)+ cvmGet(pP1,1,0)* cvmGet(pP1,0,1));
    cvmSet(pA,3,2, cvmGet(pP1,1,0)* cvmGet(pP1,0,2)+ cvmGet(pP1,1,2)* cvmGet(pP1,0,0));
    cvmSet(pA,3,3, cvmGet(pP1,1,0)* cvmGet(pP1,0,3)+ cvmGet(pP1,1,3)* cvmGet(pP1,0,0));
    cvmSet(pA,3,4, cvmGet(pP1,1,1)* cvmGet(pP1,0,1));
    cvmSet(pA,3,5, cvmGet(pP1,1,2)* cvmGet(pP1,0,1)+ cvmGet(pP1,1,1)* cvmGet(pP1,0,2));
    cvmSet(pA,3,6, cvmGet(pP1,1,3)* cvmGet(pP1,0,1)+ cvmGet(pP1,1,1)* cvmGet(pP1,0,3));
    cvmSet(pA,3,7, cvmGet(pP1,1,2)* cvmGet(pP1,0,2));
    cvmSet(pA,3,8, cvmGet(pP1,1,3)* cvmGet(pP1,0,2)+ cvmGet(pP1,1,2)* cvmGet(pP1,0,3));
    cvmSet(pA,3,9, cvmGet(pP1,1,3)* cvmGet(pP1,0,3));
    cvmSet(pA,4,0, cvmGet(pP1,2,0)* cvmGet(pP1,1,0));
    cvmSet(pA,4,1, cvmGet(pP1,2,1)* cvmGet(pP1,1,0)+ cvmGet(pP1,2,0)* cvmGet(pP1,1,1));
    cvmSet(pA,4,2, cvmGet(pP1,2,0)* cvmGet(pP1,1,2)+ cvmGet(pP1,2,2)* cvmGet(pP1,1,0));
    cvmSet(pA,4,3, cvmGet(pP1,2,0)* cvmGet(pP1,1,3)+ cvmGet(pP1,2,3)* cvmGet(pP1,1,0));
    cvmSet(pA,4,4, cvmGet(pP1,2,1)* cvmGet(pP1,1,1));
    cvmSet(pA,4,5, cvmGet(pP1,2,2)* cvmGet(pP1,1,1)+ cvmGet(pP1,2,1)* cvmGet(pP1,1,2));
    cvmSet(pA,4,6, cvmGet(pP1,2,3)* cvmGet(pP1,1,1)+ cvmGet(pP1,2,1)* cvmGet(pP1,1,3));
    cvmSet(pA,4,7, cvmGet(pP1,2,2)* cvmGet(pP1,1,2));
    cvmSet(pA,4,8, cvmGet(pP1,2,3)* cvmGet(pP1,1,2)+ cvmGet(pP1,2,2)* cvmGet(pP1,1,3));
    cvmSet(pA,4,9, cvmGet(pP1,2,3)* cvmGet(pP1,1,3));
    cvmSet(pA,5,0, cvmGet(pP1,2,0)* cvmGet(pP1,0,0));
    cvmSet(pA,5,1, cvmGet(pP1,2,1)* cvmGet(pP1,0,0)+ cvmGet(pP1,2,0)* cvmGet(pP1,0,1));
    cvmSet(pA,5,2, cvmGet(pP1,2,0)* cvmGet(pP1,0,2)+ cvmGet(pP1,2,2)* cvmGet(pP1,0,0));
    cvmSet(pA,5,3, cvmGet(pP1,2,0)* cvmGet(pP1,0,3)+ cvmGet(pP1,2,3)* cvmGet(pP1,0,0));
    cvmSet(pA,5,4, cvmGet(pP1,2,1)* cvmGet(pP1,0,1));
    cvmSet(pA,5,5, cvmGet(pP1,2,2)* cvmGet(pP1,0,1)+ cvmGet(pP1,2,1)* cvmGet(pP1,0,2));
    cvmSet(pA,5,6, cvmGet(pP1,2,3)* cvmGet(pP1,0,1)+ cvmGet(pP1,2,1)* cvmGet(pP1,0,3));
    cvmSet(pA,5,7, cvmGet(pP1,2,2)* cvmGet(pP1,0,2));
    cvmSet(pA,5,8, cvmGet(pP1,2,3)* cvmGet(pP1,0,2)+ cvmGet(pP1,2,2)* cvmGet(pP1,0,3));
    cvmSet(pA,5,9, cvmGet(pP1,2,3)* cvmGet(pP1,0,3));




/// for the second P


    cvmSet(pA,6+0,0, pow( cvmGet(pP2,0,0), 2) - pow( cvmGet(pP2,2,0), 2));
    cvmSet(pA,6+0,1, -2 * cvmGet(pP2,2,0)* cvmGet(pP2,2,1)+ 2 * cvmGet(pP2,0,0)* cvmGet(pP2,0,1));
    cvmSet(pA,6+0,2, 2 * cvmGet(pP2,0,0)* cvmGet(pP2,0,2)- 2 * cvmGet(pP2,2,0)* cvmGet(pP2,2,2));
    cvmSet(pA,6+0,3, 2 * cvmGet(pP2,0,0)* cvmGet(pP2,0,3)- 2 * cvmGet(pP2,2,3)* cvmGet(pP2,2,0));
    cvmSet(pA,6+0,4, pow( cvmGet(pP2,0,1), 2) - pow( cvmGet(pP2,2,1), 2));
    cvmSet(pA,6+0,5, 2 * cvmGet(pP2,0,1)* cvmGet(pP2,0,2)- 2 * cvmGet(pP2,2,2)* cvmGet(pP2,2,1));
    cvmSet(pA,6+0,6, 2 * cvmGet(pP2,0,3)* cvmGet(pP2,0,1)- 2 * cvmGet(pP2,2,3)* cvmGet(pP2,2,1));
    cvmSet(pA,6+0,7, -pow( cvmGet(pP2,2,2), 2) + pow( cvmGet(pP2,0,2), 2));
    cvmSet(pA,6+0,8, -2 * cvmGet(pP2,2,2)* cvmGet(pP2,2,3)+ 2 * cvmGet(pP2,0,2)* cvmGet(pP2,0,3));
    cvmSet(pA,6+0,9, pow( cvmGet(pP2,0,3), 2) - pow( cvmGet(pP2,2,3), 2));
    cvmSet(pA,6+1,0, pow( cvmGet(pP2,1,0), 2) - pow( cvmGet(pP2,2,0), 2));
    cvmSet(pA,6+1,1, -2 * cvmGet(pP2,2,0)* cvmGet(pP2,2,1)+ 2 * cvmGet(pP2,1,0)* cvmGet(pP2,1,1));
    cvmSet(pA,6+1,2, 2 * cvmGet(pP2,1,0)* cvmGet(pP2,1,2)- 2 * cvmGet(pP2,2,0)* cvmGet(pP2,2,2));
    cvmSet(pA,6+1,3, 2 * cvmGet(pP2,1,0)* cvmGet(pP2,1,3)- 2 * cvmGet(pP2,2,3)* cvmGet(pP2,2,0));
    cvmSet(pA,6+1,4, pow( cvmGet(pP2,1,1), 2) - pow( cvmGet(pP2,2,1), 2));
    cvmSet(pA,6+1,5, 2 * cvmGet(pP2,1,1)* cvmGet(pP2,1,2)- 2 * cvmGet(pP2,2,2)* cvmGet(pP2,2,1));
    cvmSet(pA,6+1,6, 2 * cvmGet(pP2,1,3)* cvmGet(pP2,1,1)- 2 * cvmGet(pP2,2,3)* cvmGet(pP2,2,1));
    cvmSet(pA,6+1,7, -pow( cvmGet(pP2,2,2), 2) + pow( cvmGet(pP2,1,2), 2));
    cvmSet(pA,6+1,8, -2 * cvmGet(pP2,2,2)* cvmGet(pP2,2,3)+ 2 * cvmGet(pP2,1,2)* cvmGet(pP2,1,3));
    cvmSet(pA,6+1,9, pow( cvmGet(pP2,1,3), 2) - pow( cvmGet(pP2,2,3), 2));
    cvmSet(pA,6+2,0, pow( cvmGet(pP2,0,0), 2) - pow( cvmGet(pP2,1,0), 2));
    cvmSet(pA,6+2,1, -2 * cvmGet(pP2,1,0)* cvmGet(pP2,1,1)+ 2 * cvmGet(pP2,0,0)* cvmGet(pP2,0,1));
    cvmSet(pA,6+2,2, 2 * cvmGet(pP2,0,0)* cvmGet(pP2,0,2)- 2 * cvmGet(pP2,1,0)* cvmGet(pP2,1,2));
    cvmSet(pA,6+2,3, 2 * cvmGet(pP2,0,0)* cvmGet(pP2,0,3)- 2 * cvmGet(pP2,1,0)* cvmGet(pP2,1,3));
    cvmSet(pA,6+2,4, pow( cvmGet(pP2,0,1), 2) - pow( cvmGet(pP2,1,1), 2));
    cvmSet(pA,6+2,5, 2 * cvmGet(pP2,0,1)* cvmGet(pP2,0,2)- 2 * cvmGet(pP2,1,1)* cvmGet(pP2,1,2));
    cvmSet(pA,6+2,6, 2 * cvmGet(pP2,0,3)* cvmGet(pP2,0,1)- 2 * cvmGet(pP2,1,3)* cvmGet(pP2,1,1));
    cvmSet(pA,6+2,7, -pow( cvmGet(pP2,1,2), 2) + pow( cvmGet(pP2,0,2), 2));
    cvmSet(pA,6+2,8, -2 * cvmGet(pP2,1,2)* cvmGet(pP2,1,3)+ 2 * cvmGet(pP2,0,2)* cvmGet(pP2,0,3));
    cvmSet(pA,6+2,9, pow( cvmGet(pP2,0,3), 2) - pow( cvmGet(pP2,1,3), 2));
    cvmSet(pA,6+3,0, cvmGet(pP2,1,0)* cvmGet(pP2,0,0));
    cvmSet(pA,6+3,1, cvmGet(pP2,1,1)* cvmGet(pP2,0,0)+ cvmGet(pP2,1,0)* cvmGet(pP2,0,1));
    cvmSet(pA,6+3,2, cvmGet(pP2,1,0)* cvmGet(pP2,0,2)+ cvmGet(pP2,1,2)* cvmGet(pP2,0,0));
    cvmSet(pA,6+3,3, cvmGet(pP2,1,0)* cvmGet(pP2,0,3)+ cvmGet(pP2,1,3)* cvmGet(pP2,0,0));
    cvmSet(pA,6+3,4, cvmGet(pP2,1,1)* cvmGet(pP2,0,1));
    cvmSet(pA,6+3,5, cvmGet(pP2,1,2)* cvmGet(pP2,0,1)+ cvmGet(pP2,1,1)* cvmGet(pP2,0,2));
    cvmSet(pA,6+3,6, cvmGet(pP2,1,3)* cvmGet(pP2,0,1)+ cvmGet(pP2,1,1)* cvmGet(pP2,0,3));
    cvmSet(pA,6+3,7, cvmGet(pP2,1,2)* cvmGet(pP2,0,2));
    cvmSet(pA,6+3,8, cvmGet(pP2,1,3)* cvmGet(pP2,0,2)+ cvmGet(pP2,1,2)* cvmGet(pP2,0,3));
    cvmSet(pA,6+3,9, cvmGet(pP2,1,3)* cvmGet(pP2,0,3));
    cvmSet(pA,6+4,0, cvmGet(pP2,2,0)* cvmGet(pP2,1,0));
    cvmSet(pA,6+4,1, cvmGet(pP2,2,1)* cvmGet(pP2,1,0)+ cvmGet(pP2,2,0)* cvmGet(pP2,1,1));
    cvmSet(pA,6+4,2, cvmGet(pP2,2,0)* cvmGet(pP2,1,2)+ cvmGet(pP2,2,2)* cvmGet(pP2,1,0));
    cvmSet(pA,6+4,3, cvmGet(pP2,2,0)* cvmGet(pP2,1,3)+ cvmGet(pP2,2,3)* cvmGet(pP2,1,0));
    cvmSet(pA,6+4,4, cvmGet(pP2,2,1)* cvmGet(pP2,1,1));
    cvmSet(pA,6+4,5, cvmGet(pP2,2,2)* cvmGet(pP2,1,1)+ cvmGet(pP2,2,1)* cvmGet(pP2,1,2));
    cvmSet(pA,6+4,6, cvmGet(pP2,2,3)* cvmGet(pP2,1,1)+ cvmGet(pP2,2,1)* cvmGet(pP2,1,3));
    cvmSet(pA,6+4,7, cvmGet(pP2,2,2)* cvmGet(pP2,1,2));
    cvmSet(pA,6+4,8, cvmGet(pP2,2,3)* cvmGet(pP2,1,2)+ cvmGet(pP2,2,2)* cvmGet(pP2,1,3));
    cvmSet(pA,6+4,9, cvmGet(pP2,2,3)* cvmGet(pP2,1,3));
    cvmSet(pA,6+5,0, cvmGet(pP2,2,0)* cvmGet(pP2,0,0));
    cvmSet(pA,6+5,1, cvmGet(pP2,2,1)* cvmGet(pP2,0,0)+ cvmGet(pP2,2,0)* cvmGet(pP2,0,1));
    cvmSet(pA,6+5,2, cvmGet(pP2,2,0)* cvmGet(pP2,0,2)+ cvmGet(pP2,2,2)* cvmGet(pP2,0,0));
    cvmSet(pA,6+5,3, cvmGet(pP2,2,0)* cvmGet(pP2,0,3)+ cvmGet(pP2,2,3)* cvmGet(pP2,0,0));
    cvmSet(pA,6+5,4, cvmGet(pP2,2,1)* cvmGet(pP2,0,1));
    cvmSet(pA,6+5,5, cvmGet(pP2,2,2)* cvmGet(pP2,0,1)+ cvmGet(pP2,2,1)* cvmGet(pP2,0,2));
    cvmSet(pA,6+5,6, cvmGet(pP2,2,3)* cvmGet(pP2,0,1)+ cvmGet(pP2,2,1)* cvmGet(pP2,0,3));
    cvmSet(pA,6+5,7, cvmGet(pP2,2,2)* cvmGet(pP2,0,2));
    cvmSet(pA,6+5,8, cvmGet(pP2,2,3)* cvmGet(pP2,0,2)+ cvmGet(pP2,2,2)* cvmGet(pP2,0,3));
    cvmSet(pA,6+5,9, cvmGet(pP2,2,3)* cvmGet(pP2,0,3));


}
int formAYfromP1P2Pollefey(const CvMat* pP1,const CvMat* pP2, CvMat* pA, CvMat* pY)
{
    cvmSet(pA,0,0 , pow(cvmGet(pP1,0,0), 2.0) + pow(cvmGet(pP1,0,1), 2.0) - pow(cvmGet(pP1,1,0), 2.0) - pow(cvmGet(pP1,1,1), 2.0));
    cvmSet(pA,0,1 , 2.0 * cvmGet(pP1,0,0) * cvmGet(pP1,0,3) - 2.0 * cvmGet(pP1,1,0) * cvmGet(pP1,1,3));
    cvmSet(pA,0,2 , 2.0 * cvmGet(pP1,0,1) * cvmGet(pP1,0,3) - 2.0 * cvmGet(pP1,1,1) * cvmGet(pP1,1,3));
    cvmSet(pA,0,3 , 2.0 * cvmGet(pP1,0,2) * cvmGet(pP1,0,3) - 2.0 * cvmGet(pP1,1,2) * cvmGet(pP1,1,3));
    cvmSet(pA,0,4 , pow(cvmGet(pP1,0,3), 2.0) - pow(cvmGet(pP1,1,3), 2.0));
    cvmSet(pA,1,0 , 2.0 * cvmGet(pP1,1,0) * cvmGet(pP1,0,0) + 2.0 * cvmGet(pP1,1,1) * cvmGet(pP1,0,1));
    cvmSet(pA,1,1 , 2.0 * cvmGet(pP1,1,0) * cvmGet(pP1,0,3) + 2.0 * cvmGet(pP1,1,3) * cvmGet(pP1,0,0));
    cvmSet(pA,1,2 , 2.0 * cvmGet(pP1,1,1) * cvmGet(pP1,0,3) + 2.0 * cvmGet(pP1,1,3) * cvmGet(pP1,0,1));
    cvmSet(pA,1,3 , 2.0 * cvmGet(pP1,1,2) * cvmGet(pP1,0,3) + 2.0 * cvmGet(pP1,1,3) * cvmGet(pP1,0,2));
    cvmSet(pA,1,4 , 2.0 * cvmGet(pP1,1,3) * cvmGet(pP1,0,3));
    cvmSet(pA,2,0 , 2.0 * cvmGet(pP1,2,0) * cvmGet(pP1,0,0) + 2.0 * cvmGet(pP1,2,1) * cvmGet(pP1,0,1));
    cvmSet(pA,2,1 , 2.0 * cvmGet(pP1,2,0) * cvmGet(pP1,0,3) + 2.0 * cvmGet(pP1,2,3) * cvmGet(pP1,0,0));
    cvmSet(pA,2,2 , 2.0 * cvmGet(pP1,2,1) * cvmGet(pP1,0,3) + 2.0 * cvmGet(pP1,2,3) * cvmGet(pP1,0,1));
    cvmSet(pA,2,3 , 2.0 * cvmGet(pP1,2,2) * cvmGet(pP1,0,3) + 2.0 * cvmGet(pP1,2,3) * cvmGet(pP1,0,2));
    cvmSet(pA,2,4 , 2.0 * cvmGet(pP1,2,3) * cvmGet(pP1,0,3));
    cvmSet(pA,3,0 , 2.0 * cvmGet(pP1,2,0) * cvmGet(pP1,1,0) + 2.0 * cvmGet(pP1,2,1) * cvmGet(pP1,1,1));
    cvmSet(pA,3,1 , 2.0 * cvmGet(pP1,2,0) * cvmGet(pP1,1,3) + 2.0 * cvmGet(pP1,2,3) * cvmGet(pP1,1,0));
    cvmSet(pA,3,2 , 2.0 * cvmGet(pP1,2,1) * cvmGet(pP1,1,3) + 2.0 * cvmGet(pP1,2,3) * cvmGet(pP1,1,1));
    cvmSet(pA,3,3 , 2.0 * cvmGet(pP1,2,2) * cvmGet(pP1,1,3) + 2.0 * cvmGet(pP1,2,3) * cvmGet(pP1,1,2));
    cvmSet(pA,3,4 , 2.0 * cvmGet(pP1,2,3) * cvmGet(pP1,1,3));


//2nd projection matrix
    cvmSet(pA,4,0 , pow(cvmGet(pP2,0,0), 2.0) + pow(cvmGet(pP2,0,1), 2.0) - pow(cvmGet(pP2,1,0), 2.0) - pow(cvmGet(pP2,1,1), 2.0));
    cvmSet(pA,4,1 , 2.0 * cvmGet(pP2,0,0) * cvmGet(pP2,0,3) - 2.0 * cvmGet(pP2,1,0) * cvmGet(pP2,1,3));
    cvmSet(pA,4,2 , 2.0 * cvmGet(pP2,0,1) * cvmGet(pP2,0,3) - 2.0 * cvmGet(pP2,1,1) * cvmGet(pP2,1,3));
    cvmSet(pA,4,3 , 2.0 * cvmGet(pP2,0,2) * cvmGet(pP2,0,3) - 2.0 * cvmGet(pP2,1,2) * cvmGet(pP2,1,3));
    cvmSet(pA,4,4 , pow(cvmGet(pP2,0,3), 2.0) - pow(cvmGet(pP2,1,3), 2.0));
    cvmSet(pA,5,0 , 2.0 * cvmGet(pP2,1,0) * cvmGet(pP2,0,0) + 2.0 * cvmGet(pP2,1,1) * cvmGet(pP2,0,1));
    cvmSet(pA,5,1 , 2.0 * cvmGet(pP2,1,0) * cvmGet(pP2,0,3) + 2.0 * cvmGet(pP2,1,3) * cvmGet(pP2,0,0));
    cvmSet(pA,5,2 , 2.0 * cvmGet(pP2,1,1) * cvmGet(pP2,0,3) + 2.0 * cvmGet(pP2,1,3) * cvmGet(pP2,0,1));
    cvmSet(pA,5,3 , 2.0 * cvmGet(pP2,1,2) * cvmGet(pP2,0,3) + 2.0 * cvmGet(pP2,1,3) * cvmGet(pP2,0,2));
    cvmSet(pA,5,4 , 2.0 * cvmGet(pP2,1,3) * cvmGet(pP2,0,3));
    cvmSet(pA,6,0 , 2.0 * cvmGet(pP2,2,0) * cvmGet(pP2,0,0) + 2.0 * cvmGet(pP2,2,1) * cvmGet(pP2,0,1));
    cvmSet(pA,6,1 , 2.0 * cvmGet(pP2,2,0) * cvmGet(pP2,0,3) + 2.0 * cvmGet(pP2,2,3) * cvmGet(pP2,0,0));
    cvmSet(pA,6,2 , 2.0 * cvmGet(pP2,2,1) * cvmGet(pP2,0,3) + 2.0 * cvmGet(pP2,2,3) * cvmGet(pP2,0,1));
    cvmSet(pA,6,3 , 2.0 * cvmGet(pP2,2,2) * cvmGet(pP2,0,3) + 2.0 * cvmGet(pP2,2,3) * cvmGet(pP2,0,2));
    cvmSet(pA,6,4 , 2.0 * cvmGet(pP2,2,3) * cvmGet(pP2,0,3));
    cvmSet(pA,7,0 , 2.0 * cvmGet(pP2,2,0) * cvmGet(pP2,1,0) + 2.0 * cvmGet(pP2,2,1) * cvmGet(pP2,1,1));
    cvmSet(pA,7,1 , 2.0 * cvmGet(pP2,2,0) * cvmGet(pP2,1,3) + 2.0 * cvmGet(pP2,2,3) * cvmGet(pP2,1,0));
    cvmSet(pA,7,2 , 2.0 * cvmGet(pP2,2,1) * cvmGet(pP2,1,3) + 2.0 * cvmGet(pP2,2,3) * cvmGet(pP2,1,1));
    cvmSet(pA,7,3 , 2.0 * cvmGet(pP2,2,2) * cvmGet(pP2,1,3) + 2.0 * cvmGet(pP2,2,3) * cvmGet(pP2,1,2));
    cvmSet(pA,7,4 , 2.0 * cvmGet(pP2,2,3) * cvmGet(pP2,1,3));

//now Y///

    cvmSet(pY,0,0, pow(cvmGet(pP1,0,2), 2.0) - pow(cvmGet(pP1,1,2), 2.0));
    cvmSet(pY,1,0, 2.0 * cvmGet(pP1,1,2) * cvmGet(pP1,0,2));
    cvmSet(pY,2,0, 2.0 * cvmGet(pP1,2,2) * cvmGet(pP1,0,2));
    cvmSet(pY,3,0, 2.0 * cvmGet(pP1,2,2) * cvmGet(pP1,1,2));

//2nd projection matrix

    cvmSet(pY,4,0, pow(cvmGet(pP2,0,2), 2.0) - pow(cvmGet(pP2,1,2), 2.0));
    cvmSet(pY,5,0, 2.0 * cvmGet(pP2,1,2) * cvmGet(pP2,0,2));
    cvmSet(pY,6,0, 2.0 * cvmGet(pP2,2,2) * cvmGet(pP2,0,2));
    cvmSet(pY,7, 0,2.0 * cvmGet(pP2,2,2) * cvmGet(pP2,1,2));

}

int  estimateFocalLengthStrum(const CvMat* pF,int width, int height, double& foc)
{


    CvMat* G = cvCreateMat(3,3, CV_64F);
    CvMat* U = cvCreateMat(3,3, CV_64F);
    CvMat* UT = cvCreateMat(3,3, CV_64F);

    CvMat* V = cvCreateMat(3,3, CV_64F);
    CvMat* VT = cvCreateMat(3,3, CV_64F);
    CvMat* W = cvCreateMat(3,3, CV_64F);


    if (pF==NULL)
    {
        printf("****matrix is null\n");
        return -1;

    }

    if ( pF->rows!=3 || pF->cols!=3 )
    {
        cout<<"fundamental matrix is the wrong size dudes"<<endl;
        return -2;
    }

    int status=0;

    status = createPseudoFundMatrix(pF,G,width, height);


    //printf("G matrix read was\n");
    //writeCVMatrix(cout,G );



    cvSVD( G, W,  U, V );  //change all of the below back to U



    double foc1,foc2;
    status= solveFfromUVW(foc1, foc2,U,V,W);

    foc=foc1;



    cvReleaseMat(&G);
    cvReleaseMat(&U);
    cvReleaseMat(&V);
    cvReleaseMat(&VT);
    cvReleaseMat(&UT);

    cvReleaseMat(&W);

    return 0;
}
int estimateFocalLengthsHartley(const CvMat* pF, int width1, int height1,int width2, int height2,double& foc1,double& foc2)
{

    CvMat* G = cvCreateMat(3,3, CV_64F);
    CvMat* U = cvCreateMat(3,3, CV_64F);
    CvMat* UT = cvCreateMat(3,3, CV_64F);

    CvMat* V = cvCreateMat(3,3, CV_64F);
    CvMat* VT = cvCreateMat(3,3, CV_64F);
    CvMat* W = cvCreateMat(3,3, CV_64F);


    if (pF==NULL)
    {
        printf("****matrix is null\n");
        return -1;

    }

    if ( pF->rows!=3 || pF->cols!=3 )
    {
        cout<<"fundamental matrix is the wrong size dudes"<<endl;
        return -2;
    }

    int status=0;

    status = createPseudoFundMatrix(pF,G,width1, height1);
//cvCopy(pF,G);

//    printf("G matrix read was\n");
//    writeCVMatrix(cout,G );



    cvSVD( G, W,  U, V );  //change all of the below back to U




    status= solveFfromUVWHartleyMAPLE(foc1, foc2,U,V,W);//change back to hartlet
    // status= solveFfromUVWHoumanMAPLE(foc1, foc2,U,V,W);//change back to hartlet
    // status= solveFfromUVWLQ(foc1, foc2,U,V,W);



    cvReleaseMat(&G);
    cvReleaseMat(&U);
    cvReleaseMat(&V);
    cvReleaseMat(&VT);
    cvReleaseMat(&UT);

    cvReleaseMat(&W);

    return 0;
}
int createPseudoFundMatrix(const CvMat* pF,CvMat* pG,int width, int height)
{
    CvMat* Gtemp1 = cvCreateMat(3,3, CV_64F);
    CvMat* Gtemp2 = cvCreateMat(3,3, CV_64F);
    CvMat* standMatrix = cvCreateMat(3,3, CV_64F);
    CvMat* leftMatr = cvCreateMat(3,3, CV_64F);
    CvMat* rightMatr = cvCreateMat(3,3, CV_64F);



    double ux, vy, skew;

    //determinig skew and camera center (this should be known, but here we simply them)
    ux=width;
    ux/=((double)2.0);
    vy=height;
    vy/=((double)2.0);
    skew=((double)1.0);



    //  printf("ux is %f and vy is %f \n",ux,vy);
    cvSetZero(leftMatr);
    cvSetZero(rightMatr);
    cvSetZero(Gtemp1);
    cvSetZero(Gtemp2);
    cvSetZero(standMatrix);

    //left one
    cvmSet(leftMatr, 0, 0,skew );
    cvmSet(leftMatr, 1,1,1.0 );
    cvmSet(leftMatr, 2, 2, 1.0);
    cvmSet(leftMatr, 2, 0,ux );
    cvmSet(leftMatr, 2, 1,vy );


    //right one
    cvmSet(rightMatr, 0, 0,skew );
    cvmSet(rightMatr, 1,1,1.0 );
    cvmSet(rightMatr, 2, 2, 1.0);
    cvmSet(rightMatr, 0, 2,ux );
    cvmSet(rightMatr, 1, 2,vy );


    cvMatMul(leftMatr,pF , Gtemp1);   // Ma*Mb   -> Mc
    cvMatMul(Gtemp1,rightMatr , Gtemp2);   // Ma*Mb   -> Mc


    //now multiply by some random F value

    cvSetIdentity(rightMatr);

    cvmSet(rightMatr, 0, 0,typicalF );
    cvmSet(rightMatr, 1, 1,typicalF);




    cvMatMul(Gtemp2,rightMatr , Gtemp1);   // Ma*Mb   -> Mc
    cvMatMul(rightMatr,Gtemp1 , Gtemp2);   // Ma*Mb   -> Mc



    //normalizing the forbenius norm
    double norm =cvNorm( Gtemp2, 0, CV_L2 );
    norm = ((double)1.0)/norm;
    cvConvertScale( Gtemp2,Gtemp1,norm );


    cvConvertScale( Gtemp1,pG,((double)1.0) );

    cvReleaseMat(&leftMatr);
    cvReleaseMat(&rightMatr);
    cvReleaseMat(&Gtemp1);
    cvReleaseMat(&Gtemp2);
    cvReleaseMat(&standMatrix);
    return 0;
}

int solveFfromUVW(double& F1, double& F2, const CvMat* pU,const CvMat* pV,const CvMat* pW)
{
    double f1_l1, f2_l1; //linear method 1
    double f1_l2, f2_l2; //linear method 2
    double f1_q, f2_q; //quadratic equation
    int status=0;


    //not gonna use the linear equations

//   status= solveFfromUVWL1(f1_l1,f2_l1,pU,pV,pW);
//    printf("focal length accordign to L1 is %f \n",f1_l1);
//
//    status= solveFfromUVWL2(f1_l2,f2_l2,pU,pV,pW);
//    printf("focal length accordign to L2 is %f \n",f1_l2);

    status= solveFfromUVWLQ(f1_q,f2_q,pU,pV,pW);

    F1=F2=f1_q;

    //choose which to use
    return 0;
}



int  solveFfromUVWL1(double& F1, double& F2, const CvMat* pU,const CvMat* pV,const CvMat* pW)
{
    double f=0;

    double a,b,U31,U32,V31,V32;

    a    =  cvmGet( pW, 0,0 )  ;
    b =  cvmGet( pW, 1,1 )  ;

    //    printf("a is %f and b is %f \n",a,b);


    //     printf("W matrix read was\n");
    //     writeCVMatrix(cout,pW );


    //     printf("V matrix read was\n");
    //     writeCVMatrix(cout,pV );


    //     printf("U matrix read was\n");
    //     writeCVMatrix(cout,pU );



    a    =  cvmGet( pW, 0,0 )  ;
    b =  cvmGet( pW, 1,1 )  ;
    U31 =  cvmGet( pU, 2,0 )  ;
    U32 =  cvmGet( pU, 2,1 )  ;
    V31 =  cvmGet( pV, 2,0 )  ;
    V32 =  cvmGet( pV, 2,1 )  ;


    double top=-U32*V31*((a*U31*V31)+(b*U32*V32));
    double bottom=(a*U31*U32*(1-(V31*V31)))+(b*V31*V32*(1-(U32*U32)));

    f=top/bottom;
    //   printf("top is %f and bottomn is %f and f2 is %f\n",top,bottom,f);

    f=sqrt(f);

    //undoing the efffects of the multiplication by the typical f
    f*=typicalF;



    F1=F2=f;

    return 0;
}



int  solveFfromUVWL2(double& F1, double& F2, const CvMat* pU,const CvMat* pV,const CvMat* pW)
{



    double f=0;

    double a,b,U31,U32,V31,V32;

    a    =  cvmGet( pW, 0,0 )  ;
    b =  cvmGet( pW, 1,1 )  ;
    U31 =  cvmGet( pU, 2,0 )  ;
    U32 =  cvmGet( pU, 2,1 )  ;
    V31 =  cvmGet( pV, 2,0 )  ;
    V32 =  cvmGet( pV, 2,1 )  ;




    double top=-U31*V32*((a*U31*V31)+(b*U32*V32));
    double bottom=(a*V31*V32*(1-(U31*U31)))+(b*U31*U32*(1-(V32*V32)));



    f=top/bottom;

    //  printf("top is %f and bottomn is %f and f2 is %f\n",top,bottom,f);
    f=sqrt(f);
    //undoing the efffects of the multiplication by the typical f
    f*=typicalF;



    F1=F2=f;

    return 0;
}

int  solveFfromUVWLQMAPLE(double& F1, double& F2, const CvMat* pU,const CvMat* pV,const CvMat* pW)
{


}


int  solveFfromUVWLQ(double& F1, double& F2, const CvMat* pU,const CvMat* pV,const CvMat* pW)
{




    double f=0;

    double a,b,U31,U32,V31,V32;

    a    =  cvmGet( pW, 0,0 )  ;
    b =  cvmGet( pW, 1,1 )  ;
    U31 =  cvmGet( pU, 2,0 )  ;
    U32 =  cvmGet( pU, 2,1 )  ;
    V31 =  cvmGet( pV, 2,0 )  ;
    V32 =  cvmGet( pV, 2,1 )  ;


    //   printf("a is %f and b is %f and U31 is %f and U32 is %f and V31 is %f and V32 is %f\n",a,b,U31,U32,V31,V32);



    double AQ,BQ,CQ;

    AQ=(a*a*(1.0-(U31*U31))*(1.0-(V31*V31)))-(b*b*(1.0-(U32*U32))*(1.0-(V32*V32)));


    BQ=(a*a*((U31*U31)+(V31*V31)-(2.0*(U31*U31)*(V31*V31))))-(b*b*((U32*U32)+(V32*V32)-(2.0*(U32*U32)*(V32*V32))));


    CQ=(a*a*U31*U31*V31*V31)-(b*b*U32*U32*V32*V32);


    FLOAT coeff[3];
    FLOAT re[2];
    FLOAT im[2];

    coeff[2]=AQ   ;
    coeff[1]= BQ  ;
    coeff[0]=  CQ ;

    int status= FindQuadraticRoots(coeff, re, im);

//    if (status==2)
//    {
//        printf("  2 real roots %f and %f \n",re[0],re[1]);
//
//
//    }
//    if (status==1)
//        printf("  1 real, double root \n");
//
//    if (status==0)
//        printf("  2 complex roots \n");


    f=sqrt(max(re[0],re[1]));
    // printf("f after sqrt is %f and typical is %f\n",f,typicalF);
    //undoing the efffects of the multiplication by the typical f
    f*=typicalF;
    F1=F2=f;
// printf("Q: f1 was %f and f2 was %f \n ",F1,F2 );
    return 0;
}

int solveFfromUVWHartleyMAPLE(double& F1, double& F2, const CvMat* pU,const CvMat* pV,const CvMat* pW)
{

    double a,b,v21 , v12 , v11 , v22 , u31,u32,v31,v32,u22,u21,u12, u11,sigma[3];

    sigma[0]    =  cvmGet( pW, 0,0 )  ;
    sigma[1]    =  cvmGet( pW, 1,1 )  ;
    u31 =  cvmGet( pU, 2,0 )  ;
    u32 =  cvmGet( pU, 2,1 )  ;
    u11 =  cvmGet( pU, 0,0 )  ;
    u12 =  cvmGet( pU, 0,1 )  ;
    u21 =  cvmGet( pU, 1,0 )  ;
    u22 =  cvmGet( pU, 1,1 )  ;

    v12 =  cvmGet( pV, 0,1 )  ;
    v11 =  cvmGet( pV, 0,0 )  ;
    v22 =  cvmGet( pV, 1,1 )  ;
    v21 =  cvmGet( pV, 1,0 )  ;


    v31 =  cvmGet( pV, 2,0 )  ;
    v32 =  cvmGet( pV, 2,1 )  ;


//double f1_1 = -u31 * u32 / (u11 * u12 + u22 * u21);
//double f2_1 = -v32 * v31 / (v12 * v11 + v22 * v21);
    double f1_2 = -(pow(sigma[0], 2.0) * v32 * v31 * u31 * u31 * v21 * v21 + pow(sigma[0], 2.0) * v32 * v31 * u31 * u31 * v11 * v11 - pow(sigma[0], 2.0) * v22 * v21 * u31 * u31 * v31 * v31 - pow(sigma[0], 2.0) * v12 * v11 * u31 * u31 * v31 * v31 + u31 * sigma[0] * v11 * v11 * sigma[1] * v32 * v32 * u32 - u31 * sigma[0] * v31 * v31 * sigma[1] * v12 * v12 * u32 + u31 * sigma[0] * v21 * v21 * sigma[1] * v32 * v32 * u32 - u31 * sigma[0] * v31 * v31 * sigma[1] * v22 * v22 * u32 - pow(sigma[1], 2.0) * v22 * v22 * u32 * u32 * v32 * v31 + pow(sigma[1], 2.0) * v32 * v32 * u32 * u32 * v22 * v21 - pow(sigma[1], 2.0) * v12 * v12 * u32 * u32 * v32 * v31 + pow(sigma[1], 2.0) * v32 * v32 * u32 * u32 * v12 * v11) / (u11 * u11 * pow(sigma[0], 2.0) * v32 * v31 * v21 * v21 + u11 * u11 * pow(sigma[0], 2.0) * v32 * v31 * v11 * v11 - u11 * u11 * pow(sigma[0], 2.0) * v22 * v21 * v31 * v31 - u11 * u11 * pow(sigma[0], 2.0) * v12 * v11 * v31 * v31 + u11 * u12 * sigma[0] * v11 * v11 * sigma[1] * v32 * v32 - u11 * u12 * sigma[0] * v31 * v31 * sigma[1] * v12 * v12 + u11 * u12 * sigma[0] * v21 * v21 * sigma[1] * v32 * v32 - u11 * u12 * sigma[0] * v31 * v31 * sigma[1] * v22 * v22 + u12 * u12 * pow(sigma[1], 2.0) * v12 * v11 * v32 * v32 - u12 * u12 * pow(sigma[1], 2.0) * v32 * v31 * v12 * v12 - u12 * u12 * pow(sigma[1], 2.0) * v32 * v31 * v22 * v22 + u12 * u12 * pow(sigma[1], 2.0) * v22 * v21 * v32 * v32 - sigma[0] * u22 * u21 * v31 * v31 * sigma[1] * v12 * v12 + sigma[0] * u22 * u21 * v11 * v11 * sigma[1] * v32 * v32 + v32 * v31 * pow(sigma[0], 2.0) * u21 * u21 * v11 * v11 + sigma[0] * u22 * u21 * v21 * v21 * sigma[1] * v32 * v32 + v32 * v31 * pow(sigma[0], 2.0) * u21 * u21 * v21 * v21 - sigma[0] * u22 * u21 * v31 * v31 * sigma[1] * v22 * v22 - v22 * v21 * pow(sigma[0], 2.0) * u21 * u21 * v31 * v31 - v12 * v11 * pow(sigma[0], 2.0) * u21 * u21 * v31 * v31 - pow(sigma[1], 2.0) * v32 * v31 * v12 * v12 * u22 * u22 + pow(sigma[1], 2.0) * v12 * v11 * v32 * v32 * u22 * u22 + pow(sigma[1], 2.0) * v22 * v21 * v32 * v32 * u22 * u22 - pow(sigma[1], 2.0) * v32 * v31 * v22 * v22 * u22 * u22);
    double f2_2 = -(-pow(sigma[0], 2.0) * u31 * u31 * u22 * u21 * v31 * v31 - pow(sigma[0], 2.0) * u31 * u31 * u12 * u11 * v31 * v31 + pow(sigma[1], 2.0) * u22 * u21 * v32 * v32 * u32 * u32 - pow(sigma[1], 2.0) * u32 * u31 * v32 * v32 * u22 * u22 - sigma[0] * u31 * u31 * sigma[1] * u12 * u12 * v32 * v31 + pow(sigma[1], 2.0) * u12 * u11 * v32 * v32 * u32 * u32 + u32 * pow(sigma[0], 2.0) * u21 * u21 * u31 * v31 * v31 + u32 * pow(sigma[0], 2.0) * u11 * u11 * u31 * v31 * v31 - sigma[0] * u31 * u31 * sigma[1] * u22 * u22 * v32 * v31 + sigma[0] * u21 * u21 * sigma[1] * u32 * u32 * v32 * v31 + sigma[0] * u11 * u11 * sigma[1] * u32 * u32 * v32 * v31 - pow(sigma[1], 2.0) * u32 * u31 * v32 * v32 * u12 * u12) / (u32 * pow(sigma[0], 2.0) * u11 * u11 * u31 * v11 * v11 - pow(sigma[0], 2.0) * u31 * u31 * u22 * u21 * v11 * v11 - pow(sigma[0], 2.0) * u31 * u31 * u12 * u11 * v11 * v11 + u32 * pow(sigma[0], 2.0) * u21 * u21 * u31 * v11 * v11 - sigma[0] * v12 * v11 * u31 * u31 * sigma[1] * u12 * u12 + sigma[0] * v12 * v11 * u11 * u11 * sigma[1] * u32 * u32 + sigma[0] * v12 * v11 * u21 * u21 * sigma[1] * u32 * u32 - sigma[0] * v12 * v11 * u31 * u31 * sigma[1] * u22 * u22 - pow(sigma[1], 2.0) * u32 * u31 * v12 * v12 * u22 * u22 + pow(sigma[1], 2.0) * u12 * u11 * v12 * v12 * u32 * u32 + pow(sigma[1], 2.0) * u22 * u21 * v12 * v12 * u32 * u32 - pow(sigma[1], 2.0) * u32 * u31 * v12 * v12 * u12 * u12 - pow(sigma[1], 2.0) * u32 * u31 * v22 * v22 * u12 * u12 - sigma[0] * v22 * v21 * u31 * u31 * sigma[1] * u12 * u12 - sigma[0] * v22 * v21 * u31 * u31 * sigma[1] * u22 * u22 + u32 * pow(sigma[0], 2.0) * u21 * u21 * u31 * v21 * v21 - pow(sigma[1], 2.0) * u32 * u31 * v22 * v22 * u22 * u22 + pow(sigma[1], 2.0) * u12 * u11 * v22 * v22 * u32 * u32 + pow(sigma[1], 2.0) * u22 * u21 * v22 * v22 * u32 * u32 + sigma[0] * v22 * v21 * u21 * u21 * sigma[1] * u32 * u32 - pow(sigma[0], 2.0) * u31 * u31 * u22 * u21 * v21 * v21 + u32 * pow(sigma[0], 2.0) * u11 * u11 * u31 * v21 * v21 - pow(sigma[0], 2.0) * u31 * u31 * u12 * u11 * v21 * v21 + sigma[0] * v22 * v21 * u11 * u11 * sigma[1] * u32 * u32);

    F1=  sqrt(f1_2)*typicalF;
    F2=  sqrt(f2_2)*typicalF;
    // printf("HARTLEY: f1 was %f and f2 was %f \n ",F1,F2 );
}


int solveFfromUVWHoumanMAPLE(double& F1, double& F2, const CvMat* pU,const CvMat* pV,const CvMat* pW)
{

    double V21 , V12 , V11 , V22 , U31,U32,V31,V32,U22,U21,U12, U11,sigma[3];

    double a  =  cvmGet( pW, 0,0 )  ;
    double b   =  cvmGet( pW, 1,1 )  ;

    //printf("a is %f and b is %f \n",a,b);

    U31 =  cvmGet( pU, 2,0 )  ;
    U32 =  cvmGet( pU, 2,1 )  ;
    U11 =  cvmGet( pU, 0,0 )  ;
    U12 =  cvmGet( pU, 0,1 )  ;
    U21 =  cvmGet( pU, 1,0 )  ;
    U22 =  cvmGet( pU, 1,1 )  ;

    V12 =  cvmGet( pV, 0,1 )  ;
    V11 =  cvmGet( pV, 0,0 )  ;
    V22 =  cvmGet( pV, 1,1 )  ;
    V21 =  cvmGet( pV, 1,0 )  ;


    V31 =  cvmGet( pV, 2,0 )  ;
    V32 =  cvmGet( pV, 2,1 )  ;

    double fo = (b * V31 * V32 * a * U31 * U31 - a * a * U31 * U32 * V31 * V31 - b * V31 * V32 * U32 * U32 * a + U31 * b * b * V32 * V32 * U32) / (U31 * b * b * V32 * V32 * U32 - U31 * b * b * U32 + a * a * U31 * U32 - a * a * U31 * U32 * V31 * V31 - b * V31 * V32 * U32 * U32 * a + b * V31 * V32 * a * U31 * U31);
    double  f1 = (a * a * U31 * U31 * V31 * V32 + a * U31 * U32 * b * V32 * V32 - a * U31 * U32 * V31 * V31 * b - b * b * V31 * V32 * U32 * U32) / (-a * U31 * U32 * V31 * V31 * b - b * b * V31 * V32 * U32 * U32 + b * b * V31 * V32 - a * a * V31 * V32 + a * a * U31 * U31 * V31 * V32 + a * U31 * U32 * b * V32 * V32);

    F1=  sqrt(fo)*typicalF;
    F2=  sqrt(f1)*typicalF;
    //printf("Houman: f1 was %f and f2 was %f \n ",F1,F2 );
}
