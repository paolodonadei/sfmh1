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

#include "cxcore.h"
#include "highgui.h"
#define FLT_EPSILON    1.192092896e-07F
#define CONSTPARAMS 0
#include "general.h"
#include "focallength.h"
#define MINF 200
#define MAXF 2000
using namespace std;

void DrawEnergyf(vector< vector<CvMat*> > const &FV,  vector<CvMat*>  &KV ,int width, int height,vector<double>& confs)
{
    int numFrames=KV.size();

    for(int q=0; q<numFrames; q++)
    {


        readCvMatFfromfile(&KV[q],string( string("C:\\Documents and Settings\\hrast019\\Desktop\\sfmh1\\selfcalibmatlab\\currentProj\\K_") +stringify(q+1) +string(".txt")).c_str());

    }


    double svder2=findSVDerrorSequence(FV,  KV);
    printf("error for ground truth is %f \n",svder2);


    HRSelfCalibtwoFrameNonlin(FV, KV , width, height,confs);


    double svder1=findSVDerrorSequence(FV,  KV );
    printf("objective function  is measured at %f \n",svder1);

}

double HRSelfCalibtwoFrameNonlinCluster(vector< vector<CvMat*> > const &FV,  vector<CvMat*>  &KV ,int width, int height,vector<double>& confs)
{
    int numtries=80;
    double fvariance=70;
    double xvariance=30;
    double yvariance=30;
    double ARvariance=0.05;
    double skewvariance=0.05;
    double  curScore;
    int i,j,k,m,n;
    int numFrames=KV.size();

    vector<intrinsicFamily>  K_clusters;

    vector<vector<intrinsicFamily> >  K_clusters_individual;

    K_clusters.resize(numFrames);

    vector<CvMat* > tempMats;
    tempMats.resize(2);

    vector<CvMat* > tempMats2;
    tempMats2.resize(2);

    //matrix for 2 frame self calibration
    vector< vector<CvMat*> > funMatrix;
    funMatrix.resize(2);
    for (int i = 0; i < 2; ++i)
    {
        funMatrix[i].resize(numFrames);
        tempMats[i]=cvCreateMat(3,3, CV_64F);
        tempMats2[i]=cvCreateMat(3,3, CV_64F);
    }

    for (int i = 0; i < 2; ++i)
    {
        for (int j = 0; j < 2; ++j)
        {
            funMatrix[i][j]=cvCreateMat(3,3, CV_64F);

        }
    }

    vector< vector<double> > Weights;
    K_clusters_individual.resize(numFrames);

    Weights.resize(numFrames);
    for (int i = 0; i < numFrames; ++i)
    {
        Weights[i].resize(numFrames);
        K_clusters_individual[i].resize(numFrames);
    }


    for(int m=0; m<numFrames; m++)
        for(int n=0; n<numFrames; n++)
            Weights[m][n]=1;





    int cur_Index=-1;

    for(int m=0; m<numFrames; m++)
    {
        for(int n=0; n<m; n++)
        {
            cur_Index++;

            copyMatrix(FV[m][n],funMatrix[0][1]);
            cvTranspose(funMatrix[0][1], funMatrix[1][0]);

            HRSelfCalibtwoFrame(funMatrix, tempMats2 ,width, height, confs,STRUM);

            for(i=0; i<numtries; i++)
            {

                for(int q=0; q<2; q++)
                {
                    cvmSet(tempMats[q], 0, 0, random_gaussian2(cvmGet(tempMats2[q],0,0), fvariance,200,2000));
                    cvmSet(tempMats[q], 0, 2,random_gaussian2(cvmGet(tempMats2[q],0,2), xvariance,(width/2)-(width/5),(width/2)+(width/5)));
                    cvmSet(tempMats[q], 1, 2,random_gaussian2(cvmGet(tempMats2[q],1,2), yvariance,(height/2)-(height/5),(height/2)+(height/5)));
                    cvmSet(tempMats[q], 0, 1,random_gaussian2(cvmGet(tempMats2[q],0,1), skewvariance,-2,6));
                    cvmSet(tempMats[q], 1, 1,random_gaussian2(1, ARvariance,0.8,1.2)*cvmGet(tempMats[q],0,0));

                }


                double covtr=0;
                curScore=HRSelfCalibtwoFrameNonlinInitGuess(funMatrix, tempMats , width, height, confs,Weights,errnonLinFunctionSelfCalibmestimator ,&covtr);


//
//                printf("for F[%d][%d] \n",m,n);
//
//                writeCVMatrix(cout<<"after mat 1"<<endl,tempMats[0]);
//                writeCVMatrix(cout<<"after mat 2"<<endl,tempMats[1]);


//first frame
                if(cvmGet(tempMats[0],0,0)>MINF && cvmGet(tempMats[0],0,0)<MAXF)
                {


                    K_clusters[m].fx.push_back(cvmGet(tempMats[0],0,0));
                    K_clusters[m].fy.push_back(cvmGet(tempMats[0],1,1));
                    K_clusters[m].s.push_back(cvmGet(tempMats[0],0,1));
                    K_clusters[m].ux.push_back(cvmGet(tempMats[0],0,2));
                    K_clusters[m].uy.push_back(cvmGet(tempMats[0],1,2));
                    K_clusters[m].index_left.push_back(m);
                    K_clusters[m].index_right.push_back(n);
                    K_clusters[m].F_index.push_back(cur_Index);
                    K_clusters[m].score.push_back(covtr);
//second frame
                    K_clusters_individual[m][n].fx.push_back(cvmGet(tempMats[0],0,0));
                    K_clusters_individual[m][n].fy.push_back(cvmGet(tempMats[0],1,1));
                    K_clusters_individual[m][n].s.push_back(cvmGet(tempMats[0],0,1));
                    K_clusters_individual[m][n].ux.push_back(cvmGet(tempMats[0],0,2));
                    K_clusters_individual[m][n].uy.push_back(cvmGet(tempMats[0],1,2));
                    K_clusters_individual[m][n].index_left.push_back(m);
                    K_clusters_individual[m][n].index_right.push_back(n);
                    K_clusters_individual[m][n].score.push_back(covtr);
                }


                if(cvmGet(tempMats[1],0,0)>MINF && cvmGet(tempMats[1],0,0)<MAXF)
                {
                    K_clusters[n].fx.push_back(cvmGet(tempMats[1],0,0));
                    K_clusters[n].fy.push_back(cvmGet(tempMats[1],1,1));
                    K_clusters[n].s.push_back(cvmGet(tempMats[1],0,1));
                    K_clusters[n].ux.push_back(cvmGet(tempMats[1],0,2));
                    K_clusters[n].uy.push_back(cvmGet(tempMats[1],1,2));
                    K_clusters[n].index_left.push_back(m);
                    K_clusters[n].index_right.push_back(n);
                    K_clusters[n].F_index.push_back(cur_Index);
                    K_clusters[n].score.push_back(covtr);

                    K_clusters_individual[n][m].fx.push_back(cvmGet(tempMats[1],0,0));
                    K_clusters_individual[n][m].fy.push_back(cvmGet(tempMats[1],1,1));
                    K_clusters_individual[n][m].s.push_back(cvmGet(tempMats[1],0,1));
                    K_clusters_individual[n][m].ux.push_back(cvmGet(tempMats[1],0,2));
                    K_clusters_individual[n][m].uy.push_back(cvmGet(tempMats[1],1,2));
                    K_clusters_individual[n][m].index_left.push_back(m);
                    K_clusters_individual[n][m].index_right.push_back(n);
                    K_clusters_individual[n][m].score.push_back(covtr);

                }


            }
            //   printf("got %d from [%d][%d] \n",K_clusters[n].fx.size(),m,n);


        }
    }




    int numClusts=10;

    CvMat* clusters_centers = cvCreateMat( numClusts, NONLINPARMS, CV_32F );



    double *frameScores = new double [numClusts+5];


    for(int m=0; m<numFrames; m++)
    {
        double *compactness = new double [K_clusters[m].fx.size()+5];
        for(int bb=0; bb<K_clusters[m].fx.size() ; bb++)
        {
            compactness[bb]=0;
        }


        CvMat* clustering_data=cvCreateMat(K_clusters[m].fx.size(),NONLINPARMS,CV_32F);
        CvMat* zclusters = cvCreateMat( K_clusters[m].fx.size(), 1, CV_32SC1 );
        cvSetZero(clustering_data);
        cvSetZero(zclusters);
        //  printf("size of frame %d was %d\n",m, K_clusters[m].fx.size());
        for(i=0; i<K_clusters[m].fx.size(); i++)
        {

            if(NONLINPARMS>0) cvmSet(clustering_data,i,0,K_clusters[m].fx[i] );
            if(NONLINPARMS>1) cvmSet(clustering_data,i,1,K_clusters[m].ux[i] );
            if(NONLINPARMS>2) cvmSet(clustering_data,i,2,K_clusters[m].uy[i] );
            if(NONLINPARMS>3) cvmSet(clustering_data,i,3,K_clusters[m].fy[i] );
            if(NONLINPARMS>4) cvmSet(clustering_data,i,4,K_clusters[m].s[i] );

        }

        double kout=cvKMeans2(clustering_data, numClusts, zclusters, cvTermCriteria( CV_TERMCRIT_ITER, 40, 1.0 ), 2, 0,0, clusters_centers);

        for(int yy=0; yy<zclusters->rows; yy++)
        {
            for(int bb=0; bb<zclusters->cols ; bb++)
            {
                compactness[yy]+=fabs(cvmGet(clustering_data,yy,bb)-cvmGet(clusters_centers,CV_MAT_ELEM( *zclusters, int, yy, 0 ),bb));
            }
        }


        //      int numClusWinner=findClusWinner(zclusters,numClusts);
        int numClusWinner=findClusWinner2(K_clusters[m],zclusters,numClusts,clusters_centers , compactness,numFrames);

        vector<double> clustScores(numClusts,0);
        vector<double> clustMemberNumber(numClusts,0);

        for(int yy=0; yy<zclusters->rows; yy++)
        {

            // printf("compactness for point %d belonging to cluster %d is %f \n",yy,CV_MAT_ELEM( *zclusters, int, yy, 0 ),compactness[yy]);
            clustMemberNumber[CV_MAT_ELEM( *zclusters, int, yy, 0 )]=clustMemberNumber[CV_MAT_ELEM( *zclusters, int, yy, 0 )]+1;

            clustScores[CV_MAT_ELEM( *zclusters, int, yy, 0 )]=clustScores[CV_MAT_ELEM( *zclusters, int, yy, 0 )]+K_clusters[m].score[yy];
            //clustScores[CV_MAT_ELEM( *zclusters, int, yy, 0 )]=clustScores[CV_MAT_ELEM( *zclusters, int, yy, 0 )]+compactness[yy];
        }
        for(int yy=0; yy<numClusts; yy++)
        {
            clustScores[yy]/=clustMemberNumber[yy];
        }



        frameScores[m]=clustScores[numClusWinner];
        for(int yy=0; yy<numClusts; yy++)
        {



            printf("compactness for %d was %f and number in the cluster is %f and %s\n",yy,clustScores[yy],clustMemberNumber[yy],(yy==numClusWinner)?"WINNER":" ");

        }
//        for(i=0; i<K_clusters[m].fx.size(); i++)
//        {
//            printf(" %d , %d ,  %f , %f , %f  , %f  , %f , %d , %d , %d , %s \n",m,i,
//                   K_clusters[m].fx[i],K_clusters[m].fy[i],
//                   K_clusters[m].ux[i],K_clusters[m].uy[i],K_clusters[m].s[i],
//                   K_clusters[m].index_left[i],K_clusters[m].index_right[i] , CV_MAT_ELEM( *zclusters, int, i, 0 ) , (CV_MAT_ELEM( *zclusters, int, i, 0 )==numClusWinner)?"*": " "   );
//
//        }

        cvSetIdentity(KV[m]);
        if(NONLINPARMS>0) cvmSet(KV[m],0,0,cvmGet(clusters_centers,numClusWinner,0) );


        //setting default values
        cvmSet(KV[m],1,1,cvmGet(clusters_centers,numClusWinner,0) );
        cvmSet(KV[m],0,2,width/2 );
        cvmSet(KV[m],1,2,height/2 );
        cvmSet(KV[m],0,1,0);

        if(NONLINPARMS>1)  cvmSet(KV[m],0,2,cvmGet(clusters_centers,numClusWinner,1) );
        if(NONLINPARMS>2)  cvmSet(KV[m],1,2,cvmGet(clusters_centers,numClusWinner,2) );
        if(NONLINPARMS>3)  cvmSet(KV[m],1,1,cvmGet(clusters_centers,numClusWinner,3) );
        if(NONLINPARMS>4)  cvmSet(KV[m],0,1,cvmGet(clusters_centers,numClusWinner,4) );


        printf("_____________________________________\n");

        cvReleaseMat( &zclusters );
        cvReleaseMat(&clustering_data);
        delete [] compactness;

    }
    printf("_____________________________________\n");
    printf("_____________________________________\n");
    printf("_____________________________________\n");
    for(int m=0; m<numFrames; m++)
    {
        stats mystat=findStatsArray(K_clusters[m].fx);
        printf("for frame %d the average fx is %f and the median is %f\n",m,mystat.mean,mystat.median );
    }
    printf("_____________________________________\n");
    printf("_____________________________________\n");
    printf("_____________________________________\n");

    for(int m=0; m<numFrames; m++)
    {
        printf("for frame %d the clusterint fx is %f and ux is %f and uy is %f and score is %f \n",m,cvmGet(KV[m],0,0),cvmGet(KV[m],0,2),cvmGet(KV[m],1,2),frameScores[m]);
    }



////////////////////////////end of clustering, the rest determines the weights
    double threshold=200;


    for(int m=0; m<numFrames; m++)
    {

        for(int n=0; n<numFrames; n++)
        {
            if(m!=n)
            {
                intrinsicFamily fam;

                fam.fx.push_back(cvmGet(KV[m],0,0));
                fam.ux.push_back(cvmGet(KV[m],0,2));
                fam.uy.push_back(cvmGet(KV[m],1,2));
                fam.fy.push_back(cvmGet(KV[m],1,1));
                fam.s.push_back(cvmGet(KV[m],0,1));

                double distCluster=findDistanceClosestVectorPt(K_clusters_individual[m][n] ,fam,NONLINPARMS);

                printf("distance [%d][%d]=%f \n",m,n,distCluster);
                if(distCluster>threshold )
                {
                    Weights[m][n]=0;
                    Weights[n][m]=0;
                    printf("setting weights [%d][%d]=0 \n",m,n);
                }
            }
        }
    }

    HRSelfCalibtwoFrame(FV, KV ,width, height, confs,STRUM);
    HRSelfCalibtwoFrameNonlinInitGuess(FV, KV , width, height, confs,Weights,errnonLinFunctionSelfCalibmestimator );


    for (int i = 0; i < 2; ++i)
    {

        cvReleaseMat(&tempMats[i]);
        cvReleaseMat(&tempMats2[i]);
        for (int j = 0; j < 2; ++j)
        {
            cvReleaseMat(&funMatrix[i][j]);

        }
    }


    delete [] frameScores;
}


