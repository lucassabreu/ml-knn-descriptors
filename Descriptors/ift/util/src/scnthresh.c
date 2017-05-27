#include "common.h"
#include "scene.h"
#include "segmentation3.h"

int main(int argc, char **argv)
{
  char filename[200];
  Scene *scn,*bin;

  /*--------------------------------------------------------*/

  void *trash = malloc(1);                 
  struct mallinfo info;   
  int MemDinInicial, MemDinFinal;
  free(trash); 
  info = mallinfo();
  MemDinInicial = info.uordblks;

  /*--------------------------------------------------------*/

  if (argc != 4) {
    fprintf(stderr,"usage: %s <basename> <dx> <dy>\n",argv[0]);
    exit(-1);
  }
  sprintf(filename,"%s.scn",argv[1]);
  scn    = ReadScene(filename);
  bin = Threshold3(scn,atoi(argv[2]),atoi(argv[3])); 
  sprintf(filename,"%s_bin.scn",argv[1]);
  WriteScene(bin,filename);
  DestroyScene(&scn);
  DestroyScene(&bin);

  /* ---------------------------------------------------------- */

  info = mallinfo();
  MemDinFinal = info.uordblks;
  if (MemDinInicial!=MemDinFinal)
    printf("\n\nDinamic memory was not completely deallocated (%d, %d)\n",
	   MemDinInicial,MemDinFinal);   

  return(0);
}

