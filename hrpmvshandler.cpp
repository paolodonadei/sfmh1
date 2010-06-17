#include <iostream>
#include "boost/filesystem.hpp"   // includes all needed Boost.Filesystem declarations
#include "boost/filesystem/operations.hpp"
#include "boost/filesystem/path.hpp"
#include "highgui.h"
#include "hrpmvshandler.h"

#if defined(_WIN32) || defined(WIN32) || defined(__CYGWIN__) || defined(__MINGW32__) || defined(__BORLANDC__)
#define OS_WIN
#endif

using namespace std;
namespace fs = boost::filesystem;
const string linuxExecName="PMVS2.exe";
const string windowsExecName="pmvs2";
const string modelsFolder="models";
const string imageFolder="visualize";
const string projectionFolder="txt";
const string optionFilename="option.txt";
const string pmvsprefix="pmvs";
const string projectionMatrHeader="CONTOUR";

HRpmvshandler::HRpmvshandler(string tempFolder, string origFolder , HRImageSet* pimSet)
{
    printf("input folder %s output folder %s \n", origFolder.c_str() ,tempFolder.c_str() );
    SettempFOLD(tempFolder );
    SetorigFOLD(origFolder);
    SetimSet(pimSet);
}

HRpmvshandler::~HRpmvshandler()
{
    //dtor
}
void HRpmvshandler::runPMVS()
{
    createPMVSFolder();
    copyImagesToPMVSFolder();
    copyProjectionsToPMVSFolder();

#ifdef OS_WIN
    runPMVSWindows();
#else
    runPMVSLinux();
#endif
    writeOptionFile();

    cleanUpPMVS();
}




void HRpmvshandler::createPMVSFolder()
{

    string pmname=pmvsprefix + mimSet->dirStemName;
    pmvsPath =( fs::system_complete( fs::path( tempFOLD, fs::native ) ) / pmname  );


    if ( fs::exists( pmvsPath ) )
    {
        remove_all( pmvsPath);

    }

    if ( !fs::exists( pmvsPath ) )
    {
        create_directory( pmvsPath );
        if ( !fs::exists( pmvsPath ) )
        {
            cout<<"cant create directory "<< pmvsPath <<", exiting"<<endl;
            return;
        }
    }

    //create txt , image , models directories
    imagepath =( pmvsPath / imageFolder  );

    create_directory( imagepath  );
    if ( !fs::exists( imagepath  ) )
    {
        cout<<"cant create directory "<< imagepath  <<", exiting"<<endl;
        return;
    }


    txtpath =( pmvsPath / projectionFolder  );

    create_directory( txtpath  );
    if ( !fs::exists( txtpath  ) )
    {
        cout<<"cant create directory "<< txtpath  <<", exiting"<<endl;
        return;
    }

    fs::path modelpath =( pmvsPath / modelsFolder  );

    create_directory( modelpath );
    if ( !fs::exists( modelpath  ) )
    {
        cout<<"cant create directory "<< modelpath  <<", exiting"<<endl;
        return;
    }

}

// string tempFOLD; //!< Member variable "tempFOLD"
//        string origFOLD; //!< Member variable "origFOLD"
//         HRImageSet* mimSet;


void HRpmvshandler::copyImagesToPMVSFolder()
{
    char buffer [300];



    for (int i=0; i<mimSet->imageCollection.size(); i++)
    {
        sprintf (buffer, "%08d", i);

        string fname=string(buffer)+ string(".jpg");
        fs::path srcpath=  mimSet->imageCollection[i]->filename;
        fs::path dstpath= imagepath/ fname;


//i do this in case its not jpg
        IplImage* imgTemp=cvLoadImage(srcpath.file_string().c_str());

//cout<<"loading image: "<<srcpath.file_string()<<endl;

        if (!cvSaveImage(dstpath.file_string().c_str(),imgTemp))
        {

            cout<<"image "<<dstpath.file_string()<<" not saved\n"<<endl;
            return ;
        }


    }

}
void HRpmvshandler::copyProjectionsToPMVSFolder()
{
    char buffer [300];

    for (int i=0; i<mimSet->imageCollection.size(); i++)
    {
        sprintf (buffer, "%08d", i);
        string fname=string(buffer)+ string(".txt");

        fs::path dstpath= txtpath/ fname;

        fstream file_cmin(dstpath.file_string().c_str(),ios::out);
        file_cmin<< projectionMatrHeader <<endl;
        writeCVMatrix(file_cmin,mimSet->imageCollection[i]->projectionMatrix );

        file_cmin.close();


    }
}

void HRpmvshandler::runPMVSLinux()
{

}
void HRpmvshandler::runPMVSWindows()
{

}
void HRpmvshandler::cleanUpPMVS()
{

}
void HRpmvshandler::writeOptionFile()
{

}
