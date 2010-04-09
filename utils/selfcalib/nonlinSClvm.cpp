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



int HRSelfCalibtwoFrameNonlin(vector< vector<CvMat*> > const &FV,  vector<CvMat*>  &KV ,int width, int height)
{


    int i,j,ret;
    double opts[LM_OPTS_SZ], info[LM_INFO_SZ];
    int numFrames=KV.size();
    int m, n;
    m=NONLINPARMS*numFrames;
    n=(int)(((numFrames)*(numFrames-1))/2);

    double p[m+1], x[n+1];
    double lb[m+1], ub[m+1];

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
            p[(i*NONLINPARMS)+j]=width;
            lb[(i*NONLINPARMS)+j]=100;
            ub[(i*NONLINPARMS)+j]=2000;
            j++;

        }

        if(NONLINPARMS>1)  //X CENTER
        {
            p[(i*NONLINPARMS)+j]=width/2;
            lb[(i*NONLINPARMS)+j]=(width/2)-(width/5);
            ub[(i*NONLINPARMS)+j]=(width/2)+(width/5);
            j++;
        }


        if(NONLINPARMS>2)
        {
            //Y center
            p[(i*NONLINPARMS)+j]=height/2;
            lb[(i*NONLINPARMS)+j]=(height/2)-(height/5);
            ub[(i*NONLINPARMS)+j]=(height/2)+(height/5);
            j++;
        }

        if(NONLINPARMS>3)
        {
            //aspect ratio
            p[(i*NONLINPARMS)+j]=1.0;
            lb[(i*NONLINPARMS)+j]=0.8;
            ub[(i*NONLINPARMS)+j]=1.2;
            j++;
        }
        if(NONLINPARMS>4)
        {
            //skew
            p[(i*NONLINPARMS)+j++]=0.0;
            lb[(i*NONLINPARMS)+j]=-8;
            ub[(i*NONLINPARMS)+j]=8;
            j++;
        }


    }


//    for(int k=0; k<NONLINPARMS*numFrames; k++)
//        {
//            cout<<"number :"<<k<<"is: "<<lb[k]<<"\t"<<ub[k]<<"\t"<<p[k]<<endl;
//
//        }




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
    mySCinputs.numFrames=numFrames;
    mySCinputs.numParams=NONLINPARMS;



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

printf("m is %d and n is %d :\n",m,n);
    printf("params are :\n");
    for (int i=0; i<m; i++)
        printf("p %d is %f\n",i,p[i]);



    int i,j;
    int width=0;//figure thesed out
    int height=0;
    int numFrames=0;
    int numParams=0;

    numFrames=mySCinputs->numFrames;
    numParams=mySCinputs->numParams;
    width=mySCinputs->width;
    height=mySCinputs->height;

    vector< vector<CvMat*> > *FMat=(vector< vector<CvMat*> >*)mySCinputs->funds;
    vector<CvMat* > *pintrin= (vector< CvMat* >*)mySCinputs->intrin;
    vector<CvMat* > *tempMtx=  (vector< CvMat* >*) mySCinputs->tempMat;


///////////////////// done putting F back into matrices

    for(i=0; i<numFrames; i++)
    {

        j=0;
        cvSetIdentity((*pintrin)[i]);




        if(NONLINPARMS>0)
        {

            //focal length
            cvmSet((*pintrin)[i], 0, 0, p[(i*NONLINPARMS)+j]);
            cvmSet((*pintrin)[i], 1, 1, p[(i*NONLINPARMS)+j]);
            cvmSet((*pintrin)[i], 0, 2, ((double)(width/2.00)));
            cvmSet((*pintrin)[i], 1, 2, ((double)(height/2.00)));
            cvmSet((*pintrin)[i], 0, 1, 0.0);


            j++;

        }

        if(NONLINPARMS>1)  //X CENTER
        {

            cvmSet((*pintrin)[i], 0, 2, p[(i*NONLINPARMS)+j]);
            j++;
        }


        if(NONLINPARMS>2)
        {
            //Y center
            cvmSet((*pintrin)[i], 1, 2, p[(i*NONLINPARMS)+j]);
            j++;
        }

        if(NONLINPARMS>3)
        {
            //aspect ratio
            //Y center
            cvmSet((*pintrin)[i], 1, 1, cvmGet((*pintrin)[i],0,0)*p[(i*NONLINPARMS)+j]);
            j++;

        }
        if(NONLINPARMS>4)
        {
            //skew
            cvmSet((*pintrin)[i], 0, 1, p[(i*NONLINPARMS)+j]);
            j++;

        }

    }

///////////////////// now calculating errors

    for(i=0; i<numFrames; i++)
    {


        writeCVMatrix(cout,(*pintrin)[i]);



    }

    int count=0;
    for ( i = 0; i < n; ++i)
    {

        for ( j = 0; j < i; ++j)
        {
            hx[count++]=findSVDerror((*pintrin)[j],(*pintrin)[i],(*FMat)[i][j],tempMtx);

        }
    }

for (int i=0; i<n; i++)
        printf("hx %d is %f\n",i,hx[i]);

}


double findSVDerror(CvMat* k1,CvMat* k2,CvMat* F,vector<CvMat* > *tempMat)
{
    double    err=0;

    CvMat* temp1=(*tempMat)[0];
    CvMat* temp2=(*tempMat)[1];
    CvMat* temp3=(*tempMat)[2];
    CvMat* temp4=(*tempMat)[3];


    cvTranspose(k1, temp1);


    cvMatMul(temp1, F, temp2);   // Ma*Mb   -> Mc
    cvMatMul(temp2, k2, temp1);   // Ma*Mb   -> Mc


    cvSVD( temp1, temp2,  temp3, temp4,CV_SVD_MODIFY_A|CV_SVD_U_T |CV_SVD_V_T );  //change all of the below back to U



    err=(cvmGet(temp2,0,0)-cvmGet(temp2,1,1))/cvmGet(temp2,1,1);

    return err;
}

