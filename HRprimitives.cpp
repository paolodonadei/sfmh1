#include "HRprimitives.h"
#include <vector>
#include <iostream>
#include <fstream>
#include <iomanip>
#include "argproc.h"
#include "general.h"
#include "utils/selfcalib/focallength.h"
extern const char* TEMPDIR;

using namespace std;











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



    for (int i=0; i<ob.imIndices.size(); i++)
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

int HRCorrespond2N::readMatches(string filename)
{
    int numatches=0;
    char str[2000];

    fstream fp_in;
    fp_in.open(filename.c_str(), ios::in);


    if (!fp_in.is_open())
    {
        cout << "File " <<  filename << " does not exist" << endl;
        exit(1);
    }

    while (!fp_in.eof())
    {
        fp_in.getline(str,2000);
        numatches++;

    }

    numatches=numatches-3; //1 because there is two comment lines, and one because of the last line
    if (numatches<1)
    {
        cout<<"number of matches found is less than 1, quitting"<<endl;
        return 0;

    }

    fp_in.clear();
    fp_in.seekg (0, ios::beg);  //rewinding
    fp_in.getline(str,2000);
    fp_in.getline(str,2000);


    int i=0;


    if (imIndices.size()>0)
    {
        cout<<"WARNING:the size of the existing matches is more than zero, we are now appending matches , is this intended?"<<endl;
    }


    while (!fp_in.eof() && i<numatches) //second check is redundant, being safe
    {
        fp_in.getline(str,2000);

        string s(str);
        string out;
        istringstream ss;
        ss.str(s);

        ss>>out; //first one is the match number

        matchIndex indexTemp;




        ss>>out;
        indexTemp.imindex1=from_string<int>(out, std::dec);

        ss>>out;
        indexTemp.imindex2=from_string<int>(out, std::dec);

        ss>>out;
        indexTemp.score=from_string<double>(out, std::dec);

        imIndices.push_back(indexTemp);


        i++;

    }

    fp_in.close();

    fprintf(stdout,"Found %d matches from file %s.\n", numatches,filename.c_str());

    return numatches;

}
void HRCorrespond2N::writeIndices()
{
    string fname=TEMPDIR+string("/")+combineFnames(hr1ptr->filename,hr2ptr->filename,"_indices.txt");



    fstream fp_out;
    fp_out.open(fname.c_str(), ios::out);
    if (!fp_out)
    {
        cout << "Cannot open file.  "<<fname<<endl;
        return ;
    }


    if (hr1ptr==NULL ||hr2ptr==NULL)
    {
        cout<<"images have not been initialized"<<endl;
        return ;
    }

    fp_out <<"#"<< hr1ptr->filename <<"   ( index="<<indexIm1 <<") "<< "                 "<<hr2ptr->filename <<" ( index="<<indexIm2 <<") "<<endl;


    fp_out << "#match_num"<<setw(11)<<"index1"<<"("<<indexIm1 <<")"<<"\t"<<setw(11)<<"index2"<<"("<<indexIm2 <<")"<<"\t"<<setw(11)<<"score" <<endl;



    for (int i=0; i<imIndices.size(); i++)
    {

        fp_out <<i<<"\t\t"<<imIndices[i].imindex1<<"\t\t"<<imIndices[i].imindex2 <<"\t\t"<<setw(10)<<fixed<<setprecision(10)<< imIndices[i].score <<endl;

    }



    fp_out.close();



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
    {
        string fname=TEMPDIR+string("/")+combineFnames(hr1ptr->filename,hr2ptr->filename,"_FMotion.txt");

        motion.filenameF=fname;

        fstream fp_out;
        fp_out.open(motion.filenameF.c_str(), ios::out);
        motion.writeMatrix( fp_out, FUNDAMENTAL);
        fp_out.close();

        string fname2=TEMPDIR+string("/")+string("F_")+string(INDEXFNAME);




        fp_out.open(fname2.c_str(), ios::out |ios::app);
        fp_out<< indexIm1 << "\t"  << indexIm2 <<"\t"  << combineFnames(hr1ptr->filename,hr2ptr->filename,"_FMotion.txt") << endl;
        fp_out.close();

    }
    {
        string fname=TEMPDIR+string("/")+combineFnames(hr1ptr->filename,hr2ptr->filename,"_HMotion.txt");

        motion.filenameH=fname;

        fstream fp_out;
        fp_out.open(motion.filenameH.c_str(), ios::out);
        motion.writeMatrix(fp_out ,HOMOGRAPHY);
        fp_out.close();

        string fname2=TEMPDIR+string("/")+string("H_")+string(INDEXFNAME);




        fp_out.open(fname2.c_str(), ios::out |ios::app);
        fp_out<< indexIm1 << "\t"  << indexIm2 <<"\t"  << combineFnames(hr1ptr->filename,hr2ptr->filename,"_HMotion.txt") << endl;
        fp_out.close();
    }
    {
        string fname=TEMPDIR+string("/")+combineFnames(hr1ptr->filename,hr2ptr->filename,"_EMotion.txt");

        motion.filenameE=fname;

        fstream fp_out;
        fp_out.open(motion.filenameE.c_str(), ios::out);
        motion.writeMatrix(fp_out ,ESSENTIAL);
        fp_out.close();

        string fname2=TEMPDIR+string("/")+string("E_")+string(INDEXFNAME);




        fp_out.open(fname2.c_str(), ios::out |ios::app);
        fp_out<< indexIm1 << "\t"  << indexIm2 <<"\t"  << combineFnames(hr1ptr->filename,hr2ptr->filename,"_EMotion.txt") << endl;
        fp_out.close();
    }
}


