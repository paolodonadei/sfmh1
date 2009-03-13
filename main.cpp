#include <stdio.h>
#include <stdlib.h>
#include "argproc.h"
#include "HRImage.hpp"

int main(int argc, char *argv[])
{

    Parameters my_parms;

    //set default parameters
    set_default_param(&my_parms);

    //Process command-line arguments
    argument_processing(argc, argv, &my_parms);

   printf("directory is %s \n",my_parms.dirName.c_str());

{
    HRImageSet imSet(my_parms.dirName);

}
   return 0;
}






