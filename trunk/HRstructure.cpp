#include "HRstructure.h"
#include "5point.h"
#include "vector.h"
#include "visiongen.h"
#include "general.h"
#include "boost/filesystem.hpp"   // includes all needed Boost.Filesystem declarations
#include "boost/filesystem/operations.hpp"
#include "boost/filesystem/path.hpp"
#include "matrix.h"
#include <sba.h>

#include "eucsbademo.h"
#include "triangulate.h"


namespace fs = boost::filesystem;

HRStructure::HRStructure(HRImageSet* pimSet,string pdir )
{
    tempdir= pdir;
    int i;
    imSet=pimSet;
    numImages=(*pimSet).imageCollection.size();


}
void HRStructure::run()
{
    int i;
    vector<double> tempconf;
    tempconf.resize((*imSet).numImages);
    sfmSequence.resize((*imSet).numImages);


    for(i=0; i< (*imSet).numImages; i++)
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
    frame2=1 ;
    sfmSequence[1]=frame2;

///zzz remove this
//    readCvMatFfromfile(&((*((*imSet).imageCollection[0])).projectionMatrix),"C:\\Documents and Settings\\hrast019\\Desktop\\data\\euclidean\\merton1\\001.P");
//    readCvMatFfromfile(&((*((*imSet).imageCollection[1])).projectionMatrix),"C:\\Documents and Settings\\hrast019\\Desktop\\data\\euclidean\\merton1\\002.P");
//    readCvMatFfromfile(&((*((*imSet).imageCollection[2])).projectionMatrix),"C:\\Documents and Settings\\hrast019\\Desktop\\data\\euclidean\\merton1\\003.P");




    printf("key frames are %d and %d \n",frame1,frame2);
    initializeKeyFrames(frame1,  frame2);

    DLTUpdateStructure();


    for(i=0; i<numImages; i++)
    {
        if(i!=frame1 && i!=frame2)
        {
            addFrame(i);
            printf("added frame %d \n",i);
            DLTUpdateStructure();
        }
    }




    writeStructure("structurelast.txt");


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
    CvMat* Ttemp=cvCreateMat(3,1,CV_64F);
    CvMat* Ttemp2=cvCreateMat(3,3,CV_64F);





    int maxlength=(*imSet).myTracks.getNumTracks();

    printf("number of feature matches is %d and numframes is %d \n",(*imSet).myTracks.getNumTracks(),(*imSet).myTracks.getNumFrames());
    structure.resize(maxlength);


    int indexfirstmatch=0;
    for (j=0; j<maxlength; j++)
    {

        if((*imSet).myTracks.validTrackEntry(j,frame1)!=0 && (*imSet).myTracks.validTrackEntry(j,frame2)!=0 )
        {
            indexfirstmatch=j;
            break;
        }
        count++;

    }






//2nd frame
    decomposeEssential((*imSet).correspondencesPairWise[frame1][frame2].motion.MotionModel_E, (*imSet).myTracks.pointFromTrackloc(indexfirstmatch, frame1),(*imSet).myTracks.pointFromTrackloc(indexfirstmatch, frame2),(*((*imSet).imageCollection[frame1])).intrinsicMatrix,
                       (*((*imSet).imageCollection[frame2])).intrinsicMatrix,((*((*imSet).imageCollection[frame2])).camPose.Rm),((*((*imSet).imageCollection[frame2])).camPose.tm));






//the first frame at origin
    cvSetIdentity((*(*imSet).imageCollection[frame1]).camPose.Rm);
    cvSetZero((*(*imSet).imageCollection[frame1]).camPose.tm);





//us the poses and the intrinsics to compose projection matrices

    findProjfromcompon((*((*imSet).imageCollection[frame1])));
    findProjfromcompon((*((*imSet).imageCollection[frame2])));





    cout<<" R1 :"<<endl;

    writeCVMatrix(cout,(*((*imSet).imageCollection[frame2])).camPose.Rm);

    cout<<" T1 :"<<endl;

    writeCVMatrix(cout,((*((*imSet).imageCollection[frame2])).camPose.tm));




    cout<<" P0 :"<<endl;

    writeCVMatrix(cout,(*((*imSet).imageCollection[frame1])).projectionMatrix);

    cout<<" P1 :"<<endl;

    writeCVMatrix(cout,(*((*imSet).imageCollection[frame2])).projectionMatrix);


    cvReleaseMat(&Ttemp);
    cvReleaseMat(&Ttemp2);
    return 0;

}

