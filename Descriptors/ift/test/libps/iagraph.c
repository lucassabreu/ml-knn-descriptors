/* Graph of an interval-valued function */
/* Last edited on 2003-01-08 20:57:43 by stolfi */

#include "iagraph.h"
#include <js.h>
#include <flt.h>
#include <ia.h>
#include <ps.h>
#include <math.h>
#include <ioprotos.h>
#include <stdio.h>

void iagraph_plot
  ( FILE *psfile,
    Interval f (Interval x),
    Float xmin, Float xmax,
    Float ymin, Float ymax,
    int n
  )
  { Interval xv, yv;
    int xi;
    double gray = 0.75;

    ps_comment(psfile, "Plot of IA graph");

    for (xi=0; xi<n; xi++)
      {
        ROUND_DOWN;
        xv.lo = xmin + ((xmax - xmin)*xi)/n;

        ROUND_UP;
        xv.hi = xmin + ((xmax - xmin)*(xi+1))/n;

        yv = f(xv);

        if (ia_is_full(&yv)) { yv = (Interval){ymin, ymax}; }

        ROUND_NEAR;
        ps_fill_and_draw_rectangle(psfile, xv.lo, xv.hi, yv.lo, yv.hi, gray,gray,gray);

      }

    fprintf(stderr, "\n");
  }
