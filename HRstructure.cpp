#include "HRstructure.h"
#include "5point.h"
#include "vector.h"
#include "visiongen.h"
#include "general.h"
#include "boost/filesystem.hpp"   // includes all needed Boost.Filesystem declarations
#include "boost/filesystem/operations.hpp"
#include "boost/filesystem/path.hpp"
#include "matrix.h"
#include "triangulate.h"
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


//zzz remove these
    int frame1=indexMax(tempconf);

    sfmSequence[0]=frame1;
    tempconf[frame1]=-1;

    int frame2=indexMax(tempconf);

    sfmSequence[1]=frame2;


    printf("key frames are %d and %d \n",frame1,frame2);
    initializeKeyFrames(frame1,  frame2);

    DLTUpdateStructure();

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
    double* E= new double[10];







    int maxlength=(*imSet).myTracks.getNumTracks();

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



//    readCvMatFfromfile(&((*((*imSet).imageCollection[0])).projectionMatrix),"C:\\Documents and Settings\\hrast019\\Desktop\\data\\euclidean\\unilibrary\\001.P");
//    readCvMatFfromfile(&((*((*imSet).imageCollection[1])).projectionMatrix),"C:\\Documents and Settings\\hrast019\\Desktop\\data\\euclidean\\unilibrary\\002.P");
//   readCvMatFfromfile(&((*((*imSet).imageCollection[2])).projectionMatrix),"C:\\Documents and Settings\\hrast019\\Desktop\\data\\euclidean\\unilibrary\\003.P");
//    cvDecomposeProjectionMatrixHR((*((*imSet).imageCollection[frame1])).projectionMatrix, (*((*imSet).imageCollection[frame1])).intrinsicMatrix,Rident,tzero, 0, 0, 0, 0);
//   cvDecomposeProjectionMatrixHR((*((*imSet).imageCollection[frame2])).projectionMatrix, (*((*imSet).imageCollection[frame2])).intrinsicMatrix, poses[0].Rm,poses[0].tm, 0, 0, 0, 0);
//
//  writeCVMatrix("P1pre.txt",(*((*imSet).imageCollection[frame1])).projectionMatrix);
//
//   writeCVMatrix("P2pre.txt",(*((*imSet).imageCollection[frame2])).projectionMatrix);

    cvSetIdentity(Rident);
    cvSetZero(tzero);
//(*imSet).findEssentialMatrices();
    cvMatrixtoBuffer((*((*imSet).imageCollection[frame1])).intrinsicMatrix,&K1, 0);
    cvMatrixtoBuffer((*((*imSet).imageCollection[frame2])).intrinsicMatrix,&K2, 0);

//
//printf("essential amtrix %d - %d was \n",frame2,frame1);
// writeCVMatrix(cout,(*imSet).correspondencesPairWise[frame1][frame2].motion.MotionModel_E);


    cvMatrixtoBuffer((*imSet).correspondencesPairWise[frame1][frame2].motion.MotionModel_E,&E, 0);

   //int num_inliers = compute_pose_ransac(num_pts, k1_pts, k2_pts,K1, K2, (double) 0.05, 2512, R, t);
    int num_inliers= find_extrinsics_essential(E, k1_pts[1], k2_pts[1], R, t);

    BuffertocvMatrix(R,&(poses[0].Rm),3,3, 0);

//    matrix_transpose_product(3, 3, 3, 1, R, t, tscaled);
//   matrix_scale(3, 1, tscaled, -1.0, t);

    BuffertocvMatrix(t,&(poses[0].tm),3,1, 0);
//normalizeMatrix(poses[0].tm);
    printf("**5 point decided the number of inliers are %d\n\n", num_inliers);

//
//
// readCvMatFfromfile(&(poses[0].Rm),"C:\\Documents and Settings\\hrast019\\Desktop\\data\\euclidean\\merton2\\R2.txt");
// readCvMatFfromfile(&(poses[0].tm),"C:\\Documents and Settings\\hrast019\\Desktop\\data\\euclidean\\merton2\\t2.txt");
//
//readCvMatFfromfile(&(Rident),"C:\\Documents and Settings\\hrast019\\Desktop\\data\\euclidean\\merton2\\R1.txt");
// readCvMatFfromfile(&(tzero),"C:\\Documents and Settings\\hrast019\\Desktop\\data\\euclidean\\merton2\\t1.txt");
//



    cout<<" R0 :"<<endl;

    writeCVMatrix(cout,poses[0].Rm);

    cout<<" T0 :"<<endl;

    writeCVMatrix(cout,poses[0].tm);






    findProjfromcompon((*((*imSet).imageCollection[frame1])).projectionMatrix,Rident,tzero,(*((*imSet).imageCollection[frame1])).intrinsicMatrix);
    findProjfromcompon((*((*imSet).imageCollection[frame2])).projectionMatrix,poses[0].Rm,poses[0].tm,(*((*imSet).imageCollection[frame2])).intrinsicMatrix);

//readCvMatFfromfile(&((*((*imSet).imageCollection[frame2])).projectionMatrix),"C:\\Documents and Settings\\hrast019\\Desktop\\data\\euclidean\\merton2\\pguess.txt");


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
//    readCvMatFfromfile(&((*((*imSet).imageCollection[0])).projectionMatrix),"C:\\Documents and Settings\\hrast019\\Desktop\\data\\euclidean\\merton2\\001.P");
//    readCvMatFfromfile(&((*((*imSet).imageCollection[1])).projectionMatrix),"C:\\Documents and Settings\\hrast019\\Desktop\\data\\euclidean\\merton2\\002.P");
//    readCvMatFfromfile(&((*((*imSet).imageCollection[2])).projectionMatrix),"C:\\Documents and Settings\\hrast019\\Desktop\\data\\euclidean\\merton2\\003.P");
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
