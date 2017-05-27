#include "testsuite.h"

void WriteLaTex(AnnImg *aimg, AdjRel *A, char *filename) 
{
  FILE *fp;
  int p,q,dx,dy,x,y,unit=16,i;
  Pixel u,v;

  fp = fopen(filename,"w"); 
  fprintf(fp,"\\documentclass{article}\n");
  fprintf(fp,"\\begin{document}\n");

  fprintf(fp,"\\begin{figure}\n");
  fprintf(fp,"\\begin{center}\n");
  fprintf(fp,"\\begin{tabular}{cc}\n");
  fprintf(fp,"\\begin{picture}(%d,%d)(0,0)",unit*aimg->img->ncols,unit*aimg->img->nrows);
  for (u.y=0; u.y < aimg->img->nrows; u.y++) {
    for (u.x=0; u.x < aimg->img->ncols; u.x++) {
      p  = u.x + aimg->img->tbrow[u.y];
      for (i=0; i < A->n; i++) {
	v.x = u.x + A->dx[i];
	v.y = u.y + A->dy[i];
	if (ValidPixel(aimg->img,v.x,v.y)){
	  q  = v.x + aimg->img->tbrow[v.y];
	  if (p != q){
	    if (aimg->pred->val[p] != p){
	      fprintf(fp,"\\put(%d,%d){\\small %d}\\put(%d,%d){\\circle*{%d}}\\put(%d,%d){\\vector(%d,%d){%d}}",unit*u.x+2,unit*(aimg->img->nrows-1-u.y)+2,aimg->img->val[p],unit*u.x,unit*(aimg->img->nrows-1-u.y),unit/5,unit*u.x,unit*(aimg->img->nrows-1-u.y),A->dx[i],-A->dy[i],unit);
	    } else {
	      fprintf(fp,"\\put(%d,%d){\\small %d}\\put(%d,%d){\\circle*{%d}}\\put(%d,%d){\\vector(%d,%d){%d}}",unit*u.x+2,unit*(aimg->img->nrows-1-u.y)+2,aimg->img->val[p],unit*u.x,unit*(aimg->img->nrows-1-u.y),unit/3,unit*u.x,unit*(aimg->img->nrows-1-u.y),A->dx[i],-A->dy[i],unit);
	    }
	  } else {
	    fprintf(fp,"\\put(%d,%d){\\small %d}",unit*u.x+2,unit*(aimg->img->nrows-1-u.y)+2,aimg->img->val[p]);
	  }
	}
      }
    }
    fprintf(fp,"\n");
  }
  fprintf(fp,"\\end{picture}\n");

  fprintf(fp,"&\n");

  fprintf(fp,"\\begin{picture}(%d,%d)(0,0)",unit*aimg->pred->ncols,unit*aimg->pred->nrows);
  for (y=0; y < aimg->pred->nrows; y++) {
    for (x=0; x < aimg->pred->ncols; x++) {
      p  = x + aimg->pred->tbrow[y];
      q  = aimg->pred->val[p];
      dx = q%aimg->pred->ncols - x;
      dy = q/aimg->pred->ncols - y;
      if (p != q) {
	fprintf(fp,"\\put(%d,%d){\\small %d}\\put(%d,%d){\\circle*{%d}}\\put(%d,%d){\\vector(%d,%d){%d}}",unit*x+2,unit*(aimg->pred->nrows-1-y)+2,aimg->cost->val[p],unit*x,unit*(aimg->pred->nrows-1-y),unit/5,unit*x,unit*(aimg->pred->nrows-1-y),dx,-dy,unit);
      } else {
	fprintf(fp,"\\put(%d,%d){\\small %d}\\put(%d,%d){\\circle*{%d}}",unit*x+2,unit*(aimg->pred->nrows-1-y)+2,aimg->cost->val[p],unit*x,unit*(aimg->pred->nrows-1-y),unit/3);
      }
    }
    fprintf(fp,"\n");
  }
  fprintf(fp,"\\end{picture}\n");

  fprintf(fp,"\\\\ \n");
  fprintf(fp,"(a) & (b) \n");
  fprintf(fp,"\\end{tabular}\n");
  fprintf(fp,"\\end{center}\n");
  fprintf(fp,"\\caption{(a) An image as a directed graph. (b) A minimum-cost path forest.}\n");
  fprintf(fp,"\\label{f.ift}\n");
  fprintf(fp,"\\end{figure}\n");

  fprintf(fp,"\\end{document}\n");
  fclose(fp);
}

void WriteSaliences(Curve3D *saliences, char *filename, int npts)
{
  FILE *fp;
  int i,j;

  SortCurve3D(saliences,0,saliences->n-1,DECREASING);
    
  fp = fopen(filename,"w"); 
  j = 1;
  for (i=0; i < npts; i++) {
    fprintf(fp,"%d %5.2f\n",j,saliences->Z[i]);
    j++;
  }  
  for (i=saliences->n-npts; i < saliences->n; i++) {
    fprintf(fp,"%d %5.2f\n",j,saliences->Z[i]);
    j++;
  }
 
  fclose(fp);
}

bool ValidImage(int i)
{
  if ((i >= 0)&&(i < NIMG))
    return(true);
  else
    return(false);
}

Image *InputArray()
{
  Image *img=NULL;
  int x,y,ncols,nrows;

  fscanf(stdin,"%d",&ncols);
  fscanf(stdin,"%d",&nrows);
  img = CreateImage(ncols,nrows);
  for (y=0; y < nrows; y++)
    for (x=0; x < ncols; x++)
      fscanf(stdin,"%d",&(img->val[x+img->tbrow[y]]));
	     
  return(img);
}

void OutputArray(Image *img)
{
  int x,y;
  for (y=0; y < img->nrows; y++){
    for (x=0; x < img->ncols; x++)
      fprintf(stdout,"%d ",img->val[x+img->tbrow[y]]);
    fprintf(stdout,"\n");
  }
  fprintf(stdout,"\n");

}

Kernel *ReadKernel()
{
  char coefs[200];

  fscanf(stdin,"%s",coefs);
  return(MakeKernel(coefs));
}