double findDistanceClosestVectorPt(intrinsicFamily myvec ,intrinsicFamily val,int numValus)
{
    double curDist=0;
    double minDist=1000000;
    for(int i=0; i<myvec.fx.size(); i++)
    {
        curDist=0;
        if(NONLINPARMS>0) curDist+=((myvec.fx[i]-val.fx[0])*(myvec.fx[i]-val.fx[0]));
        if(NONLINPARMS>1) curDist+=((myvec.ux[i]-val.ux[0])*(myvec.ux[i]-val.ux[0]));
        if(NONLINPARMS>2) curDist+=((myvec.uy[i]-val.uy[0])*(myvec.uy[i]-val.uy[0]));
        if(NONLINPARMS>3) curDist+=((myvec.fy[i]-val.fy[0])*(myvec.fy[i]-val.fy[0]));
        if(NONLINPARMS>4) curDist+=(50*((myvec.s[i]-val.s[0])*(myvec.s[i]-val.s[0])));

        curDist=sqrt(curDist);

        if(curDist<minDist)
        {
            minDist=curDist;
        }
    }

    return minDist;
}
int findClusWinner2(intrinsicFamily& datafamily, CvMat* clusterlabels,int numClusts,CvMat* clusterscenters , double* compact,int numFrames)
{
    int numFs=rint((numFrames*(numFrames-1))/2);
    vector< vector<int> > memberArray;
    memberArray.resize(clusterscenters->rows);

    // printf("we have %d and %d points in the solutions \n",clusterlabels->rows,datafamily.fx.size());

    for (int i = 0; i < clusterscenters->rows; ++i)
    {
        memberArray[i].resize(numFs,0);

    }

    for(int i=0; i<clusterlabels->rows; i++)
    {
        // printf("i is %d cluster label is %d and member is %d\n",i,CV_MAT_ELEM( *clusterlabels, int, i, 0 ),datafamily.F_index[i] );

        memberArray[CV_MAT_ELEM( *clusterlabels, int, i, 0 )][datafamily.F_index[i] ]++;
    }

//    printf("memberships are:\n");
//    for (int i = 0; i < clusterscenters->rows; ++i)
//    {
//        for (int j = 0; j < numFs; ++j)
//        {
//            printf("\t %d \t",memberArray[i][j]);
//
//        }
//        printf("\n");
//    }

    int numpeaks=5;

    int memberThresh=0;
    int bestbestCluster=0;
    int prevBestClust=-1;
    while(numpeaks>1)
    {
        memberThresh++;
        vector<int>  clustScores(clusterscenters->rows ,0);

//printf("cluster scores for a threshold of %d is \n",memberThresh );

        for (int i = 0; i < clusterscenters->rows; ++i)
        {
            int tempS=0;
            for (int j = 0; j < numFs; ++j)
            {

                if(memberArray[i][j]>=memberThresh)
                {
                    tempS++;
                }

            }
            clustScores[i]=tempS;
            //     printf("FOR I=%d score was %d \n",i,clustScores[i]);

        }

        int bestClust= indexMax(clustScores);
        if(clustScores[bestClust]==0)//this means they were all the same
        {

            bestbestCluster= prevBestClust;
            break;
        }
        numpeaks=0;
        for (int i = 0; i < clusterscenters->rows; ++i)
        {
            if(clustScores[i]>=clustScores[bestClust])
            {
                numpeaks++;
            }

        }
        //  printf("best cluster is %d and number of peaks is %d\n",bestClust,numpeaks );
        bestbestCluster= bestClust;
        prevBestClust=bestClust;
    }
    // printf("winning cluster is %d\n",bestbestCluster);
    return bestbestCluster;
}
int findClusWinner(CvMat* myclusterlabels,int numClusts)
{
    if (myclusterlabels==NULL)
    {
        printf("parameters is a NULL pointer1!");
        return -1;
    }

    if (!CV_IS_MAT(myclusterlabels))
    {
        printf("Input parameter must be a matrix!");
        return -1;
    }

    if(myclusterlabels->cols!=1  )
    {
        printf("wrong dimensions!");
        return -1;

    }

    vector<int> clustLabels(numClusts,0);


    for(int i=0; i<myclusterlabels->rows; i++)
    {

        int curElement=CV_MAT_ELEM( *myclusterlabels, int, i, 0 );

        // printf("i is %d and result is %d \n",i,curElement);
        clustLabels[curElement]++;

    }

    int WinIndex=-1;

    int maxNums=-100;
    for(int i=0; i<numClusts; i++)
    {
        if(clustLabels[i]>maxNums)
        {
            maxNums=  clustLabels[i];
            WinIndex=i;
        }


    }

    return WinIndex;
}
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
                cvmSet(tempMats[i],j,k,cvmGet(KV[i],j,k));
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
                cvmSet(bestKs[i],j,k,cvmGet(KV[i],j,k));
            }
        }

    }


