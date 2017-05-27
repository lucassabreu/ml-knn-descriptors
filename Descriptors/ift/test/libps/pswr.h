/* pswr.h -- new package for generating Postscript graphics files. */
/* Last edited on 2003-01-08 21:41:57 by stolfi */

#ifndef pswr_H
#define pswr_H

#include <stdio.h>
#include <pswrdef.h>

/* POSTRCRIPT WRITER OBJECT */

typedef PSWriterPrivate PSWriter;

/* A PSWriter is an opaque object that contains a "FILE *" pointer,
  and some state information.  The graphics operations below 
  write the corresponding Postscript commands to that file. */ 

PSWriter *pswr_new_writer(FILE *psfile);
  /* Creates a new PSWriter record for file "psfile".
    Be sure to call pswr_begin_figure or pswr_begin_document
    to initialize it, and pswr_end_figureor pswr_end_document
    to finalize it. */

void pswr_delete_writer(PSWriter *pswr);
  /* Reclaims the PSWriter record  "*pswr". 
    Assumes that it has been properly finalized. */

/* There are two kinds of Postscript (PS) files, `encapsulated'
  (EPS) and `non-encapsulated' (non-EPS).
  
  An EPS file contains a single figure, of arbitrary size. It has a
  "%%BoundingBox" line at the beginning, no "%%Page" lines, and no
  explicit "show" command. It is usually meant to be included in other
  documents, and may confuse some printers.
  
  A non-EPS file contains a complete document, that uses the entire
  area of the page, and possibly multiple pages It has "%%Page"
  comments, explicit "show" commands at the end of each page, and no
  "%%BoundingBox" comment. It is usually meant to be printed by
  itself, and cannot be easily included in other documents. */

/* ENCAPSULATED FIGURES */

void pswr_begin_figure
  ( PSWriter *pswr,
    double hmin, double hmax,
    double vmin, double vmax
  );
  /* Initializes the underlying writer as an Encapsulated PostScript file.
    
    Writes the file's preamble, which defines some auxiliary
    Postscript operators and constants, sets up the caption font, etc.
    Also writes the parameters "hmin,hmax,vmin,vmax" (in pt) as the
    file's "%%BoundingBox" comment.  The plotting window is 
    initialized to cover the whole figure, with client coordinates 
    equal to the device coordinates. */
    
void pswr_end_figure(PSWriter *pswr);
  /* Finalizes an Encapsulated PostScript figure. The file
    is flushed but is left open. */

/* NON-ENCAPSULATED MULTIPAGE DOCUMENTS */

void pswr_begin_document(PSWriter *pswr, const char *papersize);
  /* Initializes a file to contain a non-encapsulated, possibly
    multi-page Postscript document.
    
    The procedure writes the file preamble, which defines some
    auxiliary Postscript operators and constants, defines the 
    caption font, etc.

    The parameter "papersize" indicates the paper size of the
    document. Valid values are: "a1" "a2" "a3" "a4" "letter".
    Other values may not be recognized by the printer.
    [R. Chencarek dec/2002]
    
    The client must initialize each page with ps_begin_page, before
    issuing any drawing commands, and finalize it with ps_end_page. */

void pswr_begin_page(PSWriter *pswr, const char *page);
  /* Starts a new page in a non-encapsulated document.  
  
    The "page" string is the logical page number; if not NULL and
    non-empty, it will be printed at the bottom of the page. It is is
    also used as the first argument of the "%%Page" structuring
    directive (where it defaults to the sequential page number,
    counting from 1). 
    
    At each new page, the plotting window is reset to the whole page, 
    and the whole Postscript state is restored to some initial state. */

void pswr_end_page(PSWriter *pswr);
  /* Finalizes a page in a non-encapsulated document. */

void pswr_end_document(PSWriter *pswr);
  /* Finalizes a multipage document.  The file is flushed but left open. */

/* PLOTTING WINDOW */

void pswr_set_window
  ( PSWriter *pswr,
    double xmin, double xmax,
    double ymin, double ymax,

    double hmin, double hmax,
    double vmin, double vmax,

    int xn, int yn
  );
  /* Changes the coordinate system and clip path for a new drawing
    inside the figure (EPS file) or inside the current page (non-EPS file).

    After this call, client coordinates will range over "[xmin _ xmax]
    x [ymin _ ymax]". The nominal plotting area will be "[hmin _ hmax]
    x [vmin _ vmax]", in pt, relative to the lower left corner of the
    figure (EPS) or the current page (non-EPS). The plotting scales
    "dh/dx" and "dv/dy" must be equal.
    
    The caption cursor is positioned to the first text line under the
    figure, aligned with its left margin. Except for pswr_add_caption
    and pswr_frame, all graphics commands will be clipped to the
    plotting window.

    The plotting area is divided implicitly into a grid of /xn/ by /yn/
    rectangular "cells", which are used by some of the plotting commands
    below. */
  
