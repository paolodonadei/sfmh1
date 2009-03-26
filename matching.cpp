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
namespace fs = boost::filesystem;
using namespace std;


int matchTWOImagesNearestNeighbour( HRImage& im1, HRImage& im2,HRCorrespond2N& hr_correspond,bool outputimage)
{


    vector<HRPointFeatures>::iterator k;
    int index=-1;

    int count = 0;

    IplImage* imgTemp=NULL;

    if (outputimage)
    {
        imgTemp=cvCreateImage(cvSize(im1.width,im1.height*2),IPL_DEPTH_8U,4);

        cvSetImageROI( imgTemp, cvRect( 0, 0, im1.width, im1.height ));
        cvSetImageROI( imgTemp, cvRect( 0, im1.height, im1.width, im1.height*2 ));

    }
    /* Match the keys in list keys1 to their best matches in keys2.
    */





    for (int i=0;i<im1.HR2DVector.size();i++)
    {

        index = CheckForMatch(im1.HR2DVector[i], im2.HR2DVector);

        if (index != -1)
        {
            count++;

            matchIndex indexTemp;
            indexTemp.imindex1=i;
            indexTemp.imindex2=index;
            hr_correspond.imIndices.push_back(indexTemp);

            if (outputimage)
            {
                cvLine(imgTemp, cvPoint((int)im1.HR2DVector[i]->location.x,(int)im1.HR2DVector[i]->location.y), \
                       cvPoint((int)im2.HR2DVector[index]->location.x,(int)im2.HR2DVector[index]->location.y+im1.height),   \
                       cvScalar(255,0,0), 1);
            }
        }




    }

    /* Write result image to standard output. */

    fprintf(stderr,"Found %d matches.\n", count);


    if (outputimage)
    {
        if ( checkTempPath()==false)
            return count;

        fs::path p1( im1.filename, fs::native );
        fs::path p2( im2.filename, fs::native );

        string fname=fs::basename(p1)+string("_")+fs::basename(p2)+string(".pgm");
        string tslash="/";
        fname=TEMPDIR+tslash+fname;

        if (!cvSaveImage(fname.c_str(),imgTemp)) printf("Could not save: %s\n",fname.c_str());

        // HRImage tempimage(fname);
        // tempimage.displayImage();
        cvReleaseImage( &imgTemp );

    }

    return count;
}



int CheckForMatch(const HRPointFeatures& key, const vector<HRPointFeatures>& HR2Dfeatures)
{
    int dsq, distsq1 = 100000000, distsq2 = 100000000;
    int minkey = -1;

    /* Find the two closest matches, and put their squared distances in
       distsq1 and distsq2.
    */
    for (int i=0;i<HR2Dfeatures.size();i++)
    {
        dsq = DistSquared(key, HR2Dfeatures[i]);

        if (dsq < distsq1)
        {
            distsq2 = distsq1;
            distsq1 = dsq;
            minkey = i;
        }
        else if (dsq < distsq2)
        {
            distsq2 = dsq;
        }
    }

    /* Check whether closest distance is less than 0.6 of second. */
    if (10 * 10 * distsq1 < 6 * 6 * distsq2)
        return minkey;
    else return -1;
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
        FatalError("Keypoint descriptor length invalid (should be 128).");

    for (i = 0; i < num; i++)
    {
        HRPointFeatures newfeature( new HRFeature);

        /* Allocate memory for the keypoint. */
        newfeature->descriptor.reserve(len);

        if (fscanf(fp, "%f %f %f %f", &(newfeature->location.y), &(newfeature->location.x), &(newfeature->scale),&(newfeature->ori)) != 4)
            FatalError("Invalid keypoint file format.");

        for (j = 0; j < len; j++)
        {
            if (fscanf(fp, "%d", &val) != 1 || val < 0 || val > 255)
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
    image.pgmfilename=fs::basename(p)+".pgm";

//create the directory for the files
    if ( checkTempPath()==false)
        return 0;

    string tslash="/";
    image.pgmfilename=TEMPDIR+tslash+image.pgmfilename;
    image.siftkeyfilename=TEMPDIR+tslash+image.siftkeyfilename;

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
//now read the key file into the feature list
    int numfeatures= readSIFTfile(image.HR2DVector,image.siftkeyfilename);
    cout<<"number of features found for : "<<image.filename<<" is equal to "<<numfeatures<<endl;
    return 1;

}
