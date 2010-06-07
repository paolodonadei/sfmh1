#include <fstream>
#include <iostream>
#include <iomanip>
#include <sstream>
#include <ctime>
#include <sys/time.h>
#include <cmath>
#include <climits>
#include <vector>
#include <stdio.h>
#include <algorithm>
#include "general.h"
#include "robust_est.h"
#include "float.h"
const double eps=1e-20;
vector<double> tempVector1;

//this doesnt return a model, just the set of inliers which can be used later to do a final fit
int ROBUST_EST(const CvMat* data,vector<double>& aprioris,int  (*fitFunctionPtr)(const CvMat* ,vector<CvMat*>& models ),
               double (*distanceFunctionPTR)(const CvMat* , const CvMat* ,vector<double>&), bool (*degenFunctionPTR)( const CvMat*),
               int s, double t, vector<bool>& inliers,int maxTrials, int maxDataTrials)

{
    int i,j;
    const double p = 0.99; //probability
    int numModels=0;
    vector<int> rndSamples(s,0);
    vector<int> rndSamplesBest(s,0);
    CvMat* rndSampleData=cvCreateMat(data->rows,s,CV_64F);

    int chosenIteration;
    int trialcount = 0;
    int N = 1;

    bool degenerate=true;
    int count;
    double score=0;
    double bestscore =  -FLT_MAX;
    int numBestInliers=0;
    int size=data->cols;
    vector<bool> bestInliers(size,false);
    vector<double> residuals(size,0.0);
    vector<CvMat*> m;

    int numInliers=0;


    srand ( time(NULL) );


    while ( trialcount<N && trialcount <= maxTrials)
    {
        degenerate=1;
        count=0;
        while (degenerate==true && count<maxDataTrials)
        {
//printf("we have %d samples\n",data->cols);
            drawRandSampleMonteCarlo(rndSamples,aprioris,true);   //fill up our randsample array with random sample indices

            extractCopySamples(rndSamples, rndSampleData, data); //this copies elements from data whose indices are in rndSamples
            degenerate=degenFunctionPTR(rndSampleData);
            count++;



            for(int pp=0; pp<m.size(); pp++) cvReleaseMat(&m[pp]);
            m.clear();



            numModels=fitFunctionPtr(rndSampleData,m);
//            printf("feeeteeeeeeccceeng  %d models\n",m.size());
            if (numModels==0)
            {
                degenerate=true;
            }
        }

        if (degenerate==true)
        {
            trialcount = trialcount + 1;
            printf("A random sample could not be selected, there are either too many degenrate samples or maxDataTrials is too low\n");
            break;
        }

        for(int pp=0; pp<m.size(); pp++)
        {


            distanceFunctionPTR(data,m[pp],residuals);

            //   printf("_____________________________________________\n");
            for (i=0; i<size; i++)
            {
                inliers[i]=(residuals[i]<=t)?true:false;
                //    printf("%s: trial %d residual of point %d was %20.20f and threshold was %0.9f NUMBER OF INLIERS IS %d\n",(inliers[i])?"inlier":"outlier",trialcount ,i,residuals[i],t,countTrues(inliers));

            }

            numInliers=countTrues(inliers);

            score=numInliers; //maybe use another function to create the score


//            printf("score was %f and number of inliers was %d\n",score,numInliers);




            if (score>bestscore)
            {
                rndSamplesBest= rndSamples ;
                bestscore=score;
                N=findTrialCount(numInliers,size, s, p); //this function finds the new max iteration count based on the new number of inliers
                bestInliers=inliers;
                numBestInliers=numInliers;
                printf("trialcount %d best score found was %f number of inliers is %d from a total of %d points with threshold %f\n",trialcount,score,numBestInliers,inliers.size(),t);


                chosenIteration=trialcount;

            }
        }
        trialcount = trialcount + 1;


    }
//free the last model
    for(int pp=0; pp<m.size(); pp++) cvReleaseMat(&m[pp]);
    m.clear();

    if (numBestInliers<s)
    {
        printf("the number of inliers found was less than the minimum number of points required to fit a model\n");
        printf("status of robust estimator: trial count is %d, last number of inliers was %d, best score was %f, N was %d\n",trialcount, numInliers, bestscore, N);

        return 0;
    }

    if (trialcount > maxTrials)
    {
        printf("ransac reached max trial counts!\n");

    }


    inliers=bestInliers;



    cvReleaseMat(&rndSampleData);
    return 1;
}




