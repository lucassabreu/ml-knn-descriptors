#include "mathematics.h"
#include "common.h"

Image  *Diff(Image *img1,Image *img2)
{
  Image *diff;
  int p,n;

  diff = CopyImage(img1);
  n = img1->ncols*img1->nrows;
  for (p=0; p < n; p++)
    diff->val[p] -= img2->val[p];
  return(diff);
}

Image  *Mult(Image *img1,Image *img2)
{
  Image *mult;
  int p,n;

  mult = CopyImage(img1);
  n = img1->ncols*img1->nrows;
  for (p=0; p < n; p++)
    mult->val[p] *= img2->val[p];
  return(mult);
}

Image  *Div(Image *img1,Image *img2)
{
  Image *div;
  int p,n,Imax1,Imin2,Dmax;

  Imin2 = MinimumValue(img2);
  if (Imin2 == 0)
    Imin2 = 1;
  Imax1 = MaximumValue(img1);
  Dmax  = Imax1/Imin2 + 1;
  div = CopyImage(img1);
  n = img1->ncols*img1->nrows;
  for (p=0; p < n; p++){
    if (img2->val[p] != 0)
      div->val[p] /= img2->val[p];
    else
      div->val[p] = Dmax;
  }
  return(div);
}

Image *Sum(Image *img1, Image *img2)
{
  Image *sum;
  int p,n;

  sum = CopyImage(img1);
  n = img1->ncols*img1->nrows;
  for (p=0; p < n; p++)
    sum->val[p] += img2->val[p];
  return(sum);
}

Image *SQRT(Image *img)
{
  Image *sqrimg=NULL;
  int p,n;
  
  n = img->ncols*img->nrows;
  sqrimg = CreateImage(img->ncols,img->nrows);
  for (p=0; p < n; p++)
    sqrimg->val[p] = (int)sqrt((double)img->val[p]);
  return(sqrimg);
}

Image  *And(Image *img1,Image *img2)
{
  Image *_and;
  int p,n;

  _and = CreateImage(img1->ncols,img1->nrows);
  n   = img1->ncols*img1->nrows;
  for (p=0; p < n; p++)
    _and->val[p] = MIN(img1->val[p],img2->val[p]);
  return(_and);
}

Image  *Or(Image *img1,Image *img2)
{
  Image *_or;
  int p,n;

  _or = CreateImage(img1->ncols,img1->nrows);
  n   = img1->ncols*img1->nrows;
  for (p=0; p < n; p++)
    _or->val[p] = MAX(img1->val[p],img2->val[p]);
  return(_or);
}


Image  *XOr(Image *img1, Image *img2){
  Image *_xor;
  int p,n;

  _xor = CreateImage(img1->ncols,img1->nrows);
  n   = img1->ncols*img1->nrows;
  for (p=0; p < n; p++)
    _xor->val[p] = MAX(img1->val[p],img2->val[p])-
                   MIN(img1->val[p],img2->val[p]);
  return(_xor);
}


Image *Add(Image *img, int value)
{
  Image *add;
  int Imax,p,n;

  add  = CopyImage(img);
  Imax = MaximumValue(img);
  n = img->ncols*img->nrows;
  for (p=0; p < n; p++)
    add->val[p] = MIN(add->val[p]+value,Imax);
  return(add);
}

Image *Sub(Image *img, int value)
{
  Image *sub;
  int p,n;

  sub = CopyImage(img);
  n = img->ncols*img->nrows;
  for (p=0; p < n; p++)
    sub->val[p] = MAX(sub->val[p]-value,0);
    
  return(sub);
}

Image *Complement(Image *img)
{
  Image *cimg=NULL;
  unsigned int p, n, r;
  int Imax;
  
  cimg = CreateImage(img->ncols, img->nrows);
  Imax = MaximumValue(img);
  n = img->ncols*img->nrows;
  r = n%4;
  n -= r;
  for (p=0; p < n; p+=4) {
    cimg->val[p] = Imax - img->val[p];
    cimg->val[p+1] = Imax - img->val[p+1];
    cimg->val[p+2] = Imax - img->val[p+2];
    cimg->val[p+3] = Imax - img->val[p+3];
  }
  while (r != 0) {
    cimg->val[p+r-1] = Imax - img->val[p+r-1];
    --r;
  }

  return(cimg);
}

Image *Abs(Image *img)
{
  Image *absimg=NULL;
  int p,n;
  
  n = img->ncols*img->nrows;
  absimg = CreateImage(img->ncols,img->nrows);
  for (p=0; p < n; p++)
    absimg->val[p] = abs(img->val[p]);
  return(absimg);
}

Image *Average(Image *img1, Image *img2)
{
  Image *sum;
  int p,n;

  sum = CopyImage(img1);
  n = img1->ncols*img1->nrows;
  for (p=0; p < n; p++)
    sum->val[p] = (img1->val[p] + img2->val[p])/2;
  return(sum);
}

Image  *Log(Image *img)
{
  Image *limg=NULL;
  int p,n;

  limg = CreateImage(img->ncols,img->nrows);
  n    = img->ncols*img->nrows;
  for (p=0; p < n; p++) 
    limg->val[p] = (int) log(1+img->val[p]);
  return(limg);
}



void Diffinplace(Image *img1,Image *img2){
  int p,n;

  n = img1->ncols*img1->nrows;
  for (p=0; p < n; p++)
    img1->val[p] -= img2->val[p];
}

void Suminplace(Image *img1, Image *img2){
  int p,n;

  n = img1->ncols*img1->nrows;
  for (p=0; p < n; p++)
    img1->val[p] += img2->val[p];
}

void Orinplace(Image *img1,Image *img2){
  int p,n;

  n   = img1->ncols*img1->nrows;
  for (p=0; p < n; p++)
    img1->val[p] = MAX(img1->val[p],img2->val[p]);
}

// mathematical functions with no images

double Factorial(int n)
{
  double fat=1.0;

  while (n > 1){
    fat *= n; n--;
  }

  return(fat);
}

double DoubleFactorial(int n)
{
  double fat=1.0;

  while (n > 1){
    fat *= n; n-=2;
  }

  return(fat);
}

double HypersphereVolume(int dim, float radius)
{
  double num,den;

  num = pow(PI,dim/2.0)*pow(radius,dim);

  if ((dim%2)==0){
    den = Factorial(dim/2);
  }else{
    den = sqrt(PI)*DoubleFactorial(dim)/pow(2.0,(dim+1)/2);
  }

  return(num/den);
}
