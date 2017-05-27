/* See aagraph.h */
/* Last edited on 2003-01-08 20:58:21 by stolfi */

#include "aagraph.h"
#include <js.h>
#include <flt.h>
#include <ia.h>
#include <aa.h>
#include <ps.h>
#include <math.h>
#include <ioprotos.h>
#include <stdio.h>

void aagraph_plot_paralelograms(
    FILE *psfile,
    AAP f (AAP x),
    Float xmin, Float xmax,
    Float ymin, Float ymax,
    int n
  )
  {
    Interval xv, yvlo, yvhi;
    int xi;
    AAP xf, yf;
    double xp[4], yp[4];
    AATerm eps[1];
    double gray = 0.75;

    ps_comment(psfile, "Plot of AA graph");

    for (xi=0; xi<n; xi++)
      {
        MemP frame = aa_top();

        ROUND_DOWN;
        xv.lo = xmin + ((xmax - xmin)*xi)/n;

        ROUND_UP;
        xv.hi = xmin + ((xmax - xmin)*(xi+1))/n;

        xf = aa_from_interval(xv);
        assert((xf->nterms == 1), "aagraph_plot_paralelograms: xf->nterms != 1");
        eps[0].id = ((AATermP)(xf + 1))->id;

        yf = f(xf);
        
        eps[0].coef = -One;
        yvlo = aa_range(aa_fix_eps(yf, 1, eps));
        eps[0].coef = One;
        yvhi = aa_range(aa_fix_eps(yf, 1, eps));

        if (ia_is_full(&yvlo) || ia_is_full(&yvhi)) 
          { yvlo = (Interval){ymin, ymax}; yvhi = yvlo; }

        ROUND_NEAR;
        xp[0] = xv.lo;  yp[0] = yvlo.lo;
        xp[1] = xv.hi;  yp[1] = yvhi.lo;
        xp[2] = xv.hi;  yp[2] = yvhi.hi;
        xp[3] = xv.lo;  yp[3] = yvlo.hi;
        
        ps_fill_and_draw_polygon(psfile, xp, yp, 4, gray,gray,gray);

        aa_flush(frame);
      }

    fprintf(stderr, "\n");
  }

void aagraph_plot_boxes(
    FILE *psfile,
    AAP f (AAP x),
    Float xmin, Float xmax,
    Float ymin, Float ymax,
    int n
  )
  {
    Interval xv, yv;
    int xi;
    AAP xf, yf;
    double gray = 0.75;

    ps_comment(psfile, "Plot of AA range graph");

    for (xi=0; xi<n; xi++)
      {
        MemP frame = aa_top();

        ROUND_DOWN;
        xv.lo = xmin + ((xmax - xmin)*xi)/n;

        ROUND_UP;
        xv.hi = xmin + ((xmax - xmin)*(xi+1))/n;

        xf = aa_from_interval(xv);

        yf = f(xf);
        yv = aa_range(yf);

        if (ia_is_full(&yv)) { yv = (Interval){ymin, ymax}; }

        ROUND_NEAR;
        ps_fill_and_draw_rectangle(psfile, xv.lo, xv.hi, yv.lo, yv.hi, gray,gray,gray);

        aa_flush(frame);
      }

    fprintf(stderr, "\n");
  }
