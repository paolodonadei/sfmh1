#include "general.h"


void  draw_cross(CvPoint2D32f center, CvScalar color, int d,IplImage* img )
{
                cvLine( img, cvPoint( center.x - d, center.y - d ),
                             cvPoint( center.x + d, center.y + d ), color, 1, 0 );
                cvLine( img, cvPoint( center.x + d, center.y - d ),
                             cvPoint( center.x - d, center.y + d ), color, 1, 0 );
}


Image HRImagetoDLImage(HRImage& hr_im)
{
    Image image;
  image = CreateImage(hr_im.height, hr_im.width);
  for (int r = 0; r < hr_im.height; r++)
    for (int c = 0; c < hr_im.width; c++)
      image->pixels[r][c] = ((float) hr_im[r][c]) / 255.0;


return image;

}

