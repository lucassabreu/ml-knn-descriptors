#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "ift.h"
#include "comptime.h"

int main(int argc, char **argv) {

  Scene *scn;
  Context *cxt;
  Shell *shell;
  char file[256];
  Image *img;
  CImage *cimg;
  AdjRel3 *A=Spheric(1.0);
  Curve *den,*grad;
  float *alpha,tc,ta,tr;
  timer *tic,*toc;
  int i;
  
  // Initial settings
  img = NULL;
  cxt = NULL;
  scn = NULL;
  cxt = NULL;
  shell =NULL;
  den = NULL;
  grad = NULL;
  tr = 0.;
  srand(1); // for random number generator

  // Read curves
  sprintf(file,"%s.den",argv[2]);
  den = ReadCurve(file);
  sprintf(file,"%s.grad",argv[2]);
  grad = ReadCurve(file);

  // Read scene
  sprintf(file,"../../data/3D/%s.lab",argv[1]);
  scn = ReadScene(file);

  // Classify scene
  tic = Tic();
  alpha = CreateOpacity(scn);
  ClassifyScene(alpha,scn,den,FDensity3);
  ClassifyScene(alpha,scn,grad,FMorphGrad3);
  toc = Toc();
  tc = CTime(tic,toc); 

  // Create context
  cxt = CreateContext(scn);
  SetAngles(cxt, 90.0, 20.0, 0.0);

  // Create shell
  tic = Tic();
  shell = CreateShell2(scn,alpha,cxt);
  toc = Toc();
  ta = CTime(tic,toc); 

  // Free unused data
  DestroyScene(&scn);
  free(alpha);

  // Render n times
  for (i=0;i<1;i++) {
    tic = Tic();
    img = SWShellRendering(shell,cxt); 
    toc = Toc();
    tr = tr + CTime(tic,toc); 
  }
  
  // Colorize and write color image
  SetObjectColor(cxt,1, 0.00, 1.00, 0.00);  
  SetObjectColor(cxt,6, 0.77, 0.66, 0.09);  
  SetObjectColor(cxt,9, 0.59, 0.73, 1.00);
  cimg = Colorize(cxt,img);
  WriteCImage(cimg,"ctest.pgm");

  // Print information
  printf("shell 2: %d voxels\n",shell->nvoxels);
  printf("createshell: %f ms\n",ta);
  printf("classifyscene: %f ms\n",tc);
  printf("render: %f ms\n",tr);
  printf("\n");

  // Destroy remaining data
  DestroyImage(&img);
  DestroyCImage(&cimg);
  DestroyShell(&shell);
  DestroyContext(&cxt);
  DestroyAdjRel3(&A);
  DestroyCurve(&den);
  DestroyCurve(&grad);
  return 0;
}

// Copy and paste


    /*    if (i%10 == 0) {
      sprintf(file,"test.%d",i+100);
      WriteImage(img,file);
      }*/


  /*
  DestroyScene(&scn);  
  sprintf(file,"../../data/3D/%s.lab",argv[1]);
  scn = ReadScene(file);
  */

    /*SetAngles(cxt,360.0*rand()/(RAND_MAX+1.0), 360.0*rand()/(RAND_MAX+1.0),\
      360.0*rand()/(RAND_MAX+1.0));	      */




