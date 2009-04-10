#include "HRprimitives.h"
#include <vector>
#include <iostream>
#include <fstream>
#include <iomanip>



using namespace std;
/** @brief HRCorrespond2N
  *
  * @todo: document this function
  */
HRCorrespond2N::HRCorrespond2N()
{
    indexIm1=0;
    indexIm2=0;
    hr1ptr=NULL;
    hr2ptr=NULL;

}


/** @brief ~HRCorrespond2N
  *
  * @todo: document this function
  */
HRCorrespond2N::~HRCorrespond2N()
{

}
// Copy Constructor
HRCorrespond2N::HRCorrespond2N(const HRCorrespond2N & rec)
{
    int indexIm1;
    int indexIm2;
    HRImage* hr1ptr;
    HRImage* hr2ptr;
    vector<matchIndex> imIndices;



    imIndices=vector<matchIndex>(rec.imIndices);
    indexIm1=rec.indexIm1;
    indexIm2=rec.indexIm2;
    hr1ptr =rec.hr1ptr ;
    hr2ptr =rec.hr2ptr ;
    motion =rec.motion;

}

// assignment operator
HRCorrespond2N & HRCorrespond2N::operator=(const HRCorrespond2N & rhs) throw()
{
    if (this == &rhs) return *this;
    imIndices=vector<matchIndex>(rhs.imIndices);
    indexIm1=rhs.indexIm1;
    indexIm2=rhs.indexIm2;
    hr1ptr =rhs.hr1ptr ;
    hr2ptr =rhs.hr2ptr ;
    motion =rhs.motion;
    return *this;
}


ostream &operator<<(ostream &stream, const HRCorrespond2N& ob)
{

    double x1,x2,y1,y2;

    double cscore; //higher is better
    double cmotionError;
    int cinlier; //we dont


    if (ob.hr1ptr==NULL || ob.hr2ptr==NULL)
    {
        stream<<"images have not been initialized"<<endl;
        return stream;
    }

    stream <<"#"<< ob.hr1ptr->filename <<"   ( index="<<ob.indexIm1 <<") "<< "                 "<<ob.hr2ptr->filename <<" ( index="<<ob.indexIm2 <<") "<<endl;


    stream << "#match_num"<<setw(11)<<"X"<<"("<<ob.indexIm1 <<")"<<"\t"<<setw(11)<<"Y"<<
    "("<<ob.indexIm1 <<")"<<"\t"<<setw(11)<<"X"<<"("<<ob.indexIm2 <<")"<<"\t"<<setw(11)<<"Y"<<"("<<ob.indexIm2 <<")"
    <<"\t"<<setw(11)<<"score"<<"\t"<<setw(11)<<"motion error"<<"\t"<<setw(11)<<"inlier"
    <<endl;



    for (int i=0;i<ob.imIndices.size();i++)
    {
        x1=ob.hr1ptr->HR2DVector[ob.imIndices[i].imindex1]->location.x;
        x2=ob.hr1ptr->HR2DVector[ob.imIndices[i].imindex1]->location.y;
        y1=ob.hr2ptr->HR2DVector[ob.imIndices[i].imindex2]->location.x;
        y2=ob.hr2ptr->HR2DVector[ob.imIndices[i].imindex2]->location.y;

        cscore=ob.imIndices[i].score; //higher is better
        cmotionError=ob.imIndices[i].motionError;
        cinlier=ob.imIndices[i].inlier;

        stream <<i<<"\t\t"<<setw(10)<<fixed<<setprecision(5)<<x1 <<"\t"<<setw(11)<<y1<<"\t"<<setw(11)<<x2<<"\t" <<setw(11)<<y2 <<"\t"
        <<"\t"<<setw(11)<<cscore<<"\t"<<setw(11)<<cmotionError<<"\t"<<setw(11)<<cinlier
        <<endl;

    }




    return stream;

}
void HRCorrespond2N::WriteMatches()
{

    string fname=TEMPDIR+string("/")+combineFnames(hr1ptr->filename,hr2ptr->filename,"_matches.txt");



    fstream fp_out;
    fp_out.open(fname.c_str(), ios::out);
    fp_out<<(*this);
    fp_out.close();

}
void HRCorrespond2N::WriteMotion()
{

    string fname=TEMPDIR+string("/")+combineFnames(hr1ptr->filename,hr2ptr->filename,"_Motion.txt");



    fstream fp_out;
    fp_out.open(fname.c_str(), ios::out);
    fp_out<<motion;
    fp_out.close();
}


int HRCorrespond2N::removeOutliers(const CvMat* tstatus)
{

    int numPoints = imIndices.size();
    std::vector<matchIndex>::iterator it;
    for (int i=0;i<numPoints;i++)
    {
        if (tstatus->data.fl[i]==0) //outlier
        {
            it = imIndices.begin() + i;
            imIndices.erase( it);
        }
    }

}