int HRCorrespond2N::removeOutliers(const CvMat* tstatus)
{

    int numPoints = imIndices.size();
    std::vector<matchIndex>::iterator it;
    for (int i=0; i<numPoints; i++)
    {
        if (tstatus->data.fl[i]==0) //outlier
        {
            it = imIndices.begin() + i;
            imIndices.erase( it);
        }
    }

}

int HRCorrespond2N::findGeomtry()
{



    int num=motion.findMotionModels(hr1ptr->HR2DVector,hr2ptr->HR2DVector,imIndices,  FUNDAMENTAL);
    motion.findMotionModels(hr1ptr->HR2DVector,hr2ptr->HR2DVector,imIndices,  HOMOGRAPHY);

    if ( num == 1 )
    {
        printf("Fundamental matrix was found between images: %d and %d, number of outliers: %d, inliers: %d with error %f\n",indexIm1,indexIm2,motion.numOutlier_F, motion.numInliers_F,motion.motionError_F);

    }
    else
    {
        printf("Fundamental matrix was not found between images: %d and %d\n",indexIm1,indexIm2);
    }
    printf("Homography matrix was found between images: %d and %d, number of outliers: %d, inliers: %d with error %f\n",indexIm1,indexIm2,motion.numOutlier_H, motion.numInliers_H,motion.motionError_H);
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


/////////////////////////////////////////////////////////


void MotionGeometry::writeMatrix(ostream &stream,MotionType mtype)
{
    CvMat* MotionModel;
    if(mtype==FUNDAMENTAL)
    MotionModel=MotionModel_F;
    else if(mtype==HOMOGRAPHY)
    MotionModel=MotionModel_H;
    else if(mtype==ESSENTIAL)
    MotionModel=MotionModel_E;
    else
    printf("incorect motion requested!\n");


    if ( MotionModel->height==0 || MotionModel->width==0 )
    {
        stream<<"EMPTY"<<endl;
        return;
    }

    int n_rows = MotionModel->rows;

    int n_cols = MotionModel->cols;





    for (int i = 0; i < n_rows; ++i)
    {
        for (int j = 0; j < n_cols; ++j)
        {


            stream<< getMotionElement( i, j, mtype) << "\t";
        }
        stream << "\n";

    }


}

MotionGeometry::MotionGeometry(const MotionGeometry & rec)
{

    MotionModel_F=cvCloneMat( rec.MotionModel_F);
    MotionModel_H=cvCloneMat( rec.MotionModel_H);
      MotionModel_E=cvCloneMat( rec.MotionModel_E);


}

// assignment operator
MotionGeometry & MotionGeometry::operator=(const MotionGeometry & rhs) throw()
{
    if (this == &rhs) return *this;

    if ( MotionModel_F!=NULL)
    {
        cvReleaseMat(&MotionModel_F);
    }

    if ( MotionModel_H!=NULL)
    {
        cvReleaseMat(&MotionModel_H);
    }
    if ( MotionModel_E!=NULL)
    {
        cvReleaseMat(&MotionModel_E);
    }
    MotionModel_F=cvCloneMat( rhs.MotionModel_F);
    MotionModel_H=cvCloneMat( rhs.MotionModel_H);
     MotionModel_E=cvCloneMat( rhs.MotionModel_E);
    return *this;
}

double MotionGeometry::calculateError()
{

}



/** @brief ~MotionGeometry
  *
  * @todo: document this function
  */
MotionGeometry::~MotionGeometry()
{
    if ( MotionModel_F!=NULL)
    {
        cvReleaseMat(&MotionModel_F);
    }
    if ( MotionModel_H!=NULL)
    {
        cvReleaseMat(&MotionModel_H);
    }
    if ( MotionModel_E!=NULL)
    {
        cvReleaseMat(&MotionModel_E);
    }


}

/** @brief MotionGeometry
  *
  * @todo: document this function
  */
MotionGeometry::MotionGeometry()
{
    MotionModel_F = cvCreateMat(3,3,CV_64FC1);
    MotionModel_H = cvCreateMat(3,3,CV_64FC1);
    MotionModel_E = cvCreateMat(3,3,CV_64FC1);

    motionError_F=0;//in pixels
    numOutlier_F=0;
    numInliers_F=0;
    motionError_H=0;//in pixels
    numOutlier_H=0;
    numInliers_H=0;
    filenameF="";
    filenameH="";
    filenameE="";
    valid=0;
}


double MotionGeometry::getMotionElement(int i,int j,MotionType mtype) const
{
    CvMat* MotionModel;
     if(mtype==FUNDAMENTAL)
    MotionModel=MotionModel_F;
    else if(mtype==HOMOGRAPHY)
    MotionModel=MotionModel_H;
    else if (mtype==ESSENTIAL)
    MotionModel=MotionModel_E;
    else
    printf("incorect motion requested!\n");


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


int MotionGeometry::findHMatrix(const vector<HRPointFeatures>&  hr1vec,const vector<HRPointFeatures>& hr2vec,  vector<matchIndex>& indices )
{


    if (indices.size()==0 || hr1vec.size()==0 || hr2vec.size()==0)
    {
        cout<<"matches dont exist, returning nothing"<<endl;
        return 0;

    }

    int numPoints = indices.size();


    if (numPoints<9)
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

    for (int i=0; i<numPoints; i++)
    {
        points1->data.fl[i*2]=hr1vec[indices[i].imindex1]->location.x;
        points1->data.fl[i*2+1]=hr1vec[indices[i].imindex1]->location.y;

        points2->data.fl[i*2]=hr2vec[indices[i].imindex2]->location.x;
        points2->data.fl[i*2+1]=hr2vec[indices[i].imindex2]->location.y;
        status->data.ptr[i]=0;
    }



    int pointsRejected=0;
    double error=0;
    // int num = cvFindFundamentalMat(points1,points2,MotionModel_F,CV_FM_RANSAC,1.0,0.99,status);

    cvFindHomography(	points1,points2,MotionModel_H,0,	1.5,(CvMat*)NULL);

    int numinliers=0;
    error= computeReprojErrorH( points1,points2, MotionModel_H, err_array ,1.5,&numinliers);

    motionError_H=error;
    numOutlier_H=numPoints-numinliers;
    numInliers_H=numinliers;





    cvReleaseMat(&points1);
    cvReleaseMat(&points2);
    cvReleaseMat(&status);
    cvReleaseMat(&err_array);


//    return num;

}
/** @brief (one liner)
*
* (documentation goes here)
*/
int MotionGeometry::findFMatrix(const vector<HRPointFeatures>& hr1vec,const vector<HRPointFeatures>&  hr2vec,  vector<matchIndex>& indices )
{



    if (indices.size()==0 || hr1vec.size()==0 || hr2vec.size()==0)
    {
        cout<<"matches dont exist, returning nothing"<<endl;
        return 0;

    }

    int numPoints = indices.size();


    if (numPoints<9)
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

    for (int i=0; i<numPoints; i++)
    {
        points1->data.fl[i*2]=hr1vec[indices[i].imindex1]->location.x;
        points1->data.fl[i*2+1]=hr1vec[indices[i].imindex1]->location.y;

        points2->data.fl[i*2]=hr2vec[indices[i].imindex2]->location.x;
        points2->data.fl[i*2+1]=hr2vec[indices[i].imindex2]->location.y;
        status->data.ptr[i]=0;
    }



    int pointsRejected=0;
    double error=0;
    int num = cvFindFundamentalMat(points1,points2,MotionModel_F,CV_FM_RANSAC,1.0,0.99,status);
    error= computeReprojErrorF( points1,points2, MotionModel_F, err_array ,status);




    for (int i=(numPoints-1); i>=0; i--)
    {
        pointsRejected=pointsRejected+(1-status->data.ptr[i]);

        indices[i].motionError=err_array->data.fl[i];
        indices[i].inlier=status->data.ptr[i];

    }


    motionError_F=error;
    numOutlier_F=pointsRejected;
    numInliers_F=numPoints-numOutlier_F;





    cvReleaseMat(&points1);
    cvReleaseMat(&points2);
    cvReleaseMat(&status);
    cvReleaseMat(&err_array);


    return num;
}
int MotionGeometry::findMotionModels(const vector<HRPointFeatures>& hr1vec,const vector<HRPointFeatures>&  hr2vec,  vector<matchIndex>& indices ,MotionType mtype)
{


    if (mtype==FUNDAMENTAL)
    {
        return findFMatrix( hr1vec,hr2vec,  indices );

    }
    if (mtype==HOMOGRAPHY)
    {
        return findHMatrix( hr1vec,hr2vec,  indices );

    }
    else
    {
        cout<<"unknown motion model"<<endl;

        return -1;

    }




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

        if (status->data.ptr[i]==1)
            totalerr+=err[i];
        else
            numOutliers++;

        // cout<<i<<"\t\t"<<err[i]<<endl;
        if (err[i]<0) cout<<"whaaaaaaaaaaaaaaaaaaat negative error\n"<<endl;
    }
    totalerr/=((double)(count-numOutliers));
    // printf ("average error method 1 is %f with %d points\n",totalerr,count);

    return totalerr;
}

double MotionGeometry::computeReprojErrorH( const CvMat* m1, const CvMat* m2, const CvMat* model, CvMat* _err , double thresh,int* numinliers)
{
    int i, count = m1->cols;
    const float*  M = m1->data.fl;
    const float*  m = m2->data.fl;
    const double* H = model->data.db;
    float* err = _err->data.fl;
    float curerror=0;
    float totalerror=0;
    (*numinliers)=0;
    for ( i = 0; i < count; i++ )
    {
        double ww = 1./(H[6]*M[i*2] + H[7]*M[i*2+1] + 1.);
        double dx = (H[0]*M[i*2] + H[1]*M[i*2+1] + H[2])*ww - m[i*2];
        double dy = (H[3]*M[i*2] + H[4]*M[i*2+1] + H[5])*ww - m[i*2+1];
        curerror= (float)(dx*dx + dy*dy);
        if (curerror<thresh)
        {
            (*numinliers)++;
            totalerror+= curerror ;
        }

        err[i] =curerror;
    }
    totalerror=totalerror/ ((double)*numinliers);
    return totalerror;
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

        if (status->data.ptr[i]==1)//dont add outliers to error
            avgErr += err;
        else
            numOutliers++;
        //  printf("the type two error is %f\n",err);
    }
    avgErr/=((double)(N-numOutliers));

    // printf( "avg err = %f for %d points\n", avgErr ,N);

    return avgErr;





}

