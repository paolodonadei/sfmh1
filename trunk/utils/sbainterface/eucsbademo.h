#ifndef _SBA_DEMO_H_
#define _SBA_DEMO_H_
#include "readparams.h"
/* pointers to additional data, used for computed image projections and their jacobians */
struct globs_{
	double *rot0params; /* initial rotation parameters, combined with a local rotation parameterization */
	double *intrcalib; /* the 5 intrinsic calibration parameters in the order [fu, u0, v0, ar, skew],
                      * where ar is the aspect ratio fv/fu.
                      * Used only when calibration is fixed for all cameras;
                      * otherwise, it is null and the intrinsic parameters are
                      * included in the set of motion parameters for each camera
                      */
  int nccalib; /* number of calibration parameters that must be kept constant.
                * 0: all parameters are free
                * 1: skew is fixed to its initial value, all other parameters vary (i.e. fu, u0, v0, ar)
                * 2: skew and aspect ratio are fixed to their initial values, all other parameters vary (i.e. fu, u0, v0)
                * 3: meaningless
                * 4: skew, aspect ratio and principal point are fixed to their initial values, only the focal length varies (i.e. fu)
                * 5: all intrinsics are kept fixed to their initial values
                * >5: meaningless
                * Used only when calibration varies among cameras
                */

  int ncdist; /* number of distortion parameters in Bouguet's model that must be kept constant.
               * 0: all parameters are free
               * 1: 6th order radial distortion term (kc[4]) is fixed
               * 2: 6th order radial distortion and one of the tangential distortion terms (kc[3]) are fixed
               * 3: 6th order radial distortion and both tangential distortion terms (kc[3], kc[2]) are fixed [i.e., only 2nd & 4th order radial dist.]
               * 4: 4th & 6th order radial distortion terms and both tangential distortion ones are fixed [i.e., only 2nd order radial dist.]
               * 5: all distortion parameters are kept fixed to their initial values
               * >5: meaningless
               * Used only when calibration varies among cameras and distortion is to be estimated
               */
  int cnp, pnp, mnp; /* dimensions */

	double *ptparams; /* needed only when bundle adjusting for camera parameters only */
	double *camparams; /* needed only when bundle adjusting for structure parameters only */
} ;
/* unit quaternion from vector part */
#define _MK_QUAT_FRM_VEC(q, v){                                     \
  (q)[1]=(v)[0]; (q)[2]=(v)[1]; (q)[3]=(v)[2];                      \
  (q)[0]=sqrt(1.0 - (q)[1]*(q)[1] - (q)[2]*(q)[2]- (q)[3]*(q)[3]);  \
}


#define CLOCKS_PER_MSEC (CLOCKS_PER_SEC/1000.0)

#define MAXITER         100
#define MAXITER2        150


#define SBA_MAX_REPROJ_ERROR    4.0 // max motion only reprojection error

#define BA_NONE                 -1
#define BA_MOTSTRUCT            0
#define BA_MOT                  1
#define BA_STRUCT               2
#define BA_MOT_MOTSTRUCT        3


