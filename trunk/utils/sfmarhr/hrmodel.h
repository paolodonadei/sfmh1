#ifndef HRMODEL_H
#define HRMODEL_H

#include <iostream>
#include <string>
#include <vector>
#include "glm.h"
using namespace std;

class HRModel
{
    public:
        /** Default constructor */
        HRModel(string fname);
        /** Default destructor */
        virtual ~HRModel();
        string filename;
        void openOBJ();
        GLMmodel * model;
        void drawOBJ();

    protected:
    private:
};

#endif // HRMODEL_H
