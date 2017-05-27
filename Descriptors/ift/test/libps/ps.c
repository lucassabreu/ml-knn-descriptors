/* See ps.h */
/* Last edited on 2003-01-08 22:17:27 by stolfi */

#include <ps.h>
#include <pswr.h>
#include <js.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <math.h>

#define FALSE 0
#define TRUE 1

/* PS WRITER TABLES */

PSWriter *ps_get_writer(FILE *psfile);
  /* Gets the current PS writer for file "psfile".
    The file must have been initialized with ps_begin_document
    or ps_begin_figure. */

PSWriter *ps_new_file(FILE *psfile);
  /* Creates a new PS writer for file "psfile" and adds it to 
    the writer table. */

void ps_delete_file(FILE *psfile);
  /* Deletes the PS writer for file "psfile" from the writer table,
    and reclaims its storage. */

/*** IMPLEMENTATIONS ***/

void ps_begin_figure
  ( FILE *psfile,
    double hmin, double hmax,
    double vmin, double vmax
  )
  { PSWriter *pswr = ps_new_file(psfile);
    /* Oh paranoia, not even Goya could draw ya... */
    assert(pswr->psfile == psfile, "bad writer record");
    pswr_begin_figure(pswr, hmin, hmax, vmin, vmax);
  }
  
void ps_end_figure(FILE *psfile)
  { PSWriter *pswr = ps_get_writer(psfile);
    pswr_end_figure(pswr);
    ps_delete_file(psfile);
  }

void ps_begin_document(FILE *psfile, const char* papersize)
  { PSWriter *pswr = ps_new_file(psfile);
    /* Oh paranoia, not even Goya could draw ya... */
    assert(pswr->psfile == psfile, "bad writer record"); /* Paranoia... */
    pswr_begin_document(pswr, papersize);
  }

void ps_end_document(FILE *psfile)
  { PSWriter *pswr = ps_get_writer(psfile);
    pswr_end_document(pswr);
    ps_delete_file(psfile);
  }

void ps_begin_page(FILE *psfile, const char *page)
  { PSWriter *pswr = ps_get_writer(psfile);
    pswr_begin_page(pswr, page);
  }
  
void ps_end_page(FILE *psfile)
  { PSWriter *pswr = ps_get_writer(psfile);
    pswr_end_page(pswr);
  }

void ps_set_window
  ( FILE *psfile,
    double xmin, double xmax,
    double ymin, double ymax,

    double hmin, double hmax,
    double vmin, double vmax,

    int xn, int yn
  )
  { PSWriter *pswr = ps_get_writer(psfile);
    pswr_set_window(pswr, xmin, xmax, ymin, ymax, hmin, hmax, vmin, vmax, xn, yn);
  }

void ps_get_paper_dimensions(const char *papersize, double *xpt, double *ypt)
  { pswr_get_paper_dimensions(papersize, xpt, ypt); }

void ps_comment(FILE *psfile, const char *title)
  { PSWriter *pswr = ps_get_writer(psfile);
    pswr_comment(pswr, title);
  }

void ps_add_caption(FILE *psfile, const char *txt)
  { PSWriter *pswr = ps_get_writer(psfile);
    pswr_add_caption(pswr, txt);
  }
  
void ps_set_label_font(FILE *psfile, const char *font, double size)
  { PSWriter *pswr = ps_get_writer(psfile);
    pswr_set_label_font(pswr, font, size);
  }
  
void ps_put_label
  ( FILE *psfile, 
    const char *text, 
    double x, double y, 
    double xalign, double yalign
  )
  { PSWriter *pswr = ps_get_writer(psfile);
    pswr_label(pswr, text, x, y, xalign, yalign);
  }

void ps_draw_frame (FILE *psfile)
  { PSWriter *pswr = ps_get_writer(psfile);
    ps_comment(psfile, "Draw frame around plot area");
    pswr_frame(pswr);
  }

void ps_set_pen
  ( FILE *psfile,
    double r, double g, double b,
    double width,
    double dashlength,
    double dashspace
  )
  { PSWriter *pswr = ps_get_writer(psfile);
    pswr_set_pen(pswr, r, g, b, width, dashlength, dashspace);
  }

void ps_draw_segment
  ( FILE *psfile,
    double xa, double ya,
    double xb, double yb
  )
  { PSWriter *pswr = ps_get_writer(psfile);
    pswr_segment(pswr, xa, ya, xb, yb);
  }

