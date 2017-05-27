
#include "radiometric.h"

int main(int argc, char **argv)
{
  Image *img,*eimg;
  int    Imax;

  /*------- -------------------------------------------------*/

  void *trash = malloc(1);                 
  struct mallinfo info;   
  int MemDinInicial, MemDinFinal;
  free(trash); 
  info = mallinfo();
  MemDinInicial = info.uordblks;

  /*--------------------------------------------------------*/
  
  if (argc != 3) {
    fprintf(stderr,"usage: equalize <image.pgm> <Imax>\n");
    exit(-1);
  }

  img  = ReadImage(argv[1]);
  Imax = atoi(argv[2]);
  eimg = Equalize(img,Imax);
  WriteImage(eimg,"equalized.pgm");

  DestroyImage(&img);
  DestroyImage(&eimg);

  /* ---------------------------------------------------------- */

  info = mallinfo();
  MemDinFinal = info.uordblks;
  if (MemDinInicial!=MemDinFinal)
    printf("\n\nDinamic memory was not completely deallocated (%d, %d)\n",
	   MemDinInicial,MemDinFinal);   

  return(0);
}