int HRCorrespond2N::findGeomtry(MotionType mtype)
{


    int num=motion.findMotionModel(hr1ptr,hr2ptr,imIndices,  mtype);

    if ( num == 1 )
    {
        printf("Fundamental matrix was found between images: %d and %d, number of outliers: %d with error %f\n",indexIm1,indexIm2,motion.numOutlier, motion.motionError);
    }
    else
    {
        printf("Fundamental matrix was not found between images: %d and %d\n",indexIm1,indexIm2);
    }

}

HRFeature::HRFeature()
{
    valid=false;
    pvi=0;//never to be used
}
HRFeature::HRFeature(int x,int y)
{
    location.x=x;
    location.y=y;
    valid=true;
    pvi=0.5;//equally likely as any other feature
    scale= ori=0;           // Scale and orientation (range [-PI,PI]) in case we are using sift//


}

HRFeature::~HRFeature()
{
}


/** @brief calculateError
  *
  * @todo: document this function
  */
double MotionGeometry::calculateError()
{

}

/** @brief writeMotionMatrix
  *
  * @todo: document this function
  */
void MotionGeometry::writeMotionMatrix()
{

}

/** @brief ~MotionGeometry
  *
  * @todo: document this function
  */
MotionGeometry::~MotionGeometry()
{
    if ( MotionModel!=NULL)
    {
        cvReleaseMat(&MotionModel);
    }

}

/** @brief MotionGeometry
  *
  * @todo: document this function
  */
MotionGeometry::MotionGeometry()
{
    MotionModel = cvCreateMat(3,3,CV_64FC1);

    motionError=0;//in pixels
    numOutlier=0;


    valid=0;
}


double MotionGeometry::getMotionElement(int i,int j) const
{
    if ( MotionModel->height==0 ||MotionModel->width==0 )
    {

        cout<<"accessing unintialized matrix, returning zero"<<endl;
        return 0;

    }

    if (i>MotionModel->height || i<0 || j>MotionModel->width || j<0)
    {
        cout<<"requested element out of range"<<endl;
        return 0;
    }

    int   step  = MotionModel->step/sizeof(double);
    double *data = MotionModel->data.db;

    return (data+i*step)[j];
}
/** @brief (one liner)
*
* (documentation goes here)
*/
int MotionGeometry::findFMatrix(const HRImage* hr1,const HRImage* hr2,  vector<matchIndex>& indices )
{




    if (hr1==NULL || hr2==NULL)
    {
        cout<<"images have not been initialized"<<endl;
        return 0;
    }
    if (indices.size()==0)
    {
        cout<<"matches dont exist, returning nothing"<<endl;
        return 0;

    }

        int numPoints = indices.size();


    if(numPoints<9)
    {

     cout<<"not enough points for matching\n"<<endl;
     return 0;
    }

    ///find F

    CvMat* points1;
    CvMat* points2;
    CvMat* status;
    CvMat* err_array;


    points1  = cvCreateMat(1,numPoints,CV_32FC2);
    points2  = cvCreateMat(1,numPoints,CV_32FC2);
    status   = cvCreateMat(1,numPoints,CV_8UC1);
    err_array = cvCreateMat( 1, numPoints, CV_32FC1 );

    for (int i=0;i<numPoints;i++)
    {
        points1->data.fl[i*2]=hr1->HR2DVector[indices[i].imindex1]->location.x;
        points1->data.fl[i*2+1]=hr1->HR2DVector[indices[i].imindex1]->location.y;

        points2->data.fl[i*2]=hr2->HR2DVector[indices[i].imindex2]->location.x;
        points2->data.fl[i*2+1]=hr2->HR2DVector[indices[i].imindex2]->location.y;
        status->data.ptr[i]=0;
    }



    int pointsRejected=0;
    double error=0;
    int num = cvFindFundamentalMat(points1,points2,MotionModel,CV_FM_RANSAC,1.0,0.99,status);
    error= computeReprojErrorF( points1,points2, MotionModel, err_array ,status);




    for (int i=(numPoints-1);i>=0;i--)
    {
        pointsRejected=pointsRejected+(1-status->data.ptr[i]);

        indices[i].motionError=err_array->data.fl[i];
        indices[i].inlier=status->data.ptr[i];

    }


    motionError=error;
    numOutlier=pointsRejected;





    cvReleaseMat(&points1);
    cvReleaseMat(&points2);
    cvReleaseMat(&status);
    cvReleaseMat(&err_array);


    return num;
}
int MotionGeometry::findMotionModel(const HRImage* hr1,const HRImage* hr2,  vector<matchIndex>& indices ,MotionType mtype)
{


    if (mtype==FUNDAMENTAL)
    {
        return findFMatrix( hr1,hr2,  indices );

    }
    else
    {
        cout<<"unknown motion model"<<endl;

        return -1;

    }




}
double MotionGeometry::computeReprojErrorFfromEpipolars( const CvMat* _m1, const CvMat* _m2, const CvMat* model, CvMat* _err, const CvMat* status )
{
    int i;
    int N=_m1->cols;
    int numOutliers=0;
    vector<CvPoint3D32f> lines[2];

    lines[0].resize(N);
    lines[1].resize(N);
    CvMat _L1 = cvMat(1, N, CV_32FC3, &lines[0][0]);
    CvMat _L2 = cvMat(1, N, CV_32FC3, &lines[1][0]);
    float* terr = _err->data.fl;
    const float* m1 = _m1->data.fl;
    const float* m2 = _m2->data.fl;

    cvComputeCorrespondEpilines( _m1, 1, model, &_L1 );
    cvComputeCorrespondEpilines( _m2, 2, model, &_L2 );
    double avgErr = 0;
    for ( i = 0; i < N; i++ )
    {

                double err = fabs(m1[i*2]*lines[1][i].x +
                          m1[i*2+1]*lines[1][i].y + lines[1][i].z)
                     + fabs(m2[i*2]*lines[0][i].x +
                            m2[i*2+1]*lines[0][i].y + lines[0][i].z);

        terr[i] = (float)err;

        if(status->data.ptr[i]==1)//dont add outliers to error
        avgErr += err;
        else
        numOutliers++;
        //  printf("the type two error is %f\n",err);
    }
    avgErr/=((double)(N-numOutliers));

    // printf( "avg err = %f for %d points\n", avgErr ,N);

    return avgErr;





}


