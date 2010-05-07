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

        }
        projMatrs.clear();
        projPoints.clear();
    }


}

double HRStructure::findReconstructionError(int usingUndistort)
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


    CvMat point3D;
    double point3D_dat[4];
    point3D = cvMat(4,1,CV_64F,point3D_dat);

    CvMat point2D;
    double point2D_dat[3];
    point2D = cvMat(3,1,CV_64F,point2D_dat);

    double x,y;
    double xr,yr,wr;
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
                    CvPoint2D32f  curPt=(*imSet).myTracks.pointFromTrackloc(i, curFrame,usingUndistort);//using undistoreted points yyy
                    CvMat* P=(*((*imSet).imageCollection[curFrame])).projectionMatrix;

                    point3D_dat[0] = structure[i].x;
                    point3D_dat[1] = structure[i].y;
                    point3D_dat[2] = structure[i].z;
                    point3D_dat[3] = 1;

                    cvmMul(P, &point3D, &point2D);


                    x = curPt.x;
                    y = curPt.y;

                    wr = (double)point2D_dat[2];
                    xr = (double)(point2D_dat[0]/wr);
                    yr = (double)(point2D_dat[1]/wr);


                    deltaX = (double)(x-xr);
                    deltaY = (double)(y-yr);
                    rep_error+=((deltaX*deltaX)+(deltaY*deltaY));


                    // printf("requesting feature %d from frame %d and track was %d\n",i,curFrame, (*imSet).myTracks.valueTrackEntry(i,curFrame));
                    //printf("point was x=%f and y=%f\n",curPt.x,curPt.y);



                }
            }
            rep_error/=((double)numFrames);
            structureErrors[i]=rep_error;
            rerror+=rep_error;
        }
    }

    rerror/=((double)numReconstructed);

    printf("reconstruction error was %f\n",rerror);

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



                if(sfmSequence[j]!=-1)
                {

                    int curFrame=sfmSequence[j];
                    if((*imSet).myTracks.validTrackEntry(i,curFrame)!=0)
                    {
                        CvPoint2D32f  curPt=(*imSet).myTracks.pointFromTrackloc(i, curFrame);


                        imgpts[0]=curPt.x  ;
                        imgpts[1]= curPt.y ;

                        imgpts+=mnp;
                    }
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
    mglobs.nccalib=0; /* number of intrinsics to keep fixed, must be between 0 and 5 */
    fixedcal=0; /* varying intrinsics */


        havedist=1; /* with distortion */
        mglobs.ncdist=0; /* number of distortion params to keep fixed, must be between 0 and 5 */



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



    start_time=clock();

    nvars=nframes*cnp+numpts3D*pnp;
    imgpts=imgpts_copy;
    motstruct=motstruct_copy;  //rewind pointer
    initrot=initrot_copy;

    saveSBAStructureDataAsPLY("structurbefore.ply", motstruct, nframes, numpts3D,cnp, pnp, 0);
    imgpts=imgpts_copy;
    motstruct=motstruct_copy;  //rewind pointer
    initrot=initrot_copy;
    n=sba_motstr_levmar_x(numpts3D, 0, nframes, nconstframes, vmask, motstruct, cnp, pnp, imgpts, covimgpts, mnp,
                          fixedcal? img_projsRTS_x : (havedist? img_projsKDRTS_x : img_projsKRTS_x),
                          analyticjac? (fixedcal? img_projsRTS_jac_x : (havedist? img_projsKDRTS_jac_x : img_projsKRTS_jac_x)) : NULL,
                              (void *)(&mglobs), MAXITER2, verbose, opts, info);



    imgpts=imgpts_copy;
    motstruct=motstruct_copy;  //rewind pointer
    initrot=initrot_copy;


    end_time=clock();
    int k=0;

    char* refcamsfname="mycams.txt";
    char* refptsfname="mypts.txt";
    if(n==SBA_ERROR) goto cleanup;


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

    saveSBAStructureDataAsPLY("structureafter.ply", motstruct, nframes, numpts3D,cnp, pnp, 0);

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