int findTrialCount(int score,int size,int s,double p)
{
    /// zzz guided mlesac paper here uses the sum of all pvis as the number of inliers, maybe i should use that
    int N;
    double  fracinliers =  ((double)score)/((double)size);
    double   pNoOutliers = 1 -  pow(fracinliers,s);
    pNoOutliers = max(eps, pNoOutliers);
    pNoOutliers = min(1-eps, pNoOutliers);//avoid division by zero
    double num=(double)log(1.00-p);
    double denom=log(pNoOutliers);
    num=num/denom;
    if (num>INT_MAX) num=INT_MAX; //overflow
    N = (int)rint(num);


    //printf("number of inliers was %d , size of data was %d and number of min sampling was %d and p was %f = new N is %d\n",score, size, s,p,N);

    return N;

}

int  countTrues(const vector<bool>& vec)
{

    return (int) count (vec.begin(), vec.end(), true);


}


int extractCopySamples(vector<int>& rndSamples,CvMat* rndSampleData,const CvMat* data)
{
    int i,j;

    if (rndSamples.size()>data->cols)
    {
        printf("error in extractcopy, the requested number of elements to be extracted is larger than the data pool, aborting\n");
        exit(0);
    }


    if (rndSamples.size()!=rndSampleData->cols)
    {
        printf("error in extractcopy, \n");
        exit(0);
    }


//assume data points are each a single column, as assumed throughout the project
    for (i=0; i<rndSamples.size(); i++)
    {
        if (rndSamples[i]<data->cols&& rndSamples[i]>=0)
        {

            for (j=0; j<data->rows; j++)
            {
                cvmSet(rndSampleData,j,i,cvmGet(data,j,rndSamples[i]));
            }

        }
        else
        {
            printf("error, wrong index in extractcopy : %d when we only have points up to %d , aborting\n",rndSamples[i],data->cols);
            abort();
        }


    }



    return 1;

}






int  drawRandSampleMonteCarlo(vector<int>& rndSample, const vector<double>& pvis,bool MCS)
{
    int newNum=0;
    int flag=0;
    int i,j;
    int size=pvis.size();
    double prob1;

    //here we are using global variables, we cant allocate new emory every time we call this function
    if (tempVector1.size()<pvis.size()) tempVector1.resize(pvis.size()+2);
    vector<double>& probContinuum=tempVector1;

    int k;

    probContinuum[0]=0;
    double p=0;

    for (i=1; i<=size; i++)
    {
        p=(MCS)?pvis[i-1]:((double)0.5);
        probContinuum[i]=p +probContinuum[i-1];
        // printf("p  of %d is %f \t",i,probContinuum[i]);

    }


    int minSize=rndSample.size();

    if (probContinuum[size]<0)
    {
        printf("the pvi arrat is all zeros, aborting\n");
        exit(1);
    }

    for (i=0; i<minSize; i++)
    {
        do
        {
            prob1=random_uniform_0_1()*probContinuum[size];

            k=1;
            while ((probContinuum[k-1]<prob1 && probContinuum[k]>=prob1)==false && k<=size)
            {
                k++;

            }

            newNum=k-1;

//            if(newNum>=size)
//            {
//            continue;
//            }

            //printf("chose %d which is outlier %d\n",newNum,feat_corr[newNum].outlier);

            //Test this number to ensure it has not already been selected
            flag=0;

            for (j=0; j<i; j++)
            {
                if (rndSample[j] == newNum)
                {
                    flag = 1;
                    break;
                }
            }

        }
        while (flag == 1);
        rndSample[i] = newNum;
        //    printf(" %d ,",newNum);
        if(newNum>=pvis.size())
        {
            printf("chose sample %d when we only had %d samples\n");
            exit(1);
        }

    }

    //   printf(" \n ,");

    return 1;
}

