#include "feature3.h"

Features3* CreateFeatures3(int xsize, int ysize, int zsize, int nfeats)
{
    int i;
    Features3 *f=(Features3 *)calloc(1,sizeof(Features3));

    f->Imax = 0;

    f->xsize  = xsize;
    f->ysize  = ysize;
    f->zsize  = zsize;
    f->nelems = f->xsize*f->ysize*f->zsize;
    f->nfeats = nfeats;
    f->elem   = (FElem *)calloc(f->nelems,sizeof(FElem));

    for (i=0; i < f->nelems; i++) {
      f->elem[i].feat = AllocFloatArray(f->nfeats);
    }

    return f;
}

void DestroyFeatures3(Features3 **f)
{
    int i;

    if ((*f)!=NULL)
    {
        for (i=0; i < (*f)->nelems; i++)
            free((*f)->elem[i].feat);
        free((*f)->elem);
        free(*f);
        (*f)=NULL;
    }
}


Features3 *MedianSceneFeats(Scene *scn, Scene *mask)
{
  Features3 *f=(Features3 *)calloc(1,sizeof(Features3));
  AdjRel3   *A=NULL;
  int       p,q,i,j,xysize=scn->xsize*scn->ysize,aux;
  Voxel     u,v;
  int       *val=NULL;

  f->Imax = MaximumValue3(scn);

  A=Spheric(1.8);
  val = AllocIntArray(A->n);
  f->xsize  = scn->xsize;
  f->ysize  = scn->ysize;
  f->zsize  = scn->zsize;
  f->nelems = f->xsize*f->ysize*f->zsize;
  f->nfeats = 3;

  f->elem   = (FElem *)calloc(f->nelems,sizeof(FElem));
  for (i=0; i < f->nelems; i++) {
    f->elem[i].feat = AllocFloatArray(f->nfeats);
  }
  for (p=0; p < f->nelems; p++) {
    aux = p%xysize;
    u.x = aux%scn->xsize;
    u.y = aux/scn->xsize;
    u.z = p/xysize;
    for (j=0; j < A->n; j++) {
      v.x = u.x + A->dx[j];
      v.y = u.y + A->dy[j];
      v.z = u.z + A->dz[j];
      q = v.x + scn->tby[v.y] + scn->tbz[v.z];
      if ( (ValidVoxel(scn,v.x,v.y,v.z)) &&
	   ( ( mask == NULL ) ||
	     ( mask->data[q] != 0 ) ) ) {
	val[j]=scn->data[q];
      }
      else {
	val[j]=scn->data[p];
      }
    }
    SelectionSort(val,A->n,INCREASING);
    f->elem[p].feat[0]=(float)val[A->n/2-1]/f->Imax;
    f->elem[p].feat[1]=(float)val[A->n/2]/f->Imax;
    f->elem[p].feat[2]=(float)val[A->n/2+1]/f->Imax;
  }

  DestroyAdjRel3(&A);
  free(val);
  return(f);
}

Features3 *MarkovSceneFeats(Scene *scn, Scene *mask, float dm)
{
  Features3 *f=(Features3 *)calloc(1,sizeof(Features3));
  AdjRel3   *A=NULL;
  int       p,q,i,j,xysize=scn->xsize*scn->ysize,aux;
  Voxel     u,v;

  f->Imax = MaximumValue3(scn);

  if (dm >= 1.0) {
    A=Spheric(dm);
    f->xsize  = scn->xsize;
    f->ysize  = scn->ysize;
    f->zsize  = scn->zsize;
    f->nelems = f->xsize*f->ysize*f->zsize;
    f->nfeats = A->n;
    f->elem   = (FElem *)calloc(f->nelems,sizeof(FElem));
    for (i=0; i < f->nelems; i++) {
      f->elem[i].feat = AllocFloatArray(f->nfeats);
    }
    for (p=0; p < f->nelems; p++) {
      aux = p%xysize;
      u.x = aux%scn->xsize;
      u.y = aux/scn->xsize;
      u.z = p/xysize;
      for (j=0; j < A->n; j++) {
	v.x = u.x + A->dx[j];
	v.y = u.y + A->dy[j];
	v.z = u.z + A->dz[j];
	q = v.x + scn->tby[v.y] + scn->tbz[v.z];
	if ( (ValidVoxel(scn,v.x,v.y,v.z)) &&
	     ( ( mask == NULL ) ||
	       ( mask->data[q] != 0 ) ) ) {
	  f->elem[p].feat[j]=(float)scn->data[q]/f->Imax;
	}
	else {
	  f->elem[p].feat[j]=(float)scn->data[p]/f->Imax;
	}
      }
    }
    DestroyAdjRel3(&A);
  }
  else {
    f->xsize  = scn->xsize;
    f->ysize  = scn->ysize;
    f->zsize  = scn->zsize;
    f->nelems = scn->n;
    f->nfeats = 1;
    f->elem   = (FElem *)calloc(f->nelems,sizeof(FElem));
    for (i=0; i < f->nelems; i++) {
      f->elem[i].feat = AllocFloatArray(f->nfeats);
    }
    for (p=0; p < f->nelems; p++) {
      f->elem[p].feat[0]=(float)scn->data[p]/f->Imax;
    }
  }
  return(f);
}

