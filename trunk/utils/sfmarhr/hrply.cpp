#include "hrply.h"

#include <stream>
#include <string>
#include <vector>

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
int HRply::readPlyfile()
{


}
