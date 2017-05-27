/* aagraph.h -- Plots graph of unary function with affine arithmetic. */
/* Last edited on 2002-12-29 16:05:16 by stolfi */

#ifndef aagraph_H
#define aagraph_H

#include <flt.h>
#include <aa.h>
#include <ia.h>
#include <stdio.h>

void aagraph_plot_paralelograms
  ( FILE *psfile,
    AAP f (AAP x),
    Float xmin, Float xmax,
    Float ymin, Float ymax,
    int n
  );
  /* Writes to $psfile$ a PostScript plot of the graph of f(x),
    computed on $n$ equal sub-intervals in $[xmin .. xmax]$.
    Specifically, converts each sub-interval $xi$ into an affine form
    $xa$, with a new noise variable, and then plots the paralogram
    defined by the forms $(xa, f(xa))$.
    
    Assumes that $psfile$ has been properly initialized for a plot
    with the given coordinate ranges. */

void aagraph_plot_boxes
  ( FILE *psfile,
    AAP f (AAP x),
    Float xmin, Float xmax,
    Float ymin, Float ymax,
    int n
  );
  /* Writes to $psfile$ a PostScript plot of the graph of f(x),
    computed on $n$ equal sub-intervals in $[xmin .. xmax]$.
    Specifically, converts each sub-interval $xi$ into an affine form
    $xa$, with a new noise variable, and then plots the rectangle
    defined by the intervals $(xi, aa_range(f(xa)))$.
    
    Assumes that $psfile$ has been properly initialized for a plot
    with the given coordinate ranges. */

#endif
