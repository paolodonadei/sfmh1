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
    points_normalized.clear();
    points.clear();
    mytriangles.clear();

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
void HRply::rendertriangles()
{
    double R,G,B;
    double x,y,z;
    glClear(GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT);
    int cursize=mytriangles.size();

    glBegin(GL_TRIANGLES);

    for(int i=0; i<cursize; i++)
    {


        R=mytriangles[i].col.R;
        G=mytriangles[i].col.G;
        B=mytriangles[i].col.B;

        R=R/255.0;
        G=G/255.0;
        B=B/255.0;

        glColor3f(R,G, B);

        glVertex3f(mytriangles[i].v1.x,mytriangles[i].v1.y,mytriangles[i].v1.z);
        glVertex3f(mytriangles[i].v2.x,mytriangles[i].v2.y,mytriangles[i].v2.z);
        glVertex3f(mytriangles[i].v3.x,mytriangles[i].v3.y,mytriangles[i].v3.z);


    }
    glEnd();



}
void HRply::rendernormals()
{
    double R,G,B;
    double x,y,z;

    int cursize=mytriangles.size();


    glLineWidth(2);
    glBegin(GL_LINES);


    for(int i=0; i<cursize; i++)
    {


        R=mytriangles[i].col.R;
        G=mytriangles[i].col.G;
        B=mytriangles[i].col.B;

        R=R/255.0;
        G=G/255.0;
        B=B/255.0;
        glColor3f(R,G, B);

        x=points_normalized[i].loc.x;
        y=points_normalized[i].loc.y;
        z=points_normalized[i].loc.z;



        glVertex3f(x,y,z);
        x=x+0.5*points_normalized[i].pnorm.xn;
        y=y+0.5*points_normalized[i].pnorm.yn;
        z=z+0.5*points_normalized[i].pnorm.zn;

        glVertex3f(x,y,z);


    }
    glEnd();






}
void HRply::rendertrianglesingle( int trinum)
{
    double R,G,B;
    double x,y,z;
    glClear(GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT);
    int cursize=1;

    int i=trinum;

    glBegin(GL_TRIANGLES);




    R=mytriangles[i].col.R;
    G=mytriangles[i].col.G;
    B=mytriangles[i].col.B;

    R=R/255.0;
    G=G/255.0;
    B=B/255.0;

    glColor3f(R,G, B);

    glVertex3f(mytriangles[i].v1.x,mytriangles[i].v1.y,mytriangles[i].v1.z);
    glVertex3f(mytriangles[i].v2.x,mytriangles[i].v2.y,mytriangles[i].v2.z);
    glVertex3f(mytriangles[i].v3.x,mytriangles[i].v3.y,mytriangles[i].v3.z);

    glEnd();

    glLineWidth(2);

    glBegin(GL_LINES);


    x=points_normalized[i].loc.x;
    y=points_normalized[i].loc.y;
    z=points_normalized[i].loc.z;

    R=mytriangles[i].col.R;
    G=mytriangles[i].col.G;
    B=mytriangles[i].col.B;

    R=R/255.0;
    G=G/255.0;
    B=B/255.0;

    glColor3f(G, B,R);

    glVertex3f(x,y,z);
    x=x+6*points_normalized[i].pnorm.xn;
    y=y+6*points_normalized[i].pnorm.yn;
    z=z+6*points_normalized[i].pnorm.zn;

    glVertex3f(x,y,z);


    glEnd();


}


void HRply::renderpoints()
{
    if(normalized!=1)
    {
        //printf("cant render since points are not normalized to fit viewing frustum\n");
        return ;
    }
//    int i;
    double x,y,z;
    double R,G,B;
    glPointSize(7);
    glClear(GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT);
    glBegin(GL_POINTS);


    for(int i=0; i<points.size(); i++)
    {
        x=points_normalized[i].loc.x;
        y=points_normalized[i].loc.y;
        z=points_normalized[i].loc.z;

        R=points_normalized[i].col.R/255.0f;
        G=points_normalized[i].col.G/255.0f;
        B=points_normalized[i].col.B/255.0f;

        glColor3f(R,G, B);

        glVertex3f(x,y,z);
    }
    glEnd();
}