double HRStructure::bundleAdjust()
{

}
int HRStructure::addFrame(int framenum)
{

    vector<int> neighbours;
    findBestTwoNehgbourFrames( framenum,neighbours);

    int numCommonPts=0;

    int count=0;

    int i,j;


    for(i=0; i< numImages; i++)
    {
        if(sfmSequence[i]!=-1) //this array should indicate how many frazmes have had their proj matrices found
            count++;
    }


    int maxlength=(*imSet).myTracks.getNumTracks();


    int numValidProjs= count;
    vector<int> goodMatchIndex;
    int numrejects=0;
    for ( i = 0; i < maxlength; i++)
    {
        if(structureValid[i]!=0 && (*imSet).myTracks.validTrackEntry(i,framenum)!=0)
        {

            if(matchThreeWayValid(i,framenum,neighbours))
            {
                goodMatchIndex.push_back(i);
            }
            else
            {
                printf("rejected point %d\n",i);
                numrejects++;
                //   (*imSet).showTrackNumberwithEpipolars(i);
            }

        }
    }

    int numGoodMatches=goodMatchIndex.size();

    printf("rejected %d points and using %d points\n ",numrejects,numGoodMatches);
    CvMat* imgPts=cvCreateMat(numGoodMatches,2,CV_64F);
    CvMat* objectPts=cvCreateMat(numGoodMatches,3,CV_64F);
    CvMat* rvec=cvCreateMat(3,1,CV_64F);



    for ( j = 0; j < numGoodMatches; j++)
    {
        i=goodMatchIndex[j];

        CvPoint2D32f  curPt=(*imSet).myTracks.pointFromTrackloc(i, framenum);

        cvmSet(imgPts,j,0,curPt.x );
        cvmSet(imgPts,j,1,curPt.y );

        cvmSet(objectPts,j,0,structure[i].x);
        cvmSet(objectPts,j,1,structure[i].y);
        cvmSet(objectPts,j,2,structure[i].z);




    }




    cvFindExtrinsicCameraParams2(objectPts, imgPts,(*((*imSet).imageCollection[framenum])).intrinsicMatrix, (*((*imSet).imageCollection[framenum])).distortion, rvec, (*((*imSet).imageCollection[framenum])).camPose.tm);
    cvRodrigues2(rvec, (*((*imSet).imageCollection[framenum])).camPose.Rm);

    findProjfromcompon((*((*imSet).imageCollection[framenum])));

    printf("inside pose estimation\n");
    writeCVMatrix(cout<<"rvec"<<endl,rvec );
    writeCVMatrix(cout<<"tvec"<<endl,(*((*imSet).imageCollection[framenum])).camPose.tm );
    writeCVMatrix(cout<<"rotation"<<endl,(*((*imSet).imageCollection[framenum])).camPose.Rm );
    writeCVMatrix(cout<<"distortion"<<endl,(*((*imSet).imageCollection[framenum])).distortion );
    writeCVMatrix(cout<<"intrinsic"<<endl,(*((*imSet).imageCollection[framenum])).intrinsicMatrix);
    writeCVMatrix(cout<<"projection"<<endl,(*((*imSet).imageCollection[framenum])).projectionMatrix);
    cvReleaseMat(&imgPts);
    cvReleaseMat(&objectPts);
    cvReleaseMat(&rvec);

    for(i=0; i< numImages; i++)
    {
        if(sfmSequence[i]==-1)
        {
            sfmSequence[i]=framenum;
            break;
        }
    }



}
//this finds the best closes neighbour frames whose pose has been estimated
int HRStructure::findBestTwoNehgbourFrames(int frame,vector<int>& neighbourFrames)
{
    int i;
    vector<int> numMatches;
    neighbourFrames.clear();

    for(i=0; i< sfmSequence.size(); i++)
    {

        if(sfmSequence[i]!=-1)
            numMatches.push_back((*imSet).correspondencesPairWise[frame][sfmSequence[i]].imIndices.size());
        else
            numMatches.push_back(0);

    }



    do
    {
        int framen=indexMax(numMatches);
        if(sfmSequence[framen]==-1)
        {
            printf("this is impossible \n");
        }
        else
        {
            neighbourFrames.push_back(sfmSequence[framen]);
        }
        numMatches[framen]=0;


    }
    while(neighbourFrames.size()<2);


    printf("best neighbours to frame %d are frames %d and %d with %d matches and %d matches respectively \n",frame,neighbourFrames[0],neighbourFrames[1],(*imSet).correspondencesPairWise[frame][neighbourFrames[0]].imIndices.size(),(*imSet).correspondencesPairWise[frame][neighbourFrames[1]].imIndices.size() );
    return 0;
}



bool HRStructure::matchThreeWayValid(int feature,int frame,vector<int>& neighbourFrames)
{



    if(structureValid[feature]==0 || (*imSet).myTracks.validTrackEntry(feature,frame)==0)
    {
        return false;
    }
    if(neighbourFrames.size()!=2)
    {
        printf("neighbours didnt exist, thisis wrong\n");
        return false;
    }

//find two nearest frames that are done

    CvMat* lines1= cvCreateMat(1,1,CV_32FC3);
    CvMat* lines2= cvCreateMat(1,1,CV_32FC3);



    CvPoint2D32f  curPt=(*imSet).myTracks.pointFromTrackloc(feature, frame);
    CvPoint2D32f  curPtN1=(*imSet).myTracks.pointFromTrackloc(feature, neighbourFrames[0]);
    CvPoint2D32f  curPtN2=(*imSet).myTracks.pointFromTrackloc(feature, neighbourFrames[1]);



    if((*imSet).EpilineFromTrackloc(feature,neighbourFrames[0] ,frame,lines1)==-1)
    {
        return false;
    }


    if((*imSet).EpilineFromTrackloc(feature,neighbourFrames[1] ,frame,lines2)==-1)
    {
        return false;
    }



    CvPoint2D32f ptProj=findIntersection(lines1,lines2);

    cvReleaseMat(&lines1);
    cvReleaseMat(&lines2);


    double dist=sqrt((ptProj.x-curPt.x)*(ptProj.x-curPt.x))+((ptProj.y-curPt.y)*(ptProj.y-curPt.y));



    double threshold=15;

    if(dist>threshold)
    {
        printf("rejectig existing point is (%f, %f) and predicted is (%f,%f) distance was %f \n",curPt.x,curPt.y,ptProj.x,ptProj.y,dist );
        return false;
    }
    else
    {
        return true;
    }


}

