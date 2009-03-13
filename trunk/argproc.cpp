#include "argproc.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "boost/filesystem.hpp"   // includes all needed Boost.Filesystem declarations
#include <iostream>



void set_default_param(Parameters *params)
{
    params->dirName="";
    params->matchType=SIFTCLOSEST;
    params->featureType=SIFT;
}


void argument_processing(int argc, char *argv[], Parameters *params)
{
    int CurrArg=1;

    if (argc==1 || strcmp(argv[CurrArg],"-help")==0)
    {
        UsageShow();
        exit(1);
    }


    while (CurrArg<argc)
    {
        if (argv[CurrArg][0] != '-')
        {
            UsageShow();
            exit(1);
        }

        switch (argv[CurrArg][1])
        {
        case 'D':
            params->dirName =  argv[++CurrArg] ;
            break;
        case 'M':
            params->matchType = atoi( argv[++CurrArg] );
            break;
        case 'F':
            params->featureType = atoi( argv[++CurrArg] );
            break;
        default :
            printf("invalid option %s\n", argv[CurrArg]);
        }
        CurrArg++;
    } //end of input of parameter

//////////here is the error checking for the arguments////
//1. directory is non empty
    if (params->dirName.length()==0)
    {
        std::cout << "directory not defined, use option -D\n";
        UsageShow();
        exit(EXIT_FAILURE);
    }
//2. directory exists

    if ( !boost::filesystem::exists( params->dirName ) )
    {
        std::cout << "directory: "<<params->dirName <<" does not exist, exiting\n";
        UsageShow();
        exit(EXIT_FAILURE);
    }
//3. if its not a directory
    if ( !boost::filesystem::is_directory( params->dirName ) )
    {
        std::cout << "this: "<<params->dirName <<" is not a directory\n";
        UsageShow();
        exit(EXIT_FAILURE);
    }


}





void UsageShow()
{
    printf("\n"
           "	\n"
           "	-----------------------------------------------------------------\n"
           "                Structure from Motion Code Rev 1 by Houman Rastgar\n"
           "                                     University of Ottawa \n"
           "                                  code started in March 2009 \n"
           "	-----------------------------------------------------------------\n"
           "\n"
           "Usage: sfmh1 [options]\n"
           "\n"
           "Options: (default values are in [])\n"
           "\n"
           "  -D <Directory where the image set resides>  [].\n"
           "  -F <feature matching technique> 1=SIFT, 2=PCA SIFT [1]. \n"
           "  -M <matching type> 1= exhaustive closes point [1] "
           "\r\n");


}


