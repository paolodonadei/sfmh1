#include <stdio.h>
#include <stdlib.h>
#include "argproc.h"
#include "HRImage.hpp"
#include "HRstructure.h"



int main(int argc, char *argv[])
{

    Parameters my_parms;

    //set default parameters
    set_default_param(&my_parms);

    //Process command-line arguments
    argument_processing(argc, argv, &my_parms);

    printf("directory is %s \n",my_parms.dirName.c_str());

    {
        HRImageSet imSet(my_parms.dirName,my_parms.tempdirname);
        imSet.featureDetectSift();
        //   imSet.showOneByOneFeature();

        imSet.exhaustiveSIFTMatching();
        //   imSet.showOneByOneFeatureMotions();

        imSet.multipleViewEstimate();
        imSet.drawallMatches();
        imSet.createFeatureTrackMatrix();
        imSet.SelfCalibrate();
        imSet.findEssentialMatrices();
        imSet.writeMotions();

        HRStructure mystruct(&imSet,imSet.outdirStemName);
        mystruct.run();
        mystruct.printSBAstyleData();
        mystruct.sba_driver_interface();
    }
    return 0;
}






