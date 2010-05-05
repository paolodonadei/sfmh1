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

// this is all wrong, you need to find the best two frames and consecutively add more frames
//zzz remove these
    int frame1=indexMax(tempconf);
    frame1=0;
    sfmSequence[0]=frame1;
    tempconf[frame1]=-1;

    int frame2=indexMax(tempconf);
    frame2=1;
    sfmSequence[1]=frame2;


    printf("key frames are %d and %d \n",frame1,frame2);
    initializeKeyFrames(frame1,  frame2);

    DLTUpdateStructure();

    writeStructure("structure1.txt");

//    sfmSequence[2]=2;
//    DLTUpdateStructure();
//
//
//    writeStructure("structure2.txt");
}


HRStructure::HRStructure()
{


}
HRStructure::~HRStructure()
{

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

//frame 1 would go to the coordinate system origin





    int maxlength=(*imSet).myTracks.getNumTracks();

    printf("number of feature matches is %d and numframes is %d \n",(*imSet).myTracks.getNumTracks(),(*imSet).myTracks.getNumFrames());
    structure.resize(maxlength);



    for (j=0; j<maxlength; j++)
    {

        if((*imSet).myTracks.validTrackEntry(j,frame1)!=0 && (*imSet).myTracks.validTrackEntry(j,frame2)!=0 )
            count++;

    }






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

            count++;
        }
    }

//
//
////
//  writeCVMatrix("P1pre.txt",(*((*imSet).imageCollection[frame1])).projectionMatrix);
//
//   writeCVMatrix("P2pre.txt",(*((*imSet).imageCollection[frame2])).projectionMatrix);


//(*imSet).findEssentialMatrices();
    cvMatrixtoBuffer((*((*imSet).imageCollection[frame1])).intrinsicMatrix,&K1, 0);
    cvMatrixtoBuffer((*((*imSet).imageCollection[frame2])).intrinsicMatrix,&K2, 0);

//
    printf("essential amtrix %d - %d was \n",frame2,frame1);
    writeCVMatrix(cout,(*imSet).correspondencesPairWise[frame1][frame2].motion.MotionModel_E);


    cvMatrixtoBuffer((*imSet).correspondencesPairWise[frame1][frame2].motion.MotionModel_E,&E, 0);

    // int num_inliers = compute_pose_ransac(num_pts, k1_pts, k2_pts,K1, K2, (double) 0.05, 2512, R, t);
    int num_inliers= find_extrinsics_essential(E, k1_pts[1], k2_pts[1], R, t);

    BuffertocvMatrix(R,&((*((*imSet).imageCollection[frame2])).camPose.Rm),3,3, 0);
    BuffertocvMatrix(t,&((*((*imSet).imageCollection[frame2])).camPose.tm),3,1, 0);


    //camera 1 goes to origin

    cvSetIdentity((*(*imSet).imageCollection[frame1]).camPose.Rm);
    cvSetZero((*(*imSet).imageCollection[frame1]).camPose.tm);

    printf("**5 point decided the number of inliers are %d\n\n", num_inliers);





    findProjfromcompon((*((*imSet).imageCollection[frame1])));
    findProjfromcompon((*((*imSet).imageCollection[frame2])));

//    readCvMatFfromfile(&((*((*imSet).imageCollection[0])).projectionMatrix),"C:\\Documents and Settings\\hrast019\\Desktop\\data\\euclidean\\merton2\\001.P");
//    readCvMatFfromfile(&((*((*imSet).imageCollection[1])).projectionMatrix),"C:\\Documents and Settings\\hrast019\\Desktop\\data\\euclidean\\merton2\\002.P");
//    readCvMatFfromfile(&((*((*imSet).imageCollection[2])).projectionMatrix),"C:\\Documents and Settings\\hrast019\\Desktop\\data\\euclidean\\merton2\\003.P");
//    cvDecomposeProjectionMatrixHR((*((*imSet).imageCollection[frame1])).projectionMatrix, (*((*imSet).imageCollection[frame1])).intrinsicMatrix,(*((*imSet).imageCollection[frame1])).camPose.Rm,(*((*imSet).imageCollection[frame1])).camPose.tm, 0, 0, 0, 0);
//    cvDecomposeProjectionMatrixHR((*((*imSet).imageCollection[frame2])).projectionMatrix, (*((*imSet).imageCollection[frame2])).intrinsicMatrix,(*((*imSet).imageCollection[frame2])).camPose.Rm,(*((*imSet).imageCollection[frame2])).camPose.tm, 0, 0, 0, 0);
//



    cout<<" R0 :"<<endl;

    writeCVMatrix(cout,(*((*imSet).imageCollection[frame2])).camPose.Rm);

    cout<<" T0 :"<<endl;

    writeCVMatrix(cout,((*((*imSet).imageCollection[frame2])).camPose.tm));




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

