#include "HRstructure.h"
#include "5point.h"
#include "vector.h"
#include "visiongen.h"
#include "boost/filesystem.hpp"   // includes all needed Boost.Filesystem declarations
#include "boost/filesystem/operations.hpp"
#include "boost/filesystem/path.hpp"
#include "matrix.h"
namespace fs = boost::filesystem;

HRStructure::HRStructure(HRImageSet* pimSet,string pdir )
{
    tempdir= pdir;
    int i;
    imSet=pimSet;
    numImages=(*pimSet).imageCollection.size();

    (*imSet).myTracks.writeTrackMatrix("trackafter1.txt");
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
    tempconf.resize((*imSet).numImages);
    sfmSequence.resize((*imSet).numImages);


    for(i=0; i< (*imSet).confid.size(); i++)
    {
        tempconf[i]=(*imSet).confid[i];
        sfmSequence[i]=-1;

    }

    int maxlength=(*imSet).myTracks.getNumTracks();
    structureErrors.resize(maxlength);
    structureValid.resize(maxlength);
    for(i=0; i< maxlength; i++)
    {
        structureErrors[i]=0;
        structureValid[i]=0;
    }


    int frame1=indexMax(tempconf);
    sfmSequence[0]=frame1;
    tempconf[frame1]=-1;
    int frame2=indexMax(tempconf);
    sfmSequence[1]=frame2;
    (*imSet).myTracks.writeTrackMatrix("trackafter2.txt");
    printf("key frames are %d and %d \n",frame1,frame2);
    initializeKeyFrames(frame1,  frame2);
    (*imSet).myTracks.writeTrackMatrix("trackafter3.txt");
    DLTUpdateStructure();
    (*imSet).myTracks.writeTrackMatrix("trackafter4.txt");
    writeStructure();
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
    ////find projection matrices
    CvMat* Rident=cvCreateMat(3,3,CV_64F);
    CvMat* tzero=cvCreateMat(3,1,CV_64F);

    double* R=new double[9];
    double* t= new double[3]  ;
    double* tscaled= new double[3]  ;
    double* K1= new double[10];
    double* K2= new double[10];


    int maxlength=(*imSet).myTracks.getNumTracks();
    printf("hi jackass\n");
    printf("number of feature matches is %d and numframes is %d \n",(*imSet).myTracks.getNumTracks(),(*imSet).myTracks.getNumFrames());
    structure.resize(maxlength);



    for (j=0; j<maxlength; j++)
    {

        if((*imSet).myTracks.validTrackEntry(j,frame1)!=0 && (*imSet).myTracks.validTrackEntry(j,frame2)!=0 )
            count++;

    }




    poses[0].frame1=frame1;
    poses[0].frame2=frame2;



    int num_pts = (int) count;

    v2_t *k1_pts = new v2_t[num_pts];
    v2_t *k2_pts = new v2_t[num_pts];


    count=0;
    for (int i = 0; i < maxlength; i++)
    {
        if((*imSet).myTracks.validTrackEntry(i,frame1)!=0 && (*imSet).myTracks.validTrackEntry(i,frame2)!=0 )
        {

            CvPoint2D32f p1 = (*imSet).myTracks.pointFromTrackloc(i, frame1) ;
            CvPoint2D32f p2 = (*imSet).myTracks.pointFromTrackloc(i, frame2) ;

            k1_pts[count] = v2_new(p1.x, p1.y);
            k2_pts[count] = v2_new(p2.x, p2.y);
            //  printf("point %d is x=%f and y=%f out of %d\n",count,(double)k1_pts[count].p[0],(double)k1_pts[count].p[1]   ,num_pts);
            count++;
        }
    }



    readCvMatFfromfile(&((*((*imSet).imageCollection[0])).projectionMatrix),"C:\\Documents and Settings\\hrast019\\Desktop\\data\\euclidean\\merton1\\001.P");
    readCvMatFfromfile(&((*((*imSet).imageCollection[1])).projectionMatrix),"C:\\Documents and Settings\\hrast019\\Desktop\\data\\euclidean\\merton1\\002.P");
    readCvMatFfromfile(&((*((*imSet).imageCollection[2])).projectionMatrix),"C:\\Documents and Settings\\hrast019\\Desktop\\data\\euclidean\\merton1\\003.P");
    cvDecomposeProjectionMatrixHR((*((*imSet).imageCollection[frame1])).projectionMatrix, (*((*imSet).imageCollection[frame1])).intrinsicMatrix,Rident,tzero, 0, 0, 0, 0);
    cvDecomposeProjectionMatrixHR((*((*imSet).imageCollection[frame2])).projectionMatrix, (*((*imSet).imageCollection[frame2])).intrinsicMatrix, poses[0].Rm,poses[0].tm, 0, 0, 0, 0);


    cvSetIdentity(Rident);
    cvSetZero(tzero);

    cvMatrixtoBuffer((*((*imSet).imageCollection[frame1])).intrinsicMatrix,&K1, 0);
    cvMatrixtoBuffer((*((*imSet).imageCollection[frame2])).intrinsicMatrix,&K2, 0);



    int num_inliers = compute_pose_ransac(num_pts, k1_pts, k2_pts,K1, K2, (double) 0.05, 2512, R, t);

//i dont knwo why this is a transpose
    //matrix_transpose_product(3, 3, 3, 1, R, t, tscaled);
    //  matrix_scale(3, 1,  tscaled, -1.0,t);
////zzz remove the next 5 lines ground truth K, R , T



    printf("**5 point decided the number of inliers are %d\n\n", num_inliers);

    BuffertocvMatrix(t,&(poses[0].tm),3,1, 0);
    BuffertocvMatrix(R,&(poses[0].Rm),3,3, 0);

normalizeMatrix(poses[0].tm);//normalizing the translation

// 5 point method is being a retard

    cout<<" R0 :"<<endl;

    writeCVMatrix(cout,poses[0].Rm);

    cout<<" T0 :"<<endl;

    writeCVMatrix(cout,poses[0].tm);






    findProjfromcompon((*((*imSet).imageCollection[frame1])).projectionMatrix,Rident,tzero,(*((*imSet).imageCollection[frame1])).intrinsicMatrix);
    findProjfromcompon((*((*imSet).imageCollection[frame2])).projectionMatrix,poses[0].Rm,poses[0].tm,(*((*imSet).imageCollection[frame2])).intrinsicMatrix);

    cout<<" P0 :"<<endl;

    writeCVMatrix(cout,(*((*imSet).imageCollection[frame1])).projectionMatrix);

    cout<<" P1 :"<<endl;

    writeCVMatrix(cout,(*((*imSet).imageCollection[frame2])).projectionMatrix);




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
void HRStructure::DLTUpdateStructure()
{

    int i,j;

//
//
////zzz remove this, it uses actual projection amtrices
//    readCvMatFfromfile(&((*((*imSet).imageCollection[0])).projectionMatrix),"C:\\Documents and Settings\\hrast019\\Desktop\\data\\euclidean\\merton1\\001.P");
//    readCvMatFfromfile(&((*((*imSet).imageCollection[1])).projectionMatrix),"C:\\Documents and Settings\\hrast019\\Desktop\\data\\euclidean\\merton1\\002.P");
//    readCvMatFfromfile(&((*((*imSet).imageCollection[2])).projectionMatrix),"C:\\Documents and Settings\\hrast019\\Desktop\\data\\euclidean\\merton1\\003.P");
////zzz remove this , this is the projective geometry
//ProjectiveMatFromF( (*imSet).correspondencesPairWise[sfmSequence[0]][sfmSequence[1]].motion.MotionModel_F, ((*((*imSet).imageCollection[sfmSequence[0]])).projectionMatrix),((*((*imSet).imageCollection[sfmSequence[1]])).projectionMatrix));
//


    int count=0;
    for(i=0; i< numImages; i++)
    {
        if(sfmSequence[i]!=-1) //this array should indicate how many frazmes have had their proj matrices found
            count++;
    }

    int maxlength=(*imSet).myTracks.getNumTracks();


    int numValidProjs= count;


    for ( i = 0; i < maxlength; i++)
    {
        vector<CvMat*> projMatrs;
        vector<CvPoint2D32f> projPoints;
        int numPts=0;
        for (j = 0; j < numValidProjs; j++)
        {
            int curFrame=sfmSequence[j];
            if((*imSet).myTracks.validTrackEntry(i,curFrame)!=0)
            {

                numPts++;
                CvPoint2D32f  curPt=(*imSet).myTracks.pointFromTrackloc(i, curFrame);
                projMatrs.push_back((*((*imSet).imageCollection[curFrame])).projectionMatrix);
                // printf("requesting feature %d from frame %d and track was %d\n",i,curFrame, (*imSet).myTracks.valueTrackEntry(i,curFrame));
                //printf("point was x=%f and y=%f\n",curPt.x,curPt.y);
                projPoints.push_back(curPt);
            }
        }
        if(numPts>1)
        {

            structureValid[i]++;

            structureErrors[i]= cvTriangulatePointsNframs(numPts, projMatrs,projPoints,structure[i] );

        }
        projMatrs.clear();
        projPoints.clear();
    }


}

void HRStructure::writeStructure()
{
    int i;
    int maxlength=(*imSet).myTracks.getNumTracks();



    string tempfilename="structure.txt";
    fs::path p( tempfilename, fs::native );



    string fname_out=(fs::path( tempdir, fs::native )/fs::path( tempfilename, fs::native )).file_string();


    fstream fp_out;

    fp_out.open(fname_out.c_str(), ios::out);

    cout<<fname_out<<endl;
    if (!fp_out)
    {
        cout << "Cannot open file.\n";
        return ;
    }



    for ( i = 0; i < maxlength; i++)
    {
        if(structureValid[i]>0)
        {

            fp_out<< setw(17)<<setprecision(10)<<structure[i].x << "\t"<<structure[i].y << "\t"<<structure[i].z<< "\t"<<structureErrors[i]<<endl;

        }

    }








    fp_out.close();

}
