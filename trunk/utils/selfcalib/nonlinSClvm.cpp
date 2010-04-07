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

#include "nonlinSClvm.h"
#define NONLINPARMS 5
vector< vector<CvMat*> >const FVGlobal;

int CvMat2D_to_buff(vector< vector<CvMat*> >const &Mat,double** pbuffer)
{
    int numFrames=Mat.size();
    double * buffer = new double [(size*size*9)+1];
    int i,j,l,m;

    for(i=0; i<=(numFrames*numFrames*9); i++)
    {
        buffer[i]=0;
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

                    buffer[count++]=cvmGet(Mat[i][j],l,m);


                }
            }


        }
    }

pbuffer=&buffer;
}


int buff_to_CvMat2D(double** pbuffer,vector< vector<CvMat*> >const &Mat,I WAS HERE LAST , WRITE THE OBJETIVE FUNCTION)
{
    int numFrames=Mat.size();

    int i,j,l,m;

    for(i=0; i<=(numFrames*numFrames*9); i++)
    {
        buffer[i]=0;
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

                    buffer[count++]=cvmGet(Mat[i][j],l,m);


                }
            }


        }
    }

pbuffer=&buffer;
}

int HRSelfCalibtwoFrameNonlin(vector< vector<CvMat*> >const &FV,  vector<CvMat*> const &KV ,int width, int height)
{
    FVGlobal=FV; // change this lateer

    int i,j,ret;
    double opts[LM_OPTS_SZ], info[LM_INFO_SZ];
    int numFrames=KV.size();
    int m, n;
    m=NONLINPARMS*numFrames;
    n=numFrames;

    double p[m], x[n];
    double lb[m], ub[m];

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





    ret=dlevmar_bc_dif(errnonLinFunctionSelfCalib,  p, x, m, n, lb, ub, 1000, opts, info, work, covar, NULL);









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

    free(work);

}

void errnonLinFunctionSelfCalib(double *p, double *hx, int m, int n, void *adata)
{


}