void HRStructure::DLTUpdateStructure()
{

    int i,j;



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

    for ( i = 0; i < structureValid.size(); i++)
    {
        structureValid[i]=0;
    }
    for ( i = 0; i < maxlength; i++)
    {
        vector<string> imagesNames;
        vector<CvMat*> projMatrs;
        vector<CvPoint2D32f> projPoints;
        vector<int> pointsUsed;
        int numPts=0;
        for (j = 0; j < numValidProjs; j++)
        {
            int curFrame=sfmSequence[j];
            if((*imSet).myTracks.validTrackEntry(i,curFrame)!=0)
            {

                numPts++;
                pointsUsed.push_back(curFrame);
                CvPoint2D32f  curPt=(*imSet).myTracks.pointFromTrackloc(i, curFrame);
                projMatrs.push_back((*((*imSet).imageCollection[curFrame])).projectionMatrix);
                // printf("requesting feature %d from frame %d and track was %d\n",i,curFrame, (*imSet).myTracks.valueTrackEntry(i,curFrame));
                // printf("track point %d frame: %d point was x=%f and y=%f\n",i,curFrame,curPt.x,curPt.y);
                projPoints.push_back(curPt);
                imagesNames.push_back((*imSet).imageCollection[curFrame]->filename);
            }
        }
        if(numPts>1)
        {
            numReconstructed++;
            structureValid[i]=numPts;



//            for (int q=0; q<projPoints.size(); q++)
//            {
//                if(q==0) printf("reconstructing track %d with : ",i);
//                printf(" +frame (%d) [%f %f]+ \t",pointsUsed[q],projPoints[q].x,projPoints[q].y);
//                if(q==(numPts-1)) printf("\n");
//            }

//            if(count>2)
//            {
//                printf("showing feature %d\n",i);
//                 showMatchAcross(imagesNames,  projPoints);
//            }


            cvTriangulatePointsNframs(numPts, projMatrs,projPoints,structure[i] );

        }
        projMatrs.clear();
        projPoints.clear();
        pointsUsed.clear();
        imagesNames.clear();
    }
    double err_beforesba=findReconstructionError();
    printf("reconstructed %d points\n",numReconstructed);
    printf("error before sba=%f \t\n",err_beforesba);

    // sba_driver_interface();
    double err_aftersba=findReconstructionError();
    printf("error after sba=%f \t\n",err_aftersba);






}