void quat2vec(double *inp, int nin, double *outp, int nout);
void vec2quat(double *inp, int nin, double *outp, int nout);
/* in eucsbademo.c */
 void sba_driver(char *camsfname, char *ptsfname, char *calibfname, int cnp, int pnp, int mnp,
                       void (*caminfilter)(double *pin, int nin, double *pout, int nout),
                       void (*camoutfilter)(double *pin, int nin, double *pout, int nout),
                       int cnfp, char *refcamsfname, char *refptsfname);


 void img_projRTS(int j, int i, double *aj, double *bi, double *xij, void *adata);
  void img_projRTS_jac(int j, int i, double *aj, double *bi, double *Aij, double *Bij, void *adata);
  void img_projRT(int j, int i, double *aj, double *xij, void *adata);
   void img_projRT_jac(int j, int i, double *aj, double *Aij, void *adata);
    void img_projS(int j, int i, double *bi, double *xij, void *adata);
	void img_projS_jac(int j, int i, double *bi, double *Bij, void *adata);
	void img_projsRTS_x(double *p, struct sba_crsm *idxij, int *rcidxs, int *rcsubs, double *hx, void *adata);
	void img_projsRTS_jac_x(double *p, struct sba_crsm *idxij, int *rcidxs, int *rcsubs, double *jac, void *adata);
	void img_projsRT_x(double *p, struct sba_crsm *idxij, int *rcidxs, int *rcsubs, double *hx, void *adata);
	void img_projsRT_jac_x(double *p, struct sba_crsm *idxij, int *rcidxs, int *rcsubs, double *jac, void *adata);
	 void img_projsS_x(double *p, struct sba_crsm *idxij, int *rcidxs, int *rcsubs, double *hx, void *adata);
	  void img_projsS_jac_x(double *p, struct sba_crsm *idxij, int *rcidxs, int *rcsubs, double *jac, void *adata);
	   void img_projKRTS(int j, int i, double *aj, double *bi, double *xij, void *adata);
	   void img_projKRTS_jac(int j, int i, double *aj, double *bi, double *Aij, double *Bij, void *adata);
	    void img_projKRT(int j, int i, double *aj, double *xij, void *adata);
		 void img_projKRT_jac(int j, int i, double *aj, double *Aij, void *adata);
		  void img_projKS(int j, int i, double *bi, double *xij, void *adata);
		   void img_projKS_jac(int j, int i, double *bi, double *Bij, void *adata);
		   void img_projsKRTS_x(double *p, struct sba_crsm *idxij, int *rcidxs, int *rcsubs, double *hx, void *adata);
		   void img_projsKRTS_jac_x(double *p, struct sba_crsm *idxij, int *rcidxs, int *rcsubs, double *jac, void *adata);
		   void img_projsKRT_x(double *p, struct sba_crsm *idxij, int *rcidxs, int *rcsubs, double *hx, void *adata);
		   void img_projsKRT_jac_x(double *p, struct sba_crsm *idxij, int *rcidxs, int *rcsubs, double *jac, void *adata);
		   void img_projsKS_x(double *p, struct sba_crsm *idxij, int *rcidxs, int *rcsubs, double *hx, void *adata);
		   void img_projsKS_jac_x(double *p, struct sba_crsm *idxij, int *rcidxs, int *rcsubs, double *jac, void *adata);
		   void img_projKDRTS(int j, int i, double *aj, double *bi, double *xij, void *adata);
		   void img_projKDRTS_jac(int j, int i, double *aj, double *bi, double *Aij, double *Bij, void *adata);
		   void img_projKDRT(int j, int i, double *aj, double *xij, void *adata);
		    void img_projKDRT_jac(int j, int i, double *aj, double *Aij, void *adata);
			void img_projKDS(int j, int i, double *bi, double *xij, void *adata);
			void img_projKDS_jac(int j, int i, double *bi, double *Bij, void *adata);
			void img_projsKDRTS_x(double *p, struct sba_crsm *idxij, int *rcidxs, int *rcsubs, double *hx, void *adata);
			 void img_projsKDRTS_jac_x(double *p, struct sba_crsm *idxij, int *rcidxs, int *rcsubs, double *jac, void *adata);
			  void img_projsKDRT_x(double *p, struct sba_crsm *idxij, int *rcidxs, int *rcsubs, double *hx, void *adata);
			  void img_projsKDRT_jac_x(double *p, struct sba_crsm *idxij, int *rcidxs, int *rcsubs, double *jac, void *adata);
			  void img_projsKDS_x(double *p, struct sba_crsm *idxij, int *rcidxs, int *rcsubs, double *hx, void *adata);
			  void img_projsKDS_jac_x(double *p, struct sba_crsm *idxij, int *rcidxs, int *rcsubs, double *jac, void *adata);

void quatMultFast(double q1[FULLQUATSZ], double q2[FULLQUATSZ], double p[FULLQUATSZ]);
void quatMult(double q1[FULLQUATSZ], double q2[FULLQUATSZ], double p[FULLQUATSZ]);
#endif /* _SBA_DEMO_H_ */
