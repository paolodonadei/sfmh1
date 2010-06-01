#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include <cv.h>
#include <vector>
#include <iostream>
#include <fstream>
#include <assert.h>
#include "vector.h"
#include <iomanip>
#include <sstream>
#include "visiongen.h"
#include <highgui.h>
#include "general.h"
#include "funddrawutils.h"
using namespace std;


#define CV_Error( code, msg ) cv::error( cv::Exception(code, msg, __func__, __FILE__, __LINE__) )
#define CV_Error_( code, args ) cv::error( cv::Exception(code, cv::format args, __func__, __FILE__, __LINE__) )
#define CV_Assert( expr ) { if(!(expr)) cv::error( cv::Exception(CV_StsAssert, #expr, __func__, __FILE__, __LINE__) ); }


void cvDecomposeProjectionMatrixHR( const CvMat *projMatr, CvMat *calibMatr,
                                    CvMat *rotMatr, CvMat *posVect,
                                    CvMat *rotMatrX, CvMat *rotMatrY,
                                    CvMat *rotMatrZ, CvPoint3D64f *eulerAngles)
{
    CvMat* ttemp = cvCreateMat(4,1, CV_64F);
    cvDecomposeProjectionMatrix(projMatr, calibMatr,rotMatr,ttemp,rotMatrX, rotMatrY,rotMatrZ,eulerAngles);

    // printf("ttemp is \n");
    // writeCVMatrix(cout,ttemp);

    int j;
    for (j=0; j<3; j++)
    {
        cvmSet(posVect,j,0,cvmGet(ttemp,j,0)/cvmGet(ttemp,3,0)); // Set M(i,j)

    }

    // printf("posVect is \n");
    // writeCVMatrix(cout,posVect);


    cvReleaseMat(&ttemp);

}



/////////////////////////////////////////////
int ProjectiveMatFromF( const CvMat *F, CvMat *P1,CvMat *P2)
{
//to see the purpose of v and scale see page 256 in the multiple view book
    CvMat* v = cvCreateMat(3,1, CV_64F);

    cvmSet(v, 0, 0,0.0 );
    cvmSet(v, 1, 0,0.0 );
    cvmSet(v, 2, 0,0.0);

    ProjectiveMatFromF( F, P1,P2,v,1.00 );
    cvReleaseMat(&v);

}
int findEpipoles(const CvMat *F, CvMat *e1,CvMat *e2)
{
    if (checkMatrixOK(F,3,3)== false || checkMatrixOK(e1,3,1)==false || checkMatrixOK(e2,3,1)==false)
    {
        return 0;
    }

    //http://www4.ncsu.edu/~ipsen/REU09/chapter4.pdf
    CvMat* U = cvCreateMat(3,3, CV_64F);
    CvMat* V = cvCreateMat(3,3, CV_64F);
    CvMat* W = cvCreateMat(3,3, CV_64F);
    CvMat* FC;


    FC=cvCloneMat(F);

    cvSVD( FC, W,  U, V );  //change all of the below back to U

//epipole 1, left null vector
    cvmSet(e1, 0, 0,cvmGet(V,0,2) );
    cvmSet(e1, 1, 0,cvmGet(V,1,2) );
    cvmSet(e1, 2, 0,cvmGet(V,2,2));


//epipole 2, right null vector
    cvmSet(e2, 0, 0,cvmGet(U,0,2) );
    cvmSet(e2, 1, 0,cvmGet(U,1,2) );
    cvmSet(e2, 2, 0,cvmGet(U,2,2));


    normalizeMatrix(e1);
    normalizeMatrix(e2);


//
//    printf("Debugging info for epipole estimation: \n Fundamental matrix read was\n");
//    writeCVMatrix(cout,F );
//    printf("Left Epipole (left null space) was\n");
//    writeCVMatrix(cout,e1 );
//    printf("Right Epipole (left null space) was\n");
//    writeCVMatrix(cout,e2 );


    cout<<endl;








    cvReleaseMat(&FC);

    cvReleaseMat(&U);
    cvReleaseMat(&V);
    cvReleaseMat(&W);
}

