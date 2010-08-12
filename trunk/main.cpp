#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include "argproc.h"
#include "HRImage.hpp"
#include "HRstructure.h"
#include "hrpmvshandler.h"


int main(int argc, char *argv[])
{
    time_t start,end;

    double dif;

    time (&start);

    Parameters my_parms;

    //set default parameters
    set_default_param(&my_parms);

    //Process command-line arguments
    argument_processing(argc, argv, &my_parms);

    printf("directory is %s \n",my_parms.dirName.c_str());

    {

        HRImageSet imSet(my_parms.dirName,my_parms.tempdirname);

        printf("##### FEATURE DETECTION\n");
        imSet.featureDetectSift();
        //imSet.showOneByOneFeature();

        printf("##### FEATURE MATCHING\n");
        imSet.exhaustiveSIFTMatching();


        printf("##### multiple view geometry \n");
        imSet.multipleViewEstimate();

       // printf("##### draw matches \n");
      //  imSet.drawallMatches();

        printf("##### create feature tracks \n");
        imSet.createFeatureTrackMatrix();

        printf("##### self calibrate \n");
        imSet.SelfCalibrate();

        printf("##### find essential matrices \n");
        imSet.findEssentialMatrices();


        printf("#####  write motion \n");
        imSet.writeMotions();
        //    imSet.showOneByOneFeatureMotions();

        HRStructure mystruct(&imSet,imSet.outdirStemName);
        printf("#####  run SFM\n");
        mystruct.run();

        printf("#####  write undisroted image features\n");
        imSet.writeDistANDundistFeats();
//PMVS stuff for visualization
        printf("#####  PMVS files\n");
        HRpmvshandler pmvsHandler(imSet.temporaryDir,imSet.dirName ,&imSet);
        pmvsHandler.runPMVS();
    }



    time (&end);
    dif = difftime (end,start);
    printf ("It took you %.2lf seconds to carry out the SFM \n", dif );





    return 0;
}






