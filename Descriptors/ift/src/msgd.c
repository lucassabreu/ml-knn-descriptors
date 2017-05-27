#include "msgd.h"

MSGDReg *CreateMSGDReg(int n, int m, Image *grad1, Set *S, int ncols2, int nrows2)
{
  MSGDReg *msgd = (MSGDReg *)calloc(1,sizeof(MSGDReg));
  int j;

  msgd->theta = AllocFloatArray(n);
  msgd->delta = (float **) calloc(m,sizeof(float *));
  for (j=0; j < m; j++) 
    msgd->delta[j] = (float *) calloc(n,sizeof(float));
  msgd->n      = n;
  msgd->m      = m;
  msgd->grad1  = grad1;
  msgd->S      = S;
  msgd->nrows2 = nrows2;
  msgd->ncols2 = ncols2;

  return(msgd);
}

void  DestroyMSGDReg(MSGDReg **msgd)
{
  int j;

  if (*msgd != NULL) {
    free((*msgd)->theta);
    for (j=0; j < (*msgd)->m; j++) 
      free((*msgd)->delta[j]);
    free((*msgd)->delta);
    free(*msgd);
    *msgd=NULL;
  }
}

void  SetThetaMSGDReg(MSGDReg *msgd, char *theta)
{
  int i;

  for (i=0; i < msgd->n; i++){ 
    sscanf(theta,"%f",&msgd->theta[i]);
    theta=strchr(theta,',')+1;
  }
}

void  SetDeltaMSGDReg(MSGDReg *msgd, char *delta)
{
  int i,j;

  for (j=0; j < msgd->m; j++) 
    for (i=0; i < msgd->n; i++){ 
      sscanf(delta,"%f",&msgd->delta[j][i]);
      delta=strchr(delta,',')+1;
    }
}

Set   *WatershedLines(Image *label)
{
  Set *S=NULL;
  int p,q,i;
  Pixel u,v;
  AdjRel *A=Circular(1.0);

  for (u.y=0; u.y < label->nrows; u.y++) 
    for (u.x=0; u.x < label->ncols; u.x++) {
      p = u.x + label->tbrow[u.y];
      for (i=1; i < A->n; i++){
	v.x = u.x + A->dx[i];
	v.y = u.y + A->dy[i];
	if (ValidPixel(label,v.x,v.y)){
	  q = v.x + label->tbrow[v.y];
	  if (label->val[q]!=label->val[p]){
	    InsertSet(&S,p);
	    break;
	  }
	}
      }
    }
  DestroyAdjRel(&A);
  return(S);
}

// theta[i]: alpha, Tx, Ty, Sx, Sy