int ProjectiveMatFromF( const CvMat *F, CvMat *P1,CvMat *P2,CvMat* v,double scale )
{
    CvMat* e1 = cvCreateMat(3,1, CV_64F);
    CvMat* e2 = cvCreateMat(3,1, CV_64F);
    CvMat* e2s = cvCreateMat(3,3, CV_64F);

    CvMat* left1= cvCreateMat(3,3, CV_64F);
    CvMat* left2 = cvCreateMat(3,3, CV_64F);
    CvMat* left3 = cvCreateMat(3,3, CV_64F);
    CvMat* right = cvCreateMat(3,1, CV_64F);
    CvMat* vT = cvCreateMat(1,3, CV_64F);

    findEpipoles(F, e1,e2);
    skewSymmetrify(e2, e2s);

    cvMatMul(e2s, F, left1);   // Ma*Mb   -> Mc


    cvTranspose(v, vT);
    cvMatMul(e2, vT, left2);   // Ma*Mb   -> Mc

    cvAdd(left1, left2, left3);      // Ma+Mb   -> Mc

    int i,j;
    cvSetZero(P1);
    for (j=0; j<=2; j++)
    {
        cvmSet(P1,j,j,((double)1.00)); // Set M(i,j)

    }




    for (i=0; i<=2; i++)
    {
        for (j=0; j<=2; j++)
        {
            cvmSet(P2,i,j,cvmGet(left3,i,j)); // Set M(i,j)

        }

    }

    for (i=0; i<=2; i++)
    {

        cvmSet(P2,i,3,scale*cvmGet(e2,i,0)); // Set M(i,j)
    }

//debugging part
    {
//        CvMat* FDebug= cvCreateMat(3,3, CV_64F);
//        FfromProjectionMatrices(P1,P2,FDebug);
//
//
//        printf("Original F was: \nF:\n");
//        writeCVMatrix(cout,F );
//        printf("estimated projection matrices are: \nP1:\n");
//        writeCVMatrix(cout,P1 );
//        printf("\nP2:\n");
//        writeCVMatrix(cout,P2 );
//        printf("estimated F was: \nF:\n");
//        writeCVMatrix(cout,FDebug );
//        cvReleaseMat(&FDebug);

    }




    cvReleaseMat(&e1);
    cvReleaseMat(&e2);
    cvReleaseMat(&e2s);
    cvReleaseMat(&left1);
    cvReleaseMat(&left2);
    cvReleaseMat(&left3);
    cvReleaseMat(&right);
    cvReleaseMat(&vT);
}


int skewSymmetrify(const CvMat *in, CvMat *out)
{
    if (checkMatrixOK(in,3,1)== false || checkMatrixOK(out,3,3)==false)
    {
        return 0;
    }
    cvSetZero( out);

    cvmSet(out, 0, 1,-1.0*cvmGet(in,2,0));
    cvmSet(out, 0, 2,cvmGet(in,1,0));

    cvmSet(out, 1, 0,cvmGet(in,2,0));
    cvmSet(out, 1, 2,-1.0*cvmGet(in,0,0));

    cvmSet(out, 2, 0,-1.0*cvmGet(in,1,0));
    cvmSet(out, 2, 1,cvmGet(in,0,0));


//    printf("Debugging info for skew symmetric estimation, input: \n");
//    writeCVMatrix(cout,in );
//    printf("output\n");
//    writeCVMatrix(cout,out );

    return true;

}
bool checkMatrixOK(const CvMat *in,int h,int w)
{
    if (in == 0)
    {
        printf("parameters is a NULL pointer2!");
        return false;
    }

    if (!CV_IS_MAT(in))
    {
        printf("Input parameter must be a matrix!");
        return false;
    }

    if (in->cols != w ||in->rows != h)
    {

        printf( "Size of  matrix must be %d x %d but it is %d X %d!\n",h,w,in->rows,in->cols);
        assert(0);
        return false;
    }

    return true;
}
int FfromProjectionMatrices(const CvMat* P1,const CvMat* P2, CvMat* F)
{
    CvMat* P1pseudo = cvCreateMat(4,3, CV_64F);
    CvMat* temp1 = cvCreateMat(3,4, CV_64F);
    CvMat* C = cvCreateMat(4,1, CV_64F);
    CvMat* e = cvCreateMat(3,1, CV_64F);
    CvMat* e_skew = cvCreateMat(3,3, CV_64F);


    findCameraCenter(P1, C);

    findPseudoInverse_3x4(P1,P1pseudo);



    cvMatMul(P2, C, e);
    skewSymmetrify(e,e_skew);
    cvMatMul(e_skew,P2, temp1);
    cvMatMul(temp1,P1pseudo, F);
    normalizeMatrix(F);

    cvReleaseMat(&temp1);
    cvReleaseMat(&P1pseudo);
    cvReleaseMat(&C);
    cvReleaseMat(&e);
    cvReleaseMat(&e_skew );
}


