#include "libcolordescriptors.h"

int main(int argc, char** argv)
{
  Histogram *gch1=NULL;
  Histogram *gch2=NULL;
  ulong distance;
  char filename1[20], filename2[20];
  unsigned i1, j1, i2, j2;
  FILE *fout;

  int n = 6400;

  fout = fopen("gch_distances2.bin", "wb");
  fwrite(&n, sizeof(int), 1, fout);
  for(i1=0;i1<80;i1++) { for(j1=0;j1<80;j1++) {
      sprintf(filename1,"size30_%d_%d.ppm.fv",i1,j1);
      gch1 = ReadFileHistogram(filename1);
      printf("%s     \r",filename1);
      for(i2=0;i2<80;i2++) { for(j2=0;j2<80;j2++) {
        sprintf(filename2,"size30_%d_%d.ppm.fv",i2,j2);
        gch2 = ReadFileHistogram(filename2);
        distance=L1Distance(gch1, gch2);
        float fdistance = (float)distance;
        fwrite(&fdistance, sizeof(float), 1, fout);
        DestroyHistogram(&gch2);
      }}
      DestroyHistogram(&gch1);
  }}
  fclose(fout);
  return(0);
}
