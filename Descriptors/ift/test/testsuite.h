#include <ift.h>
#include "libps/ps.h"
#include "libps/js.h"

#define _LINUX_ 1
#define NIMG 5

bool   ValidImage(int i);
Image *InputArray();
void   OutputArray(Image *img);
Kernel *ReadKernel();
void   WriteSaliences(Curve3D *saliences, char *filename, int npts);
void   WriteLaTex(AnnImg *aimg, AdjRel *A, char *filename); 
void   WriteEPS(AnnImg *aimg, AdjRel *A, char *figname); 



