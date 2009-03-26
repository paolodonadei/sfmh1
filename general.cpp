#include "general.h"
#include "boost/filesystem.hpp"   // includes all needed Boost.Filesystem declarations
#include "boost/filesystem/operations.hpp"
#include "boost/filesystem/path.hpp"

#define DEBUGLVL 0
#define TEMPDIR "tempdir"
namespace fs = boost::filesystem;
using namespace std;

void  draw_cross(CvPoint2D32f center, CvScalar color, int d,IplImage* img )
{
    cvLine( img, cvPoint( center.x - d, center.y - d ),
            cvPoint( center.x + d, center.y + d ), color, 1, 0 );
    cvLine( img, cvPoint( center.x + d, center.y - d ),
            cvPoint( center.x - d, center.y + d ), color, 1, 0 );
}


Image HRImagetoDLImage(HRImage& hr_im)
{
    Image image;
    image = CreateImage(hr_im.height, hr_im.width);
    for (int r = 0; r < hr_im.height; r++)
        for (int c = 0; c < hr_im.width; c++)
            image->pixels[r][c] = ((float) hr_im[r][c]) / 255.0;


    return image;

}

bool checkTempPath()
{
    fs::path temp_path = fs::system_complete( fs::path( TEMPDIR, fs::native ) );

    if ( !fs::exists( temp_path ) )
    {
        create_directory( temp_path );
        if ( !fs::exists( temp_path ) )
        {
            cout<<"cant create directory, exiting"<<endl;
            return false;
        }
    }
    return true;
}

string combineFnames(const string& f1,const string& f2,string extension)
{

        fs::path p1( f1, fs::native );
        fs::path p2( f2, fs::native );

        string fname=fs::basename(p1)+string("_")+fs::basename(p2)+extension;
      return fname;




}
