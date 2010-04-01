#include <iostream>
#include <iomanip>
#include <fstream>
#include "boost/filesystem.hpp"   // includes all needed Boost.Filesystem declarations
#include "boost/filesystem/operations.hpp"
#include "boost/filesystem/path.hpp"


#include "matching.hpp"
#include "sift.h"
#include "pgmutils.hpp"
#include "general.h"
#include "HRprimitives.h"
#include "argproc.h"
#define DEBUGLVL 0

#if defined(_WIN32) || defined(WIN32) || defined(__CYGWIN__) || defined(__MINGW32__) || defined(__BORLANDC__)
#define OS_WIN
#endif

#define SIFT_MULT 0.6
#define PCA_MULT 0.6

#define SIFT_THRESH 90
#define PCA_THRESH 3000
#define RECREATEFILES 0

#define SIFTPCA 1
extern const char* TEMPDIR;

namespace fs = boost::filesystem;
using namespace std;



int matchTWOImagesNearestNeighbour( HRImage& im1, HRImage& im2,HRCorrespond2N& hr_correspond)
{
    double score;
    vector<HRPointFeatures>::iterator k;
    int index=-1;

    int count = 0;

    fs::path p1(fs::path( TEMPDIR, fs::native )/fs::path( combineFnames(hr_correspond.hr1ptr->filename,hr_correspond.hr2ptr->filename,"_indices.txt"), fs::native ));

    string fname=p1.file_string();

    if (!RECREATEFILES && fs::exists( p1 ) )
    {
        count= hr_correspond.readMatches(fname);
        return count;
    }




    for (int i=0; i<im1.HR2DVector.size(); i++)
    {

        CheckForMatch(im1.HR2DVector[i], im2.HR2DVector,index,score);

        if (index != -1)
        {
            count++;

            matchIndex indexTemp;
            indexTemp.imindex1=i;
            indexTemp.imindex2=index;
            indexTemp.score=score;

            hr_correspond.imIndices.push_back(indexTemp);

        }

    }

    /* Write result image to standard output. */

    fprintf(stderr,"read Found %d matches.\n", count);

    hr_correspond.writeIndices();

    return count;
}

int drawMatchesPair(HRImage& im1, HRImage& im2,HRCorrespond2N& hr_correspond)
{



    IplImage* imgTemp=concatImagesVertical(im1.cv_img,im2.cv_img);

    int x0,y0,x1,y1;
    int inlier;

    for (int i=0; i<hr_correspond.imIndices.size(); i++)
    {

        x0=im1.HR2DVector[hr_correspond.imIndices[i].imindex1]->location.x;
        y0=im1.HR2DVector[hr_correspond.imIndices[i].imindex1]->location.y;
        x1=im2.HR2DVector[hr_correspond.imIndices[i].imindex2]->location.x;
        y1=im2.HR2DVector[hr_correspond.imIndices[i].imindex2]->location.y+im1.height;

        inlier=hr_correspond.imIndices[i].inlier;

        cvLine(imgTemp, cvPoint(x0,y0),cvPoint(x1,y1), (inlier)?cvScalar(0,255,0):cvScalar(0,0,255), 1);

        //print correspondences 1 to 1
        if (1==0) printLine(im1, im2, cvPoint(x0,y0), cvPoint(x1,y1), i);
    }





    if ( checkTempPath()==false)
        return 0;

    fs::path tempath( TEMPDIR, fs::native );
    string fname=combineFnames(im1.filename,im2.filename,".png");
    tempath/=fname;
    fname=tempath.file_string();

    if (!cvSaveImage(fname.c_str(),imgTemp)) printf("Could not save: %s\n",fname.c_str());

    // HRImage tempimage(fname);
    // tempimage.displayImage();
    cvReleaseImage( &imgTemp );



    return 0;
}


int drawMatchesSingle(HRImage& im1, HRImage& im2,HRCorrespond2N& hr_correspond)
{

    IplImage* imgTemp = cvCreateImage(cvGetSize(im1.cv_img), IPL_DEPTH_8U, 3);//png doesnt support alpha channel in opencv
    cvCvtColor(im1.cv_img, imgTemp, CV_GRAY2BGR);



    int x0,y0,x1,y1;
    int inlier;

    for (int i=0; i<hr_correspond.imIndices.size(); i++)
    {

        x0=im1.HR2DVector[hr_correspond.imIndices[i].imindex1]->location.x;
        y0=im1.HR2DVector[hr_correspond.imIndices[i].imindex1]->location.y;
        x1=im2.HR2DVector[hr_correspond.imIndices[i].imindex2]->location.x;
        y1=im2.HR2DVector[hr_correspond.imIndices[i].imindex2]->location.y;

        inlier=hr_correspond.imIndices[i].inlier;

        cvLine(imgTemp, cvPoint(x0,y0),cvPoint(x1,y1), (inlier)?cvScalar(0,255,0):cvScalar(0,0,255), 1);

        //print correspondences 1 to 1
        if (1==0) printLine(im1, im2, cvPoint(x0,y0), cvPoint(x1,y1), i);
    }





    if ( checkTempPath()==false)
        return 0;

    fs::path tempath( TEMPDIR, fs::native );
    string fname=combineFnames(im1.filename,im2.filename,"single.png");
    tempath/=fname;
    fname=tempath.file_string();

    if (!cvSaveImage(fname.c_str(),imgTemp)) printf("Could not save: %s\n",fname.c_str());

    // HRImage tempimage(fname);
    // tempimage.displayImage();
    cvReleaseImage( &imgTemp );



    return 0;
}


