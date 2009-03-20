#include "matching.hpp"
#include "sift.h"
#include "boost/filesystem.hpp"   // includes all needed Boost.Filesystem declarations
#include "boost/filesystem/operations.hpp"
#include "boost/filesystem/path.hpp"
#include "pgmutils.hpp"
#include "general.h"
#define DEBUGLVL 0
#define TEMPDIR "tempdir"
namespace fs = boost::filesystem;
using namespace std;


int matchTWOImagesNearestNeighbour( HRImage& im1, HRImage& im2,HRCorrespond2N& hr_correspond,bool outputimage)
{


    Keypoint k, match;

    int count = 0;

    Image result;

    if (outputimage)
    {
        Image tempim1=HRImagetoDLImage(im1);
        Image tempim2=HRImagetoDLImage(im2);
        result = CombineImagesVertically(tempim1, tempim2);
        freeImage(tempim1);
        freeImage(tempim2);
    }
    /* Match the keys in list keys1 to their best matches in keys2.
    */
    for (k= keys1; k != NULL; k = k->next)
    {
        match = CheckForMatch(k, keys2);

        if (outputimage)
        {
            if (match != NULL)
            {
                count++;
                if (outputimage)
                {
                    DrawLine(result, (int) k->row, (int) k->col,(int) (match->row + im1->rows), (int) match->col);
                }
            }
        }



    }

    /* Write result image to standard output. */

    fprintf(stderr,"Found %d matches.\n", count);






    if (outputimage)
    {
        fs::path p1( im1.filename, fs::native );
        fs::path p2( im2.filename, fs::native );

        string fname=fs::basename(p1)+string("_")+fs::basename(p2)+string(".pgm");
        WritePGM((char*)fname.c_str(),result);
        freeImage(result);
    }


}
//Keypoint CheckForMatch(Keypoint key, Keypoint klist)
//{
//    int dsq, distsq1 = 100000000, distsq2 = 100000000;
//    Keypoint k, minkey = NULL;
//
//    /* Find the two closest matches, and put their squared distances in
//       distsq1 and distsq2.
//    */
//    for (k = klist; k != NULL; k = k->next)
//    {
//        dsq = DistSquared(key, k);
//
//        if (dsq < distsq1)
//        {
//            distsq2 = distsq1;
//            distsq1 = dsq;
//            minkey = k;
//        }
//        else if (dsq < distsq2)
//        {
//            distsq2 = dsq;
//        }
//    }
//
//    /* Check whether closest distance is less than 0.6 of second. */
//    if (10 * 10 * distsq1 < 6 * 6 * distsq2)
//        return minkey;
//    else return NULL;
//}
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
    fs::path temp_path = fs::system_complete( fs::path( TEMPDIR, fs::native ) );

    if ( !fs::exists( temp_path ) )
    {
        create_directory( temp_path );
        if ( !fs::exists( temp_path ) )
        {
            cout<<"cant create directory, exiting"<<endl;
            return 0;
        }
    }


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
