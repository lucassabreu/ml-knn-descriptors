#ifndef GABOR_H_
#define GABOR_H_

void GaborFilteredImg(Matrix *FilteredImg_real, Matrix *FilteredImg_imag, Matrix *img, int side, double Ul, double Uh, 
int scale, int orientation, int flag);

#endif /*GABOR_H_*/