///////////multi step optimization

///zzz change this maybe
    int numtries=20;
    double fvariance=70;
    double xvariance=30;
    double yvariance=30;
    double ARvariance=0.05;
    double skewvariance=0.05;

    double maxScore=100000;
    double curScore=0;

    vector< vector<double> > Weights;

    Weights.resize(numFrames);
    for (int i = 0; i < numFrames; ++i)
        Weights[i].resize(numFrames);

    for(int m=0; m<numFrames; m++)
        for(int n=0; n<numFrames; n++)
            Weights[m][n]=1;


    for(i=0; i<numtries; i++)
    {

        for(int q=0; q<numFrames; q++)
        {
            cvmSet(tempMats[q], 0, 0, random_gaussian2(cvmGet(KV[q],0,0), fvariance,200,2000));
            cvmSet(tempMats[q], 0, 2,random_gaussian2(cvmGet(KV[q],0,2), xvariance,(width/2)-(width/5),(width/2)+(width/5)));
            cvmSet(tempMats[q], 1, 2,random_gaussian2(cvmGet(KV[q],1,2), yvariance,(height/2)-(height/5),(height/2)+(height/5)));
            cvmSet(tempMats[q], 0, 1,random_gaussian2(cvmGet(KV[q],0,1), skewvariance,-2,6));
            cvmSet(tempMats[q], 1, 1,random_gaussian2(1, ARvariance,0.8,1.2)*cvmGet(tempMats[q],0,0));

//            if(q==0)  readCvMatFfromfile(&tempMats[q],"C:\\Documents and Settings\\hrast019\\Desktop\\sfmh1\\selfcalibmatlab\\currentProj\\K_1.txt");
//            if(q==1)  readCvMatFfromfile(&tempMats[q],"C:\\Documents and Settings\\hrast019\\Desktop\\sfmh1\\selfcalibmatlab\\currentProj\\K_2.txt");
//            if(q==2)  readCvMatFfromfile(&tempMats[q],"C:\\Documents and Settings\\hrast019\\Desktop\\sfmh1\\selfcalibmatlab\\currentProj\\K_3.txt");
//            if(q==3)  readCvMatFfromfile(&tempMats[q],"C:\\Documents and Settings\\hrast019\\Desktop\\sfmh1\\selfcalibmatlab\\currentProj\\K_4.txt");
//            if(q==4)  readCvMatFfromfile(&tempMats[q],"C:\\Documents and Settings\\hrast019\\Desktop\\sfmh1\\selfcalibmatlab\\currentProj\\K_5.txt");


//            writeCVMatrix(cout<<"before:"<<endl,KV[q]);
//            writeCVMatrix(cout<<"after:"<<endl,tempMats[q]);

        }
        double svder1=findSVDerrorSequence(FV,  tempMats );
        printf("error before nonlin is %f \n",svder1);

        double covtr=0;

        curScore=HRSelfCalibtwoFrameNonlinInitGuess(FV, tempMats , width, height, confs,Weights,errnonLinFunctionSelfCalibmestimator ,&covtr);

        double svder2=findSVDerrorSequence(FV,  tempMats );
        printf("error after nonlin is %f \n",svder2);


        printf("iteration %d score %10.10f  cov %10.10f focal %f\n",i,curScore,covtr,cvmGet(tempMats[0],0,0));
        if(curScore<maxScore)
        {
            printf("%d  -> cur score was %0.29f for focal length:  ",i,curScore);
            for(int q=0; q<numFrames; q++)
            {
                printf("\t %f \t",cvmGet(tempMats[q],0,0));
            }
            printf("\n");
            maxScore=curScore;
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


    printf("best score was %0.29f\n",maxScore);

    for(i=0; i<numFrames; i++)
    {
        cvReleaseMat(&bestKs[i]);
        cvReleaseMat(&tempMats[i]);

    }

}
double HRSelfCalibtwoFrameNonlinMULTIStep2(vector< vector<CvMat*> > const &FV,  vector<CvMat*>  &KV ,int width, int height,vector<double>& confs)
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
                cvmSet(tempMats[i],j,k,cvmGet(KV[i],j,k));
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
                cvmSet(bestKs[i],j,k,cvmGet(KV[i],j,k));
            }
        }

    }


