#ifndef ARGPROC_H
#define ARGPROC_H

#include <string>
#include <iostream>



using namespace std;


enum { SIFT=1, HARRIS };
enum { SIFTCLOSEST=1, SIFTPCA};



typedef struct
{
string dirName;
int matchType;
int featureType;
char* tempdirname;
} Parameters;

void set_default_param(Parameters *params);
void argument_processing(int argc, char *argv[], Parameters *params);
void UsageShow();

#endif
