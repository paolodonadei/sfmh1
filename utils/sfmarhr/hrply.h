#ifndef HRPLY_H
#define HRPLY_H

#include <iostream>
#include <string>
#include <vector>

using namespace std;

struct plycolor
{
    int R;
    int G;
    int B;
};


struct plynormal
{
    double xn;
    double yn;
    double zn;
};

struct pt3D
{
    double x;
    double y;
    double z;
};

struct plyPt
{
    plycolor col;
    plynormal pnorm;
    pt3D loc;
};

struct mtriangles
{
    pt3D v1,v2,v3;
    plynormal pnorm;
    plycolor col;
};

class HRply
{
public:
 void draw_axis( float scale );
    vector<plyPt> points;
    vector<plyPt> points_normalized;
    vector<mtriangles> mytriangles;
    int numPts;
    int readPlyfile();
    void normalizePts(double min, double max);
    void renderpoints(int ptsize);
    void rendertriangles();
    void rendernormals();
    void printPlyPts(ostream &stream);
    string filename;
    int normalized;
    void formTriangles(double size);
    /** Default constructor */
    HRply(string fname);
    /** Default destructor */
    virtual ~HRply();
    mtriangles formTriangle(plynormal pn,pt3D loc,double radius);
    void rendertrianglesingle( int trinum);
protected:
private:
};

#endif // HRPLY_H