///////////multi step optimization

///zzz change this maybe
    int numtries=20;
    double fvariance=70;
    double xvariance=30;
    double yvariance=30;
    double ARvariance=0.05;
    double skewvariance=0.05;

    double maxScore=100000;
    double curScore=0;

    vector< vector<double> > Weights;

    Weights.resize(numFrames);
    for (int i = 0; i < numFrames; ++i)
        Weights[i].resize(numFrames);

    for(int m=0; m<numFrames; m++)
        for(int n=0; n<numFrames; n++)
            Weights[m][n]=1;


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


        double covtr=0;

        curScore=HRSelfCalibtwoFrameNonlinInitGuess(FV, tempMats , width, height, confs,Weights,errnonLinFunctionSelfCalibmestimator ,&covtr);

        curScore=covtr;

        printf("iteration %d score %10.10f  cov %10.10f focal %f\n",i,curScore,covtr,cvmGet(tempMats[0],0,0));

        if(curScore<maxScore)
        {
            printf("%d  -> cur score was %0.29f for focal length %0.9f\n",i,curScore,cvmGet(tempMats[0],0,0));
            maxScore=curScore;
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


    printf("best score was %0.29f\n",maxScore);

    for(i=0; i<numFrames; i++)
    {
        cvReleaseMat(&bestKs[i]);
        cvReleaseMat(&tempMats[i]);

    }

}