int CheckForMatch(const HRPointFeatures& key, const vector<HRPointFeatures>& HR2Dfeatures, int& winKey, double& score)
{
    int dsq, distsq1 = 100000000, distsq2 = 100000000;

    winKey= -1;

    /* Find the two closest matches, and put their squared distances in
       distsq1 and distsq2.
    */
    for (int i=0; i<HR2Dfeatures.size(); i++)
    {
        dsq = Dist(key, HR2Dfeatures[i]);

        if (dsq < distsq1)
        {
            distsq2 = distsq1;
            distsq1 = dsq;
            winKey = i;
        }
        else if (dsq < distsq2)
        {
            distsq2 = dsq;
        }
    }

    if (SIFTPCA)  //siftpca claims to work better with an absolute threshold distance whereas the author of sift pca claims sift works better with relative distance so here i modified the program
    {

        /* PCA-SIFT KEYS */
        if (distsq1 < PCA_THRESH)
        {
            winKey=winKey;
            score=-distsq1;
            return 1;
        }
        else
        {
            winKey=-1;
            score=-distsq1;
            return -1;
        }

    }
    else
    {
        /* SIFT KEYS */
        if (distsq1 < SIFT_MULT * distsq2)
        {
            winKey=winKey;
            score=-(distsq1/distsq2);
            return 1;
        }
        else
        {
            winKey=-1;
            score=-(distsq1/distsq2);
            return -1;
        }

    }
//    /* Check whether closest distance is less than 0.6 of second. */
//    if (10 * 10 * distsq1 < 6 * 6 * distsq2)
//        return minkey;
//    else return -1;
}



int DistSquared(const HRPointFeatures& k1, const HRPointFeatures& k2)
{
    int i, dif, distsq = 0;
    vector<double>& pk1=k1->descriptor;
    vector<double>& pk2=k2->descriptor;

    if (pk1.size()!=pk2.size())
    {
        cout<<"the size of the two feature descriptors are not the same, quitting"<<endl;
        return -1;

    }

    for (i = 0; i < pk1.size(); i++)
    {
        dif = (int) pk1[i] - (int) pk2[i];
        distsq += dif * dif;
    }
    return distsq;
}

int Dist(const HRPointFeatures& k1, const HRPointFeatures& k2)
{
    int i, dif = 0;
    long int  distsq=0;

    vector<double>& pk1=k1->descriptor;
    vector<double>& pk2=k2->descriptor;

    if (pk1.size()!=pk2.size())
    {
        cout<<"the size of the two feature descriptors are not the same, quitting"<<endl;
        return -1;

    }

    for (i = 0; i < pk1.size(); i++)
    {
        dif = (int) pk1[i] - (int) pk2[i];
        distsq += dif * dif;
    }
    return (int) sqrt(distsq);
}

//
//
///* Return squared distance between two keypoint descriptors.
//*/
//int DistSquared(Keypoint k1, Keypoint k2)
//{
//    int i, dif, distsq = 0;
//    unsigned char *pk1, *pk2;
//
//    pk1 = k1->descrip;
//    pk2 = k2->descrip;
//
//    for (i = 0; i < 128; i++)
//    {
//        dif = (int) *pk1++ - (int) *pk2++;
//        distsq += dif * dif;
//    }
//    return distsq;
//}
//












//////////////////////////////////////////////////
int readSIFTfile(vector<HRPointFeatures>& siftVector,string filename)
{
    FILE *fp;

    fp = fopen (filename.c_str(), "r");
    if (! fp)
        FatalError("Could not open file: %s", filename.c_str());

//
    int i, j, num, len, val;
    i= j= num= len= val=0;


    if (fscanf(fp, "%d %d", &num, &len) != 2)
        FatalError("Invalid keypoint file beginning.");

    if (len != 128)
    {
        cout<<"reading PCA descriptors rather than plain sift"<<endl;
    }

    for (i = 0; i < num; i++)
    {
        HRPointFeatures newfeature( new HRFeature);

        /* Allocate memory for the keypoint. */
        newfeature->descriptor.reserve(len);

        if (fscanf(fp, "%f %f %f %f", &(newfeature->location.y), &(newfeature->location.x), &(newfeature->scale),&(newfeature->ori)) != 4)
            FatalError("Invalid keypoint file format.");

        for (j = 0; j < len; j++)
        {
            if (fscanf(fp, "%d", &val) != 1/* || val < 0 || val > 255 */)
                FatalError("Invalid keypoint file value.");

            newfeature->descriptor.push_back((double) val);
        }
        siftVector.push_back(newfeature);
    }


    fclose(fp);
    return num;
}

