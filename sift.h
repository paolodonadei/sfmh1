#ifndef SIFT_HPP_INCLUDED
#define SIFT_HPP_INCLUDED
/************************************************************************
Demo software: Invariant keypoint matching.
Author: David Lowe

defs.h:
This file contains the headers for a sample program to read images and
  keypoints, then perform simple keypoint matching.
*************************************************************************/

/* From the standard C libaray: */
#include <stdlib.h>
#include <math.h>
#include <assert.h>
#include <stdio.h>
#include <string.h>

/*------------------------------ Macros  ---------------------------------*/
#define ABS(x)    (((x) > 0) ? (x) : (-(x)))

/*---------------------------- Structures --------------------------------*/

/* Data structure for a float image.
*/
typedef struct ImageSt {
  int rows, cols;          /* Dimensions of image. */
  float **pixels;          /* 2D array of image pixels. */
  struct ImageSt *next;    /* Pointer to next image in sequence. */
} *Image;


/* Data structure for a keypoint.  Lists of keypoints are linked
   by the "next" field.
*/
typedef struct KeypointSt {
  float row, col;             /* Subpixel location of keypoint. */
  float scale, ori;           /* Scale and orientation (range [-PI,PI]) */
  unsigned char *descrip;     /* Vector of descriptor values */
  struct KeypointSt *next;    /* Pointer to next keypoint in list. */
} *Keypoint;



/*-------------------------- Function prototypes -------------------------*/
/* These are prototypes for the external functions that are shared
   between files.
*/

/* From util.c */
void FatalError(char *fmt, ...);
Image CreateImage(int rows, int cols);
Image ReadPGMFile(char *filename);
Image ReadPGM(FILE *fp);
void WritePGM(FILE *fp, Image image);
void DrawLine(Image image, int r1, int c1, int r2, int c2);
Keypoint ReadKeyFile(char *filename);
Keypoint ReadKeys(FILE *fp);
void FindMatches(Image im1, Keypoint keys1, Image im2, Keypoint keys2);
Keypoint CheckForMatch(Keypoint key, Keypoint klist);
int DistSquared(Keypoint k1, Keypoint k2);
Image CombineImagesVertically(Image im1, Image im2);
void WritePGM(char* filename, Image image);
void DeAllocMatrix(float** matrix);
void freeImage(Image im);
#endif //SIFT_HPP_INCLUDED
