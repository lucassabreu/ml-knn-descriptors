#include "libtexturedescriptors.h"

double Distance(char *fv1_path, char *fv2_path)
{
    Histogram *lbp1=NULL;
    Histogram *lbp2=NULL;
    double distance;

    lbp1 = ReadFileHistogram(fv1_path);
    lbp2 = ReadFileHistogram(fv2_path);

    distance=L1Distance(lbp1, lbp2);

    DestroyHistogram(&lbp1);
    DestroyHistogram(&lbp2);

    return (distance);
}

int main(int argc, char** argv)
{
    double distance;

    if (argc != 3) {
        fprintf(stderr,"usage: lbp_distance <fv1_path> <fv2_path>\n");
        exit(-1);
    }

    distance=Distance(argv[1], argv[2]);
    printf("%f\n",distance);

    return(0);
}