int findSIFTfeatures( HRImage& image)
{

#ifdef OS_WIN
    string siftexec="utils\\siftWin32.exe";

#else
    string siftexec="utils/sift";
#endif

    string siftpcaname="";
    if (image.flag_valid==0)
    {
        cout<<"image not loaded\n"<<endl;
        return 0;
    }

    //check see if sift exists
    if ( !fs::exists( siftexec ) )
    {
        cout << "sift for matching is unavailable\n";
        exit(EXIT_FAILURE);
    }


    fs::path p( image.filename, fs::native );
    image.pgmfilename="";
    image.siftkeyfilename=fs::basename(p)+".key";
    siftpcaname=fs::basename(p)+"_pca.key";
    image.pgmfilename=fs::basename(p)+".pgm";



//create the directory for the files
    if ( checkTempPath()==false)
        return 0;



    image.pgmfilename=(fs::path( TEMPDIR, fs::native )/fs::path( image.pgmfilename, fs::native )).file_string();
    image.siftkeyfilename=(fs::path( TEMPDIR, fs::native )/fs::path( image.siftkeyfilename, fs::native )).file_string();
    siftpcaname=(fs::path( TEMPDIR, fs::native )/fs::path( siftpcaname, fs::native )).file_string();

//ALWAYS USE file_string instead of string, because this way it will convert it to the native OS format

    cout<<"saving file: "<<image.pgmfilename<<endl;

    // printf("the step size is %d \n",image.step);


//this is necessary due to the step parameter
    unsigned char* dataptr=(unsigned char*) malloc(sizeof(unsigned char)*(image.height*image.width)  +1);

    // invert the image
    int z=0;

    for (int i=0; i<image.height; i++)
    {
        for (int j=0; j<image.width; j++)
        {
            //   printf(" i=%d and j=%d and z=%d\n",i,j,z);
            dataptr[z]= image.data[i*image.step+j];
            z++;
        }
    }



//here im saving the image using pgm utils but its not working
//    if (!write_pgm_image((char*)image.pgmfilename.c_str(), dataptr, image.height,image.width, "Houman Rastgar",255))
//    {
//
//        cout<<"image "<<image.pgmfilename <<" not saved\n"<<endl;
//        return 0;
//    }
    int zp[3];

    zp[0] = CV_IMWRITE_PXM_BINARY;
    zp[1] = 1;
    zp[2] = 0;


    if (!cvSaveImage((char*)image.pgmfilename.c_str(),image.cv_img,zp)) printf("Could not save: %s\n",(char*)image.pgmfilename.c_str());


    fs::path p2( image.pgmfilename, fs::native );
    if ( !fs::exists( p2 ) )
    {
        cout << "file was not saved: "<<image.pgmfilename<<endl;
        exit(EXIT_FAILURE  );
    }

    free(dataptr);
//////////////ok now run sift//////////////
    if (system(NULL)==0)
    {
        cout<<"command processor not available , no features found"<<endl;
        exit(EXIT_FAILURE);

    }






    string command_run=string(siftexec)+string(" <")+image.pgmfilename+string("> ")+image.siftkeyfilename;



    if (DEBUGLVL>0) cout<<"Executing command ..."<<command_run<<endl;

    fs::path p3( image.siftkeyfilename, fs::native );

    if (RECREATEFILES || !fs::exists( p3 ) )
    {
        system (command_run.c_str());
        cout<<" command is "<<command_run<<endl;
    }



    //if pca then reproject
    if (SIFTPCA)
    {
#ifdef OS_WIN
        string command_run=string("utils\\recalckeys.exe utils\\gpcavects.txt ")+image.pgmfilename+string(" ")+image.siftkeyfilename+string(" ")+siftpcaname+string("  > NULL");
#else

        string command_run=string("utils/recalckeys utils/gpcavects.txt ")+image.pgmfilename+string(" ")+image.siftkeyfilename+string(" ")+siftpcaname+string("  > /dev/null");
#endif






        if (DEBUGLVL>0) cout<<"Executing command ..."<<command_run<<endl;

        fs::path p4( siftpcaname, fs::native );
///zzzz remove this is for debig


        cout<<" command is "<<command_run<<endl;


        if (RECREATEFILES || !fs::exists( p4 ) )
            system (command_run.c_str());


        image.siftkeyfilename=siftpcaname;   //now i made it so that the feature point refers to the pca one, so form now on pca will be used

    }

//now read the key file into the feature list
    int numfeatures= readSIFTfile(image.HR2DVector,image.siftkeyfilename);
    cout<<"number of features found for : "<<image.filename<<" is equal to "<<numfeatures<<endl;
    return 1;

}












