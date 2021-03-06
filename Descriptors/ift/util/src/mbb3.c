#include "common.h"
#include "scene.h"

int main(int argc, char **argv)
{
  char filename[200];
  Scene *scn,*mbb;

  /*--------------------------------------------------------*/

  void *trash = malloc(1);                 
  struct mallinfo info;   
  int MemDinInicial, MemDinFinal;
  free(trash); 
  info = mallinfo();
  MemDinInicial = info.uordblks;

  /*--------------------------------------------------------*/

  if (argc != 2) {
    fprintf(stderr,"usage: mbb3 <basename>\n");
    exit(-1);
  }
  sprintf(filename,"%s.scn",argv[1]);
  scn  = ReadScene(filename);
  mbb  = MBB3(scn);
  sprintf(filename,"%s_mbb.scn",argv[1]);
  WriteScene(mbb,filename);
  DestroyScene(&scn);
  DestroyScene(&mbb);

  /* ---------------------------------------------------------- */

  info = mallinfo();
  MemDinFinal = info.uordblks;
  if (MemDinInicial!=MemDinFinal)
    printf("\n\nDinamic memory was not completely deallocated (%d, %d)\n",
	   MemDinInicial,MemDinFinal);   

  return(0);
}
