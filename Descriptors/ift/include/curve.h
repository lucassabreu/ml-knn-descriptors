#ifndef _CURVE_H_
#define _CURVE_H_

#include "scene.h"

typedef struct _curve { /* Curve */
  double *X;
  double *Y;
  int n; 
} Curve;

typedef Curve *Ap_Curve;

typedef struct _curve3d { /* 3D Curve */
  double *X;
  double *Y;
  double *Z;
  int n; 
} Curve3D;

Curve   *CreateCurve(int n);
void     DestroyCurve(Curve **curve);
Curve   *ReadCurve(char *filename);
void     WriteCurve(Curve *curve,char *filename);
void     WriteCurve2Gnuplot(Curve *curve,char *filename);
Curve   *CopyCurve(Curve *curve);


Curve3D *CreateCurve3D(int n);
void     DestroyCurve3D(Curve3D **curve);
void     WriteCurve3D(Curve3D *curve,char *filename);

void     SortCurve3D(Curve3D *curve, int left, int right, char order);
int      PartCurve3D (Curve3D *curve, int left, int right, char order);

void     SortCurve(Curve *curve, int left, int right, char order);
int      PartCurve (Curve *curve, int left, int right, char order);


void     InvertXY(Curve *curve);
Curve    *FillCurve(Curve *c1, double dx);
Curve    *JoinCurve(Curve *c1, Curve *c2);
double    CurveMaximum(Curve *c);
Curve3D  *GetLinePoints (Voxel *p, Voxel *q);

#endif




