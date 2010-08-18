#include "hrmodel.h"
#include "glm.h"
#include <stdio.h>


#include <stdlib.h>
HRModel::HRModel(string fname)
{
    filename=fname;
    model=NULL;
}

HRModel::~HRModel()
{
    //dtor
}
void HRModel::openOBJ()
{


    model=glmReadOBJ(filename.c_str());
    if (!model)
    {
        printf("\nUsage: objviewV2 <-s> <obj filename>\n");
        exit(0);
    }

     glmUnitize(model);


    glmFacetNormals(model);

    glmVertexNormals(model, 90.0, GL_TRUE);


    printf("number of triangles is %d \n",model->groups->numtriangles);

}
void HRModel::drawOBJ()
{
       glPushMatrix();
 glmDraw(model, GLM_SMOOTH);
glPopMatrix();
}
