#include "../include/libcolordescriptors.h"
#include "mpeg7_htd.c"


int main(int argc, char** argv)
{
  if (argc != 3) {
    fprintf(stderr,"usage: %s <fv1_path> <fv2_path>\n",argv[0]);
    exit(-1);
  }
  double distance = Distance(argv[1],argv[2]);
  printf("%lf\n",distance);
  return(0);
}
