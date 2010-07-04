#include "hrply.h"

#include <iostream>
#include <string>
#include <vector>
#include <fstream>
#include <stdio.h>
#include <stdlib.h>
#include <iomanip>
#include <sstream>
#include "wx/glcanvas.h"

#include "wx/wx.h"
#include <GL/glut.h>    // Header File For The GLUT Library
#include <GL/gl.h>	// Header File For The OpenGL32 Library
#include <GL/glu.h>	//

using namespace std;


HRply::HRply(string fname)
{
    filename=fname;
    readPlyfile();
    normalized=0;
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

void HRply::renderpoints()
{
    if(normalized!=1)
    {
        //printf("cant render since points are not normalized to fit viewing frustum\n");
        return ;
    }
//    int i;
    for(int i=0; i<points.size(); i++)
    {

    }

}
void HRply::normalizePts(double pmin, double pmax)
{
    int i;
    normalized=1;
    if(points.size()==0)
    {
        printf("not normalizing because there are no points available\n");
        return ;
    }

    double rangex;
    double rangey;
    double rangez;

    double xmin, xmax, ymin, ymax, zmin, zmax;


    double x,y,z;

    xmin= xmax=points[0].loc.x  ;
    ymin= ymax=points[0].loc.y  ;
    zmin= zmax=points[0].loc.z  ;

    for(i=0; i<points.size(); i++)
    {
        x=points[i].loc.x;
        y=points[i].loc.y;
        z=points[i].loc.z;

        if(x>xmax) xmax=x;
        if(x<xmin) xmin=x;

        if(y>ymax) ymax=y;
        if(y<ymin) ymin=y;

        if(z>zmax) zmax=z;
        if(z<zmin) zmin=z;
    }
        cout<<"x: "<<xmin<<"\t" <<xmax<<endl;
    cout<<"y: "<<ymin<<"\t" <<ymax<<endl;
    cout<<"z: "<<zmin<<"\t" <<zmax<<endl;


    rangex=xmax-xmin;
    rangey=ymax-ymin;
    rangez=zmax-zmin;


    double rangemultiple;


    if(rangex > rangey && rangex > rangez)
    {

        rangemultiple= (pmax-pmin)/rangex;
    }
    else if(rangey > rangex && rangey > rangez)
    {

        rangemultiple= (pmax-pmin)/rangey;
    }
    else
    {

        rangemultiple= (pmax-pmin)/rangez;
    }


    double xoffset= -((xmax-xmin)*(rangemultiple))/2.0;
    double yoffset=-( (ymax-ymin)*(rangemultiple))/2.0;
    double zoffset=-((zmax-zmin)*(rangemultiple))/2.0;
//////////
    points_normalized.resize(points.size());

    for(i=0; i<points.size(); i++)
    {
        points_normalized[i]=points[i];

        x=points[i].loc.x;
        y=points[i].loc.y;
        z=points[i].loc.z;

        x=((x-xmin)*rangemultiple)+xoffset;
        y=((y-ymin)*rangemultiple)+yoffset;
        z=((z-zmin)*rangemultiple)+zoffset;



        points_normalized[i].loc.x=x;
        points_normalized[i].loc.y=y;
        points_normalized[i].loc.z=z;

    }
    /////

    xmin= xmax=points_normalized[0].loc.x  ;
    ymin= ymax=points_normalized[0].loc.y  ;
    zmin= zmax=points_normalized[0].loc.z  ;

    for(i=0; i<points_normalized.size(); i++)
    {
        x=points_normalized[i].loc.x;
        y=points_normalized[i].loc.y;
        z=points_normalized[i].loc.z;

        if(x>xmax) xmax=x;
        if(x<xmin) xmin=x;

        if(y>ymax) ymax=y;
        if(y<ymin) ymin=y;

        if(z>zmax) zmax=z;
        if(z<zmin) zmin=z;
    }

    cout<<"x: "<<xmin<<"\t" <<xmax<<endl;
    cout<<"y: "<<ymin<<"\t" <<ymax<<endl;
    cout<<"z: "<<zmin<<"\t" <<zmax<<endl;



}