void MatchingTransform(float T[3][3], float *theta, MSGDReg *msgd)
{
  double trash, sin_alpha, cos_alpha,yc=msgd->nrows2/2.0,xc=msgd->ncols2/2.0;
  float R[3][3],S[3][3],T1[3][3],T2[3][3],T3[3][3],M1[3][3],M2[3][3];  
  int i,j,k;

  theta[0] = 360.*modf(theta[0]/360.,&trash);
  if (theta[0] < 0.0)
    theta[0] += 360.0;
  theta[0] = theta[0]*PI/180.0;

  cos_alpha = cos(theta[0]);
  sin_alpha = sin(theta[0]);

  R[0][0]=cos_alpha;   R[0][1]=sin_alpha;   R[0][2]=0.0; 
  R[1][0]=-sin_alpha;  R[1][1]=cos_alpha;   R[1][2]=0.0; 
  R[2][0]=0.0;         R[2][1]=0.0;         R[2][2]=1.0; 

  S[0][0]=theta[3];    S[0][1]=0.0;         S[0][2]=0.0; 
  S[1][0]=0.0;         S[1][1]=theta[4];    S[1][2]=0.0; 
  S[2][0]=0.0;         S[2][1]=0.0;         S[2][2]=1.0; 

  T1[0][0]=1.0;         T1[0][1]=0.0;         T1[0][2]=theta[1]; 
  T1[1][0]=0.0;         T1[1][1]=1.0;         T1[1][2]=theta[2]; 
  T1[2][0]=0.0;         T1[2][1]=0.0;         T1[2][2]=1.0; 

  T2[0][0]=1.0;         T2[0][1]=0.0;         T2[0][2]=-xc; 
  T2[1][0]=0.0;         T2[1][1]=1.0;         T2[1][2]=-yc; 
  T2[2][0]=0.0;         T2[2][1]=0.0;         T2[2][2]=1.0; 

  T3[0][0]=1.0;         T3[0][1]=0.0;         T3[0][2]=msgd->grad1->ncols/2.0; 
  T3[1][0]=0.0;         T3[1][1]=1.0;         T3[1][2]=msgd->grad1->nrows/2.0; 
  T3[2][0]=0.0;         T3[2][1]=0.0;         T3[2][2]=1.0; 
 
  // M1 = R T2

  for (k=0; k < 3; k++)
    for (j=0; j < 3; j++)
      M1[k][j] = 0.0;

  for (k=0; k < 3; k++)
    for (j=0; j < 3; j++)
      for (i=0; i < 3; i++)
	M1[k][j]+=R[k][i]*T2[i][j];

  // M2 = S M1

  for (k=0; k < 3; k++)
    for (j=0; j < 3; j++)
      M2[k][j] = 0.0;

  for (k=0; k < 3; k++)
    for (j=0; j < 3; j++)
      for (i=0; i < 3; i++)
	M2[k][j]+=S[k][i]*M1[i][j];

  // M1 = T1 M2

  for (k=0; k < 3; k++)
    for (j=0; j < 3; j++)
      M1[k][j] = 0.0;

  for (k=0; k < 3; k++)
    for (j=0; j < 3; j++)
      for (i=0; i < 3; i++)
	M1[k][j]+=T1[k][i]*M2[i][j];

  // T = T3 M1

  for (k=0; k < 3; k++)
    for (j=0; j < 3; j++)
      T[k][j] = 0.0;

  for (k=0; k < 3; k++)
    for (j=0; j < 3; j++)
      for (i=0; i < 3; i++)
	T[k][j]+=T3[k][i]*M1[i][j];

}

// theta[i]: alpha, Tx, Ty, Sx, Sy

void MatchingInverseTransform(float T[3][3], float *theta, MSGDReg *msgd)
{
  double trash, sin_alpha, cos_alpha,yc=msgd->grad1->nrows/2.0,xc=msgd->grad1->ncols/2.0;
  float R[3][3],S[3][3],T1[3][3],T2[3][3],T3[3][3],M1[3][3],M2[3][3];  
  int i,j,k;

  theta[0] = 360.*modf(theta[0]/360.,&trash);
  if (theta[0] < 0.0)
    theta[0] += 360.0;
  theta[0] = theta[0]*PI/180.0;

  cos_alpha = cos(theta[0]);
  sin_alpha = sin(theta[0]);

  R[0][0]=cos_alpha;   R[0][1]=-sin_alpha;  R[0][2]=0.0; 
  R[1][0]=sin_alpha;   R[1][1]=cos_alpha;   R[1][2]=0.0; 
  R[2][0]=0.0;         R[2][1]=0.0;         R[2][2]=1.0; 

  S[0][0]=1.0/theta[3];S[0][1]=0.0;         S[0][2]=0.0; 
  S[1][0]=0.0;         S[1][1]=1.0/theta[4];S[1][2]=0.0; 
  S[2][0]=0.0;         S[2][1]=0.0;         S[2][2]=1.0; 

  T1[0][0]=1.0;         T1[0][1]=0.0;         T1[0][2]=-theta[1]; 
  T1[1][0]=0.0;         T1[1][1]=1.0;         T1[1][2]=-theta[2]; 
  T1[2][0]=0.0;         T1[2][1]=0.0;         T1[2][2]=1.0; 

  T2[0][0]=1.0;         T2[0][1]=0.0;         T2[0][2]=-xc; 
  T2[1][0]=0.0;         T2[1][1]=1.0;         T2[1][2]=-yc; 
  T2[2][0]=0.0;         T2[2][1]=0.0;         T2[2][2]=1.0; 

  T3[0][0]=1.0;         T3[0][1]=0.0;         T3[0][2]=msgd->ncols2/2.0; 
  T3[1][0]=0.0;         T3[1][1]=1.0;         T3[1][2]=msgd->nrows2/2.0; 
  T3[2][0]=0.0;         T3[2][1]=0.0;         T3[2][2]=1.0; 
 
  // M1 = R T2

  for (k=0; k < 3; k++)
    for (j=0; j < 3; j++)
      M1[k][j] = 0.0;

  for (k=0; k < 3; k++)
    for (j=0; j < 3; j++)
      for (i=0; i < 3; i++)
	M1[k][j]+=R[k][i]*T2[i][j];

  // M2 = S M1

  for (k=0; k < 3; k++)
    for (j=0; j < 3; j++)
      M2[k][j] = 0.0;

  for (k=0; k < 3; k++)
    for (j=0; j < 3; j++)
      for (i=0; i < 3; i++)
	M2[k][j]+=S[k][i]*M1[i][j];

  // M1 = T1 M2

  for (k=0; k < 3; k++)
    for (j=0; j < 3; j++)
      M1[k][j] = 0.0;

  for (k=0; k < 3; k++)
    for (j=0; j < 3; j++)
      for (i=0; i < 3; i++)
	M1[k][j]+=T1[k][i]*M2[i][j];

  // T = T3 M1

  for (k=0; k < 3; k++)
    for (j=0; j < 3; j++)
      T[k][j] = 0.0;

  for (k=0; k < 3; k++)
    for (j=0; j < 3; j++)
      for (i=0; i < 3; i++)
	T[k][j]+=T3[k][i]*M1[i][j];

}

