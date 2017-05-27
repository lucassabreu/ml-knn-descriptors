/* See pswrvis.h */
/* Last edited on 2003-01-08 21:34:17 by stolfi */

#include <pswr.h>
#include <pswrvis.h>
#include <stdio.h>
#include <values.h>

#define Sqrt3 1.73205080756887729352

#define DEFWINDOW \
    double xmin = pswr->hmin - 3.0; \
    double xmax = pswr->hmax + 3.0; \
    double ymin = pswr->vmin - 3.0; \
    double ymax = pswr->vmax + 3.0

#define INITBBOX \
    double xlo = MAXDOUBLE; \
    double xhi = -MAXDOUBLE; \
    double ylo = MAXDOUBLE; \
    double yhi = -MAXDOUBLE
    
#define XBOX(x) { if ((x) < xlo) { xlo = (x); } if ((x) > xhi) { xhi = (x); } }
#define YBOX(y) { if ((y) < ylo) { ylo = (y); } if ((y) > yhi) { yhi = (y); } }

#define CHECKBOX \
    return ((xlo > xmax) || (xhi < xmin) || (ylo > ymax) || (yhi < ymin))

int pswr_segment_is_invisible
  ( PSWriter *pswr,
    double xa, double ya, 
    double xb, double yb
  )
  { DEFWINDOW;
    INITBBOX;
    XBOX(xa); YBOX(ya);
    XBOX(xb); YBOX(yb);
    CHECKBOX;
  }

int pswr_curve_is_invisible
  ( PSWriter *pswr,
    double xa, double ya, 
    double xb, double yb, 
    double xc, double yc, 
    double xd, double yd
  )
  { DEFWINDOW;
    INITBBOX;
    XBOX(xa); YBOX(ya);
    XBOX(xb); YBOX(yb);
    XBOX(xc); YBOX(yc);
    XBOX(xd); YBOX(yd);
    CHECKBOX;
  }

int pswr_rectangle_is_invisible
  ( PSWriter *pswr,
    double xlo, double xhi, 
    double ylo, double yhi
  )
  { DEFWINDOW;
    CHECKBOX;
  }

int pswr_triangle_is_invisible
  ( PSWriter *pswr,
    double xa, double ya, 
    double xb, double yb, 
    double xc, double yc
  )
  { DEFWINDOW;
    INITBBOX;
    XBOX(xa); YBOX(ya);
    XBOX(xb); YBOX(yb);
    XBOX(xc); YBOX(yc);
    CHECKBOX;
  }

int pswr_polygon_is_invisible
  ( PSWriter *pswr,
    double x[], double y[], int npoints
  )
  { DEFWINDOW;
    INITBBOX;
    int i;
    for (i = 0; i < npoints; i++) { XBOX(x[i]); YBOX(y[i]); }
    CHECKBOX;
  }

int pswr_circle_is_invisible
  ( PSWriter *pswr,
    double xc, double yc, double rad
  )
  { DEFWINDOW;
    INITBBOX;
    XBOX(xc-rad); YBOX(yc-rad);
    XBOX(xc+rad); YBOX(yc+rad);
    CHECKBOX;
  }


int pswr_lune_is_invisible
  ( PSWriter *pswr,
    double xc, double yc, double rad, 
    double tilt
  )
  { DEFWINDOW;
    INITBBOX;
    /* The tip of the lune lies "rad*sqrt(3)" away from center: */ 
    double height = rad*Sqrt3;
    XBOX(xc-height); YBOX(yc-height);
    XBOX(xc+height); YBOX(yc+height);
    CHECKBOX;
  }

int pswr_slice_is_invisible
  ( PSWriter *pswr,
    double xc, double yc, 
    double rad, 
    double start, double stop
  )
  { DEFWINDOW;
    INITBBOX;
    /* Use whole circle, igoring "start,stop": */
    XBOX(xc-rad); YBOX(yc-rad);
    XBOX(xc+rad); YBOX(yc+rad);
    CHECKBOX;
  }