//this was stolen from opencv , i didnt know how else to use it without having to use the whole class
double MotionGeometry::computeReprojErrorF( const CvMat* _m1, const CvMat* _m2, const CvMat* model, CvMat* _err , const CvMat* status )
{

//    const CvMat* _m1temp;
//    _m1temp=_m1;
//    _m1= _m2;
//    _m2=_m1temp;
int numOutliers=0;

    int i, count = _m1->cols;
    const float* m1 = _m1->data.fl;
    const float* m2 = _m2->data.fl;
    const double* FF = model->data.db;
    float* err = _err->data.fl;

    double totalerr=0;
    for ( i = 0; i < count; i++ )
    {

        double a, b, c, d1, d2, s1, s2;

        a = FF[0]*m1[i*2] + FF[1]*m1[i*2+1] + FF[2];
        b = FF[3]*m1[i*2] + FF[4]*m1[i*2+1] + FF[5];
        c = FF[6]*m1[i*2] + FF[7]*m1[i*2+1] + FF[8];

        s2 = 1./(a*a + b*b);
        d2 = m2[i*2]*a + m2[i*2+1]*b + c;

        a = FF[0]*m2[i*2] + FF[3]*m2[i*2+1] + FF[6];
        b = FF[1]*m2[i*2] + FF[4]*m2[i*2+1] + FF[7];
        c = FF[2]*m2[i*2] + FF[5]*m2[i*2+1] + FF[8];

        s1 = 1./(a*a + b*b);
        d1 = m1[i*2]*a + m1[i*2+1]*b + c;

        err[i] = sqrt((float)(d1*d1*s1)) + sqrt((float)(d2*d2*s2));

        if(status->data.ptr[i]==1)
        totalerr+=err[i];
        else
        numOutliers++;

       // cout<<i<<"\t\t"<<err[i]<<endl;
        if(err[i]<0) cout<<"whaaaaaaaaaaaaaaaaaaat negative error\n"<<endl;
    }
    totalerr/=((double)(count-numOutliers));
    // printf ("average error method 1 is %f with %d points\n",totalerr,count);

    return totalerr;
}

ostream &operator<<(ostream &stream,  const MotionGeometry& ob)
{

    if ( ob.MotionModel->height==0 || ob.MotionModel->width==0 )
    {
        stream<<"EMPTY"<<endl;
        return stream;
    }

    int n_rows = ob.MotionModel->rows;

    int n_cols = ob.MotionModel->cols;





    for (int i = 0; i < n_rows; ++i)
    {
        for (int j = 0; j < n_cols; ++j)
        {


            stream<< ob.getMotionElement( i, j) << "\t";
        }
        stream << "\n";

    }




    return stream;




}



MotionGeometry::MotionGeometry(const MotionGeometry & rec)
{

    MotionModel=cvCloneMat( rec.MotionModel);



}

// assignment operator
MotionGeometry & MotionGeometry::operator=(const MotionGeometry & rhs) throw()
{
    if (this == &rhs) return *this;

    if ( MotionModel!=NULL)
    {
        cvReleaseMat(&MotionModel);
    }


    MotionModel=cvCloneMat( rhs.MotionModel);

    return *this;
}
