#include "HRprimitives.h"
#include <vector>

using namespace std;

/** @brief HRCorrespond2N
  *
  * @todo: document this function
  */
 HRCorrespond2N::HRCorrespond2N(int index1,int index2, string f1name, string f2name)
{
 indexIm1=index1;
  indexIm2=index2;
  im1Name=f1name;
  im2Name=f2name;
}

/** @brief ~HRCorrespond2N
  *
  * @todo: document this function
  */
 HRCorrespond2N::~HRCorrespond2N()
{

}

/** @brief HRCorrespond2N
  *
  * @todo: document this function
  */
 HRCorrespond2N::HRCorrespond2N()
{
 indexIm1=0;
  indexIm2=0;
  im1Name="";
  im2Name="";
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
