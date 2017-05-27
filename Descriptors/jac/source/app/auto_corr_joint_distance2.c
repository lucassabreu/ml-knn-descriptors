#include "../include/libcolordescriptors.h"
#include "auto_corr_joint.c"

int main(int argc, char** argv)
{
  char filename1[20], filename2[20];
  unsigned i1, j1, i2, j2;
  FILE *fout;

  int n = 6400;

  fout = fopen("jac_distances.bin", "wb");
  fwrite(&n, sizeof(int), 1, fout);
  for(i1=0;i1<80;i1++) { for(j1=0;j1<80;j1++) {
      sprintf(filename1,"size30_%d_%d.ppm.fv",i1,j1);
      for(i2=0;i2<80;i2++) { for(j2=0;j2<80;j2++) {
        sprintf(filename2,"size30_%d_%d.ppm.fv",i2,j2);
        float distance = (float)Distance(filename1,filename2);
        fwrite(&distance, sizeof(float), 1, fout);
      }}
  }}
/*
  if (argc != 3) {
    fprintf(stderr,"usage: %s <fv1_path> <fv2_path>\n",argv[0]);
    exit(-1);
  }
  double distance = Distance(argv[1],argv[2]);
  printf("%lf\n",distance);
*/
  return(0);
}
