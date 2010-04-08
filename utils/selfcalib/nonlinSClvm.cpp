////////////////////////////////////////////////////////////////////////
//
// hello-world.cpp
//
// This is a simple, introductory OpenCV program. The program reads an
// image from a file, inverts it, and displays the result.
//
////////////////////////////////////////////////////////////////////////
#include <math.h>
#include "levmar.h"

#include <iostream>
#include "nonlinSClvm.h"
#define NONLINPARMS 1
#include "general.h"
using namespace std;

vector< vector<CvMat*> >const FVGlobal;

int CvMat2D_to_buff(vector< vector<CvMat*> >const &Mat,double** pbuffer)
{
    int numFrames=Mat.size();
    *pbuffer = new double [(numFrames*numFrames*9)+1];
    int i,j,l,m;

    for(i=0; i<=(numFrames*numFrames*9); i++)
    {
        (*pbuffer)[i]=0;
    }



    int count=0;
    for(i=0; i<numFrames; i++)
    {
        for(j=0; j<numFrames; j++)
        {

            for(l=0; l<3; l++)
            {
                for(m=0; m<3; m++)
                {

                    (*pbuffer)[count++]=cvmGet(Mat[i][j],l,m);


                }
            }


        }
    }


}

//this matrix should have been preallocated
int buff_to_CvMat2D(double* pbuffer,vector< vector<CvMat*> >const &Mat)
{
    int numFrames=Mat.size();

    int i,j,l,m;

    int count=0;

    for(i=0; i<numFrames; i++)
    {
        for(j=0; j<numFrames; j++)
        {

            for(l=0; l<3; l++)
            {
                for(m=0; m<3; m++)
                {

                    cvmSet(Mat[i][j],l,m,pbuffer[count++]);


                }
            }


        }
    }


}

int HRSelfCalibtwoFrameNonlin(vector< vector<CvMat*> > const &FV,  vector<CvMat*>  &KV ,int width, int height)
{


    int i,j,ret;
    double opts[LM_OPTS_SZ], info[LM_INFO_SZ];
    int numFrames=KV.size();
    int m, n;
    m=NONLINPARMS*numFrames;
    n=(int)(((NONLINPARMS)*(NONLINPARMS-1))/2);

    double p[m], x[n];
    double lb[m], ub[m];

vector<CvMat* > tempMats;
tempMats.resize(4);
  for(i=0; i<4; i++)
  {
      tempMats[i]=cvCreateMat(3,3, CV_64F);
  }

//initializing measurements
    for(i=0; i<n; i++)
        x[i]=0.0;


//initializing the parameters
    for(i=0; i<numFrames; i++)
    {

        j=0;

        if(NONLINPARMS>0)
        {
            //focal length
            p[(i*numFrames)+j]=width;
            lb[(i*numFrames)+j]=100;
            ub[(i*numFrames)+j]=2000;
            j++;

        }

        if(NONLINPARMS>1)  //X CENTER
        {
            p[(i*numFrames)+j]=width/2;
            lb[(i*numFrames)+j]=(width/2)-(width/5);
            ub[(i*numFrames)+j]=(width/2)+(width/5);
            j++;
        }


        if(NONLINPARMS>2)
        {
            //Y center
            p[(i*numFrames)+j]=height/2;
            lb[(i*numFrames)+j]=(height/2)-(height/5);
            ub[(i*numFrames)+j]=(height/2)+(height/5);
            j++;
        }

        if(NONLINPARMS>3)
        {
            //aspect ratio
            p[(i*numFrames)+j]=1.0;
            lb[(i*numFrames)+j]=0.8;
            ub[(i*numFrames)+j]=1.2;
            j++;
        }
        if(NONLINPARMS>4)
        {
            //skew
            p[(i*numFrames)+j++]=0.0;
            lb[(i*numFrames)+j]=0.8;
            ub[(i*numFrames)+j]=1.2;
            j++;
        }

    }
    double *work, *covar;
    work=(double*)malloc((LM_DIF_WORKSZ(m, n)+m*m)*sizeof(double));
    if(!work)
    {
        fprintf(stderr, "memory allocation request failed in main()\n");
        exit(1);
    }
    covar=work+LM_DIF_WORKSZ(m, n);

    opts[0]=LM_INIT_MU;
    opts[1]=1E-15;
    opts[2]=1E-15;
    opts[3]=1E-20;
    opts[4]= LM_DIFF_DELTA;


    SCinputs mySCinputs;
    mySCinputs.width=width;
    mySCinputs.height=height;
    mySCinputs.funds=&FV;
    mySCinputs.intrin=&KV;
    mySCinputs.tempMat=&tempMats;

    ret=dlevmar_bc_dif(errnonLinFunctionSelfCalib,  p, x, m, n, lb, ub, 1000, opts, info, work, covar, (void*)&mySCinputs);









    printf("Covariance of the fit:\n");
    for(i=0; i<m; ++i)
    {
        for(j=0; j<m; ++j)
        {
            printf("%g ", covar[i*m+j]);
        }

        printf("\n");
    }
    printf("\n");



 for(i=0; i<4; i++)
  {
        cvReleaseMat(&tempMats[i]);
  }
    free(work);

}