//in this function, the KV is used to store the output but it is also assumed to contain some initial values
double HRSelfCalibtwoFrameNonlinInitGuess(vector< vector<CvMat*> > const &FV,  vector<CvMat*>  &KV ,int width, int height,vector<double>& confs,vector< vector<double> >& Weights, void (*func)(double *p, double *hx, int m, int n, void *adata),double* covtr)
{




    int i,j,ret;
    double opts[LM_OPTS_SZ], info[LM_INFO_SZ];
    int numFrames=KV.size();
    int unKnownframes=(CONSTPARAMS==0?numFrames:1);
    int m, n;

    m=NONLINPARMS*unKnownframes;

//read my comment in the error function to see why im doing this
    // n=(int)(((numFrames)*(numFrames-1))/2);

    n=m;
    // printf("the number of parameters is %d and the number of measurements is %d \n",m,n);

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
            lb[(i*NONLINPARMS)+j]=MINF;
            ub[(i*NONLINPARMS)+j]=MAXF;
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
            p[(i*NONLINPARMS)+j]=cvmGet(KV[i], 0, 1);
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
    opts[1]=1E-18;
    opts[2]=1E-28;
    opts[3]=1E-28;
    opts[4]= LM_DIFF_DELTA;


    SCinputs mySCinputs;
    mySCinputs.width=width;
    mySCinputs.height=height;
    mySCinputs.funds=&FV;
    mySCinputs.intrin=&KV;
    mySCinputs.Weights=&Weights;
    mySCinputs.confidences=&confs;
    mySCinputs.tempMat=&tempMats;
    mySCinputs.numFrames=numFrames;
    mySCinputs.numParams=NONLINPARMS;
    mySCinputs.numunknownframes=unKnownframes;


//constrained
    ret=dlevmar_bc_dif(func,  p, x, m, n, lb, ub, 100, opts, info, work, covar, (void*)&mySCinputs);

    //no constraints
    //ret=dlevmar_dif(func,  p, x, m, n,  1000, opts, info, work, covar, (void*)&mySCinputs);

    printf("Levenberg-Marquardt returned %d in %g iter, reason %g\nSolution: ", ret, info[5], info[6]);

    printf("\n\nMinimization info:\n");
    for(i=0; i<LM_INFO_SZ; ++i)
        printf("%d : %g \n",i, info[i]);
    printf("\n");

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

    if(covtr!=NULL)
    {


        double traceCov=0;
        //finding trace of the covariance
        for(i=0; i<m; ++i)
        {
            j=i;
            traceCov+=fabs(covar[i*m+j]);
        }
        (*covtr)= traceCov;
    }
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

    HRSelfCalibtwoFrame(FV, KV ,width, height, confs,STRUM);



    vector< vector<double> > Weights;

    Weights.resize(numFrames);
    for (int i = 0; i < numFrames; ++i)
        Weights[i].resize(numFrames);

    for(int m=0; m<numFrames; m++)
        for(int n=0; n<numFrames; n++)
            Weights[m][n]=1;


    return HRSelfCalibtwoFrameNonlinInitGuess(FV, KV , width, height, confs, Weights);



}