int main() {
  
  char action[20], filename[100], curve[20], seeds[200];
  Image *img[NIMG],*aux[NIMG];
  int option,i,p,input,output,match,refer,ntimes,marker,mask,side,orientation;
  int orientimage,cost,seed,thres,label,lower,higher,H,Co,Ao;
  int maxdist,*path=NULL,value,src,dst,angle,f1,f2,g1,g2, degree, reg, nbins;  
  Pixel u,v;
  float mean,stdev,radius,perc,theta,Sx,Sy;
  AdjRel *A=NULL;
  Kernel *kernel;
  AnnImg *aimg=NULL;
  Curve *hist=NULL,*fractal=NULL;
  Curve3D *saliences=NULL;
  timer tic,toc;
  FILE *ftime=fopen("time.txt","w");
  Set *set=NULL;
  double from, to;
  Polynom *P;

#ifdef _LINUX_  
  void *trash = malloc(1);                 
  struct mallinfo info;   
  int MemDinInicial, MemDinFinal;
  free(trash); 
  info = mallinfo();
  MemDinInicial = info.uordblks;
#endif

  for (i=0; i < NIMG; i++){
    img[i] = NULL;
    aux[i] = NULL;
  }

  fscanf(stdin,"%s",action);
  while(strcmp(action,"exit")!=0) { 

    if (strcmp(action,"ReadImage")==0) {
      fscanf(stdin,"%s",filename);
      fscanf(stdin,"%d",&output);
      if (ValidImage(output)){ 
	if (img[output] != NULL) DestroyImage(&img[output]);
	img[output] = ReadImage(filename);	
      }
      goto NEXT;
    }

    if (strcmp(action,"WriteImage")==0) {    
      fscanf(stdin,"%d",&input);
      fscanf(stdin,"%s",filename);
      if (ValidImage(input)&&(img[input] != NULL)){ 
	WriteImage(img[input],filename);
      }
      goto NEXT;
    }

    if (strcmp(action,"InputArray")==0) {
      fscanf(stdin,"%d",&output);
      if (ValidImage(output)){ 
	if (img[output] != NULL) DestroyImage(&img[output]);
	img[output] = InputArray();
      } else { /* trash the input */
	aux[0] = InputArray();
	DestroyImage(&aux[0]);
      }
      goto NEXT;
    }

    if (strcmp(action,"OutputArray")==0) {    
      fscanf(stdin,"%d",&input);
      if (ValidImage(input)&&(img[input] != NULL)){ 
	  OutputArray(img[input]);
      }
      goto NEXT;
    }

    if (strcmp(action,"CopyImage")==0) {    
      fscanf(stdin,"%d",&input);
      fscanf(stdin,"%d",&output);
      if (ValidImage(input)&&ValidImage(output)&&(img[input]!=NULL)){
	if (img[output] != NULL) DestroyImage(&img[output]);
	img[output] = CopyImage(img[input]);
      }
      goto NEXT;
    }

    if (strcmp(action,"ROI")==0) {    
      fscanf(stdin,"%d",&input);
      fscanf(stdin,"%d",&output);
      fscanf(stdin,"%d %d",&u.x,&u.y);
      fscanf(stdin,"%d %d",&v.x,&v.y);
      if (ValidImage(input)&&ValidImage(output)&&(img[input]!=NULL)){
	if (img[output] != NULL) DestroyImage(&img[output]);
	img[output] = ROI(img[input],u.x,u.y,v.x,v.y);
      }
      goto NEXT;
    }

    if (strcmp(action,"MBB")==0) {    
      fscanf(stdin,"%d",&input);
      fscanf(stdin,"%d",&output);
      if (ValidImage(input)&&ValidImage(output)&&(img[input]!=NULL)){
	if (img[output] != NULL) DestroyImage(&img[output]);
	img[output] = MBB(img[input]);
      }
      goto NEXT;
    }

    if (strcmp(action,"Threshold")==0) {    
      fscanf(stdin,"%d",&input);
      fscanf(stdin,"%d",&output);
      fscanf(stdin,"%d",&lower);
      fscanf(stdin,"%d",&higher);
      if (ValidImage(input)&&ValidImage(output)&&(img[input]!=NULL)){
	if (img[output] != NULL) DestroyImage(&img[output]);
	img[output] = Threshold(img[input],lower,higher);
      }
      goto NEXT;
    }

    if (strcmp(action,"MakeFrame")==0) {    
      fscanf(stdin,"%d",&input);
      fscanf(stdin,"%d",&output);
      if (ValidImage(input)&&(img[input] != NULL)&&ValidImage(output)){ 
	if (img[output] != NULL) DestroyImage(&img[output]);
	img[output] = MakeFrame(img[input]->ncols,img[input]->nrows,1);
      }
      goto NEXT;
    }

    if (strcmp(action,"RegMin")==0) {    
      fscanf(stdin,"%d",&input);
      fscanf(stdin,"%d",&output);
      fscanf(stdin,"%f",&radius);
      if (ValidImage(input)&&ValidImage(output)&&(img[input]!=NULL)){ 
	if (img[output] != NULL) DestroyImage(&img[output]);
	A = Circular(radius);
	gettimeofday(&tic,NULL);  
	img[output] = RegMin(img[input],A);
	gettimeofday(&toc,NULL);
	fprintf(ftime,"RegMin in %f milliseconds\n",(toc.tv_sec-tic.tv_sec)*1000.0 + (toc.tv_usec-tic.tv_usec)*0.001);
	DestroyAdjRel(&A);
      }
      goto NEXT;
    }
      
    if (strcmp(action,"RegMax")==0) {    
      fscanf(stdin,"%d",&input);
      fscanf(stdin,"%d",&output);
      fscanf(stdin,"%f",&radius);
      if (ValidImage(input)&&ValidImage(output)&&(img[input]!=NULL)){ 
	if (img[output] != NULL) DestroyImage(&img[output]);
	A = Circular(radius);
	img[output] = RegMax(img[input],A);
	DestroyAdjRel(&A);
      }
      goto NEXT;
    }

    if (strcmp(action,"HBasins")==0) {    
      fscanf(stdin,"%d",&input);
      fscanf(stdin,"%d",&output);
      fscanf(stdin,"%f",&radius);
      fscanf(stdin,"%d",&H);
      if (ValidImage(input)&&ValidImage(output)&&(img[input]!=NULL)){ 
	if (img[output] != NULL) DestroyImage(&img[output]);
	A = Circular(radius);
	img[output] = HBasins(img[input],A,H);
	DestroyAdjRel(&A);
      }
      goto NEXT;
    }

    if (strcmp(action,"HDomes")==0) {    
      fscanf(stdin,"%d",&input);
      fscanf(stdin,"%d",&output);
      fscanf(stdin,"%f",&radius);
      fscanf(stdin,"%d",&H);
      if (ValidImage(input)&&ValidImage(output)&&(img[input]!=NULL)){ 
	if (img[output] != NULL) DestroyImage(&img[output]);
	A = Circular(radius);
	img[output] = HDomes(img[input],A,H);
	DestroyAdjRel(&A);
      }
      goto NEXT;
    }

    if (strcmp(action,"SelectHBasins")==0) {    
      fscanf(stdin,"%d",&input);
      fscanf(stdin,"%d",&seed);
      fscanf(stdin,"%d",&output);
      fscanf(stdin,"%f",&radius);
      fscanf(stdin,"%d",&H);
      if (ValidImage(input)&&ValidImage(seed)&&
	  ValidImage(output)&&(img[input]!=NULL)&&(img[seed]!=NULL)){ 
	if (img[output] != NULL) DestroyImage(&img[output]);
	A = Circular(radius);
	gettimeofday(&tic,NULL);  
	img[output] = SelectHBasins(img[input],img[seed],A,H);
	gettimeofday(&toc,NULL);
	fprintf(ftime,"SelectHBasins in %f milliseconds\n",(toc.tv_sec-tic.tv_sec)*1000.0 + (toc.tv_usec-tic.tv_usec)*0.001);
	DestroyAdjRel(&A);
      }
      goto NEXT;
    }

    if (strcmp(action,"SelectHDomes")==0) {    
      fscanf(stdin,"%d",&input);
      fscanf(stdin,"%d",&seed);
      fscanf(stdin,"%d",&output);
      fscanf(stdin,"%f",&radius);
      fscanf(stdin,"%d",&H);
      if (ValidImage(input)&&ValidImage(seed)&&
	  ValidImage(output)&&(img[input]!=NULL)&&(img[seed]!=NULL)){ 
	if (img[output] != NULL) DestroyImage(&img[output]);
	A = Circular(radius);
	gettimeofday(&tic,NULL);  
	img[output] = SelectHDomes(img[input],img[seed],A,H);
	gettimeofday(&toc,NULL);
	fprintf(ftime,"SelectHDomes in %f milliseconds\n",(toc.tv_sec-tic.tv_sec)*1000.0 + (toc.tv_usec-tic.tv_usec)*0.001);
	DestroyAdjRel(&A);
      }
      goto NEXT;
    }

    if (strcmp(action,"MBB")==0) {    
      fscanf(stdin,"%d",&input);
      fscanf(stdin,"%d",&output);
      if (ValidImage(input)&&ValidImage(output)&&(img[input]!=NULL)){
	if (img[output] != NULL) DestroyImage(&img[output]);
	img[output] = MBB(img[input]);
      }
      goto NEXT;
    }

    if (strcmp(action,"AddFrame")==0) {    
      fscanf(stdin,"%d",&input);
      fscanf(stdin,"%d",&output);
      fscanf(stdin,"%d",&maxdist);
      fscanf(stdin,"%d",&value);
      if (ValidImage(input)&&ValidImage(output)&&(img[input]!=NULL)){
	if (img[output] != NULL) DestroyImage(&img[output]);
	img[output] = AddFrame(img[input],maxdist,value);
      }
      goto NEXT;
    }

    if (strcmp(action,"LabelComp")==0) {    
      fscanf(stdin,"%d",&input);
      fscanf(stdin,"%d",&output);
      fscanf(stdin,"%f",&radius);
      fscanf(stdin,"%d",&thres);
      if (ValidImage(input)&&ValidImage(output)&&(img[input]!=NULL)){
	if (img[output] != NULL) DestroyImage(&img[output]);
	A=Circular(radius);
	gettimeofday(&tic,NULL);
	img[output] = LabelComp(img[input],A,thres);
	gettimeofday(&toc,NULL);
	fprintf(ftime,"LabelComp in %f milliseconds\n",(toc.tv_sec-tic.tv_sec)*1000.0 + (toc.tv_usec-tic.tv_usec)*0.001);
	DestroyAdjRel(&A);
      }
      goto NEXT;
    }

    if (strcmp(action,"LabelBinComp")==0) {    
      fscanf(stdin,"%d",&input);
      fscanf(stdin,"%d",&output);
      fscanf(stdin,"%f",&radius);
      if (ValidImage(input)&&ValidImage(output)&&(img[input]!=NULL)){
	if (img[output] != NULL) DestroyImage(&img[output]);
	A=Circular(radius);
	gettimeofday(&tic,NULL);
	img[output] = LabelBinComp(img[input],A);
	gettimeofday(&toc,NULL);
	fprintf(ftime,"LabelBinComp in %f milliseconds\n",(toc.tv_sec-tic.tv_sec)*1000.0 + (toc.tv_usec-tic.tv_usec)*0.001);
	DestroyAdjRel(&A);
      }
      goto NEXT;
    }

    if (strcmp(action,"LabelContour")==0) {    
      fscanf(stdin,"%d",&input);
      fscanf(stdin,"%d",&output);
      if (ValidImage(input)&&ValidImage(output)&&(img[input]!=NULL)){
	if (img[output] != NULL) DestroyImage(&img[output]);
	img[output] = LabelContour(img[input]);
      }
      goto NEXT;
    }

    if (strcmp(action,"Paint")==0) {    
      fscanf(stdin,"%d",&input);
      fscanf(stdin,"%d",&output);
      fscanf(stdin,"%d",&value);      
      if (ValidImage(input)&&ValidImage(output)&&(img[input]!=NULL)){
	if (img[output] != NULL) DestroyImage(&img[output]);
	img[output] = Paint(img[input],value);
      }
      goto NEXT;
    }

    if (strcmp(action,"LabelContPixel")==0) {    
      fscanf(stdin,"%d",&input);
      fscanf(stdin,"%d",&output);
      if (ValidImage(input)&&ValidImage(output)&&(img[input]!=NULL)){
	if (img[output] != NULL) DestroyImage(&img[output]);
	img[output] = LabelContPixel(img[input]);
      }
      goto NEXT;
    }

    if (strcmp(action,"InputSeeds")==0) {    
	fscanf(stdin,"%d",&refer);
	fscanf(stdin,"%d",&output);
	fscanf(stdin,"%s",seeds);
	if (ValidImage(output)&&ValidImage(refer)&&(img[refer]!=NULL)){
	  if (img[output] != NULL) DestroyImage(&img[output]);
	  img[output] = InputSeeds(img[refer]->ncols,img[refer]->nrows,seeds);
	}
      goto NEXT;
    }

    if (strcmp(action,"AddSeed")==0) {    
      if (aimg != NULL){
	fscanf(stdin,"%d",&u.x);
	fscanf(stdin,"%d",&u.y);
	fscanf(stdin,"%d",&cost);	
	fscanf(stdin,"%d",&label);	
	if (ValidPixel(aimg->img,u.x,u.y)){
	  p = u.x+aimg->img->tbrow[u.y];
	  if (cost == -1)
	    AddSeed(aimg,p,aimg->img->val[p],label,p);
	  else
	    AddSeed(aimg,p,cost,label,p);	  
	}
      } else {
	fscanf(stdin,"%d",&u.x);
	fscanf(stdin,"%d",&u.y);
	fscanf(stdin,"%d",&cost);	
	fscanf(stdin,"%d",&label);	
      }
      goto NEXT;
    }

    if (strcmp(action,"RemSeed")==0) {    
      fscanf(stdin,"%f",&radius);
      if (aimg != NULL){
	fscanf(stdin,"%d",&u.x);
	fscanf(stdin,"%d",&u.y);
	if (ValidPixel(aimg->img,u.x,u.y)){
	  p = u.x+aimg->img->tbrow[u.y];
	  A = Circular(radius);
	  RemSeed(aimg,A,p);
	  DestroyAdjRel(&A);
	}
      } else { 
	fscanf(stdin,"%d",&u.x);
	fscanf(stdin,"%d",&u.y);
      }
      goto NEXT;
    }

    if (strcmp(action,"LinearFilter")==0) {    
      fscanf(stdin,"%d",&input);
      fscanf(stdin,"%d",&output);
      if (ValidImage(input)&&ValidImage(output)&&(img[input]!=NULL)){ 
	if (img[output] != NULL) DestroyImage(&img[output]);
	kernel = ReadKernel(); 
	img[output] = LinearFilter(img[input],kernel);
	DestroyKernel(&kernel);
      }
      goto NEXT;
    }

    if (strcmp(action,"Histogram")==0) {    
      fscanf(stdin,"%d",&input);
      if (ValidImage(input)&&(img[input]!=NULL)){
	if (hist != NULL) DestroyCurve(&hist);
	hist = Histogram(img[input]);
      }
      goto NEXT;
    }

    if (strcmp(action,"NormHistogram")==0) {    
      fscanf(stdin,"%d",&input);
      if (ValidImage(input)&&(img[input]!=NULL)){
	if (hist != NULL) DestroyCurve(&hist);
	hist = NormHistogram(img[input]);
      }
      goto NEXT;
    }

    if (strcmp(action,"Rotate")==0) {    
      fscanf(stdin,"%d",&input);
      fscanf(stdin,"%d",&output);
      fscanf(stdin,"%f",&theta);      
      if (ValidImage(input)&&ValidImage(output)&&(img[input]!=NULL)){ 
	if (img[output] != NULL) DestroyImage(&img[output]);
	img[output] = Rotate(img[input],theta);
      }
      goto NEXT;
    }

    if (strcmp(action,"Scale")==0) {    
      fscanf(stdin,"%d",&input);
      fscanf(stdin,"%d",&output);
      fscanf(stdin,"%f",&Sx);      
      fscanf(stdin,"%f",&Sy);      
      if (ValidImage(input)&&ValidImage(output)&&(img[input]!=NULL)){ 
	if (img[output] != NULL) DestroyImage(&img[output]);
	img[output] = Scale(img[input],Sx,Sy);
      }
      goto NEXT;
    }

    if (strcmp(action,"LinearStretch")==0) {    
      fscanf(stdin,"%d",&input);
      fscanf(stdin,"%d",&output);
      fscanf(stdin,"%d",&f1);
      fscanf(stdin,"%d",&f2);
      fscanf(stdin,"%d",&g1);
      fscanf(stdin,"%d",&g2);
      if (ValidImage(input)&&ValidImage(output)&&(img[input]!=NULL)){
	if (img[output] != NULL) DestroyImage(&img[output]);
	img[output] = LinearStretch(img[input],f1,f2,g1,g2);
      }
      goto NEXT;
    }

    if (strcmp(action,"GaussStretch")==0) {    
      fscanf(stdin,"%d",&input);
      fscanf(stdin,"%d",&output);
      fscanf(stdin,"%f",&mean);
      fscanf(stdin,"%f",&stdev);
      if (ValidImage(input)&&ValidImage(output)&&(img[input]!=NULL)){ 
	if (img[output] != NULL) DestroyImage(&img[output]);
	img[output] = GaussStretch(img[input],mean,stdev);
      }
      goto NEXT;
    }

    if (strcmp(action,"Equalize")==0) {    
      fscanf(stdin,"%d",&input);
      fscanf(stdin,"%d",&output);
      if (ValidImage(input)&&ValidImage(output)&&(img[input]!=NULL)){
	if (img[output] != NULL) DestroyImage(&img[output]);
	img[output] = Equalize(img[input],255);
      }
      goto NEXT;
    }

    if (strcmp(action,"MatchHistogram")==0) {    
      fscanf(stdin,"%d",&input);
      fscanf(stdin,"%d",&match);
      fscanf(stdin,"%d",&output);
      if (ValidImage(input)&&ValidImage(output)&&
	  ValidImage(match)&&(img[input]!=NULL)&&(img[match]!=NULL)){
	if (img[output] != NULL) DestroyImage(&img[output]);
	img[output] = MatchHistogram(img[input],img[match]);
      }
      goto NEXT;
    }

    if (strcmp(action,"Complement")==0) {    
      fscanf(stdin,"%d",&input);
      fscanf(stdin,"%d",&output);
      if (ValidImage(input)&&ValidImage(output)&&(img[input]!=NULL)){ 
	if (img[output] != NULL) DestroyImage(&img[output]);
	img[output] = Complement(img[input]);
      }
      goto NEXT;
    }
   
    if (strcmp(action,"And")==0) {    
      fscanf(stdin,"%d",&input);
      fscanf(stdin,"%d",&mask);
      fscanf(stdin,"%d",&output);
      if (ValidImage(input)&&ValidImage(output)&&
	  ValidImage(mask)&&(img[input]!=NULL)&&(img[mask]!=NULL)){
	if (img[output] != NULL) DestroyImage(&img[output]);
	img[output] = And(img[input],img[mask]);
      }
      goto NEXT;
    }

    if (strcmp(action,"Or")==0) {    
      fscanf(stdin,"%d",&input);
      fscanf(stdin,"%d",&mask);
      fscanf(stdin,"%d",&output);
      if (ValidImage(input)&&ValidImage(output)&&
	  ValidImage(mask)&&(img[input]!=NULL)&&(img[mask]!=NULL)){
	if (img[output] != NULL) DestroyImage(&img[output]);
	img[output] = Or(img[input],img[mask]);
      }
      goto NEXT;
    }

    if (strcmp(action,"Diff")==0) {    
      fscanf(stdin,"%d",&input);
      fscanf(stdin,"%d",&mask);
      fscanf(stdin,"%d",&output);
      if (ValidImage(input)&&ValidImage(output)&&
	  ValidImage(mask)&&(img[input]!=NULL)&&(img[mask]!=NULL)){
	if (img[output] != NULL) DestroyImage(&img[output]);
	img[output] = Diff(img[input],img[mask]);
      }
      goto NEXT;
    }

    if (strcmp(action,"Sum")==0) {    
      fscanf(stdin,"%d",&input);
      fscanf(stdin,"%d",&mask);
      fscanf(stdin,"%d",&output);
      if (ValidImage(input)&&ValidImage(output)&&
	  ValidImage(mask)&&(img[input]!=NULL)&&(img[mask]!=NULL)){
	if (img[output] != NULL) DestroyImage(&img[output]);
	img[output] = Sum(img[input],img[mask]);
      }
      goto NEXT;
    }

    if (strcmp(action,"Mult")==0) {    
      fscanf(stdin,"%d",&input);
      fscanf(stdin,"%d",&mask);
      fscanf(stdin,"%d",&output);
      if (ValidImage(input)&&ValidImage(output)&&
	  ValidImage(mask)&&(img[input]!=NULL)&&(img[mask]!=NULL)){
	if (img[output] != NULL) DestroyImage(&img[output]);
	img[output] = Mult(img[input],img[mask]);
      }
      goto NEXT;
    }

    if (strcmp(action,"Div")==0) {    
      fscanf(stdin,"%d",&input);
      fscanf(stdin,"%d",&mask);
      fscanf(stdin,"%d",&output);
      if (ValidImage(input)&&ValidImage(output)&&
	  ValidImage(mask)&&(img[input]!=NULL)&&(img[mask]!=NULL)){
	if (img[output] != NULL) DestroyImage(&img[output]);
	img[output] = Div(img[input],img[mask]);
      }
      goto NEXT;
    }

    if (strcmp(action,"Abs")==0) {    
      fscanf(stdin,"%d",&input);
      fscanf(stdin,"%d",&output);
      if (ValidImage(input)&&ValidImage(output)&&(img[input]!=NULL)){
	if (img[output] != NULL) DestroyImage(&img[output]);
	img[output] = Abs(img[input]);
      }
      goto NEXT;
    }

    if (strcmp(action,"Sub")==0) {    
      fscanf(stdin,"%d",&input);
      fscanf(stdin,"%d",&output);
      fscanf(stdin,"%d",&value);
      if (ValidImage(input)&&ValidImage(output)&&(img[input]!=NULL)){
	if (img[output] != NULL) DestroyImage(&img[output]);
	img[output] = Sub(img[input],value);
      }
      goto NEXT;
    }

    if (strcmp(action,"Add")==0) {    
      fscanf(stdin,"%d",&input);
      fscanf(stdin,"%d",&output);
      fscanf(stdin,"%d",&value);
      if (ValidImage(input)&&ValidImage(output)&&(img[input]!=NULL)){
	if (img[output] != NULL) DestroyImage(&img[output]);
	img[output] = Add(img[input],value);
      }
      goto NEXT;
    }

    if (strcmp(action,"Dilate")==0) {    
      fscanf(stdin,"%d",&input);
      fscanf(stdin,"%d",&output);
      fscanf(stdin,"%f",&radius);
      if (ValidImage(input)&&ValidImage(output)&&(img[input]!=NULL)){ 
	if (img[output] != NULL) DestroyImage(&img[output]);
	A = Circular(radius);
	gettimeofday(&tic,NULL);  
	img[output] = Dilate(img[input],A);
	gettimeofday(&toc,NULL);
	fprintf(ftime,"Dilate in %f milliseconds\n",(toc.tv_sec-tic.tv_sec)*1000.0 + (toc.tv_usec-tic.tv_usec)*0.001);
	DestroyAdjRel(&A);
      }
      goto NEXT;
    }

    if (strcmp(action,"DilateBin")==0) {    
      fscanf(stdin,"%d",&input);
      fscanf(stdin,"%d",&output);
      fscanf(stdin,"%f",&radius);
      if (ValidImage(input)&&ValidImage(output)&&(img[input]!=NULL)){ 
	if (img[output] != NULL) DestroyImage(&img[output]);
	if (set != NULL) DestroySet(&set);
	gettimeofday(&tic,NULL);  
	img[output] = DilateBin(img[input],&set,radius);
	gettimeofday(&toc,NULL);
	fprintf(ftime,"DilateBin in %f milliseconds\n",(toc.tv_sec-tic.tv_sec)*1000.0 + (toc.tv_usec-tic.tv_usec)*0.001);
      }
      goto NEXT;
    }

    if (strcmp(action,"Erode")==0) {    
      fscanf(stdin,"%d",&input);
      fscanf(stdin,"%d",&output);
      fscanf(stdin,"%f",&radius);
      if (ValidImage(input)&&ValidImage(output)&&(img[input]!=NULL)){ 
	if (img[output] != NULL) DestroyImage(&img[output]);
	A = Circular(radius);
	gettimeofday(&tic,NULL);  
	img[output] = Erode(img[input],A);
	gettimeofday(&toc,NULL);
	fprintf(ftime,"Erode in %f milliseconds\n",(toc.tv_sec-tic.tv_sec)*1000.0 + (toc.tv_usec-tic.tv_usec)*0.001);
	DestroyAdjRel(&A);
      }
      goto NEXT;
    }

    if (strcmp(action,"ErodeBin")==0) {    
      fscanf(stdin,"%d",&input);
      fscanf(stdin,"%d",&output);
      fscanf(stdin,"%f",&radius);
      if (ValidImage(input)&&ValidImage(output)&&(img[input]!=NULL)){ 
	if (img[output] != NULL) DestroyImage(&img[output]);
	if (set != NULL) DestroySet(&set);
	gettimeofday(&tic,NULL);  
	img[output] = ErodeBin(img[input],&set,radius);
	gettimeofday(&toc,NULL);
	fprintf(ftime,"ErodeBin in %f milliseconds\n",(toc.tv_sec-tic.tv_sec)*1000.0 + (toc.tv_usec-tic.tv_usec)*0.001);
      }
      goto NEXT;
    }

    if (strcmp(action,"CloseBin")==0) {    
      fscanf(stdin,"%d",&input);
      fscanf(stdin,"%d",&output);
      fscanf(stdin,"%f",&radius);
      if (ValidImage(input)&&ValidImage(output)&&(img[input]!=NULL)){ 
	if (img[output] != NULL) DestroyImage(&img[output]);
	gettimeofday(&tic,NULL);  
	img[output] = CloseBin(img[input],radius);
	gettimeofday(&toc,NULL);
	fprintf(ftime,"CloseBin in %f milliseconds\n",(toc.tv_sec-tic.tv_sec)*1000.0 + (toc.tv_usec-tic.tv_usec)*0.001);
      }
      goto NEXT;
    }

    if (strcmp(action,"OpenBin")==0) {    
      fscanf(stdin,"%d",&input);
      fscanf(stdin,"%d",&output);
      fscanf(stdin,"%f",&radius);
      if (ValidImage(input)&&ValidImage(output)&&(img[input]!=NULL)){ 
	if (img[output] != NULL) DestroyImage(&img[output]);
	gettimeofday(&tic,NULL);  
	img[output] = OpenBin(img[input],radius);
	gettimeofday(&toc,NULL);
	fprintf(ftime,"OpenBin in %f milliseconds\n",(toc.tv_sec-tic.tv_sec)*1000.0 + (toc.tv_usec-tic.tv_usec)*0.001);
      }
      goto NEXT;
    }

    if (strcmp(action,"AsfOCBin")==0) {    
      fscanf(stdin,"%d",&input);
      fscanf(stdin,"%d",&output);
      fscanf(stdin,"%f",&radius);
      if (ValidImage(input)&&ValidImage(output)&&(img[input]!=NULL)){ 
	if (img[output] != NULL) DestroyImage(&img[output]);
	gettimeofday(&tic,NULL);  
	img[output] = AsfOCBin(img[input],radius);
	gettimeofday(&toc,NULL);
	fprintf(ftime,"AsfOCBin in %f milliseconds\n",(toc.tv_sec-tic.tv_sec)*1000.0 + (toc.tv_usec-tic.tv_usec)*0.001);
      }
      goto NEXT;
    }

    if (strcmp(action,"MorphGrad")==0) {    
      fscanf(stdin,"%d",&input);
      fscanf(stdin,"%d",&output);
      fscanf(stdin,"%f",&radius);
      if (ValidImage(input)&&ValidImage(output)&&(img[input]!=NULL)){ 
	if (img[output] != NULL) DestroyImage(&img[output]);
	A = Circular(radius);
	img[output] = MorphGrad(img[input],A);
	DestroyAdjRel(&A);
      }
      goto NEXT;
    }

    if (strcmp(action,"Open")==0) {    
      fscanf(stdin,"%d",&input);
      fscanf(stdin,"%d",&output);
      fscanf(stdin,"%f",&radius);
      if (ValidImage(input)&&ValidImage(output)&&(img[input]!=NULL)){ 
	if (img[output] != NULL) DestroyImage(&img[output]);
	A = Circular(radius);
	img[output] = Open(img[input],A);
	DestroyAdjRel(&A);
      }
      goto NEXT;
    }

    if (strcmp(action,"Close")==0) {    
      fscanf(stdin,"%d",&input);
      fscanf(stdin,"%d",&output);
      fscanf(stdin,"%f",&radius);
      if (ValidImage(input)&&ValidImage(output)&&(img[input]!=NULL)){ 
	if (img[output] != NULL) DestroyImage(&img[output]);
	A = Circular(radius);
	gettimeofday(&tic,NULL);  
	img[output] = Close(img[input],A);
	gettimeofday(&toc,NULL);
	fprintf(ftime,"Close in %f milliseconds\n",(toc.tv_sec-tic.tv_sec)*1000.0 + (toc.tv_usec-tic.tv_usec)*0.001);
	DestroyAdjRel(&A);
      }
      goto NEXT;
    }

    if (strcmp(action,"AsfOC")==0) {    
      fscanf(stdin,"%d",&input);
      fscanf(stdin,"%d",&output);
      fscanf(stdin,"%f",&radius);
      if (ValidImage(input)&&ValidImage(output)&&(img[input]!=NULL)){ 
	if (img[output] != NULL) DestroyImage(&img[output]);
	A = Circular(radius);
	gettimeofday(&tic,NULL);  
	img[output] = AsfOC(img[input],A);
	gettimeofday(&toc,NULL);
	fprintf(ftime,"AsfOC in %f milliseconds\n",(toc.tv_sec-tic.tv_sec)*1000.0 + (toc.tv_usec-tic.tv_usec)*0.001);
	DestroyAdjRel(&A);
      }
      goto NEXT;
    }

    if (strcmp(action,"AsfnOC")==0) {    
      fscanf(stdin,"%d",&input);
      fscanf(stdin,"%d",&output);
      fscanf(stdin,"%d",&ntimes);
      if (ValidImage(input)&&ValidImage(output)&&(img[input]!=NULL)){ 
	if (img[output] != NULL) DestroyImage(&img[output]);
	img[output] = AsfnOC(img[input],ntimes);
      }
      goto NEXT;
    }

    if (strcmp(action,"AsfOCO")==0) {    
      fscanf(stdin,"%d",&input);
      fscanf(stdin,"%d",&output);
      fscanf(stdin,"%f",&radius);
      if (ValidImage(input)&&ValidImage(output)&&(img[input]!=NULL)){ 
	if (img[output] != NULL) DestroyImage(&img[output]);
	A = Circular(radius);
	img[output] = AsfOCO(img[input],A);
	DestroyAdjRel(&A);
      }
      goto NEXT;
    }

    if (strcmp(action,"AsfnOCO")==0) {    
      fscanf(stdin,"%d",&input);
      fscanf(stdin,"%d",&output);
      fscanf(stdin,"%d",&ntimes);
      if (ValidImage(input)&&ValidImage(output)&&(img[input]!=NULL)){ 
	if (img[output] != NULL) DestroyImage(&img[output]);
	img[output] = AsfnOCO(img[input],ntimes);
      }
      goto NEXT;
    }

    if (strcmp(action,"AsfCO")==0) {    
      fscanf(stdin,"%d",&input);
      fscanf(stdin,"%d",&output);
      fscanf(stdin,"%f",&radius);
      if (ValidImage(input)&&ValidImage(output)&&(img[input]!=NULL)){ 
	if (img[output] != NULL) DestroyImage(&img[output]);
	A = Circular(radius);
	img[output] = AsfCO(img[input],A);
	DestroyAdjRel(&A);
      }
      goto NEXT;
    }

    if (strcmp(action,"AsfCOC")==0) {    
      fscanf(stdin,"%d",&input);
      fscanf(stdin,"%d",&output);
      fscanf(stdin,"%f",&radius);
      if (ValidImage(input)&&ValidImage(output)&&(img[input]!=NULL)){ 
	if (img[output] != NULL) DestroyImage(&img[output]);
	A = Circular(radius);
	img[output] = AsfCOC(img[input],A);
	DestroyAdjRel(&A);
      }
      goto NEXT;
    }

    if (strcmp(action,"AsfnCO")==0) {    
      fscanf(stdin,"%d",&input);
      fscanf(stdin,"%d",&output);
      fscanf(stdin,"%d",&ntimes);
      if (ValidImage(input)&&ValidImage(output)&&(img[input]!=NULL)){ 
	if (img[output] != NULL) DestroyImage(&img[output]);
	img[output] = AsfnCO(img[input],ntimes);
      }
      goto NEXT;
    }

    if (strcmp(action,"AsfnCOC")==0) {    
      fscanf(stdin,"%d",&input);
      fscanf(stdin,"%d",&output);
      fscanf(stdin,"%d",&ntimes);
      if (ValidImage(input)&&ValidImage(output)&&(img[input]!=NULL)){ 
	if (img[output] != NULL) DestroyImage(&img[output]);
	img[output] = AsfnCOC(img[input],ntimes);
      }
      goto NEXT;
    }

    if (strcmp(action,"HClose")==0) {    
      fscanf(stdin,"%d",&input);
      fscanf(stdin,"%d",&seed);
      fscanf(stdin,"%d",&output);
      fscanf(stdin,"%f",&radius);
      fscanf(stdin,"%d",&H);
      if (ValidImage(input)&&ValidImage(seed)&&
	  ValidImage(output)&&(img[input]!=NULL)&&(img[seed]!=NULL)){ 
	if (img[output] != NULL) DestroyImage(&img[output]);
	A = Circular(radius);
	gettimeofday(&tic,NULL);  
	img[output] = HClose(img[input],img[seed],A,H);
	gettimeofday(&toc,NULL);
	fprintf(ftime,"HClose in %f milliseconds\n",(toc.tv_sec-tic.tv_sec)*1000.0 + (toc.tv_usec-tic.tv_usec)*0.001);
	DestroyAdjRel(&A);
      }
      goto NEXT;
    }

    if (strcmp(action,"HOpen")==0) {    
      fscanf(stdin,"%d",&input);
      fscanf(stdin,"%d",&seed);
      fscanf(stdin,"%d",&output);
      fscanf(stdin,"%f",&radius);
      fscanf(stdin,"%d",&H);
      if (ValidImage(input)&&ValidImage(seed)&&
	  ValidImage(output)&&(img[input]!=NULL)&&(img[seed]!=NULL)){ 
	if (img[output] != NULL) DestroyImage(&img[output]);
	A = Circular(radius);
	gettimeofday(&tic,NULL);  
	img[output] = HOpen(img[input],img[seed],A,H);
	gettimeofday(&toc,NULL);
	fprintf(ftime,"HOpen in %f milliseconds\n",(toc.tv_sec-tic.tv_sec)*1000.0 + (toc.tv_usec-tic.tv_usec)*0.001);
	DestroyAdjRel(&A);
      }
      goto NEXT;
    }

    if (strcmp(action,"CloseHoles")==0) {    
      fscanf(stdin,"%d",&input);
      fscanf(stdin,"%d",&output);
      if (ValidImage(input)&&ValidImage(output)&&(img[input]!=NULL)){ 
	if (img[output] != NULL) DestroyImage(&img[output]);
	gettimeofday(&tic,NULL);  
	img[output] = CloseHoles(img[input]);
	gettimeofday(&toc,NULL);
	fprintf(ftime,"CloseHoles in %f milliseconds\n",(toc.tv_sec-tic.tv_sec)*1000.0 + (toc.tv_usec-tic.tv_usec)*0.001);
      }
      goto NEXT;
    }

    if (strcmp(action,"RemDomes")==0) {    
      fscanf(stdin,"%d",&input);
      fscanf(stdin,"%d",&output);
      if (ValidImage(input)&&ValidImage(output)&&(img[input]!=NULL)){ 
	if (img[output] != NULL) DestroyImage(&img[output]);
	gettimeofday(&tic,NULL);  
	img[output] = RemDomes(img[input]);
	gettimeofday(&toc,NULL);
	fprintf(ftime,"RemDomes in %f milliseconds\n",(toc.tv_sec-tic.tv_sec)*1000.0 + (toc.tv_usec-tic.tv_usec)*0.001);
      }
      goto NEXT;
    }

    if (strcmp(action,"AreaClose")==0) {    
      fscanf(stdin,"%d",&input);
      fscanf(stdin,"%d",&output);
      fscanf(stdin,"%d",&thres);
      if (ValidImage(input)&&ValidImage(output)&&(img[input]!=NULL)){ 
	if (img[output] != NULL) DestroyImage(&img[output]);
	gettimeofday(&tic,NULL);  
	img[output] = AreaClose(img[input],thres);
	gettimeofday(&toc,NULL);
	fprintf(ftime,"AreaClose in %f milliseconds\n",(toc.tv_sec-tic.tv_sec)*1000.0 + (toc.tv_usec-tic.tv_usec)*0.001);
      }
      goto NEXT;
    }

    if (strcmp(action,"AreaOpen")==0) {    
      fscanf(stdin,"%d",&input);
      fscanf(stdin,"%d",&output);
      fscanf(stdin,"%d",&thres);
      if (ValidImage(input)&&ValidImage(output)&&(img[input]!=NULL)){ 
	if (img[output] != NULL) DestroyImage(&img[output]);
	gettimeofday(&tic,NULL);  
	img[output] = AreaOpen(img[input],thres);
	gettimeofday(&toc,NULL);
	fprintf(ftime,"AreaOpen in %f milliseconds\n",(toc.tv_sec-tic.tv_sec)*1000.0 + (toc.tv_usec-tic.tv_usec)*0.001);
      }
      goto NEXT;
    }

    if (strcmp(action,"OpenRec")==0) {    
      fscanf(stdin,"%d",&input);
      fscanf(stdin,"%d",&output);
      fscanf(stdin,"%f",&radius);
      if (ValidImage(input)&&ValidImage(output)&&(img[input]!=NULL)){ 
	if (img[output] != NULL) DestroyImage(&img[output]);
	A = Circular(radius);
	gettimeofday(&tic,NULL);  
	img[output] = OpenRec(img[input],A);
	gettimeofday(&toc,NULL);
	fprintf(ftime,"OpenRec in %f milliseconds\n",(toc.tv_sec-tic.tv_sec)*1000.0 + (toc.tv_usec-tic.tv_usec)*0.001);
	DestroyAdjRel(&A);
      }
      goto NEXT;
    }

    if (strcmp(action,"CloseRec")==0) {    
      fscanf(stdin,"%d",&input);
      fscanf(stdin,"%d",&output);
      fscanf(stdin,"%f",&radius);
      if (ValidImage(input)&&ValidImage(output)&&(img[input]!=NULL)){ 
	if (img[output] != NULL) DestroyImage(&img[output]);
	A = Circular(radius);
	gettimeofday(&tic,NULL);  
	img[output] = CloseRec(img[input],A);
	gettimeofday(&toc,NULL);
	fprintf(ftime,"CloseRec in %f milliseconds\n",(toc.tv_sec-tic.tv_sec)*1000.0 + (toc.tv_usec-tic.tv_usec)*0.001);
	DestroyAdjRel(&A);
      }
      goto NEXT;
    }

    if (strcmp(action,"AsfOCRec")==0) {    
      fscanf(stdin,"%d",&input);
      fscanf(stdin,"%d",&output);
      fscanf(stdin,"%f",&radius);
      if (ValidImage(input)&&ValidImage(output)&&(img[input]!=NULL)){ 
	if (img[output] != NULL) DestroyImage(&img[output]);
	A = Circular(radius);
	gettimeofday(&tic,NULL);  
	img[output] = AsfOCRec(img[input],A);
	gettimeofday(&toc,NULL);
	fprintf(ftime,"AsfOCRec in %f milliseconds\n",(toc.tv_sec-tic.tv_sec)*1000.0 + (toc.tv_usec-tic.tv_usec)*0.001);
	DestroyAdjRel(&A);
      }
      goto NEXT;
    }

    if (strcmp(action,"AsfCORec")==0) {    
      fscanf(stdin,"%d",&input);
      fscanf(stdin,"%d",&output);
      fscanf(stdin,"%f",&radius);
      if (ValidImage(input)&&ValidImage(output)&&(img[input]!=NULL)){ 
	if (img[output] != NULL) DestroyImage(&img[output]);
	A = Circular(radius);
	gettimeofday(&tic,NULL);  
	img[output] = AsfCORec(img[input],A);
	gettimeofday(&toc,NULL);
	fprintf(ftime,"AsfCORec in %f milliseconds\n",(toc.tv_sec-tic.tv_sec)*1000.0 + (toc.tv_usec-tic.tv_usec)*0.001);
	DestroyAdjRel(&A);
      }
      goto NEXT;
    }

    if (strcmp(action,"AsfnOCRec")==0) {    
      fscanf(stdin,"%d",&input);
      fscanf(stdin,"%d",&output);
      fscanf(stdin,"%d",&ntimes);
      if (ValidImage(input)&&ValidImage(output)&&(img[input]!=NULL)){ 
	if (img[output] != NULL) DestroyImage(&img[output]);
	gettimeofday(&tic,NULL);  
	img[output] = AsfnOCRec(img[input],ntimes);
	gettimeofday(&toc,NULL);
	fprintf(ftime,"AsfnOCRec in %f milliseconds\n",(toc.tv_sec-tic.tv_sec)*1000.0 + (toc.tv_usec-tic.tv_usec)*0.001);
      }
      goto NEXT;
    }

    if (strcmp(action,"AsfnCORec")==0) {    
      fscanf(stdin,"%d",&input);
      fscanf(stdin,"%d",&output);
      fscanf(stdin,"%d",&ntimes);
      if (ValidImage(input)&&ValidImage(output)&&(img[input]!=NULL)){ 
	if (img[output] != NULL) DestroyImage(&img[output]);
	gettimeofday(&tic,NULL);  
	img[output] = AsfnCORec(img[input],ntimes);
	gettimeofday(&toc,NULL);
	fprintf(ftime,"AsfnCORec in %f milliseconds\n",(toc.tv_sec-tic.tv_sec)*1000.0 + (toc.tv_usec-tic.tv_usec)*0.001);
      }
      goto NEXT;
    }

    if (strcmp(action,"AsfOCORec")==0) {    
      fscanf(stdin,"%d",&input);
      fscanf(stdin,"%d",&output);
      fscanf(stdin,"%f",&radius);
      if (ValidImage(input)&&ValidImage(output)&&(img[input]!=NULL)){ 
	if (img[output] != NULL) DestroyImage(&img[output]);
	A = Circular(radius);
	gettimeofday(&tic,NULL);  
	img[output] = AsfOCORec(img[input],A);
	gettimeofday(&toc,NULL);
	fprintf(ftime,"AsfOCORec in %f milliseconds\n",(toc.tv_sec-tic.tv_sec)*1000.0 + (toc.tv_usec-tic.tv_usec)*0.001);
	DestroyAdjRel(&A);
      }
      goto NEXT;
    }

    if (strcmp(action,"AsfCOCRec")==0) {    
      fscanf(stdin,"%d",&input);
      fscanf(stdin,"%d",&output);
      fscanf(stdin,"%f",&radius);
      if (ValidImage(input)&&ValidImage(output)&&(img[input]!=NULL)){ 
	if (img[output] != NULL) DestroyImage(&img[output]);
	A = Circular(radius);
	gettimeofday(&tic,NULL);  
	img[output] = AsfCOCRec(img[input],A);
	gettimeofday(&toc,NULL);
	fprintf(ftime,"AsfCOCRec in %f milliseconds\n",(toc.tv_sec-tic.tv_sec)*1000.0 + (toc.tv_usec-tic.tv_usec)*0.001);
	DestroyAdjRel(&A);
      }
      goto NEXT;
    }

    if (strcmp(action,"AsfnOCORec")==0) {    
      fscanf(stdin,"%d",&input);
      fscanf(stdin,"%d",&output);
      fscanf(stdin,"%d",&ntimes);
      if (ValidImage(input)&&ValidImage(output)&&(img[input]!=NULL)){ 
	if (img[output] != NULL) DestroyImage(&img[output]);
	gettimeofday(&tic,NULL);  
	img[output] = AsfnOCORec(img[input],ntimes);
	gettimeofday(&toc,NULL);
	fprintf(ftime,"AsfnOCORec in %f milliseconds\n",(toc.tv_sec-tic.tv_sec)*1000.0 + (toc.tv_usec-tic.tv_usec)*0.001);
      }
      goto NEXT;
    }

    if (strcmp(action,"AsfnCOCRec")==0) {    
      fscanf(stdin,"%d",&input);
      fscanf(stdin,"%d",&output);
      fscanf(stdin,"%d",&ntimes);
      if (ValidImage(input)&&ValidImage(output)&&(img[input]!=NULL)){ 
	if (img[output] != NULL) DestroyImage(&img[output]);
	gettimeofday(&tic,NULL);  
	img[output] = AsfnCOCRec(img[input],ntimes);
	gettimeofday(&toc,NULL);
	fprintf(ftime,"AsfnCOCRec in %f milliseconds\n",(toc.tv_sec-tic.tv_sec)*1000.0 + (toc.tv_usec-tic.tv_usec)*0.001);
      }
      goto NEXT;
    }

    if (strcmp(action,"Leveling")==0) {    
      fscanf(stdin,"%d",&input);
      fscanf(stdin,"%d",&marker);
      fscanf(stdin,"%d",&output);
      if (ValidImage(input)&&ValidImage(marker)&&ValidImage(output)
	  &&(img[input]!=NULL)&&(img[marker]!=NULL)){ 
	if (img[output] != NULL) DestroyImage(&img[output]);
	gettimeofday(&tic,NULL); 	
	img[output] = Leveling(img[input],img[marker]);
	gettimeofday(&toc,NULL);
	fprintf(ftime,"Leveling in %f milliseconds\n",(toc.tv_sec-tic.tv_sec)*1000.0 + (toc.tv_usec-tic.tv_usec)*0.001);
      }
      goto NEXT;
    }

    if (strcmp(action,"Annotate")==0) {    
      fscanf(stdin,"%d",&input);
      fscanf(stdin,"%d",&cost);
      fscanf(stdin,"%d",&label);
      if (ValidImage(input)&&img[input]!=NULL){
	if (aimg != NULL) DeAnnotate(&aimg);
	if (ValidImage(cost)&&(img[cost]!=NULL)&&ValidImage(label)&&(img[label]!=NULL))
	  aimg=Annotate(img[input],img[cost],img[label]);
	else
	  if (ValidImage(cost)&&(img[cost]!=NULL)&&(!ValidImage(label)))
	    aimg=Annotate(img[input],img[cost],NULL);	  
	else
	  if (ValidImage(label)&&(img[label]!=NULL)&&(!ValidImage(cost)))
	    aimg=Annotate(img[input],NULL,img[label]);
	  else
	    if ((!ValidImage(cost))&&(!ValidImage(label)))
	      aimg=Annotate(img[input],NULL,NULL);
      }
      goto NEXT;
    }

    if (strcmp(action,"GetLabel")==0) {    
      fscanf(stdin,"%d",&output);
      if (ValidImage(output)&&(aimg != NULL)){
	if (img[output] != NULL) DestroyImage(&img[output]);
	img[output] = GetLabel(aimg);
      }
      goto NEXT;
    }

    if (strcmp(action,"GetCost")==0) {    
      fscanf(stdin,"%d",&output);
      if (ValidImage(output)&&(aimg != NULL)){
	if (img[output] != NULL) DestroyImage(&img[output]);
	img[output] = GetCost(aimg);
      }
      goto NEXT;
    }

    if (strcmp(action,"GetPred")==0) {    
      fscanf(stdin,"%d",&output);
      if (ValidImage(output)&&(aimg != NULL)){
	if (img[output] != NULL) DestroyImage(&img[output]);
	img[output] = GetPred(aimg);
      }
      goto NEXT;
    }

    if (strcmp(action,"GetRoot")==0) {    
      fscanf(stdin,"%d",&output);
      if (ValidImage(output)&&(aimg != NULL)){
	if (img[output] != NULL) DestroyImage(&img[output]);
	img[output] = GetRoot(aimg);
      }
      goto NEXT;
    }

    if (strcmp(action,"GetSKIZ")==0) {    
      fscanf(stdin,"%d",&output);
      if (ValidImage(output)&&(aimg != NULL)){
	if (img[output] != NULL) DestroyImage(&img[output]);
	img[output] = GetSKIZ(aimg);
      }
      goto NEXT;
    }
    if (strcmp(action,"GetMarker")==0) {    
      fscanf(stdin,"%d",&output);
      if (ValidImage(output)&&(aimg != NULL)){
	if (img[output] != NULL) DestroyImage(&img[output]);
	img[output] = GetMarker(aimg);
      }
      goto NEXT;
    }

    if (strcmp(action,"Clustering")==0) {    
      fscanf(stdin,"%d",&input);
      fscanf(stdin,"%d",&output);
      fscanf(stdin,"%f",&radius);
      fscanf(stdin,"%d",&Co);
      fscanf(stdin,"%d",&Ao);
      if (ValidImage(input)&&(img[input]!=NULL)&&
	  ValidImage(output)){
	if (img[output] != NULL) DestroyImage(&img[output]);
	A=Circular(radius);
	gettimeofday(&tic,NULL);  
	img[output] = Clustering(img[input],A,Co,Ao);
	gettimeofday(&toc,NULL);
	fprintf(ftime,"Clustering in %f milliseconds\n",(toc.tv_sec-tic.tv_sec)*1000.0 + (toc.tv_usec-tic.tv_usec)*0.001);
	DestroyAdjRel(&A);
      }
      goto NEXT;
    }

    if (strcmp(action,"iftWatershed")==0) {    
      fscanf(stdin,"%d",&output);      
      fscanf(stdin,"%f",&radius);
      if ((aimg != NULL)&&ValidImage(output)){
	if (img[output] != NULL) DestroyImage(&img[output]);
	A = Circular(radius);
	gettimeofday(&tic,NULL);  
	img[output]=iftWatershed(aimg,A);
	gettimeofday(&toc,NULL);
	fprintf(ftime,"iftWatershed in %f milliseconds\n",(toc.tv_sec-tic.tv_sec)*1000.0 + (toc.tv_usec-tic.tv_usec)*0.001);
	DestroyAdjRel(&A);
      }
      goto NEXT;
    }

    if (strcmp(action,"IncWater")==0) {    
      fscanf(stdin,"%d",&output);      
      fscanf(stdin,"%f",&radius);
      if ((aimg != NULL)&&ValidImage(output)){
	if (img[output] != NULL) DestroyImage(&img[output]);
	A = Circular(radius);
	gettimeofday(&tic,NULL);  
	img[output]=IncWater(aimg,A);
	gettimeofday(&toc,NULL);
	fprintf(ftime,"IncWater in %f milliseconds\n",(toc.tv_sec-tic.tv_sec)*1000.0 + (toc.tv_usec-tic.tv_usec)*0.001);
	DestroyAdjRel(&A);
      }
      goto NEXT;
    }

    if (strcmp(action,"DecWater")==0) {    
      fscanf(stdin,"%d",&output);      
      fscanf(stdin,"%f",&radius);
      if ((aimg != NULL)&&ValidImage(output)){
	if (img[output] != NULL) DestroyImage(&img[output]);
	A = Circular(radius);
	gettimeofday(&tic,NULL);  
	img[output]=DecWater(aimg,A);
	gettimeofday(&toc,NULL);
	fprintf(ftime,"DecWater in %f milliseconds\n",(toc.tv_sec-tic.tv_sec)*1000.0 + (toc.tv_usec-tic.tv_usec)*0.001);
	DestroyAdjRel(&A);
      }
      goto NEXT;
    }
   
    if (strcmp(action,"WaterLabel")==0) {    
      fscanf(stdin,"%d",&input);      
      fscanf(stdin,"%d",&label);
      fscanf(stdin,"%d",&output);            
      fscanf(stdin,"%f",&radius);
      if (ValidImage(input)&&ValidImage(output)&&ValidImage(label)&&
	  (img[input]!=NULL)&&(img[label]!=NULL)){
	if (img[output] != NULL) DestroyImage(&img[output]);
	A = Circular(radius);
	gettimeofday(&tic,NULL);  
	img[output]=WaterLabel(img[input],img[label],A);
	gettimeofday(&toc,NULL);
	fprintf(ftime,"WaterLabel in %f milliseconds\n",(toc.tv_sec-tic.tv_sec)*1000.0 + (toc.tv_usec-tic.tv_usec)*0.001);
	DestroyAdjRel(&A);
      }
      goto NEXT;
    }

    if (strcmp(action,"WaterBin")==0) {    
      fscanf(stdin,"%d",&input);      
      fscanf(stdin,"%d",&marker);
      fscanf(stdin,"%d",&output);            
      fscanf(stdin,"%f",&radius);
      if (ValidImage(input)&&ValidImage(output)&&ValidImage(marker)&&
	  (img[input]!=NULL)&&(img[marker]!=NULL)){
	if (img[output] != NULL) DestroyImage(&img[output]);
	A = Circular(radius);
	gettimeofday(&tic,NULL);  
	img[output]=WaterBin(img[input],img[marker],A);
	gettimeofday(&toc,NULL);
	fprintf(ftime,"WaterBin in %f milliseconds\n",(toc.tv_sec-tic.tv_sec)*1000.0 + (toc.tv_usec-tic.tv_usec)*0.001);
	DestroyAdjRel(&A);
      }
      goto NEXT;
    }

    if (strcmp(action,"WaterGray")==0) {    
      fscanf(stdin,"%d",&input);      
      fscanf(stdin,"%d",&marker);
      fscanf(stdin,"%d",&output);
      fscanf(stdin,"%f",&radius);
      if (ValidImage(input)&&ValidImage(marker)&&ValidImage(output)&&
	  (img[input]!=NULL)&&(img[marker]!=NULL)){
	if (img[output] != NULL) DestroyImage(&img[output]);
	A = Circular(radius);
	gettimeofday(&tic,NULL);  
	img[output]=WaterGray(img[input],img[marker],A);
	gettimeofday(&toc,NULL);
	fprintf(ftime,"WaterGray in %f milliseconds\n",(toc.tv_sec-tic.tv_sec)*1000.0 + (toc.tv_usec-tic.tv_usec)*0.001);
	DestroyAdjRel(&A);
      }
      goto NEXT;
    }

    if (strcmp(action,"WaterMax")==0) {    
      fscanf(stdin,"%d",&input);      
      fscanf(stdin,"%d",&marker);
      fscanf(stdin,"%d",&output);
      fscanf(stdin,"%f",&radius);
      if (ValidImage(input)&&ValidImage(marker)&&ValidImage(output)&&
	  (img[input]!=NULL)&&(img[marker]!=NULL)){
	if (img[output] != NULL) DestroyImage(&img[output]);
	A = Circular(radius);
	gettimeofday(&tic,NULL);  
	img[output]=WaterMax(img[input],img[marker],A);
	gettimeofday(&toc,NULL);
	fprintf(ftime,"WaterMax in %f milliseconds\n",(toc.tv_sec-tic.tv_sec)*1000.0 + (toc.tv_usec-tic.tv_usec)*0.001);
	DestroyAdjRel(&A);
      }
      goto NEXT;
    }

    if (strcmp(action,"iftSupRec")==0) {    
      fscanf(stdin,"%d",&output);      
      fscanf(stdin,"%f",&radius);
      if ((aimg != NULL)&&ValidImage(output)){
	if (img[output] != NULL) DestroyImage(&img[output]);
	A = Circular(radius);
	gettimeofday(&tic,NULL);  
	img[output]=iftSupRec(aimg,A);
	gettimeofday(&toc,NULL);
	fprintf(ftime,"iftSupRec in %f milliseconds\n",(toc.tv_sec-tic.tv_sec)*1000.0 + (toc.tv_usec-tic.tv_usec)*0.001);
	DestroyAdjRel(&A);
      }
      goto NEXT;
    }

    if (strcmp(action,"iftInfRec")==0) {    
      fscanf(stdin,"%d",&output);      
      fscanf(stdin,"%f",&radius);
      if ((aimg != NULL)&&ValidImage(output)){
	if (img[output] != NULL) DestroyImage(&img[output]);
	A = Circular(radius);
	gettimeofday(&tic,NULL);  
	img[output]=iftInfRec(aimg,A);
	gettimeofday(&toc,NULL);
	fprintf(ftime,"iftInfRec in %f milliseconds\n",(toc.tv_sec-tic.tv_sec)*1000.0 + (toc.tv_usec-tic.tv_usec)*0.001);
	DestroyAdjRel(&A);
      }
      goto NEXT;
    }

    if (strcmp(action,"SupRec")==0) {    
      fscanf(stdin,"%d",&input);
      fscanf(stdin,"%d",&marker);
      fscanf(stdin,"%d",&output);
      fscanf(stdin,"%f",&radius);
      if (ValidImage(input)&&ValidImage(marker)&&ValidImage(output)
	  &&(img[input]!=NULL)&&(img[marker]!=NULL)){ 
	if (img[output] != NULL) DestroyImage(&img[output]);
	A = Circular(radius);
	gettimeofday(&tic,NULL);  
	img[output] = SupRec(img[input],img[marker],A);
	gettimeofday(&toc,NULL);
	fprintf(ftime,"SupRec in %f milliseconds\n",(toc.tv_sec-tic.tv_sec)*1000.0 + (toc.tv_usec-tic.tv_usec)*0.001);
	DestroyAdjRel(&A);
      }
      goto NEXT;
    }

    if (strcmp(action,"InfRec")==0) {    
      fscanf(stdin,"%d",&input);
      fscanf(stdin,"%d",&marker);
      fscanf(stdin,"%d",&output);
      fscanf(stdin,"%f",&radius);
      if (ValidImage(input)&&ValidImage(marker)&&ValidImage(output)
	  &&(img[input]!=NULL)&&(img[marker]!=NULL)){ 
	if (img[output] != NULL) DestroyImage(&img[output]);
	A = Circular(radius);
	gettimeofday(&tic,NULL);  
	img[output] = InfRec(img[input],img[marker],A);
	gettimeofday(&toc,NULL);
	fprintf(ftime,"InfRec in %f milliseconds\n",(toc.tv_sec-tic.tv_sec)*1000.0 + (toc.tv_usec-tic.tv_usec)*0.001);
	DestroyAdjRel(&A);
      }
      goto NEXT;
    }

    if (strcmp(action,"iftFindEdge")==0) {    
      fscanf(stdin,"%f",&radius);
      fscanf(stdin,"%d",&u.x);
      fscanf(stdin,"%d",&u.y);
      fscanf(stdin,"%d",&v.x);
      fscanf(stdin,"%d",&v.y);
      if ((aimg != NULL)&&ValidPixel(aimg->img,u.x,u.y)
	  &&ValidPixel(aimg->img,v.x,v.y)){
	src  = u.x + aimg->img->tbrow[u.y];
	dst  = v.x + aimg->img->tbrow[v.y];
	A = Circular(radius);
	if (path != NULL) {free(path); path=NULL;}
	gettimeofday(&tic,NULL);  
	path = iftFindEdge(aimg,A,src,dst);	
	gettimeofday(&toc,NULL);
	fprintf(ftime,"iftFindEdge in %f milliseconds\n",(toc.tv_sec-tic.tv_sec)*1000.0 + (toc.tv_usec-tic.tv_usec)*0.001);
	DestroyAdjRel(&A);
      }
      goto NEXT;
    }

    if (strcmp(action,"iftFindOEdge")==0) {    
      fscanf(stdin,"%f",&radius);
      fscanf(stdin,"%d",&orientimage);
      fscanf(stdin,"%d",&orientation);
      fscanf(stdin,"%d",&u.x);
      fscanf(stdin,"%d",&u.y);
      fscanf(stdin,"%d",&v.x);
      fscanf(stdin,"%d",&v.y);
      if (ValidImage(orientimage)&&(aimg != NULL)&&
	  ValidPixel(aimg->img,u.x,u.y)&&ValidPixel(aimg->img,v.x,v.y)){
	src  = u.x + aimg->img->tbrow[u.y];
	dst  = v.x + aimg->img->tbrow[v.y];
	A = Circular(radius);
	if (path != NULL) {free(path); path=NULL;}
	gettimeofday(&tic,NULL);  
	path = iftFindOEdge(aimg,A,img[orientimage],orientation,src,dst);	
	gettimeofday(&toc,NULL);
	fprintf(ftime,"iftFindOEdge in %f milliseconds\n",(toc.tv_sec-tic.tv_sec)*1000.0 + (toc.tv_usec-tic.tv_usec)*0.001);
	DestroyAdjRel(&A);
      }
      goto NEXT;
    }

    if (strcmp(action,"AcceptEdge")==0) {    
      if (aimg != NULL)
	AcceptEdge(aimg,path);
      goto NEXT;
    }
      
    if (strcmp(action,"DrawPath")==0) {    
      fscanf(stdin,"%d",&output);
      fscanf(stdin,"%d",&value);
      if ((aimg != NULL)&&ValidImage(output)
	  &&(img[output]!=NULL)&&(path!=NULL)) {
	DrawPath(img[output],aimg->pred,path[1],value);	
	PaintCircle(img[output],path[1],3.0,value);
	PaintCircle(img[output],path[path[0]],3.0,value);
      }
      goto NEXT;
    }

    if (strcmp(action,"Highlight")==0) {    
      fscanf(stdin,"%d",&input);
      fscanf(stdin,"%d",&label);
      fscanf(stdin,"%d",&output);
      fscanf(stdin,"%d",&value);
      if (ValidImage(input)&&ValidImage(label)&&
	  (img[label]!= NULL)&&(img[input] != NULL)&&ValidImage(output)) {
	if (img[output] != NULL) DestroyImage(&img[output]);
	img[output] = Highlight(img[input],img[label],value);
      }
      goto NEXT;
    }

    if (strcmp(action,"PaintCircle")==0) {    
      fscanf(stdin,"%d",&output);
      fscanf(stdin,"%d",&u.x);
      fscanf(stdin,"%d",&u.y);      
      fscanf(stdin,"%d",&value);
      if (ValidImage(output)&&(img[output]!=NULL)&&ValidPixel(img[output],u.x,u.y)){
	p = u.x + img[output]->tbrow[u.y];
	PaintCircle(img[output],p,3.0,value);
      }
      goto NEXT;
    }

    if (strcmp(action,"iftDistTrans")==0) {    
      fscanf(stdin,"%d",&output);
      fscanf(stdin,"%f",&radius);
      fscanf(stdin,"%d",&side);
      if (ValidImage(output)&&(aimg!=NULL)){
	if (img[output] != NULL) DestroyImage(&img[output]);
	A = Circular(radius);      
	gettimeofday(&tic,NULL);  
	aux[0] = iftDistTrans(aimg,A,side);
	gettimeofday(&toc,NULL);
	fprintf(ftime,"iftDistTrans in %f milliseconds\n",(toc.tv_sec-tic.tv_sec)*1000.0 + (toc.tv_usec-tic.tv_usec)*0.001);
	img[output] = SQRT(aux[0]);
	DestroyAdjRel(&A);
	DestroyImage(&aux[0]);
      }
      goto NEXT;
    }

    if (strcmp(action,"iftGeoDist")==0) {    
      if (aimg!=NULL){
	gettimeofday(&tic,NULL);  
	cost = iftGeoDist(aimg);
	gettimeofday(&toc,NULL);
	fprintf(ftime,"iftGeoDist %d in %f milliseconds\n",cost,(toc.tv_sec-tic.tv_sec)*1000.0 + (toc.tv_usec-tic.tv_usec)*0.001);	
      }
      goto NEXT;
    }

    if (strcmp(action,"iftGeoPath")==0) {    
      if (aimg!=NULL){
	if (path != NULL) {free(path); path=NULL;}
	gettimeofday(&tic,NULL);  
	path = iftGeoPath(aimg);
	gettimeofday(&toc,NULL);
	fprintf(ftime,"iftGeoPath in %f milliseconds\n",(toc.tv_sec-tic.tv_sec)*1000.0 + (toc.tv_usec-tic.tv_usec)*0.001);	
      }
      goto NEXT;
    }

    if (strcmp(action,"DistTrans")==0) {    
      fscanf(stdin,"%d",&input);
      fscanf(stdin,"%d",&output);
      fscanf(stdin,"%f",&radius);
      fscanf(stdin,"%d",&side);
      if (ValidImage(input)&&ValidImage(output)&&(img[input]!=NULL)){
	if (img[output] != NULL) DestroyImage(&img[output]);
	A = Circular(radius);      
	gettimeofday(&tic,NULL);  
	aux[0] = DistTrans(img[input],A,side);
	gettimeofday(&toc,NULL);
	fprintf(ftime,"DistTrans in %f milliseconds\n",(toc.tv_sec-tic.tv_sec)*1000.0 + (toc.tv_usec-tic.tv_usec)*0.001);
	img[output] = SQRT(aux[0]);
	DestroyAdjRel(&A);
	DestroyImage(&aux[0]);
      }
      goto NEXT;
    }

    if (strcmp(action,"ShapeFilter")==0) {    
      fscanf(stdin,"%d",&input);
      fscanf(stdin,"%d",&output);
      fscanf(stdin,"%f",&perc);
      if (ValidImage(input)&&ValidImage(output)&&(img[input] != NULL)){
	if (img[output] != NULL) DestroyImage(&img[output]);
	gettimeofday(&tic,NULL);  
	img[output] = ShapeFilter(img[input],perc);
	gettimeofday(&toc,NULL);
	fprintf(ftime,"ShapeFilter in %f milliseconds\n",(toc.tv_sec-tic.tv_sec)*1000.0 + (toc.tv_usec-tic.tv_usec)*0.001);
      }
      goto NEXT;
    }

    if (strcmp(action,"MSSkel")==0) {    
      fscanf(stdin,"%d",&input);
      fscanf(stdin,"%d",&output);
      fscanf(stdin,"%d",&side);
      if (ValidImage(input)&&ValidImage(output)&&(img[input]!=NULL)){
	if (img[output] != NULL) DestroyImage(&img[output]);
	gettimeofday(&tic,NULL);  
	img[output] = MSSkel(img[input],side);
	gettimeofday(&toc,NULL);
	fprintf(ftime,"MSSkel in %f milliseconds\n",(toc.tv_sec-tic.tv_sec)*1000.0 + (toc.tv_usec-tic.tv_usec)*0.001);
      }
      goto NEXT;
    }

    if (strcmp(action,"Skeleton")==0) {    
      fscanf(stdin,"%d",&input);
      fscanf(stdin,"%d",&output);
      fscanf(stdin,"%f",&perc);
      if (ValidImage(input)&&ValidImage(output)&&(img[input]!=NULL)){
	if (img[output] != NULL) DestroyImage(&img[output]);
	img[output] = Skeleton(img[input],perc);
      }
      goto NEXT;
    }

    if (strcmp(action,"SKIZ")==0) {    
      fscanf(stdin,"%d",&input);
      fscanf(stdin,"%d",&output);
      fscanf(stdin,"%d",&side);
      if (ValidImage(input)&&ValidImage(output)&&(img[input]!=NULL)){
	if (img[output] != NULL) DestroyImage(&img[output]);
	gettimeofday(&tic,NULL);  
	img[output] = SKIZ(img[input],side);
	gettimeofday(&toc,NULL);
	fprintf(ftime,"SKIZ in %f milliseconds\n",(toc.tv_sec-tic.tv_sec)*1000.0 + (toc.tv_usec-tic.tv_usec)*0.001);
      }
      goto NEXT;
    }

    if (strcmp(action,"MSFractal")==0) {    
      fscanf(stdin,"%d",&input);
      fscanf(stdin,"%d",&maxdist);
      fscanf(stdin, "%d", &degree);
      fscanf(stdin, "%d", &lower);
      fscanf(stdin, "%d", &higher);
      fscanf(stdin, "%d", &nbins);
      fscanf(stdin, "%d", &reg);
      fscanf(stdin, "%lf", &from);
      fscanf(stdin, "%lf", &to);
      if (ValidImage(input)&&(img[input]!=NULL)){
	if (fractal != NULL) DestroyCurve(&fractal);
	gettimeofday(&tic,NULL);  
	P = MSFractal(img[input],maxdist, degree, lower, higher, reg, from, to);
	fractal = PolynomToFractalCurve(P, lower, higher, nbins);
	DestroyPolynom(&P);
	gettimeofday(&toc,NULL);
	fprintf(ftime,"MSFractal in %f milliseconds\n",(toc.tv_sec-tic.tv_sec)*1000.0 + (toc.tv_usec-tic.tv_usec)*0.001);
      }
      goto NEXT;
    }

    if (strcmp(action,"SkelSaliences")==0) {    
      fscanf(stdin,"%d",&input);
      fscanf(stdin,"%d",&output);
      fscanf(stdin,"%d",&maxdist);
      fscanf(stdin,"%d",&angle);      
      if (ValidImage(input)&&(img[input] != NULL)&&ValidImage(output)){
	if (img[output] != NULL) DestroyImage(&img[output]);
	if (saliences != NULL) DestroyCurve3D(&saliences);
	gettimeofday(&tic,NULL);  
	saliences = SkelSaliences(img[input],maxdist,angle);
	gettimeofday(&toc,NULL);
	fprintf(ftime,"SkelSaliences in %f milliseconds\n",(toc.tv_sec-tic.tv_sec)*1000.0 + (toc.tv_usec-tic.tv_usec)*0.001);	
	img[output] = PaintSaliences(img[input],saliences);
      }
      goto NEXT;
    }

    if (strcmp(action,"SkelCont")==0) {    
      fscanf(stdin,"%d",&input);
      fscanf(stdin,"%d",&output);
      fscanf(stdin,"%d",&maxdist);
      fscanf(stdin,"%d",&thres);
      fscanf(stdin,"%d",&angle);      
      fscanf(stdin,"%d",&side);
      if (ValidImage(input)&&(img[input] != NULL)&&ValidImage(output)){
	if (img[output] != NULL) DestroyImage(&img[output]);
	if (saliences != NULL) DestroyCurve3D(&saliences);
	gettimeofday(&tic,NULL);  
	saliences = SkelCont(img[input],maxdist,thres,angle, side);
	gettimeofday(&toc,NULL);
	fprintf(ftime,"SkelCont in %f milliseconds\n",(toc.tv_sec-tic.tv_sec)*1000.0 + (toc.tv_usec-tic.tv_usec)*0.001);	
	aux[0]=MSSkel(img[input],side);
	aux[1]=Skeleton(aux[0],thres);
	img[output] = PaintSaliences(aux[1],saliences);
	DestroyImage(&aux[0]);
	DestroyImage(&aux[1]);
      }
      goto NEXT;
    }

    if (strcmp(action,"ContSaliences")==0) {    
      fscanf(stdin,"%d",&input);
      fscanf(stdin,"%d",&output);
      fscanf(stdin,"%d",&maxdist);
      fscanf(stdin,"%d",&thres);
      fscanf(stdin,"%d",&angle);
      if (ValidImage(input)&&(img[input] != NULL)&&ValidImage(output)){
	if (img[output] != NULL) DestroyImage(&img[output]);
	if (saliences != NULL) DestroyCurve3D(&saliences);
	gettimeofday(&tic,NULL);  
	saliences = ContSaliences(img[input],maxdist, thres, angle);
	gettimeofday(&toc,NULL);
	fprintf(ftime,"ContSaliences in %f milliseconds\n",(toc.tv_sec-tic.tv_sec)*1000.0 + (toc.tv_usec-tic.tv_usec)*0.001);	
	aux[0]  = LabelContour(img[input]);
	img[output] = PaintSaliences(aux[0],saliences);
	DestroyImage(&aux[0]);
      }
      goto NEXT;
    }

    if (strcmp(action,"LabelSkel")==0) {    
      fscanf(stdin,"%d",&input);
      fscanf(stdin,"%d",&output);
      fscanf(stdin,"%d",&option);
      if (ValidImage(input)&&(img[input] != NULL)&&ValidImage(output)){
	if (img[output] != NULL) DestroyImage(&img[output]);
	gettimeofday(&tic,NULL);  
	img[output] = LabelSkel(img[input],saliences, option);
	gettimeofday(&toc,NULL);
	fprintf(ftime,"LabelSkel in %f milliseconds\n",(toc.tv_sec-tic.tv_sec)*1000.0 + (toc.tv_usec-tic.tv_usec)*0.001);	
      }
      goto NEXT;
    }

    if (strcmp(action,"WriteCurve")==0) {    
      fscanf(stdin,"%s",curve);
      fscanf(stdin,"%s",filename);
      if ((strcmp(curve,"hist")==0)&&(hist!=NULL)) {    
	WriteCurve(hist,filename);
      } else{
	if ((strcmp(curve,"fractal")==0)&&(fractal!=NULL)) {    
	  WriteCurve(fractal,filename);
	}
      }
      goto NEXT;
    }

    if (strcmp(action,"WriteLaTex")==0) {    
      fscanf(stdin,"%s",filename); /* WITH extension */
      fscanf(stdin,"%f",&radius);
      if (aimg != NULL) {
	A = Circular(radius);
	WriteLaTex(aimg,A,filename);
	DestroyAdjRel(&A);
      }
      goto NEXT;
    }

    if (strcmp(action,"WriteEPS")==0) {    
      char figname[100];
      fscanf(stdin,"%s",figname); /* WITHOUT extension */
      fscanf(stdin,"%f",&radius);
      if (aimg != NULL) {
	A = Circular(radius);
	WriteEPS(aimg,A,figname);
	DestroyAdjRel(&A);
      }
      goto NEXT;
    }

    if (strcmp(action,"WriteCurve3D")==0) {    
      fscanf(stdin,"%s",filename);
      if (saliences != NULL){
	WriteCurve3D(saliences,filename);
      }
      goto NEXT;
    }

    if (strcmp(action,"WriteSaliences")==0) {    
      fscanf(stdin,"%s",filename);
      fscanf(stdin,"%d",&i);
      if (saliences != NULL){
	WriteSaliences(saliences,filename,i);
      }
      goto NEXT;
    }
      
  NEXT: fscanf(stdin,"%s",action);
  
  }

  if (aimg != NULL) DeAnnotate(&aimg);
  for (i=0; i < NIMG; i++) {
    if (img[i] != NULL) DestroyImage(&img[i]);
  }
  if (hist      != NULL) DestroyCurve(&hist);
  if (fractal   != NULL) DestroyCurve(&fractal);
  if (saliences != NULL) DestroyCurve3D(&saliences);
  if (path      != NULL) free(path);
  if (set != NULL) DestroySet(&set);


#ifdef _LINUX_
  info = mallinfo();
  MemDinFinal = info.uordblks;
  if (MemDinInicial!=MemDinFinal)
    printf("\n\nDinamic memory was not completely deallocated (%d, %d)\n",
	   MemDinInicial,MemDinFinal);   
#endif 

  fclose(ftime);  
  return 0;

}

