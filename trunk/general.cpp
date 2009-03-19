#include "general.h"


void  draw_cross(CvPoint2D32f center, CvScalar color, int d,IplImage* img )
{
                cvLine( img, cvPoint( center.x - d, center.y - d ),
                             cvPoint( center.x + d, center.y + d ), color, 1, 0 );
                cvLine( img, cvPoint( center.x + d, center.y - d ),
                             cvPoint( center.x - d, center.y + d ), color, 1, 0 );
}
