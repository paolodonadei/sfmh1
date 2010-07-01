#include "hrply.h"

#include <iostream>
#include <string>
#include <vector>
#include <fstream>
#include <stdio.h>
#include <stdlib.h>
#include <iomanip>
#include <sstream>
using namespace std;


HRply::HRply(string fname)
{
    filename=fname;
    readPlyfile();
}

HRply::~HRply()
{
    //dtor
}

void HRply::printPlyPts(ostream &stream)
{
    for(int i=0; i<points.size(); i++)
    {
        stream<<i<<"\t"<<points[i].loc.x<<"\t"<<points[i].loc.y<<"\t"<<points[i].loc.z<<"\t"<<points[i].pnorm.xn<<"\t"<<points[i].pnorm.yn<<"\t"<<points[i].pnorm.zn<<"\t"<<points[i].col.R<<"\t"<<points[i].col.G<<"\t"<<points[i].col.B<<endl;
    }


}


int HRply::readPlyfile()
{
    char str[2000];
    fstream fp_in;
    fp_in.open(filename.c_str(), ios::in);
    string endheader="end_header";
    size_t found;


    if (!fp_in.is_open())
    {
        cout << "File " <<  filename << " does not exist" << endl;
        exit(1);
    }

//reading header
    while (!fp_in.eof())
    {
        fp_in.getline(str,2000);
        string strstr(str);
        found=strstr.find(endheader);
        if (found!=string::npos)
        {
            cout<<strstr<<endl;
            break;
        }

    }
//reading data
    numPts=0;
    while (!fp_in.eof())
    {
        fp_in.getline(str,2000);
        string s(str);
        istringstream ss1;
        istringstream ss2;
        ss1.str(s);
        ss2.str(s);
        string out;

        int numelements=0;
        while ( ss1 >> out )
            numelements++;

        if(numelements!=9)
        {

            break;
        }


        plyPt curPt;
        ss2>>curPt.loc.x;
        ss2>>curPt.loc.y;
        ss2>>curPt.loc.z;

        ss2>>curPt.pnorm.xn;
        ss2>>curPt.pnorm.yn;
        ss2>>curPt.pnorm.zn;

        ss2>>curPt.col.R;
        ss2>>curPt.col.G;
        ss2>>curPt.col.B;
        points.push_back(curPt);
        numPts++;
    }

    printf("read %d points \n",numPts);

}
