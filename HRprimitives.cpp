#include "HRprimitives.h"
#include <vector>
#include <iostream>
#include <fstream>
#include <iomanip>

using namespace std;
/** @brief HRCorrespond2N
  *
  * @todo: document this function
  */
HRCorrespond2N::HRCorrespond2N()
{
    indexIm1=0;
    indexIm2=0;
    hr1ptr=NULL;
    hr2ptr=NULL;
}


/** @brief ~HRCorrespond2N
  *
  * @todo: document this function
  */
HRCorrespond2N::~HRCorrespond2N()
{

}


ostream &operator<<(ostream &stream, HRCorrespond2N ob)
{

    double x1,x2,y1,y2;


    if (ob.hr1ptr==NULL || ob.hr2ptr==NULL)
    {
        stream<<"images have not been initialized"<<endl;
        return stream;
    }

    stream <<"#"<< ob.hr1ptr->filename <<"   ( index="<<ob.indexIm1 <<") "<< "                 "<<ob.hr2ptr->filename <<" ( index="<<ob.indexIm2 <<") "<<endl;


    stream << "#match_num"<<setw(11)<<"X"<<"("<<ob.indexIm1 <<")"<<"\t"<<setw(11)<<"Y"<<"("<<ob.indexIm1 <<")"<<"\t"<<setw(11)<<"X"<<"("<<ob.indexIm2 <<")"<<"\t"<<setw(11)<<"Y"<<"("<<ob.indexIm2 <<")"<<endl;


    for (int i=0;i<ob.imIndices.size();i++)
    {
        x1=ob.hr1ptr->HR2DVector[ob.imIndices[i].imindex1]->location.x;
        x2=ob.hr1ptr->HR2DVector[ob.imIndices[i].imindex1]->location.y;
        y1=ob.hr2ptr->HR2DVector[ob.imIndices[i].imindex2]->location.x;
        y2=ob.hr2ptr->HR2DVector[ob.imIndices[i].imindex2]->location.y;


        stream <<i<<"\t\t"<<setw(10)<<fixed<<setprecision(5)<<x1 <<"\t"<<setw(11)<<y1<<"\t"<<setw(11)<<x2<<"\t" <<setw(11)<<y2 <<"\t"<<endl;

    }




    return stream;

}


HRFeature::HRFeature()
{
    valid=false;
    pvi=0;//never to be used
}
HRFeature::HRFeature(int x,int y)
{
    location.x=x;
    location.y=y;
    valid=true;
    pvi=0.5;//equally likely as any other feature
    scale= ori=0;           // Scale and orientation (range [-PI,PI]) in case we are using sift//


}

HRFeature::~HRFeature()
{
}
