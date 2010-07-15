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

#define NONLINPARMS 3
#define CONSTPARAMS 1
#include "general.h"
#include "focallength.h"
using namespace std;



double HRSelfCalibtwoFrameNonlinMULTIStep(vector< vector<CvMat*> > const &FV,  vector<CvMat*>  &KV ,int width, int height,vector<double>& confs)
{
    int i,j,k;
    int numFrames=KV.size();
//so now we have initial points from Sturm
    HRSelfCalibtwoFrame(FV, KV ,width, height, confs,STRUM);
//copy solutins to temporary place

    vector<CvMat* > tempMats;
    tempMats.resize(numFrames);
    for(i=0; i<numFrames; i++)
    {
        tempMats[i]=cvCreateMat(3,3, CV_64F);

        for(j=0; j<3; j++)
        {
            for(k=0; k<3; k++)
            {
                cvmSet(tempMats[i],i,j,cvmGet(KV[i],i,j));
            }
        }

    }

    vector<CvMat* > bestKs;
    bestKs.resize(numFrames);
    for(i=0; i<numFrames; i++)
    {
        bestKs[i]=cvCreateMat(3,3, CV_64F);

        for(j=0; j<3; j++)
        {
            for(k=0; k<3; k++)
            {
                cvmSet(bestKs[i],i,j,cvmGet(KV[i],i,j));
            }
        }

    }
///////////multi step optimization
    int numtries=100;
    double fvariance=70;
    double xvariance=30;
    double yvariance=30;
    double ARvariance=0.05;
    double skewvariance=0.05;

    double maxScore=100000;
    double curScore=0;


    for(i=0; i<numtries; i++)
    {

        for(int q=0; q<numFrames; q++)
        {
            cvmSet(tempMats[q], 0, 0, random_gaussian2(cvmGet(KV[q],0,0), fvariance,200,2000));
            cvmSet(tempMats[q], 0, 2,random_gaussian2(cvmGet(KV[q],0,2), xvariance,(width/2)-(width/5),(width/2)+(width/5)));
            cvmSet(tempMats[q], 1, 2,random_gaussian2(cvmGet(KV[q],1,2), yvariance,(height/2)-(height/5),(height/2)+(height/5)));
            cvmSet(tempMats[q], 0, 1,random_gaussian2(cvmGet(KV[q],0,1), skewvariance,-2,6));
            cvmSet(tempMats[q], 1, 1,random_gaussian2(1, ARvariance,0.8,1.2)*cvmGet(tempMats[q],0,0));


//            writeCVMatrix(cout<<"before:"<<endl,KV[q]);
//            writeCVMatrix(cout<<"after:"<<endl,tempMats[q]);

        }



        curScore=HRSelfCalibtwoFrameNonlinInitGuess(FV, tempMats , width, height, confs);

     //   printf("++++++++cur score was %f\n",curScore);
        if(curScore<maxScore)
        {
            for(int q=0; q<numFrames; q++)
            {

                for(j=0; j<3; j++)
                {
                    for(k=0; k<3; k++)
                    {
                        cvmSet(bestKs[q],j,k,cvmGet(tempMats[q],j,k));
                    }
                }

            }
        }

    }




    for(int q=0; q<numFrames; q++)
    {

        for(j=0; j<3; j++)
        {
            for(k=0; k<3; k++)
            {
                cvmSet(KV[q],j,k,cvmGet(bestKs[q],j,k));
            }
        }

    }




    for(i=0; i<numFrames; i++)
    {
        cvReleaseMat(&bestKs[i]);
        cvReleaseMat(&tempMats[i]);

    }

}