double HRSelfCalibtwoFrameNonlinMEstimator(vector< vector<CvMat*> > const &FV,  vector<CvMat*>  &KV ,int width, int height,vector<double>& confs)
{

    if( remove( "weights.csv" ) != 0 )
        perror( "Error deleting file" );



    if( remove( "errors.csv" ) != 0 )
        perror( "Error deleting file" );


    FILE *fpw;
    FILE *fpe;

    if((fpw=fopen("weights.csv", "w")) == NULL)
    {
        printf("Cannot open file.\n");
        exit(1);
    }
    if((fpe=fopen("errors.csv", "w")) == NULL)
    {
        printf("Cannot open file.\n");
        exit(1);
    }

    int i,j,ret;

    int numFrames=KV.size();
    int MAXITER=10;
    int itercounter=0;
    double threshold;



    vector<CvMat* > tempMats;
    tempMats.resize(4);
    for(i=0; i<4; i++)
    {
        tempMats[i]=cvCreateMat(3,3, CV_64F);
        cvSetZero(tempMats[i]);
    }

//how should i start the weights? should i get some info from the matching like mendonca does?
    vector< vector<double> > Weights;

    Weights.resize(numFrames);
    for (int i = 0; i < numFrames; ++i)
        Weights[i].resize(numFrames);

    for(int m=0; m<numFrames; m++)
        for(int n=0; n<numFrames; n++)
            Weights[m][n]=1;


///zzz change this, a hardcoded threshold is not a good diea
    threshold=0.005;
    vector<double> residuals;

    while(itercounter<MAXITER)
    {
        itercounter++;


        HRSelfCalibtwoFrame(FV, KV ,width, height, confs,STRUM);

        HRSelfCalibtwoFrameNonlinInitGuess(FV, KV , width, height, confs,Weights,errnonLinFunctionSelfCalibmestimator );

        for(int q=0; q<4; q++)
        {

            cvSetZero(tempMats[q]);
        }

        residuals.clear();

        for(int m=0; m<numFrames; m++)
        {
            for(int n=0; n<m; n++)
            {
                double curError=findSVDerror(KV[n],KV[m],FV[m][n],&tempMats);
                residuals.push_back(curError);

            }
        }
//this is tukey's bisquare tuning parameter
        threshold= mediaAbsoluteDev(residuals)*4.685;
        printf("residual was %f\n",threshold);

        for(int m=0; m<numFrames; m++)
        {
            for(int n=0; n<m; n++)
            {
                double curError=findSVDerror(KV[n],KV[m],FV[m][n],&tempMats);

                if(curError>threshold)
                {
                    Weights[m][n]=0;
                    Weights[n][m]=0;
                }
                else
                {
                    Weights[m][n]=pow((1.0-(pow((curError/threshold),2))),2);
                    Weights[n][m]=pow((1.0-(pow((curError/threshold),2))),2);
                }

                fprintf(fpe,"   %f,   ",curError);
                fprintf(fpw,"   %f,   ",Weights[m][n]);
                // printf("(%d, %d) %f w-> %f e \t",m,n,Weights[m][n],curError);
            }
            // printf(" \n");
        }
        //  printf(" _________________________________\n");
        fprintf(fpe," \n");
        fprintf(fpw," \n");
    }

    fclose(fpe);
    fclose(fpw);
    for(i=0; i<4; i++)
    {
        cvReleaseMat(&tempMats[i]);
    }

}


