#include "ift.h"


int main(int argc, char **argv) 
{
  timer    *t1=NULL,*t2=NULL;
  CImage   *cimg1=NULL,*cimg2=NULL,*cimg3=NULL;

  /*--------------------------------------------------------*/

  void *trash = malloc(1);                 
  struct mallinfo info;   
  int MemDinInicial, MemDinFinal;
  free(trash); 
  info = mallinfo();
  MemDinInicial = info.uordblks;

  /*--------------------------------------------------------*/

  if (argc!=5)
    Error("Usage: registerimages <input1.ppm> <input2.ppm> <output.ppm> <perc_volume>","main");

  t1 = Tic();

  cimg1 = ReadCImage(argv[1]);
  cimg2 = ReadCImage(argv[2]);
  cimg3 = RegisterCImages(cimg1,cimg2,atof(argv[4]));

  t2 = Toc();

  fprintf(stdout,"registerimages in %f ms\n",CTime(t1,t2));

  WriteCImage(cimg3,argv[3]);

  DestroyCImage(&cimg1);
  DestroyCImage(&cimg2);
  DestroyCImage(&cimg3);


  /* ---------------------------------------------------------- */

  info = mallinfo();
  MemDinFinal = info.uordblks;
  if (MemDinInicial!=MemDinFinal)
    printf("\n\nDinamic memory was not completely deallocated (%d, %d)\n",
	   MemDinInicial,MemDinFinal);   

  return(0);
}


 