void ps_draw_curve
  ( FILE *psfile,
    double xa, double ya,
    double xb, double yb,
    double xc, double yc,
    double xd, double yd
  )
  { PSWriter *pswr = ps_get_writer(psfile);
    pswr_curve(pswr, xa, ya, xb, yb, xc, yc, xd, yd);
  }
  
void ps_draw_rectangle
  ( FILE *psfile,
    double xlo, double xhi,
    double ylo, double yhi
  )
  { PSWriter *pswr = ps_get_writer(psfile);
    pswr_rectangle(pswr, xlo, xhi, ylo, yhi, FALSE, TRUE);
  }

void ps_fill_rectangle
  ( FILE *psfile,
    double xlo, double xhi,
    double ylo, double yhi,
    double r, double g, double b
  )
  { PSWriter *pswr = ps_get_writer(psfile);
    pswr_set_fill_color(pswr, r, g, b);
    pswr_rectangle(pswr, xlo, xhi, ylo, yhi, TRUE, FALSE);
  }

void ps_fill_and_draw_rectangle
  ( FILE *psfile,
    double xlo, double xhi,
    double ylo, double yhi,
    double r, double g, double b
  )
  { PSWriter *pswr = ps_get_writer(psfile);
    pswr_set_fill_color(pswr, r, g, b);
    pswr_rectangle(pswr, xlo, xhi, ylo, yhi, TRUE, TRUE);
  }

void ps_draw_polygon
  ( FILE *psfile,
    double x[], double y[],
    int npoints
  )
  { PSWriter *pswr = ps_get_writer(psfile);
    pswr_polygon(pswr, x, y, npoints, FALSE, TRUE);
  }
    
void ps_fill_polygon
  ( FILE *psfile,
    double x[], double y[],
    int npoints,
    double r, double g, double b
  )
  { PSWriter *pswr = ps_get_writer(psfile);
    pswr_set_fill_color(pswr, r, g, b);
    pswr_polygon(pswr, x, y, npoints, TRUE, FALSE);
  }
    
void ps_fill_and_draw_polygon
  ( FILE *psfile,
    double x[], double y[],
    int npoints,
    double r, double g, double b
  )
  { PSWriter *pswr = ps_get_writer(psfile);
    pswr_set_fill_color(pswr, r, g, b);
    pswr_polygon(pswr, x, y, npoints, TRUE, TRUE);
  }
    
void ps_fill_circle
  ( FILE *psfile,
    double xc, double yc, double radius,
    double r, double g, double b
  )
  { PSWriter *pswr = ps_get_writer(psfile);
    pswr_set_fill_color(pswr, r, g, b);
    pswr_circle(pswr, xc, yc, radius, TRUE, FALSE);
  }
  
void ps_draw_circle
  ( FILE *psfile,
    double xc, double yc, double radius
  )
  { PSWriter *pswr = ps_get_writer(psfile);
    pswr_circle(pswr, xc, yc, radius, FALSE, TRUE);
  }
  
void ps_fill_and_draw_circle
  ( FILE *psfile,
    double xc, double yc, double radius,
    double r, double g, double b
  )
  { PSWriter *pswr = ps_get_writer(psfile);
    pswr_set_fill_color(pswr, r, g, b);
    pswr_circle(pswr, xc, yc, radius, TRUE, TRUE);
  }
 
void ps_fill_dot
  ( FILE *psfile,
    double xc, double yc, double radius,
    double r, double g, double b
  )
  { PSWriter *pswr = ps_get_writer(psfile);
    pswr_set_fill_color(pswr, r, g, b);
    pswr_dot(pswr, xc, yc, radius, TRUE, FALSE);
  }
  
void ps_draw_dot
  ( FILE *psfile,
    double xc, double yc, double radius
  )
  { PSWriter *pswr = ps_get_writer(psfile);
    pswr_dot(pswr, xc, yc, radius, FALSE, TRUE);
  }
  
void ps_fill_and_draw_dot
  ( FILE *psfile,
    double xc, double yc, double radius,
    double r, double g, double b
  )
  { PSWriter *pswr = ps_get_writer(psfile);
    pswr_set_fill_color(pswr, r, g, b);
    pswr_dot(pswr, xc, yc, radius, TRUE, TRUE);
  }
  
