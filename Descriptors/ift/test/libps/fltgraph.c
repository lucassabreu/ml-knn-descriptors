/* See fltgraph.h */
/* Last edited on 2003-01-08 20:58:08 by stolfi */

#include "fltgraph.h"
#include <js.h>
#include <flt.h>
#include <ps.h>
#include <math.h>
#include <stdio.h>
#include <stdlib.h>

void fltgraph_plot
  ( FILE *psfile,
    Float f (Float x),
    Float xmin, Float xmax,
    Float ymin, Float ymax,
    int m
  )
  { int xi;
    Float x0, y0, x1, y1;

    ROUND_NEAR;
    ps_comment(psfile, "Plot of actual graph");

    x1 = xmin;
    y1 = f(x1);

    for (xi=0; xi<m; xi++)
      { x0 = x1;
        y0 = y1;

        x1 = xmin + ((xmax - xmin)*(xi+1))/m;
        y1 = f(x1);

        if 
          ( ((abs(y1) < Infinity) && (abs(y0) < Infinity)) &&
            ((y0 >= ymin) || (y1 >= ymin)) &&
            ((y0 <= ymax) || (y1 <= ymax))
          )
          { ps_draw_segment(psfile, x0, y0, x1, y1); }
      }
  }

void fltgraph_draw_axes
  ( FILE *psfile,
    Float xmin, Float xmax,
    Float ymin, Float ymax
  )
  { if ((xmin < Zero) && (xmax > Zero))
      { ps_draw_coord_line(psfile, 'x', 0.0); }
    if ((ymin < Zero) && (ymax > Zero))
      { ps_draw_coord_line(psfile, 'y', 0.0); }
  }

