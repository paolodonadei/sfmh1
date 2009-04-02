#include "matching.hpp"
#include "sift.h"
#include "boost/filesystem.hpp"   // includes all needed Boost.Filesystem declarations
#include "boost/filesystem/operations.hpp"
#include "boost/filesystem/path.hpp"
#include "pgmutils.hpp"
#include "general.h"
#include "HRprimitives.h"
#define DEBUGLVL 0
#define TEMPDIR "tempdir"

#define SIFT_MULT 0.6
#define PCA_MULT 0.6

#define SIFT_THRESH 90
#define PCA_THRESH 3000

#define SIFTPCA 1
namespace fs = boost::filesystem;
using namespace std;

CvScalar colors[3]={cvScalar(255,0,0), cvScalar(0,255,0), cvScalar(0,0,255)};

int matchTWOImagesNearestNeighbour( HRImage& im1, HRImage& im2,HRCorrespond2N& hr_correspond)
{
    double score;

    vector<HRPointFeatures>::iterator k;
    int index=-1;

    int count = 0;


    for (int i=0;i<im1.HR2DVector.size();i++)
    {

        CheckForMatch(im1.HR2DVector[i], im2.HR2DVector,index,score);

        if (index != -1)
        {
            count++;

            matchIndex indexTemp;
            indexTemp.imindex1=i;
            indexTemp.imindex2=index;
            indexTemp.score;
            hr_correspond.imIndices.push_back(indexTemp);

        }

    }

    /* Write result image to standard output. */

    fprintf(stderr,"Found %d matches.\n", count);



    return count;
}

int drawMatchesPair(HRImage& im1, HRImage& im2,HRCorrespond2N& hr_correspond)
{



    IplImage* imgTemp=concatImagesVertical(im1.cv_img,im2.cv_img);

    int x0,y0,x1,y1;

    for (int i=0;i<hr_correspond.imIndices.size();i++)
    {

        x0=im1.HR2DVector[hr_correspond.imIndices[i].imindex1]->location.x;
        y0=im1.HR2DVector[hr_correspond.imIndices[i].imindex1]->location.y;
        x1=im2.HR2DVector[hr_correspond.imIndices[i].imindex2]->location.x;
        y1=im2.HR2DVector[hr_correspond.imIndices[i].imindex2]->location.y+im1.height;

        cvLine(imgTemp, cvPoint(x0,y0),cvPoint(x1,y1), cvScalar(255,0,0), 1);

        //print correspondences 1 to 1
        if (1==0) printLine(im1, im2, cvPoint(x0,y0), cvPoint(x1,y1), i);
    }





    if ( checkTempPath()==false)
        return 0;

    string tslash="/";
    string fname=TEMPDIR+tslash+combineFnames(im1.filename,im2.filename,".png");

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
    for (int i=0;i<HR2Dfeatures.size();i++)
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

    string siftpcaname="";
    if (image.flag_valid==0)
    {
        cout<<"image not loaded\n"<<endl;
        return 0;
    }

    //check see if sift exists
    if ( !fs::exists( "utils/sift" ) )
    {
        cout << "sift for matching is unavailable\n";
        return 0;
    }


    fs::path p( image.filename, fs::native );
    image.pgmfilename="";
    image.siftkeyfilename=fs::basename(p)+".key";
    siftpcaname=fs::basename(p)+"_pca.key";
    image.pgmfilename=fs::basename(p)+".pgm";

//create the directory for the files
    if ( checkTempPath()==false)
        return 0;

    string tslash="/";
    image.pgmfilename=TEMPDIR+tslash+image.pgmfilename;
    image.siftkeyfilename=TEMPDIR+tslash+image.siftkeyfilename;
    siftpcaname=TEMPDIR+tslash+siftpcaname;

    cout<<"saving file: "<<image.pgmfilename<<endl;


    if (!write_pgm_image((char*)image.pgmfilename.c_str(), image.data, image.height,image.width, "Houman Rastgar",255))
    {

        cout<<"image "<<image.pgmfilename <<" not saved\n"<<endl;
        return 0;
    }
    fs::path p2( image.pgmfilename, fs::native );
    if ( !fs::exists( p2 ) )
    {
        cout << "file was not saved: "<<image.pgmfilename<<endl;
        exit(EXIT_FAILURE  );
    }


//////////////ok now run sift//////////////
    if (system(NULL)==0)
    {
        cout<<"command processor not available , no features found"<<endl;
        return 0;

    }
    string command_run=string("utils/sift ")+string("<")+image.pgmfilename+string("> ")+image.siftkeyfilename;



    if (DEBUGLVL>0) cout<<"Executing command ..."<<command_run<<endl;
    system (command_run.c_str());

    //if pca then reproject
    if (SIFTPCA)
    {
        string command_run=string("utils/recalckeys utils/gpcavects.txt ")+image.pgmfilename+string(" ")+image.siftkeyfilename+string(" ")+siftpcaname;
        if (DEBUGLVL>0) cout<<"Executing command ..."<<command_run<<endl;
        system (command_run.c_str());
        image.siftkeyfilename=siftpcaname;   //now i made it so that the feature point refers to the pca one, so form now on pca will be used
    }

//now read the key file into the feature list
    int numfeatures= readSIFTfile(image.HR2DVector,image.siftkeyfilename);
    cout<<"number of features found for : "<<image.filename<<" is equal to "<<numfeatures<<endl;
    return 1;

}

