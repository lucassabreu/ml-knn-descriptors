#include "../include/libcolordescriptors.h"
#include "mpeg7_htd.c"

int main(int argc, char** argv)
{
  if (argc != 3) {
    fprintf(stderr,"usage: %s <image_path> <fv_path>\n",argv[0]);
    exit(-1);
  }
  Extraction(argv[1],argv[2]);
  return(0);
}
