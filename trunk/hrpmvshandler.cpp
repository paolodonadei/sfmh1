#include <iostream>
#include "boost/filesystem.hpp"   // includes all needed Boost.Filesystem declarations
#include "boost/filesystem/operations.hpp"
#include "boost/filesystem/path.hpp"

#include "hrpmvshandler.h"

#if defined(_WIN32) || defined(WIN32) || defined(__CYGWIN__) || defined(__MINGW32__) || defined(__BORLANDC__)
#define OS_WIN
#endif

using namespace std;

const string linuxExecName="PMVS2.exe";
const string windowsExecName="pmvs2";
const string modelsFolder="models";
const string imageFolder="visualize";
const string projectionFolder="txt";
const string optionFilename="option.txt";

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

}
void HRpmvshandler::copyImagesToPMVSFolder()
{

}
void HRpmvshandler::copyProjectionsToPMVSFolder()
{

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