double HRStructure::findReconstructionError()
{

    int i,j;
    int numbads=0;
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




    double x,y;

    double deltaX,deltaY;
    double rep_error=0;
    int numFrames=0;
    for ( i = 0; i < maxlength; i++)
    {
        if(structureValid[i]!=0)
        {


            rep_error=0;
            numFrames=0;
            numReconstructed++;
            for (j = 0; j < numValidProjs; j++)
            {
                int curFrame=sfmSequence[j];
                if((*imSet).myTracks.validTrackEntry(i,curFrame)!=0)
                {
                    numFrames++;
                    CvPoint2D32f  curPt=(*imSet).myTracks.pointFromTrackloc(i, curFrame,1);//using undistoreted points yyy
                    CvMat* P=(*((*imSet).imageCollection[curFrame])).projectionMatrix;


                    rep_error+= projectionErrorSquared( P,structure[i],curPt);


                    //    printf("requesting feature %d from frame %d and track was %d\n",i,curFrame, (*imSet).myTracks.valueTrackEntry(i,curFrame));
                    //  printf("point was x=%f and y=%f\n",curPt.x,curPt.y);
                    //    printf("point number %d was x=%f and y=%f z=%f, numpts=%d\n",i,structure[i].x,structure[i].y,structure[i].z,structureValid[i] );
                    //   printf("error was %f\n",rep_error);
                    //   printf("________________________\n");

                }
            }
            rep_error/=((double)numFrames);
            if(rep_error>1)
            {
                //      printf("point %d had error %f \n",i,rep_error);
                numbads++;
            }

            structureErrors[i]=rep_error;
            rerror+=rep_error;
        }
    }

    rerror/=((double)numReconstructed);
    printf("number of bad points is %d  or %f %%\n",numbads,(((double)numbads)/((double)numReconstructed))*100.0    );


    return rerror;
}
int HRStructure::printSBAstyleData(string camFname, string ptFname)
{


    int i,j;
    CvMat* curq=cvCreateMat(4,1,CV_64F);


    int maxlength=(*imSet).myTracks.getNumTracks();



    fstream file_cams(camFname.c_str() ,ios::out);
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
    fstream file_pts(ptFname.c_str() ,ios::out);
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
                        file_pts<<j<<" "<<curPt.x<<" "<<curPt.y<<" ";

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

int HRStructure::decomposeEssential(CvMat* E, CvPoint2D32f p1,CvPoint2D32f p2,CvMat* K1, CvMat* K2, CvMat* R,CvMat* t)
{
    int i,j;
    int exitflag=0;
    double depth1,depth2;
    CvMat* W=cvCreateMat(3,3,CV_64F);
    CvMat* WT=cvCreateMat(3,3,CV_64F);
    CvMat* U=cvCreateMat(3,3,CV_64F);
    CvMat* VT=cvCreateMat(3,3,CV_64F);
    CvMat* WW=cvCreateMat(3,3,CV_64F);

    CvMat* temp1=cvCreateMat(3,3,CV_64F);
    CvMat* temp2=cvCreateMat(3,3,CV_64F);

    CvMat* PT=cvCreateMat(4,1,CV_64F);
    CvMat* PT_R=cvCreateMat(4,1,CV_64F);

    CvMat* R1=cvCreateMat(3,3,CV_64F);
    CvMat* R2=cvCreateMat(3,3,CV_64F);
    CvMat* t1=cvCreateMat(3,1,CV_64F);
    CvMat* t2=cvCreateMat(3,1,CV_64F);

    CvMat* P1=cvCreateMat(3,4,CV_64F);
    CvMat* P2=cvCreateMat(3,4,CV_64F);
    CvMat* P3=cvCreateMat(3,4,CV_64F);
    CvMat* P4=cvCreateMat(3,4,CV_64F);

    CvMat* PTemp=cvCreateMat(3,4,CV_64F);


    CvMat* POrigin=cvCreateMat(3,4,CV_64F);

    CvMat* Rident=cvCreateMat(3,3,CV_64F);
    CvMat* tzero=cvCreateMat(3,1,CV_64F);



//zzz in the matlab file this is done differently , the image location is multiplied by inverse of K





    cvSetZero(W);
    cvmSet(W,0,1,-1.0);
    cvmSet(W,1,0,1.0);
    cvmSet(W,2,2,1.0);

    cvTranspose(W,WT);

    //create p matrix at origin

    cvSetIdentity(Rident);
    cvSetZero(tzero);
///////////////////////////// svd of E



    copyMatrix(E,temp1);
    writeCVMatrix(cout<<"essential matrix was:\n"<<endl,temp1);
    cvSVD( temp1, WW,  U, VT,CV_SVD_V_T );


/////////////////////////////// get R1, R2 , T1 , T2

    cvMatMul(U,W,temp1);
    cvMatMul(temp1,VT,R1);


    cvMatMul(U,WT,temp1);
    cvMatMul(temp1,VT,R2);


//flipping rotations if determinant is negative
    if(cvDet(R1)<0.0)
    {
        scaleMatrix(R1,-1.0);
    }
    if(cvDet(R2)<0.0)
    {
        scaleMatrix(R2,-1.0);
    }




    for(i=0; i<3; i++)
    {
        cvmSet(t1,i,0,cvmGet(U,i,2));
        cvmSet(t2,i,0,cvmGet(U,i,2));
    }

    scaleMatrix(t2,-1.0);

// finished buildong R, t
    for(i=0; i<3; i++)
    {
        for(j=0; j<3; j++)
        {
            cvmSet(P1,i,j, cvmGet(R1,i,j));
            cvmSet(P2,i,j, cvmGet(R1,i,j));
            cvmSet(P3,i,j, cvmGet(R2,i,j));
            cvmSet(P4,i,j, cvmGet(R2,i,j));
            cvmSet(POrigin,i,j, cvmGet(Rident,i,j));
        }

    }

    for(i=0; i<3; i++)
    {
        cvmSet(P1,i,3, cvmGet(t1,i,0));
        cvmSet(P2,i,3, cvmGet(t2,i,0));
        cvmSet(P3,i,3, cvmGet(t1,i,0));
        cvmSet(P4,i,3, cvmGet(t2,i,0));
        cvmSet(POrigin,i,3, cvmGet(tzero,i,0));
    }



    cvMatMul(K1,POrigin,PTemp);
    copyMatrix(PTemp,POrigin);

    cvMatMul(K2,P1,PTemp);
    copyMatrix(PTemp,P1);

    cvMatMul(K2,P2,PTemp);
    copyMatrix(PTemp,P2);


    cvMatMul(K2,P3,PTemp);
    copyMatrix(PTemp,P3);


    cvMatMul(K2,P4,PTemp);
    copyMatrix(PTemp,P4);

///////

    writeCVMatrix(cout<<"porigin:"<<endl,POrigin);
    writeCVMatrix(cout<<"P1:"<<endl,P1);
    writeCVMatrix(cout<<"P2:"<<endl,P2);
    writeCVMatrix(cout<<"P3:"<<endl,P3);
    writeCVMatrix(cout<<"P4:"<<endl,P4);



// go through the cases

    vector<CvMat*> projMatrs;
    vector<CvPoint2D32f> projPoints;
    projPoints.push_back(p1);
    projPoints.push_back(p2);


//case 1
    if(exitflag==0)
    {
        projMatrs.push_back(POrigin);
        projMatrs.push_back(P1);

        CvPoint3D32f S1;
        cvTriangulatePointsNframs(2, projMatrs,projPoints,S1 );

        depth1=findDepth(POrigin,S1);
        depth2=findDepth(P1,S1);

        printf("for P1 depth1 is %f and depth2 is %f\n",depth1,depth2);

        if(depth1>0 && depth2>0)
        {
            exitflag=1;

            copyMatrix(R1,R);
            copyMatrix(t1,t);
        }


        projMatrs.clear();
    }

//case 2
    if(exitflag==0)
    {
        projMatrs.push_back(POrigin);
        projMatrs.push_back(P2);

        CvPoint3D32f S2;
        cvTriangulatePointsNframs(2, projMatrs,projPoints,S2 );

        depth1=findDepth(POrigin,S2);
        depth2=findDepth(P2,S2);
        printf("for P2 depth1 is %f and depth2 is %f\n",depth1,depth2);

        if(depth1>0 && depth2>0)
        {
            exitflag=1;

            copyMatrix(R1,R);
            copyMatrix(t2,t);
        }

        projMatrs.clear();

    }

//case 3
    if(exitflag==0)
    {
        projMatrs.push_back(POrigin);
        projMatrs.push_back(P3);

        CvPoint3D32f S3;
        cvTriangulatePointsNframs(2, projMatrs,projPoints,S3 );

        depth1=findDepth(POrigin,S3);
        depth2=findDepth(P3,S3);
        printf("for P3 depth1 is %f and depth2 is %f\n",depth1,depth2);

        if(depth1>0 && depth2>0)
        {
            exitflag=1;

            copyMatrix(R2,R);
            copyMatrix(t1,t);
        }



        projMatrs.clear();
    }
//case 4
    if(exitflag==0)
    {
        projMatrs.push_back(POrigin);
        projMatrs.push_back(P4);

        CvPoint3D32f S4;
        cvTriangulatePointsNframs(2, projMatrs,projPoints,S4 );

        depth1=findDepth(POrigin,S4);
        depth2=findDepth(P4,S4);
        printf("for P4 depth1 is %f and depth2 is %f\n",depth1,depth2);

        if(depth1>0 && depth2>0)
        {
            exitflag=1;

            copyMatrix(R2,R);
            copyMatrix(t2,t);
        }


        projMatrs.clear();

    }

    cvReleaseMat(&Rident);
    cvReleaseMat(&tzero);

    cvReleaseMat(&R1);
    cvReleaseMat(&R2);
    cvReleaseMat(&t1);
    cvReleaseMat(&t2);
    cvReleaseMat(&P1);
    cvReleaseMat(&P2);
    cvReleaseMat(&P3);
    cvReleaseMat(&P4);

    cvReleaseMat(&POrigin);
    cvReleaseMat(&W);
    cvReleaseMat(&WT);
    cvReleaseMat(&WW);
    cvReleaseMat(&U);
    cvReleaseMat(&VT);
    cvReleaseMat(&temp1);
    cvReleaseMat(&temp2);

    cvReleaseMat(&PTemp);

    cvReleaseMat(&PT);

    cvReleaseMat(&PT_R);

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
////////////////////////////////////////////////////////////////////

/* interface for the Driver for sba_xxx_levmar */
int HRStructure::sba_driver_interface()
{

    int cnp=16, /* 3 rot params + 3 trans params */
            pnp=3, /* euclidean 3D points */
                mnp=2; /* image points are 2D */
    int filecnp=cnp+1;
    int i,j;
    double *motstruct, *motstruct_copy, *imgpts, *imgpts_copy, *covimgpts, *initrot, *initrot_copy;
    double K[9], ical[5]; // intrinsic calibration matrix & temp. storage for its params
    char *vmask, tbuf[32];
    double opts[SBA_OPTSSZ], info[SBA_INFOSZ], phi;
    int howto, expert, analyticjac, fixedcal, havedist, n, prnt, verbose=0;
    int nframes, numpts3D, numprojs, nvars;
    const int nconstframes=0;

    FILE *fp;

    static char *howtoname[]= {"BA_MOTSTRUCT", "BA_MOT", "BA_STRUCT", "BA_MOT_MOTSTRUCT"};

    clock_t start_time, end_time;

    /* Notice the various BA options demonstrated below */

    /* minimize motion & structure, motion only, or
     * motion and possibly motion & structure in a 2nd pass?
     */
    howto=BA_MOTSTRUCT;
    //howto=BA_MOT;
    //howto=BA_STRUCT;
    //howto=BA_MOT_MOTSTRUCT;

    /* simple or expert drivers? */
    //expert=0;
    expert=1;

    /* analytic or approximate jacobian? */
    //analyticjac=0;
    analyticjac=1;

    /* prinstructureValid[i]t motion & structure estimates,
     * motion only or structure only upon completion?
     */
    prnt=BA_NONE;
    //  prnt=BA_MOTSTRUCT;
    //prnt=BA_MOT;
    //prnt=BA_STRUCT;

    int havecov=0;

///// MY PART BEGINS

    int count=0;
    for(i=0; i< numImages; i++)
    {
        if(sfmSequence[i]!=-1) //this array should indicate how many frazmes have had their proj matrices found
            count++;
    }

    nframes=count;

    string numberFrames=stringify(nframes);

    numprojs=0;
    int maxlength=(*imSet).myTracks.getNumTracks();
    count=0;
    for ( i = 0; i < maxlength; i++)
    {
        if(structureValid[i]!=0)
            count++;

        numprojs=numprojs+structureValid[i];
    }

    numpts3D=count;



    motstruct=(double *)malloc((nframes*cnp + numpts3D*pnp)*sizeof(double));
    motstruct_copy=motstruct;
    if(motstruct==NULL)
    {
        fprintf(stderr, "memory allocation for 'motstruct' failed in readInitialSBAEstimate()\n");
        exit(1);
    }
    initrot=(double *)malloc((nframes*FULLQUATSZ)*sizeof(double)); // Note: this assumes quaternions for rotations!
    initrot_copy= initrot;
    if(initrot==NULL)
    {
        fprintf(stderr, "memory allocation for 'initrot' failed in readInitialSBAEstimate()\n");
        exit(1);
    }
    imgpts=(double *)malloc(numprojs*mnp*sizeof(double));
    imgpts_copy=imgpts;
    if(imgpts==NULL)
    {
        fprintf(stderr, "memory allocation for 'imgpts' failed in readInitialSBAEstimate()\n");
        exit(1);
    }
    if(havecov)
    {
        covimgpts=(double *)malloc(numprojs*mnp*mnp*sizeof(double));
        if(covimgpts==NULL)
        {
            fprintf(stderr, "memory allocation for 'covimgpts' failed in readInitialSBAEstimate()\n");
            exit(1);
        }
    }
    else
        covimgpts=NULL;
    vmask=(char *)malloc(numpts3D *nframes * sizeof(char));
    if(vmask==NULL)
    {
        fprintf(stderr, "memory allocation for 'vmask' failed in readInitialSBAEstimate()\n");
        exit(1);
    }
    memset(vmask, 0, numpts3D * nframes * sizeof(char)); /* clear vmask */


//camera params
    CvMat* curq=cvCreateMat(4,1,CV_64F);
    double *tofilter;


    if((tofilter=(double *)malloc(filecnp*sizeof(double)))==NULL)
    {
        fprintf(stderr, "memory allocation failed in readCameraParams()\n");
        exit(1);
    }

    printf("num images is %d and size of sfm is %d \n",numImages,sfmSequence.size());



    for (j = 0; j < numImages; j++)
    {
        if(sfmSequence[j]!=-1)
        {
            int curFrame=sfmSequence[j];
            printf("2 sba current frame is %d our j was %d\n",curFrame,j );
            CvMat* curK=(*((*imSet).imageCollection[curFrame])).intrinsicMatrix;
            CvMat* curR=(*((*imSet).imageCollection[curFrame])).camPose.Rm;
            CvMat* curt=(*((*imSet).imageCollection[curFrame])).camPose.tm;
            CvMat* disto=(*((*imSet).imageCollection[curFrame])).distortion;

            matrix_to_quaternion(curR,curq);

            tofilter[0]=cvmGet(curK,0,0);
            tofilter[1]=cvmGet(curK,0,2);
            tofilter[2]=cvmGet(curK,1,2);
            tofilter[3]= cvmGet(curK,1,1)/cvmGet(curK,0,0);
            tofilter[4]=cvmGet(curK,0,1);

            tofilter[5]=cvmGet(disto,0,0);
            tofilter[6]=cvmGet(disto,1,0);
            tofilter[7]=cvmGet(disto,2,0);
            tofilter[8]=cvmGet(disto,3,0);
            tofilter[9]=cvmGet(disto,4,0);

            tofilter[10]=cvmGet(curq,0,0);
            tofilter[11]=cvmGet(curq,1,0);
            tofilter[12]=cvmGet(curq,2,0);
            tofilter[13]=cvmGet(curq,3,0);

            tofilter[14]=cvmGet(curt,0,0);
            tofilter[15]=cvmGet(curt,1,0);
            tofilter[16]=cvmGet(curt,2,0);

//
//            printf("params for cams is \n");
//            for(int u=0; u<=16; u++)
//            {
//                printf("  %f  ",tofilter[u]);
//            }
//            printf("\n");

            quat2vec(tofilter, filecnp, motstruct, cnp);



            /* save rotation assuming the last 3 parameters correspond to translation */
            initrot[1]=motstruct[cnp-6];
            initrot[2]=motstruct[cnp-5];
            initrot[3]=motstruct[cnp-4];
            initrot[0]=sqrt(1.0 - initrot[1]*initrot[1] - initrot[2]*initrot[2] - initrot[3]*initrot[3]);

            motstruct+=cnp;
            initrot+=FULLQUATSZ;

        }


    }



    free(tofilter);

//end of reading cameras
//read image points
    int ptno=0;
    motstruct=motstruct_copy+(nframes*cnp);

    for ( i = 0; i < maxlength; i++)
    {
        if(structureValid[i]!=0)
        {


            motstruct[0]=structure[i].x;
            motstruct[1]=structure[i].y;
            motstruct[2]=structure[i].z;
            motstruct+=pnp;
            for (j = 0; j < numImages; j++)
            {

                int curFrame=sfmSequence[j];
                if((*imSet).myTracks.validTrackEntry(i,curFrame)!=0)
                {
                    CvPoint2D32f  curPt=(*imSet).myTracks.pointFromTrackloc(i, curFrame);


                    imgpts[0]=curPt.x  ;
                    imgpts[1]=curPt.y ;

                    imgpts+=mnp;
                    vmask[ptno*nframes+j]=1;
                }


            }
            ptno++;
        }

    }

    imgpts=imgpts_copy;
    motstruct=motstruct_copy;  //rewind pointer
    initrot=initrot_copy;


///// MY PART ENDS




//printSBAData(stdout, motstruct, cnp, pnp, mnp, camoutfilter, filecnp, nframes, numpts3D, imgpts, numprojs, vmask);

    if(howto!=BA_STRUCT)
    {
        /* initialize the local rotation estimates to 0, corresponding to local quats (1, 0, 0, 0) */
        for(i=0; i<nframes; ++i)
        {
            register int j;

            j=(i+1)*cnp; // note the +1, below we move from right to left, assuming 3 parameters for the translation!
            motstruct[j-4]=motstruct[j-5]=motstruct[j-6]=0.0; // clear rotation
        }
    }
    /* note that if howto==BA_STRUCT the rotation parts of motstruct actually equal the initial rotations! */

    /* set up globs structure */
    globs_ mglobs;
    mglobs.cnp=cnp;
    mglobs.pnp=pnp;
    mglobs.mnp=mnp;
    mglobs.rot0params=initrot;

    mglobs.intrcalib=NULL;
    /* specify the number of intrinsic parameters that are to be fixed
     * equal to their initial values, as follows:
     * 0: all free, 1: skew fixed, 2: skew, ar fixed, 4: skew, ar, ppt fixed
     * Note that a value of 3 does not make sense
     */
    mglobs.nccalib=1; /* number of intrinsics to keep fixed, must be between 0 and 5 */
    //zzz this is important, maybe change this to 5
    fixedcal=0; /* varying intrinsics */


    havedist=1; /* with distortion */
    //zzz unfix these distortion parameters, right now i keep 3 parameters fixed, wich 3 are these?
    mglobs.ncdist=5; /* number of distortion params to keep fixed, must be between 0 and 5 */



    mglobs.ptparams=NULL;
    mglobs.camparams=NULL;

    /* call sparse LM routine */
    opts[0]=SBA_INIT_MU;
    opts[1]=SBA_STOP_THRESH;
    opts[2]=SBA_STOP_THRESH;
    opts[3]=SBA_STOP_THRESH;
//opts[3]=0.05*numprojs; // uncomment to force termination if the average reprojection error drops below 0.05
    opts[4]=0.0;
//opts[4]=1E-05; // uncomment to force termination if the relative reduction in the RMS reprojection error drops below 1E-05

//    FILE *f1p;
//    f1p=fopen("imgpts.txt", "w");
//    for (i=0; i< numprojs*mnp; i++)
//    {
//        fprintf(f1p,"%f\n",imgpts[i]);
//    }
//    fclose(f1p);
//
//    f1p=fopen("motstruct.txt", "w");
//    for (i=0; i< (nframes*cnp + numpts3D*pnp); i++)
//    {
//        fprintf(f1p,"%f\n",motstruct[i]);
//    }
//    fclose(f1p);
//
//    f1p=fopen("initrot.txt", "w");
//    for (i=0; i< nframes*FULLQUATSZ; i++)
//    {
//        fprintf(f1p,"%f\n", initrot[i]);
//    }
//    fclose(f1p);
//
//
//    f1p=fopen("vmask.txt", "w");
//    for (i=0; i< numpts3D; i++)
//    {
//        for (j=0; j< nframes; j++)
//        {
//            fprintf(f1p,"\t%d", (int)vmask[i*nframes+j]);
//        }
//        fprintf(f1p,"\n");
//    }
//    fclose(f1p);




    start_time=clock();

    nvars=nframes*cnp+numpts3D*pnp;
    imgpts=imgpts_copy;
    motstruct=motstruct_copy;  //rewind pointer
    initrot=initrot_copy;


    {


        string tfname="structurbefore.ply";
        string mfname=string("frames_")+numberFrames+tfname;
        fs::path tempath( (*imSet).temporaryDir, fs::native );
        tempath/=mfname;
        string newfname=tempath.file_string();

        saveSBAStructureDataAsPLY((char*)newfname.c_str(), motstruct, nframes, numpts3D,cnp, pnp, 0);
    }

    imgpts=imgpts_copy;
    motstruct=motstruct_copy;  //rewind pointer
    initrot=initrot_copy;
    n=sba_motstr_levmar_x(numpts3D, 0, nframes, nconstframes, vmask, motstruct, cnp, pnp, imgpts, covimgpts, mnp,
                          img_projsKDRTS_x ,img_projsKDRTS_jac_x ,
                          (void *)(&mglobs), MAXITER2, verbose, opts, info);



    imgpts=imgpts_copy;
    motstruct=motstruct_copy;  //rewind pointer
    initrot=initrot_copy;


    end_time=clock();
    int k=0;

    char* refcamsfname="mycams.txt";
    char* refptsfname="mypts.txt";
    if(n==SBA_ERROR)
    {
        printf("SBA error happened\n");
        goto cleanup;
    }


    if(howto!=BA_STRUCT)
    {
        /* combine the local rotation estimates with the initial ones */
        for(i=0; i<nframes; ++i)
        {
            double *v, qs[FULLQUATSZ], *q0, prd[FULLQUATSZ];

            /* retrieve the vector part */
            v=motstruct + (i+1)*cnp - 6; // note the +1, we access the motion parameters from the right, assuming 3 for translation!
            _MK_QUAT_FRM_VEC(qs, v);

            q0=initrot+i*FULLQUATSZ;
            quatMultFast(qs, q0, prd); // prd=qs*q0

            /* copy back vector part making sure that the scalar part is non-negative */
            if(prd[0]>=0.0)
            {
                v[0]=prd[1];
                v[1]=prd[2];
                v[2]=prd[3];
            }
            else  // negate since two quaternions q and -q represent the same rotation
            {
                v[0]=-prd[1];
                v[1]=-prd[2];
                v[2]=-prd[3];
            }
        }
    }

    fflush(stdout);
    fprintf(stdout, "SBA using %d 3D pts, %d frames and %d image projections, %d variables\n", numpts3D, nframes, numprojs, nvars);
    if(havedist) sprintf(tbuf, " (%d fixed)", mglobs.ncdist);
    fprintf(stdout, "\nMethod %s, %s driver, %s Jacobian, %s covariances, %s distortion%s, %s intrinsics", howtoname[howto],
            expert? "expert" : "simple",
            analyticjac? "analytic" : "approximate",
            covimgpts? "with" : "without",
            havedist? "variable" : "without",
            havedist? tbuf : "",
            fixedcal? "fixed" : "variable");
    if(!fixedcal) fprintf(stdout, " (%d fixed)", mglobs.nccalib);
    fputs("\n\n", stdout);
    fprintf(stdout, "SBA returned %d in %g iter, reason %g, error %g [initial %g], %d/%d func/fjac evals, %d lin. systems\n", n,
            info[5], info[6], info[1]/numprojs, info[0]/numprojs, (int)info[7], (int)info[8], (int)info[9]);
    fprintf(stdout, "Elapsed time: %.2lf seconds, %.2lf msecs\n", ((double) (end_time - start_time)) / CLOCKS_PER_SEC,
            ((double) (end_time - start_time)) / CLOCKS_PER_MSEC);
    fflush(stdout);

    /* refined motion and structure are now in motstruct */

////put the parameters back
    imgpts=imgpts_copy;
    motstruct=motstruct_copy;  //rewind pointer
    initrot=initrot_copy;


//first cameras
    double *filtered;

    if((filtered=(double *)malloc(filecnp*sizeof(double)))==NULL)
    {
        fprintf(stderr, "memory allocation failed in printSBAMotionData()\n");
        exit(1);
    }




    for (j = 0; j < numImages; j++)
    {
        if(sfmSequence[j]!=-1)
        {
            vec2quat(motstruct, cnp, filtered,  filecnp);

            int curFrame=sfmSequence[j];
            CvMat* curK=(*((*imSet).imageCollection[curFrame])).intrinsicMatrix;
            CvMat* curR=(*((*imSet).imageCollection[curFrame])).camPose.Rm;
            CvMat* curt=(*((*imSet).imageCollection[curFrame])).camPose.tm;
            CvMat* disto=(*((*imSet).imageCollection[curFrame])).distortion;
            if(mglobs.ncdist<5)
            {
                (*((*imSet).imageCollection[curFrame])).havedistortion=1;
            }


            cvmSet(curK,0,0,filtered[0]);
            cvmSet(curK,0,2,filtered[1]);
            cvmSet(curK,1,2,filtered[2]);
            cvmSet(curK,1,1,filtered[3]*cvmGet(curK,0,0));
            cvmSet(curK,0,1,filtered[4]);

            cvmSet(disto,0,0,filtered[5]);
            cvmSet(disto,1,0,filtered[6]);
            cvmSet(disto,2,0,filtered[7]);
            cvmSet(disto,3,0,filtered[8]);
            cvmSet(disto,4,0,filtered[9]);

            cvmSet(curq,0,0,filtered[10]);
            cvmSet(curq,1,0,filtered[11]);
            cvmSet(curq,2,0,filtered[12]);
            cvmSet(curq,3,0, filtered[13]);

            cvmSet(curt,0,0,filtered[14]);
            cvmSet(curt,1,0,filtered[15]);
            cvmSet(curt,2,0,filtered[16]);

            quaternion_to_matrix(curq,curR);

//putting the P matrix back together

            findProjfromcompon((*((*imSet).imageCollection[curFrame])));
            (*((*imSet).imageCollection[curFrame])).undistortPoints();
            motstruct=motstruct+(cnp);

        }


    }


    free(filtered);

    cvReleaseMat(&curq);

    //end of cameras
    imgpts=imgpts_copy;
    motstruct=motstruct_copy;  //rewind pointer
    initrot=initrot_copy;


//beginning of taking back the 3D points

    motstruct+=(nframes*cnp);

    for ( i = 0; i < maxlength; i++)
    {
        if(structureValid[i]!=0)
        {


            structure[i].x=motstruct[(k*pnp)+0];
            structure[i].y=motstruct[(k*pnp)+1];
            structure[i].z=motstruct[(k*pnp)+2];
            k++;

        }

    }









    //end of parameters putting back
    imgpts=imgpts_copy;
    motstruct=motstruct_copy;  //rewind pointer
    initrot=initrot_copy;


    // printSBAMotionData(stdout, motstruct, nframes, cnp, vec2quat, filecnp);
    //printSBAStructureData(stdout, motstruct, nframes, numpts3D, cnp, pnp);

    imgpts=imgpts_copy;
    motstruct=motstruct_copy;  //rewind pointer
    initrot=initrot_copy;

    {


        string tfname="structurafter.ply";
        string mfname=string("frames_")+numberFrames+tfname;
        fs::path tempath( (*imSet).temporaryDir, fs::native );
        tempath/=mfname;
        string newfname=tempath.file_string();

        saveSBAStructureDataAsPLY((char*)newfname.c_str(), motstruct, nframes, numpts3D,cnp, pnp, 0);
    }
cleanup:
    /* just in case... */
    mglobs.intrcalib=NULL;
    mglobs.nccalib=0;
    mglobs.ncdist=0;

    free(motstruct);
    free(imgpts);
    free(initrot);
    mglobs.rot0params=NULL;
    if(covimgpts) free(covimgpts);
    free(vmask);
}


