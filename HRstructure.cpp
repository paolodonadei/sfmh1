#include "HRstructure.h"
#include "5point.h"
#include "vector.h"
#include "visiongen.h"

HRStructure::HRStructure(HRImageSet* pimSet )
{
    int i;
    imSet=pimSet;
    numImages=(*pimSet).imageCollection.size();
    poses.resize(numImages);

    for(i=0; i<numImages; i++)
    {
        poses[i].tm=cvCreateMat(3,1,CV_64F);
        poses[i].Rm=cvCreateMat(3,3,CV_64F);

    }

}
void HRStructure::run()
{
    int i;
    vector<double> tempconf;
    tempconf.resize((*imSet).confid.size());
    sfmSequence.resize((*imSet).confid.size());
    for(i=0; i< (*imSet).confid.size(); i++)
    {
        tempconf[i]=(*imSet).confid[i];
        sfmSequence[i]=0;
    }

    int frame1=indexMax(tempconf);
    sfmSequence[0]=frame1;
    tempconf[frame1]=0;
    int frame2=indexMax(tempconf);
    sfmSequence[1]=frame2;
printf("key frames are %d and %d \n",frame1,frame2);
    initializeKeyFrames(frame1,  frame2);
}


HRStructure::HRStructure()
{


}
HRStructure::~HRStructure()
{
    int i;
    for(i=0; i<numImages; i++)
    {
        cvReleaseMat(&poses[i].tm);
        cvReleaseMat(&poses[i].Rm);

    }
}
int HRStructure::initializeKeyFrames(int frame1, int frame2)
{
    int i,j;
    int count=0;
    int maxlength=(*imSet).myTracks.trackMatrix[0].size();

    for (j=0; j<maxlength; j++)
    {

        if((*imSet).myTracks.trackMatrix[frame1][j]!=-1 && (*imSet).myTracks.trackMatrix[frame2][j]!=-1 )
            count++;

    }



    poses[0].frame1=frame1;
    poses[0].frame2=frame2;

    double* R=new double[9];
    double* t= new double[3]  ;
    double* K1= new double[10];
    double* K2= new double[10];

    int num_pts = (int) count;

    v2_t *k1_pts = new v2_t[num_pts];
    v2_t *k2_pts = new v2_t[num_pts];

    for (int i = 0; i < maxlength; i++)
    {
        if((*imSet).myTracks.trackMatrix[frame1][i]!=-1 && (*imSet).myTracks.trackMatrix[frame2][i]!=-1 )
        {

            CvPoint2D32f p1 = (*imSet).myTracks.pointFromTrackloc(i, frame1) ;
            CvPoint2D32f p2 = (*imSet).myTracks.pointFromTrackloc(i, frame2) ;

            k1_pts[i] = v2_new(p1.x, p1.y);
            k2_pts[i] = v2_new(p2.x, p2.y);
        }
    }



    cvMatrixtoBuffer((*((*imSet).imageCollection[frame1])).intrinsicMatrix,&K1, 0);
    cvMatrixtoBuffer((*((*imSet).imageCollection[frame2])).intrinsicMatrix,&K2, 0);


    int num_inliers = compute_pose_ransac(num_pts, k1_pts, k2_pts,K1, K2, (double) 0.25 * 9, 512, R, t);


    BuffertocvMatrix(t,&poses[0].tm,1,3, 1);
    BuffertocvMatrix(R,&poses[0].Rm,3,3, 1);

    cout<<" R0 :"<<endl;

    writeCVMatrix(cout,poses[0].Rm);

    cout<<" T0 :"<<endl;

    writeCVMatrix(cout,poses[0].tm);


    ////find projection matrices
    CvMat* Rident=cvCreateMat(3,3,CV_64F);
    CvMat* tzero=cvCreateMat(3,3,CV_64F);
    cvSetIdentity(Rident);
    cvSetZero(tzero);

    findProjfromcompon((*((*imSet).imageCollection[frame1])).projectionMatrix,Rident,tzero,(*((*imSet).imageCollection[frame1])).intrinsicMatrix);
    findProjfromcompon((*((*imSet).imageCollection[frame2])).projectionMatrix,poses[0].Rm,poses[0].tm,(*((*imSet).imageCollection[frame2])).intrinsicMatrix);

    cout<<" P0 :"<<endl;

    writeCVMatrix(cout,(*((*imSet).imageCollection[frame1])).projectionMatrix);

    cout<<" P1 :"<<endl;

    writeCVMatrix(cout,(*((*imSet).imageCollection[frame1])).projectionMatrix);

//left here
yo yo i left here, so now you have to add in teh stuff for triangulation,
the bundler has a triangulation file inside the image file i think, use that
one maybe if its approproiate and save the 3D data in the meshlab format


    delete [] k1_pts;
    delete [] k2_pts;
    delete []  R;
    delete []  t;
    delete []  K1;
    delete []  K2;

    cvReleaseMat(&Rident);
    cvReleaseMat(&tzero);
    return num_inliers;

}
double HRStructure::bundleAdjust()
{

}
int HRStructure::addFrame(int framenum)
{

}