int MSGDMatching(float *theta, float *delta, int i, int dir, MSGDReg *msgd)
{
  float T[3][3],*theta_prime=AllocFloatArray(msgd->n);
  int k,similarity,p;
  Set *S;
  float x2,y2,x1,y1;

  if (delta == NULL) {
    MatchingTransform(T,theta,msgd);
  } else {    
    if (i==-1){
      for (k=0; k < msgd->n; k++) 
	theta_prime[k] = theta[k]+delta[k];
    }else{
      for (k=0; k < msgd->n; k++) 
	theta_prime[k] = theta[k];
      theta_prime[i] += dir*delta[i];
    }
    MatchingTransform(T,theta_prime,msgd);
  }

  S = msgd->S; similarity=0.0;
  while (S!=NULL) {
    x2 = S->elem%msgd->ncols2;
    y2 = S->elem/msgd->ncols2;
    x1 = T[0][0]*x2 + T[0][1]*y2 + T[0][2];
    y1 = T[1][0]*x2 + T[1][1]*y2 + T[1][2];
    if (ValidPixel(msgd->grad1,(int)x1,(int)y1)){
      p  = (int)x1 + msgd->grad1->tbrow[(int)y1];
      similarity += msgd->grad1->val[p];
    }else {
      similarity -= 1000;
    }
    S = S->next;
  }

  free(theta_prime);

  return(similarity);
}

float *MSGDSearchTransform(MSGDReg *msgd)
{
  float *theta_star = AllocFloatArray(msgd->n);
  float *delta_star = AllocFloatArray(msgd->n);
  int   VF_star=0,VF=0,VF0=0,VF1=0,VF2=0,i,j;  

  VF_star = MSGDMatching(msgd->theta,NULL,-1,0,msgd);
  for (i=0; i < msgd->n; i++) theta_star[i]=msgd->theta[i];
  do {
    VF0 = VF_star; 
    for (i=0; i < msgd->n; i++) msgd->theta[i]=theta_star[i];

    for (j=0; j < msgd->m; j++) {
      for (i=0; i < msgd->n; i++) {
	VF = VF0;
	delta_star[i]=0;
	VF1 = MSGDMatching(msgd->theta,msgd->delta[j],i,1,msgd);
	VF2 = MSGDMatching(msgd->theta,msgd->delta[j],i,-1,msgd);
	if (VF1 > VF) { VF = VF1; delta_star[i] = msgd->delta[j][i];}
	if (VF2 > VF) { delta_star[i] = -msgd->delta[j][i];}
      }	
      VF = MSGDMatching(msgd->theta,delta_star,-1,0,msgd);
      if (VF > VF_star) {
	VF_star = VF;
	for (i=0; i < msgd->n; i++) theta_star[i] = msgd->theta[i] + delta_star[i];
      }
    }    
  } while (VF_star > VF0);
  
  printf("VF_star %d\n",VF_star);

  free(delta_star);

  return(theta_star);
}

