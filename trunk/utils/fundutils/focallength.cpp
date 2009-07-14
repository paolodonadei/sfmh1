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

#include "general.h"
#include "HRprimitives.h"
#include "projdecompose.h"
#include "focallength.h"
const double typicalF= 1.0;

using namespace std;
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
    if (d > 0) {
        register double_t q;
        d = sqrt(d);
        q = (-b + ((b < 0) ? -d : d)) * 0.5;
        re[0] = q / a;
        re[1] = c / q;
        return (2);
    }

    /* One real double root */
    else if (d == 0) {
        re[0] = re[1] = -b / (2 * a);
        return (1);
    }

    /* Two complex conjugate roots */
    else {                                      /* d < 0 */
        re[0] = re[1] = -b / (2 * a);
        if (im != NULL) {
            im[0] = d / (2 * a);
            im[1] = -im[0];
        }
        return (0);
    }

}

#undef a
#undef b
#undef c


int  estimateFocalLengthStrum(const CvMat* pF,int width, int height, double& foc1, double& foc2)
{

    foc1=foc2=0;
    CvMat* G = cvCreateMat(3,3, CV_64F);
    CvMat* U = cvCreateMat(3,3, CV_64F);
    CvMat* V = cvCreateMat(3,3, CV_64F);
    CvMat* W = cvCreateMat(3,3, CV_64F);


    if(pF==NULL)
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


    cvSVD( G, W,  U, V );
    status= solveFfromUVW(foc1, foc2,U,V,W);





    cvReleaseMat(&G);
    cvReleaseMat(&U);
    cvReleaseMat(&V);
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


    status= solveFfromUVWL1(f1_l1,f2_l1,pU,pV,pW);
    printf("focal length accordign to L1 is %f \n",f1_l1);

    status= solveFfromUVWL2(f1_l2,f2_l2,pU,pV,pW);
    printf("focal length accordign to L2 is %f \n",f1_l2);

    status= solveFfromUVWLQ(f1_q,f2_q,pU,pV,pW);
    printf("focal length accordign to Q is %f \n",f1_q);



//all for debugging
    CvMat* AA = cvCreateMat(3,3, CV_64F);
    CvMat* BB = cvCreateMat(3,3, CV_64F);
    CvMat* CC = cvCreateMat(3,3, CV_64F);


    cvMatMul(pU,pW ,AA);   // Ma*Mb   -> Mc
    cvTranspose(pV, BB);      // transpose(Ma) -> Mb 
    cvMatMul(AA,BB ,CC);   // Ma*Mb   -> Mc
    
    

    return 0;
}



int  solveFfromUVWL1(double& F1, double& F2, const CvMat* pU,const CvMat* pV,const CvMat* pW)
{
    double f=0;
    
    double a,b,U31,U32,U33,V31,V32,V33;

    a    =  cvmGet( pW, 0,0 )  ;
    b =  cvmGet( pW, 1,1 )  ;
    U31 =  cvmGet( pU, 2,0 )  ;
    U32 =  cvmGet( pU, 2,1 )  ;
    U33 =  cvmGet( pU, 2,2 )  ;
    V31 =  cvmGet( pV, 2,0 )  ;
    V32 =  cvmGet( pV, 2,1 )  ;
    V33 =  cvmGet( pV, 2,2 )  ;



    double top=-U32*V31*((a*U31*V31)+(b*U32*V32));
    double bottom=(a*U31*U32*(1-(V31*V31)))+(b*V31*V32*(1-(U32*U32)));
    
    f=top/bottom;
    

    f=sqrt(f);

    //undoing the efffects of the multiplication by the typical f
    f*=typicalF;
    f*=typicalF;

    
    F1=F2=f;
   
    return 0;
}



int  solveFfromUVWL2(double& F1, double& F2, const CvMat* pU,const CvMat* pV,const CvMat* pW)
{



    double f=0;
    
    double a,b,U31,U32,U33,V31,V32,V33;

    a    =  cvmGet( pW, 0,0 )  ;
    b =  cvmGet( pW, 1,1 )  ;
    U31 =  cvmGet( pU, 2,0 )  ;
    U32 =  cvmGet( pU, 2,1 )  ;
    U33 =  cvmGet( pU, 2,2 )  ;
    V31 =  cvmGet( pV, 2,0 )  ;
    V32 =  cvmGet( pV, 2,1 )  ;
    V33 =  cvmGet( pV, 2,2 )  ;



    double top=-U31*V32*((a*U31*V31)+(b*U32*V32));
    double bottom=(a*V31*V32*(1-(U31*U31)))+(b*U31*U32*(1-(V32*V32)));
    
    printf("top is %f and bottomn is %f\n",top,bottom);
    
    f=top/bottom;
    

    f=sqrt(f);
    //undoing the efffects of the multiplication by the typical f
    f*=typicalF;
    f*=typicalF;

    
    F1=F2=f;

    return 0;
}




int  solveFfromUVWLQ(double& F1, double& F2, const CvMat* pU,const CvMat* pV,const CvMat* pW)
{




    double f=0;
    
    double a,b,U31,U32,U33,V31,V32,V33;

    a    =  cvmGet( pW, 0,0 )  ;
    b =  cvmGet( pW, 1,1 )  ;
    U31 =  cvmGet( pU, 2,0 )  ;
    U32 =  cvmGet( pU, 2,1 )  ;
    U33 =  cvmGet( pU, 2,2 )  ;
    V31 =  cvmGet( pV, 2,0 )  ;
    V32 =  cvmGet( pV, 2,1 )  ;
    V33 =  cvmGet( pV, 2,2 )  ;
    
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
    
    if(status==2)
    {
	printf("  2 real roots %f and %f \n",re[0],re[1]);
	

    }
    if(status==1)
	printf("  1 real, double root \n");

    if(status==0)
	printf("  2 complex roots \n");


    f=sqrt(max(re[0],re[1]));
    // printf("f after sqrt is %f and typical is %f\n",f,typicalF);
    //undoing the efffects of the multiplication by the typical f
    f*=typicalF;
    F1=F2=f;
    
    return 0;
}