int findCameraCenter(const CvMat* P, CvMat* C)
{

    checkMatrixOK(P,3,4);
    checkMatrixOK(C,4,1);

    //http://www4.ncsu.edu/~ipsen/REU09/chapter4.pdf
    CvMat* U = cvCreateMat(4,4, CV_64F);
    CvMat* V = cvCreateMat(4,4, CV_64F);
    CvMat* W = cvCreateMat(4,4, CV_64F);
    CvMat* PCl= cvCreateMat(4,4, CV_64F);

    cvSetZero( PCl);
    int i,j;


    for (i=0; i<=2; i++)
    {
        for (j=0; j<=3; j++)
        {
            cvmSet(PCl,i,j,cvmGet(P,i,j)); // Set M(i,j)

        }

    }

    cvSVD( PCl, W,  U, V );  //change all of the below back to U


    for (j=0; j<=3; j++)
    {
        cvmSet(C,j,0,cvmGet(V,j,3)); // Set M(i,j)

    }



    normalizeMatrix(C);
    cvReleaseMat(&PCl);
    cvReleaseMat(&U);
    cvReleaseMat(&V);
    cvReleaseMat(&W);
}

int findPseudoInverse_3x4(const CvMat* in,CvMat* out)
{
    checkMatrixOK(in,3,4);
    checkMatrixOK(out,4,3);


    CvMat* U = cvCreateMat(4,4, CV_64F);
    CvMat* UT = cvCreateMat(4,4, CV_64F);
    CvMat* V = cvCreateMat(4,4, CV_64F);
    CvMat* W = cvCreateMat(4,4, CV_64F);
    CvMat* inCl= cvCreateMat(4,4, CV_64F);
    CvMat* outful= cvCreateMat(4,4, CV_64F);


    cvSetZero( inCl);
    int i,j;


    for (i=0; i<=2; i++)
    {
        for (j=0; j<=3; j++)
        {
            cvmSet(inCl,i,j,cvmGet(in,i,j)); // Set M(i,j)

        }

    }

    cvSVD( inCl, W,  U, V );  //change all of the below back to U


    for (i=0; i<=3; i++)
    {
        double t=cvmGet(W,i,i);
        cvmSet(W,i,i,(t<0.00000000001)?0:(((double)1.0)/t)); //inverse
    }

    cvTranspose(U, UT);

    cvMatMul(V, W, inCl);
    cvMatMul(inCl, UT, outful);



    for (i=0; i<=3; i++)
    {
        for (j=0; j<=2; j++)
        {
            cvmSet(out,i,j,cvmGet(outful,i,j)); // Set M(i,j)

        }

    }

    cvReleaseMat(&outful);
    cvReleaseMat(&inCl);
    cvReleaseMat(&U);
    cvReleaseMat(&UT);
    cvReleaseMat(&V);
    cvReleaseMat(&W);
}

int normalizeMatrix( CvMat* in)
{
    if (in == 0)
    {
        printf("parameters is a NULL pointer3!");
        return false;
    }

    if (!CV_IS_MAT(in))
    {
        printf("Input parameter must be a matrix!");
        return false;
    }

    int h = in->rows;

    int w = in->cols;
    double s=cvmGet(in,h-1,w-1);
    if (fabs(s)<=0.00000000001)
    {
        printf("cant normalize matridx if last element is zero\n");
        return 0;
    }
    else
    {
        scaleMatrix( in, ((double)1.00)/s);
    }

}


void scaleMatrix(CvMat* in,double s)
{
    if (in == 0)
    {
        printf("parameters is a NULL pointer4!");
        return ;
    }

    if (!CV_IS_MAT(in))
    {
        printf("Input parameter must be a matrix!");
        return ;
    }

    int h = in->rows;

    int w = in->cols;

    int i,j;



    for (i=0; i<h; i++)
    {
        for (j=0; j<w; j++)
        {
            double t=cvmGet(in,i,j);
            cvmSet(in,i,j,t*s);

        }

    }

}