void HRply::formTriangles(double size)
{
    int i;
    double x,y,z,xn,yn,zn;
    double R,G,B;

    mytriangles.clear();

    for(int i=0; i<points_normalized.size(); i++)
    {
        x=points_normalized[i].loc.x;
        y=points_normalized[i].loc.y;
        z=points_normalized[i].loc.z;

        R=(points_normalized[i].col.R);
        G=(points_normalized[i].col.G);
        B=(points_normalized[i].col.B);

        xn=points_normalized[i].pnorm.xn;
        yn=points_normalized[i].pnorm.yn;
        zn=points_normalized[i].pnorm.zn;

        mtriangles cur_triangle=formTriangle(points_normalized[i].pnorm,points_normalized[i].loc,size);



        cur_triangle.col.R=R;
        cur_triangle.col.G=G;
        cur_triangle.col.B=B;

        cur_triangle.pnorm.xn=xn ;
        cur_triangle.pnorm.xn=yn ;
        cur_triangle.pnorm.xn=zn;

//        cur_triangle.v1.x= ;
//        cur_triangle.v1.y= ;
//        cur_triangle.v1.z= ;
//
//
//        cur_triangle.v2.x= ;
//        cur_triangle.v2.y= ;
//        cur_triangle.v2.z= ;
//
//
//        cur_triangle.v3.x= ;
//        cur_triangle.v3.y= ;
//        cur_triangle.v3.z= ;

        mytriangles.push_back(cur_triangle);
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
    cout<<"new range "<<endl;
    cout<<"x: "<<xmin<<"\t" <<xmax<<endl;
    cout<<"y: "<<ymin<<"\t" <<ymax<<endl;
    cout<<"z: "<<zmin<<"\t" <<zmax<<endl;



}

mtriangles HRply::formTriangle(plynormal pn,pt3D loc,double radius)
{
    mtriangles mytri;
    double vnorm=0;

    pt3D v1;
    pt3D v2;
    pt3D v3;

    pt3D tempV;
    pt3D tempV2;

    v1=loc; //first point is the point itself

    double nxrightside=(pn.xn*loc.x)+(pn.yn*loc.y)+(pn.zn*loc.z);


    if(fabs(pn.xn)>0.00001)
    {
        tempV.y=1.0;
        tempV.z=1.0;

        tempV.x=(nxrightside-(pn.yn*tempV.y)-(pn.zn*tempV.z))/pn.xn;
    }
    else if(fabs(pn.yn)>0.00001)
    {
        tempV.x=1.0;
        tempV.z=1.0;

        tempV.y=(nxrightside-(pn.zn*tempV.z)-(pn.xn*tempV.x))/pn.yn;
    }
    else if(fabs(pn.zn)>0.00001)
    {
        tempV.x=1.0;
        tempV.y=1.0;

        tempV.z=(nxrightside-(pn.xn*tempV.x)-(pn.yn*tempV.y))/pn.zn;
    }
    else
    {
        cout<<"none of the normals are large enough, this cant be"<<endl;
        printf("nx = %f  ny= %f  nz= %f \n",pn.xn,pn.yn,pn.zn);
        exit(1);
    }

//    {
//        double dotp=((tempV.x-loc.x)*pn.xn)+((tempV.y-loc.y)*pn.yn)+((tempV.z-loc.z)*pn.zn);
//        printf("dot should be zero but it is %f \n",dotp);
//    }


    tempV2.x=tempV.x-v1.x;
    tempV2.y=tempV.y-v1.y;
    tempV2.z=tempV.z-v1.z;

    vnorm=sqrt( (tempV2.x*tempV2.x)+(tempV2.y*tempV2.y)+(tempV2.z*tempV2.z));

    tempV2.x/=vnorm;
    tempV2.y/=vnorm;
    tempV2.z/=vnorm;

    v2.x=v1.x+(radius*tempV2.x);
    v2.y=v1.y+(radius*tempV2.y);
    v2.z=v1.z+(radius*tempV2.z);

//////////
    pt3D tv1;
    pt3D tv2;

    tv1.x=v2.x-v1.x;
    tv1.y=v2.y-v1.y;
    tv1.z=v2.z-v1.z;

    tv2.x=pn.xn;
    tv2.y=pn.yn;
    tv2.z=pn.zn;



    tempV.x = (tv2.y * tv1.z) - (tv2.z * tv1.y);
    tempV.y = (tv2.z * tv1.x) - (tv2.x * tv1.z);
    tempV.z = (tv2.x * tv1.y) - (tv2.y * tv1.x);






    vnorm=sqrt( (tempV.x*tempV.x)+(tempV.y*tempV.y)+(tempV.z*tempV.z));

    tempV.x/=vnorm;
    tempV.y/=vnorm;
    tempV.z/=vnorm;

    v3.x=v1.x+(radius*tempV.x);
    v3.y=v1.y+(radius*tempV.y);
    v3.z=v1.z+(radius*tempV.z);

//////////
    pt3D centroid;

    centroid.x=(v1.x+v2.x+v3.x)/3.0;
    centroid.y=(v1.y+v2.y+v3.y)/3.0;
    centroid.z=(v1.z+v2.z+v3.z)/3.0;


    tempV2.x=v1.x-centroid.x;
    tempV2.y=v1.y-centroid.y;
    tempV2.z=v1.z-centroid.z;


    v1.x+= tempV2.x;
    v1.y+= tempV2.y;
    v1.z+= tempV2.z;


    v2.x+= tempV2.x;
    v2.y+= tempV2.y;
    v2.z+= tempV2.z;




    v3.x+= tempV2.x;
    v3.y+= tempV2.y;
    v3.z+= tempV2.z;

    mytri.v1=v1;
    mytri.v2=v2;
    mytri.v3=v3;

    return mytri;
}

 void  HRply::draw_axis( float scale )
{
 // glPushMatrix();
 // glDisable(GL_LIGHTING);
 // glDisable(GL_TEXTURE_2D);

   glLineWidth(scale);
  glBegin( GL_LINES );

  glColor3f( 1.0, 0.0, 0.0 );
  glVertex3f( .8f, 0.05f, 0.0 );  glVertex3f( 1.0, 0.25f, 0.0 ); // Letter X
  glVertex3f( 0.8f, .25f, 0.0 );  glVertex3f( 1.0, 0.05f, 0.0 );
  glVertex3f( 0.0, 0.0, 0.0 );  glVertex3f( 1.0, 0.0, 0.0 );     // X axis

  glColor3f( 0.0, 1.0, 0.0 );
  glVertex3f( 0.0, 0.0, 0.0 );  glVertex3f( 0.0, 1.0, 0.0 );	 // Y axis

  glColor3f( 0.0, 0.0, 1.0 );
  glVertex3f( 0.0, 0.0, 0.0 );  glVertex3f( 0.0, 0.0, 1.0 );	 // Z axis
  glEnd();
 // glEnable(GL_LIGHTING);
 // glEnable(GL_TEXTURE_2D);
 // glPopMatrix();

}
