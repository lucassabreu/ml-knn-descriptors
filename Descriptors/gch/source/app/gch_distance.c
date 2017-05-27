#include "libcolordescriptors.h"

int main(int argc, char** argv)
{
  Histogram *gch1=NULL;
  Histogram *gch2=NULL;
  ulong distance;

  if (argc != 3) {
    fprintf(stderr,"usage: gch_distance <fv1_path> <fv2_path>\n");
    exit(-1);
  }

  gch1 = ReadFileHistogram(argv[1]);
  gch2 = ReadFileHistogram(argv[2]);

  distance=L1Distance(gch1, gch2);

  printf("%ld\n",distance);

  DestroyHistogram(&gch1);
  DestroyHistogram(&gch2);

  return(0);
}
