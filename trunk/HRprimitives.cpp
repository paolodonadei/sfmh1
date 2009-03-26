#include "HRprimitives.h"
#include <vector>

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


friend ostream &operator<<(ostream &stream, HRCorrespond2N ob)
{

    double x1,x2,y1,y2;


    if (hr1ptr=NULL || hr2ptr==NULL)
    {
        stream<<"images have not been initialized"<<endl;
        return stream;
    }

    stream << hr1ptr->filename <<"#   ( index="<<indexIm1 <<") "<< "                 "<<hr2ptr->filename <<" ( index="<<indexIm2 <<") "<<endl;


    stream << "#match_num"<<"\t"<<setw(11)<<"X"<<"("<<indexIm1 <<")"<<"\t"<<setw(11)<<"Y"<<"("<<indexIm1 <<")"<<"\t"<<setw(11)<<"X"<<"("<<indexIm2 <<")"<<"\t"<<setw(11)<<"Y"<<"("<<indexIm2 <<")"<<endl;


    for (i=0;i<imIndices.size();i++)
    {
        x1=hr1ptr->HR2DVector[imIndices[i].imindex1]->location.x;
        x2=hr1ptr->HR2DVector[imIndices[i].imindex1]->location.y;
        y1=hr2ptr->HR2DVector[imIndices[i].imindex2]->location.x;
        y2=hr2ptr->HR2DVector[imIndices[i].imindex2]->location.y;


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