int findProjfromcompon(HRImage& img)
{

    findProjfromcompon(img.projectionMatrix,img.camPose.Rm,img.camPose.tm,img.intrinsicMatrix);


}


int findProjfromcompon(CvMat* P,CvMat* R,CvMat* t,CvMat* K)
{

    ////zzz remove this!
    //return 0;

    int i,j;
    CvMat* Ptemp=cvCreateMat(3,4,CV_64F);
    CvMat* Ttemp=cvCreateMat(3,1,CV_64F);
    CvMat* Rtemp=cvCreateMat(3,3,CV_64F);

    checkMatrixOK(P,3,4);
    checkMatrixOK(R,3,3);
    checkMatrixOK(K,3,3);
    checkMatrixOK(t,3,1);


//cvTranspose(R, Rtemp);
//cvMatMul(Rtemp, t, Ttemp);
    for (i=0; i<3; i++)
    {
        cvmSet(Ttemp,i,0,cvmGet(t,i,0));
    }

    //whether or not this rotation matrix should be transposed is still under debate , noah does it but just for multiplying with the t
    //cvTranspose(R, Rtemp);
    //cvMatMul(Rtemp, t, Ttemp);
    cvMatMul(R, t, Ttemp);
    scaleMatrix(Ttemp,-1);


    for (i=0; i<3; i++)
    {
        for (j=0; j<3; j++)
        {

            cvmSet(P,i,j,cvmGet(R,i,j));

        }

    }
//change back zzzz i should put -Rt or t here?
    for (j=0; j<3; j++)
    {

        cvmSet(P,j,3,cvmGet(t,j,0));

    }

    cvMatMul(K, P, Ptemp);

    for (i=0; i<3; i++)
    {
        for (j=0; j<4; j++)
        {

            cvmSet(P,i,j,cvmGet(Ptemp,i,j));

        }

    }


    cvReleaseMat(&Ptemp);
    cvReleaseMat(&Ttemp);
}
void showMatchAcross(vector<string>& fnames,  const  vector<vector<CvPoint2D32f> >& projPoints,const  vector<vector<vector<CvMat*> > >& lines)
{

    if(fnames.size()!= projPoints.size())
    {
        printf("sizes are nto compatible\n");
        return;

    }

    int numimg=projPoints.size();
    int j,i;

    vector<string> winNames;
    vector<IplImage*> imagesCopy;

    vector<vector<CvScalar> > colors;
    colors.resize(numimg);
    for(i=0; i<numimg; i++)
    {
        int numpts=projPoints[i].size();

        for(j=0; j<numpts; j++)
        {
            double r1=	255*(   (double)rand() / ((double)(RAND_MAX)+(double)(1)) );
            double r2=	255*(   (double)rand() / ((double)(RAND_MAX)+(double)(1)) );
            double r3=	255*(   (double)rand() / ((double)(RAND_MAX)+(double)(1)) );
            colors[i].push_back(CV_RGB(r1,r2,r3));

            //  printf("color [%d][%d] = %f %f %f \n",i,j,colors[i][j].val[0],colors[i][j].val[1],colors[i][j].val[2]);
        }
    }



    for(i=0; i<numimg; i++)
    {
        IplImage* imgTemp=cvLoadImage(fnames[i].c_str());
        int numpts=projPoints[i].size();
        for(j=0; j<numpts; j++)
        {
            draw_cross(projPoints[i][j], colors[i][j],4,imgTemp );

        }

        for(int k=0; k<numimg; k++)
        {
            for(j=0; j<numpts; j++)
            {
                if(lines[k][i][j]!=NULL)
                    drawLineonIMG(imgTemp,lines[k][i][j],colors[k][j]);
            }
        }

        string winname=fnames[i].c_str();
        winNames.push_back(winname);
        imagesCopy.push_back(imgTemp);
        cvNamedWindow( winname.c_str(), CV_WINDOW_AUTOSIZE);

        cvMoveWindow( winname.c_str(), 100+(i*50), 100);

        cvShowImage( winname.c_str(),imagesCopy[i]  );


    }



    cvWaitKey(0);

    for(i=0; i<numimg; i++)
    {
        cvDestroyWindow(winNames[i].c_str());

        cvReleaseImage(&imagesCopy[i]);
    }

}

