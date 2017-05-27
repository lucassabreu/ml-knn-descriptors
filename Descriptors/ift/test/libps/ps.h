/* Routines for creating PostScript files with graphics and text. */
/* Last edited on 2003-01-08 21:43:14 by stolfi */

#ifndef ps_H
#define ps_H

#include <stdio.h>

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

/*** ENCAPSULATED FIGURES ***/

void ps_begin_figure 
  ( FILE *psfile,
    double hmin, double hmax,
    double vmin, double vmax
  );
  /* Initializes an Encapsulated PostScript file, by writing to it a
    suitable preamble.
    
    The procedure writes the file's preamble, which defines some
    auxiliary Postscript operators and constants, sets up the caption
    font, etc. 
    
    The procedure also writes the parameters "hmin,hmax,vmin,vmax" (in
    pt) as the file's "%%BoundingBox" comment. */

void ps_end_figure (FILE *psfile);
  /* Finalizes an Encapsulated PostScript figure.
    The file is flushed but is left open. */
   
/*** NON-ENCAPSULATED MULTIPAGE DOCUMENTS ***/

void ps_begin_document (FILE *psfile, const char *papersize);
  /* Initializes a file to contain a non-encapsulated, possibly
    multi-page Postscript document.
    
    The procedure writes the file preamble, which defines some
    auxiliary Postscript operators and constants, defines the 
    caption font, etc.

    The parameter "papersize" indicates the paper size of the
    document. Valid values are: "a1" "a2" "a3" "a4" "letter".
    Other values may not be recognized by the printer.
    [R. Chencarek dec/2002]
    
    The client must call ps_begin_page and ps_end_page around each
    page. of the document, and ps_set_window before each separate
    drawing on the same page. */

void ps_begin_page(FILE *psfile, const char *page);
  /* Starts a new page of a PS document.  
  
    The "page" string
    is the logical page number, shown at the bottom of the page
    and as the first argument of the "%%Page" directive.
    If NULL or "", the page number is not printed, and 
     defaults to the sequential page number, counting from 1. */

void ps_end_page(FILE *psfile);
  /* Finalizes a page: Writes page trailer line, etc. */

void ps_end_document (FILE *psfile);
  /* Finalizes a multipage document. The file is flushed but is left open. */
 
void ps_get_paper_dimensions(const char *papersize, double *xpt, double *ypt);
  /* Sets *xpt and *ypt to the dimensions of the specified paper type,
    in points. Knows about US sizes "letter", "ledger", "tabloid",
    "legal", "executive", and the ISO "A" sizes (from "4A0" to
    "A10"). */

/* COORDINATE SETUP */
  
void ps_set_window
  ( FILE *psfile,
    double xmin, double xmax,
    double ymin, double ymax,

    double hmin, double hmax,
    double vmin, double vmax,

    int xn, int yn
  );  
  /* Sets up the coordinate system and clip path for a new drawing
    inside the figure (EPS file) or the current page (non-EPS file).

    Client coordinates will range over [xmin _ xmax] x [ymin _ ymax].
    The nominal plotting area is [hmin _ hmax] x [vmin _ vmax], in pt,
    relative to the lower left corner of the figure (EPS) or the
    current page (non-EPS). The plotting scales dh/dx and dv/dy must
    be equal.

    The plotting area is divided implicitly into a grid of /xn/ by /yn/
    rectangular "cells", which are used by some of the plotting commands
    below. */

/*** DRAWING COMMANDS ***/

void ps_set_pen 
  ( FILE *psfile,
    double r, double g, double b,
    double width,
    double dashlength,
    double dashspace
  );
  /* Sets pen parameters and ink color for line drawing.
    Dimensions are in *millimeters* */

void ps_draw_segment
  ( FILE *psfile,
    double xa, double ya,
    double xb, double yb
  );
  /* Draws segment from (xa,ya) to (xb,yb) with current pen and color */

void ps_draw_curve
  ( FILE *psfile,
    double xa, double ya,
    double xb, double yb,
    double xc, double yc,
    double xd, double yd
  );
  /* Draws a Bezier arc with given control points, using the current
    pen and color. */
  
void ps_draw_coord_line (FILE *psfile, char axis, double coord);
  /* Draws a reference line perpendicular to the given axis 
    at the given coordinate value. */

void ps_draw_grid_lines(FILE *psfile);
  /* Draws the cell boundaries with the current pen and color. */

void ps_draw_frame (FILE *psfile);
  /* Draws a frame around the plotting area. (The frame 
    will extend half a line width outside the nominal bounding box.) */

void ps_draw_rectangle
  ( FILE *psfile,
    double xlo, double xhi,
    double ylo, double yhi
  );
  /* Draws the outline of the given rectangle using the current pen. */

