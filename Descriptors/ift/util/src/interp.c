#include "common.h"
#include "scene.h"

int main(int argc, char **argv)
{
  char filename[200];
  Scene *scn,*inter;
  float dx,dy,dz,min;
  
  /*--------------------------------------------------------*/

  void *trash = malloc(1);                 
  struct mallinfo info;   
  int MemDinInicial, MemDinFinal;
  free(trash); 
  info = mallinfo();
  MemDinInicial = info.uordblks;

  /*--------------------------------------------------------*/

  if (argc != 5 && argc !=2) {
    fprintf(stderr,"usage: interp <basename> [<dx> <dy> <dz>]\n");
    fprintf(stderr,"       Obs: If dx,dy,dz is omitted. It is interpolated to isotropic voxels using the minimum voxel dimension.\n");
    fprintf(stderr,"\n");
    
    exit(-1);
  }
  sprintf(filename,"%s.scn",argv[1]);
  scn    = ReadScene(filename);
  if (argc!=5) {
    min = scn->dx;
    if (scn->dy<min) min=scn->dy;
    if (scn->dz<min) min=scn->dz;
    dx=dy=dz=min;
  }
  else {
    dx=atof(argv[2]);
    dy=atof(argv[3]);
    dz=atof(argv[4]);
  }
  inter = LinearInterp(scn,dx,dy,dz); 
  sprintf(filename,"%s_int.scn",argv[1]);
  WriteScene(inter,filename);
  DestroyScene(&scn);
  DestroyScene(&inter);

  /* ---------------------------------------------------------- */

  info = mallinfo();
  MemDinFinal = info.uordblks;
  if (MemDinInicial!=MemDinFinal)
    printf("\n\nDinamic memory was not completely deallocated (%d, %d)\n",
	   MemDinInicial,MemDinFinal);   

  return(0);
}