void showMatchAcross(vector<string>& fnames,  const  vector<vector<CvPoint2D32f> >& projPoints)
{
    int numimages=fnames.size();
    int numpts=projPoints[0].size();  ///yyy i take rthe number of points to be that of the first frame, this might not always be the case

    vector< vector< vector < CvMat* >  > > lines(numimages, vector< vector < CvMat*> > (numimages, vector<CvMat*>(numpts)) );


    for(int k=0; k< lines.size() ; k++ )
    {
        for(int j=0; j< lines[k].size() ; j++ )
        {
            for(int m=0; m< lines[k][j].size() ; m++ )
            {
                lines[k][j][m]=NULL;

            }
        }

    }

    showMatchAcross( fnames,  projPoints,lines);
}

double cvTriangulatePointsNframs(int numframes, vector<CvMat*>& projMatrs,vector<CvPoint2D32f>& projPoints,CvPoint3D32f& spPoint)
{

    int i,j;

    if(projMatrs.size()!=numframes || projPoints.size()!=numframes  )
    {
        printf("wrong size of vectors in triangulation\n");
        printf("number of projection matrices was %d and number of points was %d and the unmfram was %d\n",projMatrs.size() , projPoints.size() , numframes);

    }


    for(i=0; i<numframes; i++)
    {

        if( projMatrs[i] == 0)
            CV_Error( CV_StsNullPtr, "Some of parameters is a NULL pointer" );

        if( !CV_IS_MAT(projMatrs[i])  )
            CV_Error( CV_StsUnsupportedFormat, "Input parameters must be matrices" );

        if( projMatrs[i]->cols != 4 || projMatrs[i]->rows != 3 )
            CV_Error( CV_StsUnmatchedSizes, "Size of projection matrices must be 3x4" );

    }






    CvMat* matrV=cvCreateMat(4,4,CV_64F);
    CvMat* matrW=cvCreateMat(numframes*2,4,CV_64F);
    CvMat* matrA=cvCreateMat(numframes*2,4,CV_64F);
    /* Solve system for each point */
//printf("__________________________________\n");

    for( j = 0; j < numframes; j++ )/* For each view */
    {
        double x,y;
        x = projPoints[j].x;
        y = projPoints[j].y;
        //  printf("point [%f  %f]\t",x,y);
        //    writeCVMatrix(cout<<"P"<<endl,projMatrs[j] );

        for( int k = 0; k < 4; k++ )
        {
            cvmSet(matrA, j*2+0, k, x * cvmGet(projMatrs[j],2,k) -     cvmGet(projMatrs[j],0,k) );
            cvmSet(matrA, j*2+1, k, y * cvmGet(projMatrs[j],2,k) -     cvmGet(projMatrs[j],1,k) );
            //    cvmSet(matrA, j*3+2, k, x * cvmGet(projMatrs[j],1,k) - y * cvmGet(projMatrs[j],0,k) );
        }
    }



    cvSVD(matrA,matrW,0,matrV,CV_SVD_V_T);

    /* Copy computed point */
    spPoint.x=    cvmGet(matrV,3,0)/cvmGet(matrV,3,3);/* X */
    spPoint.y=    cvmGet(matrV,3,1)/cvmGet(matrV,3,3);/* Y */
    spPoint.z=    cvmGet(matrV,3,2)/cvmGet(matrV,3,3);/* Z */


//printf("mapped point to [%lf %lf %lf]\n"  ,spPoint.x,spPoint.y,spPoint.z);


    cvReleaseMat(&matrV);
    cvReleaseMat(&matrW);
    cvReleaseMat(&matrA);

    return 0;




}

double projectionErrorSquared(const CvMat* P, CvPoint3D32f s_pt,CvPoint2D32f im_pt)
{
    CvPoint2D32f im_proj_pt=project3DPoint(P, s_pt);

    double deltaX,deltaY;
    double rep_error=0;




    deltaX = (double)(im_proj_pt.x-im_pt.x);
    deltaY = (double)(im_proj_pt.y-im_pt.y);
    rep_error=sqrt((deltaX*deltaX)+(deltaY*deltaY));

    return rep_error;
}

