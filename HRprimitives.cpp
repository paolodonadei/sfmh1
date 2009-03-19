#include "HRprimitives.h"
#include <vector>

using namespace std;



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