void pswr_get_paper_dimensions(const char *papersize, double *xpt, double *ypt);
  /* Sets *xpt and *ypt to the dimensions of the specified paper type,
    in points. Knows about US sizes "letter", "ledger", "tabloid",
    "legal", "executive", and the ISO "A" sizes (from "4A0" to
    "A10"). */

/* DRAWING COMMANDS */

void pswr_set_pen 
  ( PSWriter *pswr,
    double r, double g, double b,
    double width,
    double dashlength,
    double dashspace
  );
  /* Sets pen parameters and ink color for line/outline drawing.
    Dimensions are in *millimeters* */

void pswr_segment
  ( PSWriter *pswr,
    double xa, double ya,
    double xb, double yb
  );
  /* Draws segment from (xa,ya) to (xb,yb). */
  
void pswr_curve
  ( PSWriter *pswr,
    double xa, double ya,
    double xb, double yb,
    double xc, double yc,
    double xd, double yd
  );
  /* Draws a Bezier arc with given control points. */
    
void pswr_x_coord_line (PSWriter *pswr, double coord);
void pswr_y_coord_line (PSWriter *pswr, double coord);
  /* Draws a reference line perpendicular to the given axis 
    at the given coordinate value. */

void pswr_grid_lines(PSWriter *pswr);
  /* Draws the cell boundaries with the current pen and color. */

void pswr_frame (PSWriter *pswr);
  /* Draws the outline of the current plotting area. The outline 
    will be drawn half-inside, half-outside the area. */

/* FIGURE FILLING & DRAWING COMMANDS */

/* For all commands of this section, if the "fill" argument
  is true, the specified figure is painted with the 
  current fill color; then, if "draw" is true, the outline
  of the figure is drawn with the current pen parameters. */

void pswr_set_fill_color(PSWriter *pswr, double r, double g, double b);
  /* Defines the fill color for subsequent filling operations on "pswr". */

void pswr_rectangle
  ( PSWriter *pswr,
    double xlo, double xhi,
    double ylo, double yhi,
    int fill, int draw
  );
  /* Fills and/or outlines the given rectangle. */
  
void pswr_triangle
  ( PSWriter *pswr,
    double xa, double ya,
    double xb, double yb,
    double xc, double yc,
    int fill, int draw
  );
  /* Fills and/or outlines the triangle with corners "a,b,c". */
   
void pswr_polygon
  ( PSWriter *pswr,
    double x[], double y[],
    int npoints,
    int fill, int draw
  );
  /* Fills and/or outlines the polygon (x[1],y[1]),.. (x[n],y[n]). */
  
void pswr_circle
  ( PSWriter *pswr,
    double xc, double yc, double rad,
    int fill, int draw
  );
  /* Fills and/or outlines the circle with given center and radius. */
    
void pswr_dot
  ( PSWriter *pswr,
    double xc, double yc, double rad,
    int fill, int draw
  );
  /* Same as pswr_circle, except that the radius is in millimeters,
    irrespective of the current scale. */
  
void pswr_lune
  ( PSWriter *pswr,
    double xc, double yc, double rad, double tilt,
    int fill, int draw
  );
  /* Fills and/or outlines the lune with given center, radius, and tilt.
    (A "lune" is the intersection of two circles with the 
    given radius, and with their centers spaced "rad" apart.) */
  
void pswr_slice
  ( PSWriter *pswr,
    double xc, double yc, double rad,
    double start, double stop,
    int fill, int draw
  );
  /* Fills and/or outlines the pie slice with given center, 
    radius, and angle range. */
    
void pswr_grid_cell
  ( PSWriter *pswr, 
    int xi, int yi,
    int fill, int draw
  );
  /* Fills and/or outlines cell "[xi,yy]" of the curent grid.
     Cell "[0,0]" lies at the bottom left corner. */
    
/* TEXT PRINTING */

void pswr_set_label_font(PSWriter *pswr, const char *font, double size);
  /* Sets the name and point size of the font to be used by pswr_label. */

void pswr_label
  ( PSWriter *pswr, 
    const char *text, 
    double x, double y, 
    double xalign, double yalign
  );
  /* Prints "label" at point (x,y), using the current label font size. 
    The parameter "xalign" (resp. "yalign)" specifies which point of the string's 
    bounding box will end up at (x,y): 0.0 means the left (resp. bottom) side,
    1.0 means the right (resp. top) side.  Default is (0.5, 0.5), meaning 
    the box will be centered at (x,y). */

void pswr_add_caption(PSWriter *pswr, const char *txt);
  /* Adds a caption text below the drawing, *outside* the nominal
    bounding box. */

/* MISCELLANEOUS */ 

void pswr_comment (PSWriter *pswr, const char *title);
  /* Writes a Postscript comment line to the underlying file.
    It is intended for documentation and debugging
    purposes only; it has no visible effect on the printed page. */

#endif