CvPoint2D32f project3DPoint(const CvMat* P, CvPoint3D32f S)
{
    CvMat point3D;
    double point3D_dat[4];
    point3D = cvMat(4,1,CV_64F,point3D_dat);

    CvMat point2D;
    double point2D_dat[3];
    point2D = cvMat(3,1,CV_64F,point2D_dat);


    point3D_dat[0] =S.x;
    point3D_dat[1] = S.y;
    point3D_dat[2] = S.z;
    point3D_dat[3] = 1;


    cvmMul(P, &point3D, &point2D);


    double wr = (double)point2D_dat[2];
    double xr = (double)(point2D_dat[0]/wr);
    double yr = (double)(point2D_dat[1]/wr);

    CvPoint2D32f  curPt;
    curPt.x= xr ;
    curPt.y= yr ;


    return curPt;


}
double findDepth(CvMat* P,CvPoint3D32f S)
{

    int i,j;
    double w;
    double T=1.0;
    double deter=0;
    double norm=0;
    double sig=0;
    double depth=0;

    CvMat* PT=cvCreateMat(4,1,CV_64F);
    CvMat* PTim=cvCreateMat(3,1,CV_64F);

    CvMat* M=cvCreateMat(3,3,CV_64F);





    for(i=0; i<3; i++)
    {
        for(j=0; j<3; j++)
        {
            cvmSet(M,i,j, cvmGet(P,i,j));
        }

    }



    cvmSet(PT,0,0,S.x);
    cvmSet(PT,1,0,S.y);
    cvmSet(PT,2,0,S.z);
    cvmSet(PT,3,0,T);

    cvMatMul(P,PT,PTim);

    w=cvmGet(PTim,2,0);

    norm= sqrt( (cvmGet(M,2,0)*cvmGet(M,2,0))+(cvmGet(M,2,1)*cvmGet(M,2,1))+(cvmGet(M,2,2)*cvmGet(M,2,2)) );
    deter=cvDet(M);

    if(deter<=0)
        sig=-1.0;
    else
        sig=1.0;

    depth=(sig*w)/norm;

    cvReleaseMat(&PT);
    cvReleaseMat(&M);
    cvReleaseMat(&PTim);

    return depth;
}


int  findProjDLTMinimal(const CvMat* data,vector<CvMat*>& models )
{
    // we are going to assume that the data is laid out like this:
    //x1 x2 ...
    //y1 y2 ...
    //X1 X2 ...
    //Y1 Y2 ...
    //Z1 Z2 ...

    // so each data is a single column, the image points is the first two rows, the space point is the next 3
//179 of zesserman

//
//
//so the question is what to do with planar scenes, in those cases use the homography to weed out the outliers rather than using the ransac thing
//so check if the motion is a homography between the target frame and any reconstructed frames, and if yes use the H to root out outliers and then
//do the resectioning with opencv's find extrinsics,
//
//also experiment with the camera calibrate function , that might be able to refine your intrinsics but i dont thnk it can do planar scnees

    int i,j;
    CvMat* P=cvCreateMat(3,4,CV_64F);


    cvSetZero(P);

    if(data->cols<6)
    {
        printf("\nnot enough points\n");
        return -1;
    }

    CvMat* A=cvCreateMat(2*data->cols,12,CV_64F);
    CvMat* U=cvCreateMat(2*data->cols,2*data->cols,CV_64F);
    CvMat* V=cvCreateMat(12,12,CV_64F);
    CvMat* W=cvCreateMat(2*data->cols,12,CV_64F);



    for(int i=0; i<data->cols; i++)
    {
        int curRow=i*2;

        double xp= cvmGet(data,0 ,i)  ;
        double yp= cvmGet(data,1 ,i)  ;
        double wp= 1.0  ;

        double Xw= cvmGet(data,2 ,i)  ;
        double Yw= cvmGet(data,3 ,i)  ;
        double Zw= cvmGet(data,4 ,i)  ;
        double Ww= 1.0;


        //first row
        cvmSet(A,curRow,0,0.0);
        cvmSet(A,curRow,1,0.0);
        cvmSet(A,curRow,2,0.0);
        cvmSet(A,curRow,3,0.0);

        cvmSet(A,curRow,4,-wp*Xw);
        cvmSet(A,curRow,5,-wp*Yw);
        cvmSet(A,curRow,6,-wp*Zw);
        cvmSet(A,curRow,7,-wp*Ww);

        cvmSet(A,curRow,8,yp*Xw);
        cvmSet(A,curRow,9,yp*Yw);
        cvmSet(A,curRow,10,yp*Zw);
        cvmSet(A,curRow,11,yp*Ww);

//second row


        cvmSet(A,curRow+1,0,wp*Xw);
        cvmSet(A,curRow+1,1,wp*Yw);
        cvmSet(A,curRow+1,2,wp*Zw);
        cvmSet(A,curRow+1,3,wp*Ww);

        cvmSet(A,curRow+1,4,0.0);
        cvmSet(A,curRow+1,5,0.0);
        cvmSet(A,curRow+1,6,0.0);
        cvmSet(A,curRow+1,7,0.0);

        cvmSet(A,curRow+1,8,-xp*Xw);
        cvmSet(A,curRow+1,9,-xp*Yw);
        cvmSet(A,curRow+1,10,-xp*Zw);
        cvmSet(A,curRow+1,11,-xp*Ww);
    }



    cvSVD( A,  W, U,  V,CV_SVD_MODIFY_A);

    int k=0;
    for(i=0; i<3; i++)
    {
        for(j=0; j<4; j++)
        {
            cvmSet(P,i,j,cvmGet(V,k++,11));
        }
    }



    for(int pp=0; pp<models.size(); pp++) cvReleaseMat(&models[pp]);
    models.clear();

    models.push_back(P);

    cvReleaseMat(&A);
    cvReleaseMat(&U);
    cvReleaseMat(&V);
    cvReleaseMat(&W);
    return 1;
}

