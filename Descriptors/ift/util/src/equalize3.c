#include "radiometric3.h"

int main(int argc, char **argv)
{
  Scene *img,*eimg;
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
    fprintf(stderr,"usage: equalize3 <input.scn> <Imax>\n");
    exit(-1);
  }

  img  = ReadScene(argv[1]);
  Imax = atoi(argv[2]);
  eimg = Equalize3(img,Imax);
  WriteScene(eimg,"equalized.scn");

  DestroyScene(&img);
  DestroyScene(&eimg);

  /* ---------------------------------------------------------- */

  info = mallinfo();
  MemDinFinal = info.uordblks;
  if (MemDinInicial!=MemDinFinal)
    printf("\n\nDinamic memory was not completely deallocated (%d, %d)\n",
	   MemDinInicial,MemDinFinal);   

  return(0);
}