void transferIntrinsicBufferToMatrices(SCinputs* mySCinputs,double *p)
{
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


}


void errnonLinFunctionSelfCalibmestimator(double *p, double *hx, int m, int n, void *adata)
{

    SCinputs* mySCinputs=(SCinputs*)adata;


    int i,j;

    int numFrames=0;



    numFrames=mySCinputs->numFrames;

    vector< vector<CvMat*> > *FMat=(vector< vector<CvMat*> >*)mySCinputs->funds;

    vector< vector<double> >* myWeights=(vector< vector<double> >*)mySCinputs->Weights;

    vector<CvMat* > *tempMtx=  (vector< CvMat* >*) mySCinputs->tempMat;

    vector<CvMat* > *pintrin= (vector< CvMat* >*)mySCinputs->intrin;

    transferIntrinsicBufferToMatrices(mySCinputs,p);


    double count=0;
    double totEr=0;

    double curError=0;

    double sumweights=0;

    for (int i=0; i<n; i++)
    {
        hx[i]=0;
    }

    for ( i = 0; i < numFrames; ++i)
    {

        for ( j = 0; j < i; ++j)
        {
            sumweights+=(*myWeights)[i][j];
        }
    }

    int numfr;
    int counter=0;
    totEr=0;
    //printf("number of frames is %d \n",numFrames);
    for ( i = 0; i < numFrames; ++i)
    {

        for ( j = 0; j < i; ++j)
        {
            curError=0;
            if((*myWeights)[i][j]>FLT_EPSILON)
            {
                curError=(((*myWeights)[i][j])*findSVDerror((*pintrin)[j],(*pintrin)[i],(*FMat)[i][j],tempMtx))/sumweights;
                totEr=totEr+curError;
            }

         //    printf("weight %f product %f given to matrix [%d][%d] with content:\n",(*myWeights)[i][j],curError,i,j);
          //    writeCVMatrix(cout<<"Mat is "<<endl,(*FMat)[i][j]);




        }
    }
//printf("number of measurements was %d \n",counter-1);

    // printf("num frames was %d\n",numFrames);

//the errors in hx are a source of confusion
//the right way to do this is to assign the difference in the svd of each fundamental matrix
//to one element inside the vector hx, but that would make levmar not work sometimes if the
//number of fundamental matrices is not large enough , so if n<m you get an error from the function
//also you might think i am messing mestimator by putting all errors into one but the reality is that
///the real errros aref found in the calling function

// this scaling is important to make sure we dont stop because of a small dp
//double mult=1;
double mult=1000000.0;
//this is to sum all the errors
totEr=totEr*mult;
    for (int i=0; i<n; i++)
    {
        hx[i]=(totEr);
    }
//printf("error was %f \n",totEr );

//    printf("errrs are :\n");
//    for (int i=0; i<n; i++)
//        printf("hx %d is %f\n",i,hx[i]);

}
double findSVDerrorSequence(vector< vector<CvMat*> > const &FV,  vector<CvMat*>  &KV )
{
    double sumSVDerr=0;

    int numFrames=KV.size();

    vector<CvMat* > tempMats;
    tempMats.resize(4);
    for(int i=0; i<4; i++)
    {
        tempMats[i]=cvCreateMat(3,3, CV_64F);
        cvSetZero(tempMats[i]);
    }

    for(int m=0; m<numFrames; m++)
    {
        for(int n=0; n<m; n++)
        {
            sumSVDerr+=findSVDerror(KV[n],KV[m],FV[m][n],&tempMats);
        }
    }

    for(int i=0; i<4; i++)
    {
        cvReleaseMat(&tempMats[i]);
    }
    return sumSVDerr;
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

//   writeCVMatrix(cout<<"K1"<<endl,k1);
//writeCVMatrix(cout<<"K2"<<endl,k2);
//writeCVMatrix(cout<<"F"<<endl,F);
//printf("Error was %f \n",err);
    if (isinf(err) || isnan(err))
        err=100;

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