//    readCvMatFfromfile(&((*((*imSet).imageCollection[0])).projectionMatrix),"C:\\Documents and Settings\\hrast019\\Desktop\\data\\euclidean\\merton2\\001.P");
//    readCvMatFfromfile(&((*((*imSet).imageCollection[1])).projectionMatrix),"C:\\Documents and Settings\\hrast019\\Desktop\\data\\euclidean\\merton2\\002.P");
//    readCvMatFfromfile(&((*((*imSet).imageCollection[2])).projectionMatrix),"C:\\Documents and Settings\\hrast019\\Desktop\\data\\euclidean\\merton2\\003.P");
    double rerror=0;
    int numReconstructed=0;

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
            numReconstructed++;
            structureValid[i]=numPts;

            structureErrors[i]= cvTriangulatePointsNframs(numPts, projMatrs,projPoints,structure[i] );
            rerror+=structureErrors[i];
        }
        projMatrs.clear();
        projPoints.clear();
    }

    rerror/=((double)numReconstructed);

    printf("reconstruction error was %f\n",rerror);
}
int HRStructure::printSBAstyleData()
{


    int i,j;
    CvMat* curq=cvCreateMat(4,1,CV_64F);


    int maxlength=(*imSet).myTracks.getNumTracks();


    fstream file_cams("cameras.txt" ,ios::out);
    file_cams<<"# fu, u0, v0, ar, s   kc(1:5)   quaternion translation"<<endl;
    for (j = 0; j < numImages; j++)
    {
        if(sfmSequence[j]!=-1)
        {
            int curFrame=sfmSequence[j];
            CvMat* curK=(*((*imSet).imageCollection[curFrame])).intrinsicMatrix;
            CvMat* curR=(*((*imSet).imageCollection[curFrame])).camPose.Rm;
            CvMat* curt=(*((*imSet).imageCollection[curFrame])).camPose.tm;
            CvMat* disto=(*((*imSet).imageCollection[curFrame])).distortion;

            matrix_to_quaternion(curR,curq);

            file_cams<<cvmGet(curK,0,0)<<" "<<cvmGet(curK,0,2)<<" "<<cvmGet(curK,1,2)<<" "<<cvmGet(curK,1,1)/cvmGet(curK,0,0)<<" "<<cvmGet(curK,0,1);
            file_cams<<" "<<cvmGet(disto,0,0)<<" "<<cvmGet(disto,1,0)<<" "<<cvmGet(disto,2,0)<<" "<<cvmGet(disto,3,0)<<" "<<cvmGet(disto,4,0);
            file_cams<<" "<<cvmGet(curq,0,0)<<" "<<cvmGet(curq,1,0)<<" "<<cvmGet(curq,2,0)<<" "<<cvmGet(curq,3,0);
            file_cams<<" "<<cvmGet(curt,0,0)<<" "<<cvmGet(curt,1,0)<<" "<<cvmGet(curt,2,0);
            file_cams<<endl;
        }


    }
    file_cams.close();


/////////////////////
    fstream file_pts("3Dpts.txt" ,ios::out);
    file_pts<<"# # X Y Z  nframes  frame0 x0 y0  frame1 x1 y1 ..."<<endl;
    for ( i = 0; i < maxlength; i++)
    {
        if(structureValid[i]!=0)
        {

            file_pts<<structure[i].x<<" "<<structure[i].y<<" "<<structure[i].z<<" "<<structureValid[i]<<" ";
            for (j = 0; j < numImages; j++)
            {



                if(sfmSequence[j]!=-1)
                {

                    int curFrame=sfmSequence[j];
                    if((*imSet).myTracks.validTrackEntry(i,curFrame)!=0)
                    {
                        CvPoint2D32f  curPt=(*imSet).myTracks.pointFromTrackloc(i, curFrame);
                        file_pts<<curFrame<<" "<<curPt.x<<" "<<curPt.y<<" ";

                    }
                }
            }
            file_pts<<endl;
        }

    }

    file_pts.close();



    cvReleaseMat(&curq);
    return 0;

}
void HRStructure::writeStructure(string fn)
{
    int i,j;
    int maxlength=(*imSet).myTracks.getNumTracks();



    string tempfilename=fn;
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

            fp_out<< setw(17)<<setprecision(10)<<structure[i].x << "\t"<<structure[i].y << "\t"<<structure[i].z<< "\t"<<structureErrors[i];
            for (j = 0; j < numImages; j++)
            {
                if(sfmSequence[j]!=-1)
                {
                    int curFrame=sfmSequence[j];
                    CvPoint2D32f  curPt=(*imSet).myTracks.pointFromTrackloc(i, curFrame);
                    fp_out<< "\t" << curFrame << "\t" << curPt.x<< "\t" << curPt.y;
                }

            }
            fp_out<<endl;
        }




    }
    fp_out.close();
}