//in this function, the KV is used to store the output but it is also assumed to contain some initial values
double HRSelfCalibtwoFrameNonlinInitGuess(vector< vector<CvMat*> > const &FV,  vector<CvMat*>  &KV ,int width, int height,vector<double>& confs)
{




    int i,j,ret;
    double opts[LM_OPTS_SZ], info[LM_INFO_SZ];
    int numFrames=KV.size();
    int unKnownframes=(CONSTPARAMS==0?numFrames:1);
    int m, n;

    m=NONLINPARMS*unKnownframes;


    n=m;
    // n=(int)(((numFrames)*(numFrames-1))/2);

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
    for(i=0; i<unKnownframes; i++)
    {


        j=0;

        if(NONLINPARMS>0)
        {
            //focal length
            p[(i*NONLINPARMS)+j]= cvmGet(KV[i], 0, 0);
            lb[(i*NONLINPARMS)+j]=100;
            ub[(i*NONLINPARMS)+j]=2000;
            j++;

        }

        if(NONLINPARMS>1)  //X CENTER
        {
            p[(i*NONLINPARMS)+j]=cvmGet(KV[i], 0, 2);
            lb[(i*NONLINPARMS)+j]=(width/2)-(width/5);
            ub[(i*NONLINPARMS)+j]=(width/2)+(width/5);
            j++;
        }


        if(NONLINPARMS>2)
        {
            //Y center
            p[(i*NONLINPARMS)+j]=cvmGet(KV[i], 1, 2);
            lb[(i*NONLINPARMS)+j]=(height/2)-(height/5);
            ub[(i*NONLINPARMS)+j]=(height/2)+(height/5);
            j++;
        }

        if(NONLINPARMS>3)
        {
            //aspect ratio
            p[(i*NONLINPARMS)+j]=cvmGet(KV[i], 1, 1)/cvmGet(KV[i], 0, 0);
            lb[(i*NONLINPARMS)+j]=0.8;
            ub[(i*NONLINPARMS)+j]=1.2;
            j++;
        }
        if(NONLINPARMS>4)
        {
            //skew
            p[(i*NONLINPARMS)+j++]=cvmGet(KV[i], 0, 1);
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
    mySCinputs.numunknownframes=unKnownframes;

    ret=dlevmar_bc_dif(errnonLinFunctionSelfCalib,  p, x, m, n, lb, ub, 1000, opts, info, work, covar, (void*)&mySCinputs);


//putting the p back into KV
    int numfr=0;
    for(i=0; i<numFrames; i++)
    {

        j=0;
        numfr=(CONSTPARAMS==0?i:0);

        cvSetIdentity(KV[i]);




        if(NONLINPARMS>0)
        {

            //focal length
            cvmSet(KV[i], 0, 0, p[(numfr*NONLINPARMS)+j]);
            cvmSet(KV[i], 1, 1, p[(numfr*NONLINPARMS)+j]);
            cvmSet(KV[i], 0, 2, ((double)(width/2.00)));
            cvmSet(KV[i], 1, 2, ((double)(height/2.00)));
            cvmSet(KV[i], 0, 1, 0.0);


            j++;

        }

        if(NONLINPARMS>1)  //X CENTER
        {

            cvmSet(KV[i], 0, 2, p[(numfr*NONLINPARMS)+j]);
            j++;
        }


        if(NONLINPARMS>2)
        {
            //Y center
            cvmSet(KV[i], 1, 2, p[(numfr*NONLINPARMS)+j]);
            j++;
        }

        if(NONLINPARMS>3)
        {
            //aspect ratio
            //Y center
            cvmSet(KV[i], 1, 1, cvmGet(KV[i],0,0)*p[(numfr*NONLINPARMS)+j]);
            j++;

        }
        if(NONLINPARMS>4)
        {
            //skew
            cvmSet(KV[i], 0, 1, p[(numfr*NONLINPARMS)+j]);
            j++;

        }

    }


//
//
//
//
//
//    printf("Covariance of the fit:\n");
//    for(i=0; i<m; ++i)
//    {
//        for(j=0; j<m; ++j)
//        {
//            printf("%g ", covar[i*m+j]);
//        }
//
//        printf("\n");
//    }
//    printf("\n");



    for(i=0; i<4; i++)
    {
        cvReleaseMat(&tempMats[i]);
    }
    free(work);
    return info[1];
}
double HRSelfCalibtwoFrameNonlin(vector< vector<CvMat*> > const &FV,  vector<CvMat*>  &KV ,int width, int height,vector<double>& confs)
{




    int i,j,ret;

    int numFrames=KV.size();

    for(i=0; i<numFrames; i++)
    {


        j=0;
        cvmSet(KV[i], 0, 0, width);
        cvmSet(KV[i], 1, 1, width);
        cvmSet(KV[i], 0, 2,width/2.0);
        cvmSet(KV[i], 1, 2,height/2.0);
        cvmSet(KV[i], 0, 1,0.0);

    }
    return HRSelfCalibtwoFrameNonlinInitGuess(FV, KV , width, height, confs);



}

void errnonLinFunctionSelfCalib(double *p, double *hx, int m, int n, void *adata)
{

    SCinputs* mySCinputs=(SCinputs*)adata;

//    printf("m is %d and n is %d :\n",m,n);
//    printf("params are :\n");
//    for (int i=0; i<m; i++)
//        printf("p %d is %f\n",i,p[i]);

    //  printf(".");

    int i,j;
    int width=0;//figure thesed out
    int height=0;
    int numFrames=0;
    int numParams=0;
    int unKnownFrames=0;

    numFrames=mySCinputs->numFrames;
    numParams=mySCinputs->numParams;
    width=mySCinputs->width;
    height=mySCinputs->height;
    unKnownFrames=mySCinputs->numunknownframes;

    vector< vector<CvMat*> > *FMat=(vector< vector<CvMat*> >*)mySCinputs->funds;
    vector<CvMat* > *pintrin= (vector< CvMat* >*)mySCinputs->intrin;
    vector<CvMat* > *tempMtx=  (vector< CvMat* >*) mySCinputs->tempMat;


///////////////////// done putting F back into matrices


    int numfr=0;
    for(i=0; i<numFrames; i++)
    {

        j=0;
        numfr=(CONSTPARAMS==0?i:0);

        cvSetIdentity((*pintrin)[i]);




        if(NONLINPARMS>0)
        {

            //focal length
            cvmSet((*pintrin)[i], 0, 0, p[(numfr*NONLINPARMS)+j]);
            cvmSet((*pintrin)[i], 1, 1, p[(numfr*NONLINPARMS)+j]);
            cvmSet((*pintrin)[i], 0, 2, ((double)(width/2.00)));
            cvmSet((*pintrin)[i], 1, 2, ((double)(height/2.00)));
            cvmSet((*pintrin)[i], 0, 1, 0.0);


            j++;

        }

        if(NONLINPARMS>1)  //X CENTER
        {

            cvmSet((*pintrin)[i], 0, 2, p[(numfr*NONLINPARMS)+j]);
            j++;
        }


        if(NONLINPARMS>2)
        {
            //Y center
            cvmSet((*pintrin)[i], 1, 2, p[(numfr*NONLINPARMS)+j]);
            j++;
        }

        if(NONLINPARMS>3)
        {
            //aspect ratio
            //Y center
            cvmSet((*pintrin)[i], 1, 1, cvmGet((*pintrin)[i],0,0)*p[(numfr*NONLINPARMS)+j]);
            j++;

        }
        if(NONLINPARMS>4)
        {
            //skew
            cvmSet((*pintrin)[i], 0, 1, p[(numfr*NONLINPARMS)+j]);
            j++;

        }

    }

///////////////////// now calculating errors

//    for(i=0; i<numFrames; i++)
//          writeCVMatrix(cout,(*pintrin)[i]);


    double count=0;
    double totEr=0;
    for ( i = 0; i < numFrames; ++i)
    {

        for ( j = 0; j < i; ++j)
        {
            totEr=totEr+findSVDerror((*pintrin)[j],(*pintrin)[i],(*FMat)[i][j],tempMtx);
            count=count+1.0;
        }
    }

    totEr=totEr/(count);
    for (int i=0; i<n; i++)
        hx[i]=totEr;


//    printf("errrs are :\n");
//    for (int i=0; i<n; i++)
//        printf("hx %d is %f\n",i,hx[i]);

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

int cvCholesky(CvMat* inM,CvMat* outM)
{


    if (inM==NULL)
    {
        printf("****matrix is null\n");
        return -1;

    }

    if ( inM->rows==0 || inM->cols==0 )
    {
        cout<<"EMPTY"<<endl;
        return -1;
    }

    int rows = inM->rows;

    int cols = inM->cols;


    if(rows!=cols)
    {
        printf("matrix is not square so it cant be symmetric\n");
        return -2;


    }


    //check to see if matrix is symetric
    if(checkSymmetric(inM)==0)
    {
        printf("matrix not symmetric\n");
        return -1;
    }

    double *AT;

    printf("the input matrix is \n");

    int count =0;
    for (int i = 0; i < rows; ++i)
    {
        for (int j = 0; j < i; ++j)
        {

            cvmSet( inM,i,j,0 );
        }


    }

    writeCVMatrix(cout,inM);
    cvMatrixtoBuffer(inM,&AT, 1,1);


    char L[] = {'U', '\0'};
    integer n=rows;
    integer lda=rows; //i dont knwo whats lda
    integer info=0;


    dpotrf_(L, &n, (double*)AT, &lda, &info);

    if(info== 0) printf("successful cholesky exit \n");
    if(info< 0)        printf("the %d-th argument had an illegal value \n",info);
    if(info> 0)        printf("the %d-th argument had an illegal value \n",info);
    if(info > 0)       printf("the leading minor of order %d is not positive definite, and the factorization could not be completed\n",info);


    cvSetZero(outM);



    BuffertocvMatrix(AT,&outM,rows, cols, 0,1);
    printf("output matrix is \n");
    writeCVMatrix(cout,outM);
    delete [] AT;

    return info;

}