void ps_fill_and_draw_lune
  ( FILE *psfile,
    double xc, double yc, double radius, double tilt,
    double r, double g, double b
  )
  { PSWriter *pswr = ps_get_writer(psfile);
    pswr_set_fill_color(pswr, r, g, b);
    pswr_lune(pswr, xc, yc, radius, tilt, TRUE, TRUE);
  }
  
void ps_fill_and_draw_slice
  ( FILE *psfile,
    double xc, double yc, double radius, double start, double stop,
    double r, double g, double b
  )
  { PSWriter *pswr = ps_get_writer(psfile);
    pswr_set_fill_color(pswr, r, g, b);
    pswr_slice(pswr, xc, yc, radius, start, stop, TRUE, TRUE);
  }
  
void ps_fill_triangle
  ( FILE *psfile,
    double xa, double ya,
    double xb, double yb,
    double xc, double yc,
    double r, double g, double b
  )
  { PSWriter *pswr = ps_get_writer(psfile);
    pswr_set_fill_color(pswr, r, g, b);
    pswr_triangle(pswr, xa, ya, xb, yb, xc, yc, TRUE, FALSE);
  }

void ps_fill_grid_cell(FILE *psfile, int xi, int yi, double r, double g, double b)
  { PSWriter *pswr = ps_get_writer(psfile);
    pswr_set_fill_color(pswr, r, g, b);
    pswr_grid_cell(pswr, xi, yi, TRUE, FALSE);
  }

void ps_draw_coord_line (FILE *psfile, char axis, double coord)
  { PSWriter *pswr = ps_get_writer(psfile);
    if ((axis == 'x') || (axis == 'X'))
      { pswr_x_coord_line(pswr, coord);  }
    else if ((axis == 'y') || (axis == 'Y'))
      { pswr_y_coord_line(pswr, coord);  }
    else
      { error("ps_draw_coord-line: invalid axis"); }
  }

void ps_draw_grid_lines(FILE *psfile)
  { PSWriter *pswr = ps_get_writer(psfile);
    pswr_grid_lines(pswr);
  }
  
/* WRITER RECORD HANDLING */

/* Originally the "PSWriter" information was kept in static variables
  internal to this module. That became a problem when some clients 
  needed to create two or more Postscript files in parallel. 
  
  Ideally, we should have changed the "psfile" parameter of exported
  procedures: instead of a naked "FILE *", it should point to an
  object contaning the Postscript "PSWriter" variables and the file
  itself. However, for compatibility with old clients, we cannot
  implement this change now. So, for the time being, we internally map
  "FILE *" pointers to "PSWriter" records, through the following tables. */

#define MAXSTATES 10
static int ps_nfiles = 0;              /* Number of currently open PS files */
static PSWriter *ps_writer[MAXSTATES]; /* The states of those files. */

PSWriter *ps_get_writer(FILE *psfile)
  { { int i;
      for (i = 0; i < ps_nfiles; i++)
        { PSWriter *pswri = ps_writer[i];
          if (pswri->psfile == psfile) { return pswri; }
        }
    }
    assert(0, "Postscript file was not initialized?");
    return NULL;
  }

PSWriter *ps_new_file(FILE *psfile)
  { /* Sanity check: */
    { int i;
      for (i = 0; i < ps_nfiles; i++)
        { PSWriter *pswri = ps_writer[i];
          assert(pswri->psfile != psfile, "Postscript file initialized twice?");
        }
    }
    /* Alloacate record and store in table: */
    assert(ps_nfiles < MAXSTATES, "too many simultaneous files");
    { PSWriter *pswr = pswr_new_writer(psfile);
      ps_writer[ps_nfiles] = pswr;
      ps_nfiles++;
      return pswr;
    }
  }

void ps_delete_file(FILE *psfile)
  { int i;
    /* Find record in table: */
    for (i = 0; i < ps_nfiles; i++)
      { PSWriter *pswri = ps_writer[i];
        if (pswri->psfile == psfile) { break; }
      }
    assert(i < ps_nfiles, "Postscript file not initialized?");
    /* Remove from table and free record: */
    pswr_delete_writer(ps_writer[i]);
    while (i+1 < ps_nfiles)
      { ps_writer[i] = ps_writer[i+1]; i++; }
    ps_nfiles--;
  }
 