double projError(const CvMat* data, const CvMat* P,vector<double>&  errors )
{
    if(errors.size()!=data->cols)
    {
        printf("why are teh errors not the same size\n");
        errors.resize(data->cols);
    }

    int i;
    double tot_error=0;
    double numPts=data->cols;


    for(i=0; i<data->cols; i++)
    {

        CvPoint3D32f  structure;
        CvPoint2D32f  curPt;

        curPt.x= cvmGet(data,0 ,i)  ;
        curPt.y= cvmGet(data,1 ,i)  ;


        structure.x= cvmGet(data,2 ,i)  ;
        structure.y= cvmGet(data,3 ,i)  ;
        structure.z= cvmGet(data,4 ,i)  ;


        errors[i]= projectionErrorSquared( P,structure,curPt);
//printf("the errorr for %d was %f \n",i,errors[i]);
        tot_error+=errors[i];
    }

    tot_error/=numPts;

    return tot_error;
}


bool scenePlanar(const CvMat* data)
{
    return false; //i need a way of finding planar scenes
}


int formDataMatrixRobustResectioning(CvMat** data, vector< CvPoint2D32f> impts,vector< CvPoint3D32f> wrldpts)
{

    if(impts.size()!=wrldpts.size())
    {
        printf("the size of the world points and image points is not the same\n");
        return -1;
    }

    (*data)=    cvCreateMat(5,impts.size(),CV_64F);

    for(int i=0; i<impts.size(); i++)
    {

        cvmSet((*data),0 ,i, impts[i].x)  ;
        cvmSet((*data),1 ,i, impts[i].y)  ;


        cvmSet((*data),2 ,i, wrldpts[i].x)  ;
        cvmSet((*data),3 ,i, wrldpts[i].y)  ;
        cvmSet((*data),4 ,i, wrldpts[i].z)  ;
    }


}
int writePlyFile(vector<CvPoint3D32f> & strucure,vector<int>& structureValid,string fname)
{
    FILE* fp;
    fp=fopen(fname.c_str(),"wt");
    fprintf(fp,"ply\n");
    fprintf(fp,"format ascii 1.0\n");
    fprintf(fp,"element vertex 1606\n");
    fprintf(fp,"property float x\n");
    fprintf(fp,"property float y\n");
    fprintf(fp,"property float z\n");
    fprintf(fp,"comment format of each line is xyz RGB\n");
    fprintf(fp,"end_header\n");

    for(int i=0; i<strucure.size(); i++)
    {
        if(structureValid[i]>0)
        {


            fprintf(fp,"%f %f %f\n",strucure[i].x,strucure[i].y,strucure[i].z);
        }
    }
    fclose(fp);
    return 0;
}