int drawImageTrackMatches(const vector< vector<int> >& tMatrix,const vector<HRImagePtr>& imCollection,string filname)
{
#define SINGLEMATCHPRINT 0

    int i, j, k;
    int x0,y0,x1,y1;
    IplImage* imgTemp=NULL;
    IplImage* imgTempcopy=NULL;
    IplImage* imgTemptempcopy=NULL;
    if ( checkTempPath()==false)
        return 0;

    int heightImage=0;

    if (imCollection.size()==0)
    {
        cout<<"no images exist in the database, quitting"<<endl;
        return 0;

    }
    heightImage=(*imCollection[0]).height;


    for (i=0;i<imCollection.size();i++)
    {

        IplImage* imgTemptemp=concatImagesVertical(imgTemp,(*imCollection[i]).cv_img);

        cvReleaseImage( &imgTemp );
        imgTemp=imgTemptemp;
    }

    if (SINGLEMATCHPRINT==1)
    {
        imgTempcopy=cvCloneImage(imgTemp);

    }

    int matchCountr=0;

    for (i=0;i<tMatrix.size(); i++)
    {
        if (SINGLEMATCHPRINT==1)
        {

            imgTemptempcopy=cvCloneImage(imgTempcopy);
        }
        matchCountr=0;
        for (j=1;j<tMatrix[i].size();j++)
        {

            if (tMatrix[i][j]!=-1 && tMatrix[i][j-1]!=-1)
            {
                matchCountr++;
                x0=(*imCollection[j-1]).HR2DVector[tMatrix[i][j-1]]->location.x;
                y0=(*imCollection[j-1]).HR2DVector[tMatrix[i][j-1]]->location.y+ ((j-1)*heightImage);
                x1=(*imCollection[j]).HR2DVector[tMatrix[i][j]]->location.x;
                y1=(*imCollection[j]).HR2DVector[tMatrix[i][j]]->location.y+ ((j)*heightImage);

                cvLine(imgTemp, cvPoint(x0,y0),cvPoint(x1,y1), colors[i%3], 1);

                //print correspondences 1 to 1
                if (SINGLEMATCHPRINT==1)
                {
                    cvLine(imgTemptempcopy, cvPoint(x0,y0),cvPoint(x1,y1), colors[j%3], 1);

                }
            }
        }
        if (SINGLEMATCHPRINT==1)
        {
            string tslash="/";
            string fname=TEMPDIR+tslash+string("track_i"+stringify(i)+"_d"+stringify(matchCountr)+".png");

            if (!cvSaveImage(fname.c_str(),imgTemptempcopy)) printf("Could not save: %s\n",fname.c_str());
            cvReleaseImage( &imgTemptempcopy );
        }

    }





    string tslash="/";
    string fname=TEMPDIR+tslash+filname;

    if (!cvSaveImage(fname.c_str(),imgTemp)) printf("Could not save: %s\n",fname.c_str());


    cvReleaseImage( &imgTemp );

    if (SINGLEMATCHPRINT==1)
    {
        cvReleaseImage( &imgTempcopy );


    }


    return 0;






}