Features3 *MarkovDoubleSceneFeats( Scene *scn1, Scene *scn2, Scene *mask, float dm ) {
  Features3 *f = ( Features3 * )calloc( 1, sizeof( Features3 ) );
  AdjRel3   *A = NULL;
  int       p, q, i, j;
  Voxel     u, v;
  int       max1, max2;

  max1 = MaximumValue3( scn1 );
  max2 = MaximumValue3( scn2 );
  if( max1 > max2 ) f->Imax = max1;
  else f->Imax = max2;

  if( dm >= 1.0 ) {
    A=Spheric( dm );
    f->xsize  = scn1->xsize;
    f->ysize  = scn1->ysize;
    f->zsize  = scn1->zsize;
    f->nelems = scn1->n;
    f->nfeats = 2 * A->n;
    f->elem   = ( FElem * )calloc( f->nelems, sizeof( FElem ) );
    for( i = 0; i < f->nelems; i++ ) {
      f->elem[ i ].feat = AllocFloatArray( f->nfeats );
    }

    for( p = 0; p < f->nelems; p++ ) {
      u.x = VoxelX( scn1, p );
      u.y = VoxelY( scn1, p );
      u.z = VoxelZ( scn1, p );
      for( j = 0; j < A->n; j++ ) {
	v.x = u.x + A->dx[ j ];
	v.y = u.y + A->dy[ j ];
	v.z = u.z + A->dz[ j ];
	q = VoxelAddress( scn1, v.x, v.y, v.z );
	if( ( ValidVoxel( scn1, v.x, v.y, v.z ) ) && ( ( mask == NULL ) || ( mask->data[ q ] != 0 ) ) ) {
	  f->elem[ p ].feat[ j ]= ( float ) scn1->data[ q ] / max1;
	  f->elem[ p ].feat[ A->n + j ] = ( float ) scn2->data[ q ] / max2;
	}
	else {
	  f->elem[ p ].feat[ j ] = ( float ) scn1->data[ p ] / max1;
	  f->elem[ p ].feat[ A->n + j ] = ( float ) scn2->data[ p ] / max2;
	}
      }
    }
    DestroyAdjRel3( &A );
  }
  else {
    f->xsize  = scn1->xsize;
    f->ysize  = scn1->ysize;
    f->zsize  = scn1->zsize;
    f->nelems = scn1->n;
    f->nfeats = 2;
    f->elem   = ( FElem * ) calloc( f->nelems, sizeof( FElem ) );
    for( i = 0; i < f->nelems; i++ ) {
      f->elem[ i ].feat = AllocFloatArray( f->nfeats );
    }

    for( p = 0; p < f->nelems; p++ ) {
      f->elem[ p ].feat[ 0 ] = ( float ) scn1->data[ p ] / max1;
      f->elem[ p ].feat[ 1 ] = ( float ) scn2->data[ p ] / max2;
    }
  }

  return( f );
}

Features3 *CoOccur3(Scene *orig, Scene *mask, int nfeats)
{
    Features3 *f=(Features3 *)calloc(1,sizeof(Features3));
    int     i,j,p,q,n=orig->xsize*orig->ysize*orig->zsize;
    int     tmp, qmin[ 8 ], dmin[ 8 ], aux;
    Voxel   u,v;
    AdjRel3 *A;
    A = Spheric( 1.0 );
    f->Imax = INT_MIN;
    for ( p = 0; p < n; p++ )
    {
        orig->data[ p ] = orig->data[ p ] * mask->data[ p ];
        if ( mask->data[ p ] )
        {
            if ( orig->data[ p ] > f->Imax ) f->Imax = orig->data[ p ];
        }
    }
    f->xsize  = orig->xsize;
    f->ysize  = orig->ysize;
    f->zsize  = orig->zsize;
    f->nelems = f->xsize*f->ysize*f->zsize;
    f->nfeats = nfeats;
    f->elem   = (FElem *)calloc(f->nelems,sizeof(FElem));
    for (i=0; i < f->nelems; i++)
    {
        f->elem[i].feat = AllocFloatArray(f->nfeats);
    }
    for (p=0; p < n; p++)
    {
        if (mask->data[p])
        {
            f->elem[p].feat[0]=(float)orig->data[p]/(float)f->Imax;
            u.x  = VoxelX(orig, p);
            u.y  = VoxelY(orig, p);
            u.z  = VoxelZ(orig, p);
            for ( i = 0; i < f->nfeats - 1; i++ )
            {
                dmin[ i ] = INT_MAX;
                qmin[ i ] = p;
            }
            for (i=1;i<A->n;i++)
            {
                v.x = u.x + A->dx[i];
                v.y = u.y + A->dy[i];
                v.z = u.z + A->dz[i];
                if (ValidVoxel(orig, v.x,v.y,v.z))
                {
                    q = VoxelAddress(orig,v.x,v.y,v.z);
                    if (mask->data[q])
                    {
                        tmp=abs(orig->data[p]-orig->data[q]);
                        for ( j = 0; j < f->nfeats - 1; j++ )
                        {
                            if ( tmp < dmin[ j ] )
                            {
                                aux = dmin[ j ];
                                dmin[ j ] = tmp;
                                tmp = aux;
                                aux = qmin[ j ];
                                qmin[ j ] = q;
                                q = aux;
                            }
                        }
                    }
                }
            }
            for ( i = 0; i < f->nfeats - 1; i++ )
            {
                f->elem[ p ].feat[ i + 1 ] = ( float ) orig->data[ qmin[ i ] ] / ( float ) f->Imax;
            }
        }
    }
    DestroyAdjRel3(&A);
    return(f);
}
