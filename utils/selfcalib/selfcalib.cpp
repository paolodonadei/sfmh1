////////////////////////////////////////////////////////////////////////
//
// hello-world.cpp
//
// This is a simple, introductory OpenCV program. The program reads an
// image from a file, inverts it, and displays the result.
//
////////////////////////////////////////////////////////////////////////
#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include <cv.h>
#include <highgui.h>
#include <vector>
#include <iostream>
#include <fstream>
#include <iomanip>
#include <sstream>
#include "boost/filesystem.hpp"   // includes all needed Boost.Filesystem declarations
#include "boost/filesystem/operations.hpp"
#include "boost/filesystem/path.hpp"


#include "general.h"
#include "HRprimitives.h"
#include "visiongen.h"
#include "focallength.h"



using namespace std;
namespace fs = boost::filesystem;





MotionType MotionType;



int main(int argc, char *argv[])
{
    string fil_name1;




    int numFrames=0;

    double width,height;



    vector<CvMat* > intrinMatrix;
    vector< vector<CvMat*> > funMatrix;
    vector< vector<string> > funMatrixNames;



    int mode=0;

    int i,j,k;

    if (argc<5)
    {
        printf("Usage: main [<fundamental matrix>|<index fundmatrices>] [width] [height] [mode]\n");
        printf("<fundamental matrix> name of the file containing the fundamental matrix OR\n");
        printf("<index fundmatrices> an index pointing to the set of fundamental matrices, see [mode] option\n");
        printf("[width] [height] size of the images\n");
        printf("[mode] 1 would indicate first option is a single fundamental matrix, 2 means its an index\n");
        exit(0);
    }







    fil_name1= argv[1] ;
    width=  atof(argv[2] );
    height=  atof(argv[3] );
    mode= atoi(argv[4] );

    if(mode==1)
    {
        numFrames=2;

    }
    else
    {
        int numFs=0;
        char str[2000];
        //finding number of frames
        fstream file_cm(fil_name1.c_str(),ios::in);
        while (!file_cm.eof())
        {
            file_cm.getline(str,2000);
            numFs++;
        }
        numFs--;
        file_cm.clear();
        file_cm.seekg(0, ios::beg);
        file_cm.close();

        numFrames=(int)((1.0+sqrt(1.0+(8*numFs)))/2.0);

    }



//allocating martices
    intrinMatrix.resize(numFrames);
    funMatrix.resize(numFrames);
    funMatrixNames.resize(numFrames);
    for (int i = 0; i < numFrames; ++i)
    {
        funMatrix[i].resize(numFrames);
        funMatrixNames[i].resize(numFrames);
    }

    for (int i = 0; i < numFrames; ++i)
    {
        intrinMatrix[i]=cvCreateMat(3,3, CV_64F);
        for (int j = 0; j < numFrames; ++j)
        {
            funMatrix[i][j]=cvCreateMat(3,3, CV_64F);
        }
    }



    if(mode==1)
    {
        funMatrixNames[0][1]=fil_name1;

    }
    else
    {
        int i,j;
        fs::path p( fil_name1, fs::native );
        p.remove_leaf();
        string curFname;

        char str[2000];
        //finding number of frames
        fstream file_cm(fil_name1.c_str(),ios::in);
        while (!file_cm.eof())
        {
            file_cm.getline(str,2000);
            string s(str);
            string out;
            istringstream ss;
            ss.str(s);

            ss>>i;
            ss>>j;
            ss>>curFname;

            if(i>=numFrames || j>=numFrames)
            {
                printf("something went wrong in the findex files, make sure teh indeices of the frames start with 0, aborting\n");
                exit(1);
            }

            funMatrixNames[i][j]=(p/fs::path(curFname, fs::native )).file_string();

        }
        file_cm.clear();
        file_cm.seekg(0, ios::beg);
        file_cm.close();

    }

    for(i=0; i<numFrames; i++)
    {

        for(j=0; j<numFrames; j++)
        {

            if(funMatrixNames[i][j]!="")
            {

                readCvMatFfromfile(&funMatrix[i][j],funMatrixNames[i][j]);

            }
        }

    }


    for(i=0; i<numFrames; i++)
    {

        for(j=0; j<numFrames; j++)
        {

            if(funMatrixNames[i][j]=="" && i!=j && funMatrixNames[j][i]!="")
            {

                cvTranspose(funMatrix[j][i], funMatrix[i][j]);

            }
        }

    }


    HRSelfCalibtwoFrame(funMatrix, intrinMatrix, width, height, HARTLEY);


    cout<<" According to Hartley :"<<endl;

    for (int i = 0; i < numFrames; ++i)
    {
        writeCVMatrix(cout,intrinMatrix[i]);
    }



    HRSelfCalibtwoFrame(funMatrix, intrinMatrix, width, height, STRUM);

    cout<<" According to Sturm :"<<endl;

    for (int i = 0; i < numFrames; ++i)
    {
        writeCVMatrix(cout,intrinMatrix[i]);
    }






    for (int i = 0; i < numFrames; ++i)
    {
        cvReleaseMat(&intrinMatrix[i]);
        for (int j = 0; j < numFrames; ++j)
        {
            cvReleaseMat(&funMatrix[i][j]);
        }
    }


    return 0;
}


