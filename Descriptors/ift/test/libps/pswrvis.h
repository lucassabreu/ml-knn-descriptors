/* pswrvis.h -- window visibility culling procs (in device coordinates). */
/* Last edited on 2003-01-08 21:27:09 by stolfi */

#ifndef pswrvis_H
#define pswrvis_H

#include <pswr.h>
#include <stdio.h>

/* These procedures return TRUE if the figure described by the arguments
  lies certainly outside the current clip path. They return FALSE
  when the figure *may* be visible.
  
  All coordinates are in the device coordinate system; all
  lengths are in pt. */

int pswr_segment_is_invisible
  ( PSWriter *pswr,
    double xa, double ya, 
    double xb, double yb
  );

int pswr_curve_is_invisible
  ( PSWriter *pswr,
    double xa, double ya, 
    double xb, double yb, 
    double xc, double yc, 
    double xd, double yd
  );

int pswr_rectangle_is_invisible
  ( PSWriter *pswr,
    double xlo, double xhi, 
    double ylo, double yhi
  );

int pswr_triangle_is_invisible
  ( PSWriter *pswr,
    double xa, double ya, 
    double xb, double yb, 
    double xc, double yc
  );

int pswr_polygon_is_invisible
  ( PSWriter *pswr,
    double x[], double y[], int npoints
  );

int pswr_circle_is_invisible
  ( PSWriter *pswr,
    double xc, double yc, double rad
  );

int pswr_lune_is_invisible
  ( PSWriter *pswr,
    double xc, double yc, double rad, 
    double tilt
  );

int pswr_slice_is_invisible
  ( PSWriter *pswr,
    double xc, double yc, 
    double rad, 
    double start, double stop
  );
  
#endif