void ps_fill_rectangle
  ( FILE *psfile,
    double xlo, double xhi,
    double ylo, double yhi,
    double r, double g, double b
  );
  /* Fills given rectangle with given color */

void ps_fill_and_draw_rectangle
  ( FILE *psfile,
    double xlo, double xhi,
    double ylo, double yhi,
    double r, double g, double b
  );
  /* Fills rectangle with given color, then draws its outline with
    current pen. */

void ps_fill_circle
  ( FILE *psfile,
    double xc, double yc, double radius,
    double r, double g, double b
  );
  /* Fills the circle with given center and radius, using the 
    given color. */
 
void ps_fill_dot
  ( FILE *psfile,
    double xc, double yc, double radius,
    double r, double g, double b
  );
  /* Same as ps_fill_circle, but the radius is in millimeters,
    irrespective of the current scale. */
 
void ps_draw_circle
  ( FILE *psfile,
    double xc, double yc, double radius
  );
  /* Draws the circle with given center and radius, using the current
    pen and color. */
  
void ps_draw_dot
  ( FILE *psfile,
    double xc, double yc, double radius
  );
  /* Same as ps_draw_circle, but the radius is in millimeters,
    irrespective of the current scale. */
  
void ps_fill_and_draw_circle
  ( FILE *psfile,
    double xc, double yc, double radius,
    double r, double g, double b
  );
  /* Fills the circle with given center and radius, using the given
    color, then draws its outline, using the current pen and color. */
  
void ps_fill_and_draw_dot
  ( FILE *psfile,
    double xc, double yc, double radius,
    double r, double g, double b
  );
  /* Same as ps_fill_and_draw_circle, but the radius is in
    millimeters, irrespective of the current scale. */

void ps_fill_and_draw_lune
  ( FILE *psfile,
    double xc, double yc, double radius, double tilt,
    double r, double g, double b
  );
  /* Fills the lune with given center, radius, and tilt, using the
    given color, then draws its outline, using the current pen and
    color. */
  
void ps_fill_and_draw_slice
  ( FILE *psfile,
    double xc, double yc, double radius, double start, double stop,
    double r, double g, double b
  );
  /* Fills the pie slice with given center, radius, and angle range,
    using the given color, then draws its outline, using the current
    pen and color. */
    
void ps_fill_polygon
  ( FILE *psfile,
    double x[], double y[],
    int n,
    double r, double g, double b
  );
  /* Fills the polygon (x[1],y[1]),.. (x[n],y[n]) with the given
    color. */
  
void ps_draw_polygon
  ( FILE *psfile,
    double x[], double y[],
    int n
  );
  /* Draws the contour of the polygon (x[1],y[1]),.. (x[n],y[n]) 
    using the current pen and color. */
  
void ps_fill_and_draw_polygon
  ( FILE *psfile,
    double x[], double y[],
    int n,
    double r, double g, double b
  );
  /* Fills the polygon (x[1],y[1]),.. (x[n],y[n]) with the given color,
    then draws its contour using the current pen and color. */
  
void ps_fill_triangle
  ( FILE *psfile,
    double xa, double ya,
    double xb, double yb,
    double xc, double yc,
    double r, double g, double b
  );
  /* Fills triangle /abc/ with given color. */

void ps_fill_grid_cell(FILE *psfile, int xi, int yi, double r, double g, double b);
  /* Fills the given cell of the current cell grid with the given
    color. */

void ps_set_label_font(FILE *psfile, const char *font, double size);
  /* Sets the name and point size of the font to be used by ps_put_label. */

void ps_put_label
  ( FILE *psfile, 
    const char *text, 
    double x, double y, 
    double xalign, double yalign
  );
  /* Prints "label" at point (x,y), using the current label font size. 
    The parameter "xalign" (resp. "yalign)" specifies which point of the string's 
    bounding box will end up at (x,y): 0.0 means the left (resp. bottom) side,
    1.0 means the right (resp. top) side.  Default is (0.5, 0.5), meaning 
    the box will be centered at (x,y). */

void ps_add_caption(FILE *psfile, const char *txt);
  /* Adds a caption text below the drawing, *outside* the nominal
    bounding box. */

void ps_comment(FILE *psfile, const char *title);
  /* Writes a comment line to the given file. */

/*** LOW-LEVEL HACKS ***/

void ps_put_text(FILE *psfile, const char *text, const char *newline);
  /* Writes a text string to /psfile/, in Postscript form, properly
    quoting any special chars and parentheses. Replaces any embedded
    '\n' by the given /newline/ string --- which could be, for
    example, ") show mynl (". */

#endif