void errnonLinFunctionSelfCalib(double *p, double *hx, int m, int n, void *adata)
{

    SCinputs* mySCinputs=(SCinputs*)adata;




    int i,j;
    int width=0;//figure thesed out
    int height=0;

    width=mySCinputs->width;
    height=mySCinputs->height;

    vector< vector<CvMat*> > *FMat=(vector< vector<CvMat*> >*)mySCinputs->funds;
    vector<CvMat* > *pintrin= (vector< CvMat* >*)mySCinputs->intrin;
vector<CvMat* > *tempMtx=  (vector< CvMat* >*) mySCinputs->tempMat;


///////////////////// done putting F back into matrices

    for(i=0; i<n; i++)
    {

        j=0;
        cvSetIdentity((*pintrin)[i]);




        if(NONLINPARMS>0)
        {

            //focal length
            cvmSet((*pintrin)[i], 0, 0, p[(i*n)+j]);
            cvmSet((*pintrin)[i], 1, 1, p[(i*n)+j]);
            cvmSet((*pintrin)[i], 0, 2, ((double)(width/2.00)));
            cvmSet((*pintrin)[i], 1, 2, ((double)(height/2.00)));
            cvmSet((*pintrin)[i], 0, 1, 0.0);


            j++;

        }

        if(NONLINPARMS>1)  //X CENTER
        {

            cvmSet((*pintrin)[i], 0, 2, p[(i*n)+j]);
            j++;
        }


        if(NONLINPARMS>2)
        {
            //Y center
            cvmSet((*pintrin)[i], 1, 2, p[(i*n)+j]);
            j++;
        }

        if(NONLINPARMS>3)
        {
            //aspect ratio
            //Y center
            cvmSet((*pintrin)[i], 1, 1, cvmGet((*pintrin)[i],0,0)*p[(i*n)+j]);
            j++;

        }
        if(NONLINPARMS>4)
        {
            //skew
            cvmSet((*pintrin)[i], 0, 1, p[(i*n)+j]);
            j++;

        }

    }

///////////////////// now calculating errors

int count=0;
   for ( i = 0; i < n; ++i)
    {

        for ( j = 0; j < i; ++j)
        {
            hx[count++]=hx[0]+findSVDerror((*FMat)[i][j], (*pintrin)[i],(*pintrin)[i],tempMtx);

        }
    }



}


double findSVDerror(CvMat* k1,CvMat* k2,CvMat* F,vector<CvMat* > *tempMat)
{
    double    err=0;

CvMat* temp1=(*tempMat)[0];
CvMat* temp2=(*tempMat)[2];
CvMat* temp3=(*tempMat)[3];
CvMat* temp4=(*tempMat)[4];


cvTranspose(k1, temp1);


cvMatMul(temp1, F, temp2);   // Ma*Mb   -> Mc
cvMatMul(temp2, k2, temp1);   // Ma*Mb   -> Mc


cvSVD( temp1, temp2,  temp3, temp4,CV_SVD_MODIFY_A|CV_SVD_U_T |CV_SVD_V_T );  //change all of the below back to U

writeCVMatrix(cout,temp2);

err=(cvmGet(temp2,0,0)-cvmGet(temp2,1,1))/cvmGet(temp2,1,1);

    return err;
}

