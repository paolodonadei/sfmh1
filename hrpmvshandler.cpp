#include <iostream>
#include "boost/filesystem.hpp"   // includes all needed Boost.Filesystem declarations
#include "boost/filesystem/operations.hpp"
#include "boost/filesystem/path.hpp"
#include "highgui.h"
#include "hrpmvshandler.h"
#include <fstream>

#if defined(_WIN32) || defined(WIN32) || defined(__CYGWIN__) || defined(__MINGW32__) || defined(__BORLANDC__)
#define OS_WIN
#endif

using namespace std;
namespace fs = boost::filesystem;
const string linuxExecName="pmvs2";
const string windowsExecName="PMVS2.exe";
const string modelsFolder="models";
const string imageFolder="visualize";
const string projectionFolder="txt";
const string optionFilename="option.txt";
const string pmvsprefix="pmvs";
const string projectionMatrHeader="CONTOUR";

const string poissonSurfaceWin="PoissonRecon.32.exe";
const string poissonSurfaceLin="PoissonRecon.32";
const fs::path  execpathname="utils" ;


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

    writeOptionFile();

#ifdef OS_WIN
    runPMVSWindows();
#else
    runPMVSLinux();
#endif


    cleanUpPMVS();
}




void HRpmvshandler::createPMVSFolder()
{
    //printf("what the fuck0\n");
    string pmname=pmvsprefix + mimSet->dirStemName;
    //printf("the components are %s and %s\n",tempFOLD,pmname );
    pmvsPath =( fs::path( tempFOLD )  / fs::path(pmname ) );
   // printf("removing %s\n",pmvsPath.string() );
   // printf("what the fuck1\n");

    if ( fs::exists( pmvsPath ) )
    {
     //   printf("removing %s\n",pmvsPath.string() );
        remove_all( pmvsPath);
        assert( !exists( pmvsPath ) );
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

    modelpath =( pmvsPath / modelsFolder  );

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
    fs::path execfilename= execpathname/ linuxExecName;
    string optionfname=optionFilepath.leaf();


    string commandc=execfilename.file_string()+string(" ")+  pmvsPath.file_string()+string("/ ")+optionfname;
    if (system(NULL)==0)
    {
        cout<<"command processor not available , no features found"<<endl;
        return ;

    }
    cout<<" command is: "<<commandc<<endl;
    system (commandc.c_str());

// poisson 3D shape reconstruct
    fs::path poisonexecfilename= execpathname/ poissonSurfaceLin;
    string pSetFile=optionFilepath.leaf()+string(".pset");
    fs::path pSetfilepath= modelpath /pSetFile;
    fs::path outputPath= modelpath /string(string( optionFilepath.leaf() )+string("_poisson.ply"));
    string commandpoisson=poisonexecfilename.file_string()+string(" --in ")+ pSetfilepath.file_string()+string(" --out ")+ outputPath.file_string();
    cout<<"command for poisson blending is "<< commandpoisson <<endl;
    system (commandpoisson.c_str());
}
void HRpmvshandler::runPMVSWindows()
{
    fs::path execfilename= execpathname/ windowsExecName;
    string optionfname=optionFilepath.leaf();


    string commandc=execfilename.file_string()+string(" ")+  pmvsPath.file_string()+string("\\ ")+optionfname;
    if (system(NULL)==0)
    {
        cout<<"command processor not available , no features found"<<endl;
        return ;

    }
    cout<<" command is: "<<commandc<<endl;
    system (commandc.c_str());
// poisson 3D shape reconstruct
    fs::path poisonexecfilename= execpathname/ poissonSurfaceWin;
    string pSetFile=optionFilepath.leaf()+string(".pset");
    fs::path pSetfilepath= modelpath /pSetFile;
    fs::path outputPath= modelpath /string(string( optionFilepath.leaf() )+string("_poisson.ply"));
    string commandpoisson=poisonexecfilename.file_string()+string(" --in ")+ pSetfilepath.file_string()+string(" --out ")+ outputPath.file_string();
    cout<<"command for poisson blending is "<< commandpoisson <<endl;
    system (commandpoisson.c_str());
}
void HRpmvshandler::cleanUpPMVS()
{

    if ( fs::is_directory( imagepath ) )
    {

        fs::directory_iterator end_iter;
        for ( fs::directory_iterator dir_itr( imagepath);
                dir_itr != end_iter;
                ++dir_itr )
        {
            try
            {

                if ( fs::is_regular( dir_itr->status() ) )
                {
                    if (fs::extension(dir_itr->path())==".jpg")
                    {

                        remove(dir_itr->path());



                    }


                }

            }
            catch ( const exception & ex )
            {

                cout << dir_itr->path().leaf() << " " << ex.what() << endl;
            }

        }

    }
}
void HRpmvshandler::writeOptionFile()
{

    string fname=mimSet->dirStemName+ optionFilename;
    optionFilepath= pmvsPath/ fname;
    fstream file_cmin(optionFilepath.file_string().c_str(),ios::out);

    file_cmin<<"level 2"<<endl;
    file_cmin<<"csize 2"<<endl;
    file_cmin<<"threshold 0.7"<<endl;
    file_cmin<<"wsize 7"<<endl;
    file_cmin<<"minImageNum 2"<<endl;
    //zzz i should probably write a function to use the visdata as well
    file_cmin<<"useVisData 0"<<endl;
    file_cmin<<"sequence -1"<<endl;
    ///zzz what should the last number be?
    file_cmin<<"timages -1 0 "<<mimSet->numImages<<endl;
    file_cmin<<"oimages 0"<<endl;


    file_cmin.close();
}