CImage *RegisterCImages(CImage *cimg1, CImage *cimg2, float perc_volume)
{
  Image   *grad1=NULL, *grad2=NULL, *hand2=NULL, *label2=NULL;
  CImage  *cimg3=NULL;
  AdjRel  *A=NULL;
  Set     *S=NULL;
  MSGDReg *msgd=NULL;
  float   *theta_star=NULL,T[3][3];
  int      p1,p2,n;
  float    x1,y1,x2,y2;

  // Compute gradients and watershed lines

  grad1  = CFeatureGradient(cimg1,1000);
  grad2  = CFeatureGradient(cimg2,1000);
  hand2   = Add(grad2,perc_volume*1000);
  //    hand2 = CTVolumeClose(grad2,2000);
  A      = Circular(1.5);
  label2 = WaterGray(grad2,hand2,A);
  S      = WatershedLines(label2);
  if (S==NULL) {
    DestroyImage(&grad1);
    DestroyImage(&label2);
    DestroyImage(&hand2);
    DestroyImage(&grad2);
    DestroyAdjRel(&A);
    return(NULL);
  }

  DestroyImage(&label2);
  DestroyImage(&hand2);
  DestroyImage(&grad2);
  DestroyAdjRel(&A);

  // Initialize MSGD

  msgd = CreateMSGDReg(5,19,grad1,S,cimg2->C[0]->ncols,cimg2->C[0]->nrows); 
  SetThetaMSGDReg(msgd,"0.0,0.0,0.0,1.0,1.0"); // identity transform


  SetDeltaMSGDReg(msgd,"0.5,0.0,0.0,0.05,0.05,5.0,0.0,0.0,0.1,0.1,10.0,0.0,0.0,0.2,0.2,20.0,0.0,0.0,0.25,0.25,30.0,0.0,0.0,0.3,0.3,40.0,0.0,0.0,0.35,0.35,50.0,0.0,0.0,0.4,0.4,60.0,0.0,0.0,0.5,0.5,70.0,0.0,0.0,0.55,0.55,80.0,0.0,0.0,0.6,0.6,90.0,0.0,0.0,0.65,0.65,110.0,0.0,0.0,0.7,0.7,120.0,0.0,0.0,0.75,0.75,130.0,0.0,0.0,0.8,0.8,140.0,0.0,0.0,0.85,0.85,150.0,0.0,0.0,0.9,0.9,160.0,0.0,0.0,0.95,0.95,170.0,0.0,0.0,0.15,0.15,180.0,0.0,0.0,0.0.03,0.03"); // displacements for each parameter and scale

  // Compute MSGD

  theta_star = MSGDSearchTransform(msgd);

  printf("%f %f %f %f %f\n",theta_star[0],theta_star[1],theta_star[2],theta_star[3],theta_star[4]);

  // Compute registered image

  MatchingInverseTransform(T,theta_star,msgd);

  cimg3=CreateCImage(cimg1->C[0]->ncols,cimg1->C[0]->nrows);
  n    = cimg1->C[0]->ncols*cimg1->C[0]->nrows;
  for (p1=0; p1 < n; p1++) {
    x1 = p1%cimg1->C[0]->ncols;
    y1 = p1/cimg1->C[0]->ncols;
    x2 = T[0][0]*x1 + T[0][1]*y1 + T[0][2];
    y2 = T[1][0]*x1 + T[1][1]*y1 + T[1][2];
    if (ValidPixel(cimg2->C[0],(int)x2,(int)y2)){
      p2  = (int)x2 + cimg2->C[0]->tbrow[(int)y2];
      cimg3->C[0]->val[p1] = cimg2->C[0]->val[p2];
      cimg3->C[1]->val[p1] = cimg2->C[1]->val[p2];
      cimg3->C[2]->val[p1] = cimg2->C[2]->val[p2];
    }
  }

  free(theta_star);
  DestroyMSGDReg(&msgd);
  DestroyImage(&grad1);
  DestroySet(&S);
  return(cimg3);
}