void WriteEPS(AnnImg *aimg, AdjRel *A, char *figname) 
{
  Pixel u,v;
  int i, ifile, pass;
  double unit = 32.0; /* Pixel spacing in pt */
  double mm = 72/25.4; /* One mm in pt. */
  double hx = 0.18*unit; /* Label x displacement in pt. */
  double hy = 0.25*unit; /* Label y displacement in pt. */

  for (ifile = 0; ifile < 2; ifile++)
    { 
      char *filename = txtcat(figname, (ifile ==0 ? "-a.eps" : "-b.eps"));
      FILE *fp = fopen(filename, "w"); 
      ps_begin_figure(fp, 0.0, unit*(aimg->img->ncols), 0.0, unit*(aimg->img->nrows));
      ps_set_label_font(fp, "TimesRoman", 18.0);
      ps_set_pen(fp, 0,0,0, 0.15, 0,0);
      for (pass = 0; pass < 2; pass++)
	{
	  for (u.y=0; u.y < aimg->img->nrows; u.y++)
    	    {
    	      for (u.x=0; u.x < aimg->img->ncols; u.x++) 
    		{
    		  int p = u.x + aimg->img->tbrow[u.y];
    		  double ux = unit*(u.x + 0.25);
    		  double uy = unit*(aimg->img->nrows-1 - u.y + 0.25);
    		  bool proot = (aimg->pred->val[p] == NIL);
    		  double pradius = unit*(proot ? 0.150 : 0.100);  /* radius in pt */
                  if (pass == 0)
                    {
		      /* Draw arrows: */
		      for (i=1; i < A->n; i++) 
			{
			  v.x = u.x + A->dx[i];
			  v.y = u.y + A->dy[i];
			  if (ValidPixel(aimg->img,v.x,v.y))
			    {
			      int q  = v.x + aimg->img->tbrow[v.y]; /* Index of neighbor. */
			      /* Decide whether arc should be drawn: */
			      bool drawit;
			      if (ifile == 0)
				{ drawit = true; }
			      else
				{ drawit = (q == aimg->pred->val[p]); }
			      if (drawit)
				{
				  bool qroot = (aimg->pred->val[q] == NIL);
				  double qradius = unit*(qroot ? 0.150 : 0.100);  /* radius in pt */
				  double vx = unit*(v.x+0.25);
				  double vy = unit*(aimg->img->nrows-1 - v.y + 0.25);
				  /* A vector along the arc: */
				  double dlm = hypot(vx - ux, vy - uy);
				  double dlx = (vx - ux)/dlm;
				  double dly = (vy - uy)/dlm;
				  /* A vector orthogonal to the arc: */
				  double dtx = -dly;
				  double dty = dlx;
				  /* Arrowhead tip: */
				  double ax = vx - qradius*dlx;
				  double ay = vy - qradius*dly;
				  /* Arrowhead corners: */
				  double bx = ax - unit*(0.15*dlx + 0.08*dtx);
				  double by = ay - unit*(0.15*dly + 0.08*dty);
				  double cx = ax - unit*(0.15*dlx - 0.08*dtx);
				  double cy = ay - unit*(0.15*dly - 0.08*dty);
				  /* Draw arc from p to q: */
				  ps_draw_segment(fp, ux,uy, vx,vy);
				  /* Paint arrowhead: */
				  ps_fill_triangle(fp, ax, ay, bx, by, cx, cy,  0,0,0);
				} 
			    }
			}
		    }
                  else
		    { 
		      int value = (ifile == 0 ? aimg->img->val[p] : aimg->cost->val[p]); /* Image value to show. */
		      char *cval = fmtint(value,1);
		      int dhx, dhy;
		      /* Paint dot and image value: */
                      ps_set_pen(fp, 1,1,1, 0.15, 0,0);
		      for (dhy = -1; dhy <= 1; dhy++)
                        for (dhx = -1; dhx <= 1; dhx++)
                          ps_put_label(fp, cval, ux+hx+1.0*dhx, uy+hy+1.0*dhy, 0,0);
		      ps_set_pen(fp, 0,0,0, 0.15, 0,0);
		      ps_put_label(fp, cval, ux+hx, uy+hy, 0,0);    
		      ps_fill_dot(fp, ux,uy,pradius/mm,0,0,0);
                      free(cval);
		    }
    		}
    	    }
	}
      ps_end_figure (fp);
      fclose(fp);
      free(filename);
    }
}
