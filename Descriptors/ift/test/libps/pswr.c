/* See pswr.h */
/* Last edited on 2003-05-01 17:36:35 by stolfi */

#include <pswr.h>
#include <pswrdef.h>
#include <pswraux.h>
#include <pswrvis.h>
#include <js.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <math.h>

#define FALSE 0
#define TRUE 1

/* WRITERS */

PSWriter *pswr_new_writer(FILE *psfile)
  { /* Allocate record and store in table: */
    PSWriter *pswr = (PSWriter *)notnull(malloc(sizeof(PSWriter)), "no mem"); 
    pswr->psfile = psfile;
    return pswr;
  }

void pswr_delete_writer(PSWriter *pswr)
  { free(pswr); }

/* FIGURE/DOCUMENT/PAGE CONTROL */

void pswr_begin_figure
  ( PSWriter *pswr,
    double hmin, double hmax,
    double vmin, double vmax
  )
  { FILE *psfile = pswr->psfile;
    char *date = today();
    fprintf(stderr, "date = %s\n", date);
    
    /* Initialize local state: */
    pswr->eps = 1;
    pswr->npages = 1;      /* Just in case... */
    pswr->pagestarted = 1; /* Just in case... */
    pswr->verbose = TRUE;
    pswr_save_default_window_data(pswr, hmin, hmax, vmin, vmax);

    pswr_write_eps_file_header(psfile, date, hmin, hmax, vmin, vmax);
    pswr_write_page_header(psfile, "1", 1);
    pswr_write_page_graphics_setup_cmds(psfile);
    pswr_write_window_setup_cmds(psfile, hmin, hmax, vmin, vmax, 1, 1);
    fflush(psfile);
    free(date);
  }
  
void pswr_end_figure(PSWriter *pswr)
  { FILE *psfile = pswr->psfile;
    assert(pswr->eps, "use of pswr_end_figure in non-EPS file");
    pswr_write_page_trailer(psfile, TRUE);
    pswr_write_eps_file_trailer(psfile);
    fflush(psfile);
  }

void pswr_begin_document(PSWriter *pswr, const char *papersize)
  { FILE *psfile = pswr->psfile;
    double hsize, vsize;
    char *date = today();

    fprintf(stderr, "date = %s\n", date);
    pswr_get_paper_dimensions(papersize, &hsize, &vsize);

    /* Initialize local state: */
    pswr->eps = 0;
    pswr->npages = 0;
    pswr->pagestarted = 0;
    pswr->verbose = TRUE;
    pswr_save_default_window_data(pswr, 0, hsize, 0, vsize);

    pswr_write_ps_file_header(psfile, papersize, date);
    fflush(psfile);
    free(date);
  }

void pswr_begin_page(PSWriter *pswr, const char *page)
  { FILE *psfile = pswr->psfile;
    char *date = today();
    
    assert(! pswr->eps, "use of pswr_begin_page in EPS file");
    /* In case the client forgot to call pswr_end_page: */
    if (pswr->pagestarted) { pswr_end_page(pswr); }
    
    pswr->npages++;
    pswr->pagestarted = 1;

    fprintf(stderr, "date = %s\n", date);

    pswr_write_page_header(psfile, page, pswr->npages);
    pswr_write_date_and_page_show_cmds(psfile, date, page);
    pswr_write_page_graphics_setup_cmds(psfile);
    pswr_write_window_setup_cmds
      ( psfile, 
        pswr->hmin, pswr->hmax, 
        pswr->vmin, pswr->vmax, 
        pswr->xn, pswr->yn
      );
    free(date);
  }

void pswr_end_page(PSWriter *pswr)
  { FILE *psfile = pswr->psfile;
    assert(! pswr->eps, "use of pswr_end_page in EPS file");
    assert(pswr->pagestarted, "pswr_end_page without pswr_begin_page?");
    pswr->pagestarted = 0;
    pswr_write_page_trailer(psfile, FALSE);
    /* The page trailer restores the Postscript state to its initial state: */
    pswr->fillColor[0] = -1;
    pswr->fillColor[1] = -1;
    pswr->fillColor[2] = -1;
  }

void pswr_end_document(PSWriter *pswr)
  { FILE *psfile = pswr->psfile;
    assert(! pswr->eps, "use of pswr_end_document in EPS file");
    /* In case the client forgot to call pswr_end_page: */
    if (pswr->pagestarted) { pswr_end_page(pswr); }
    pswr->pagestarted = 0;
    pswr_write_ps_file_trailer(psfile, pswr->npages);
    fflush(psfile);
  }

/* PLOTTING WINDOW */

void pswr_set_window
  ( PSWriter *pswr,
    double xmin, double xmax,
    double ymin, double ymax,

    double hmin, double hmax,
    double vmin, double vmax,

    int xn, int yn
  )
  { FILE *psfile = pswr->psfile;
    /* Save the window parameters in the "PSWriter" record: */
    pswr_save_window_data
      ( pswr,
        xmin, xmax, ymin, ymax,
        hmin, hmax, vmin, vmax,
        xn, yn
      );
    
    /* Set the window up in the Postcript file: */
    pswr_write_window_setup_cmds(psfile, hmin, hmax, vmin, vmax, xn, yn);
    fflush(psfile);
  }

/* DRAWING COMMANDS */

void pswr_set_pen
  ( PSWriter *pswr,
    double r, double g, double b,
    double width,
    double dashlength,
    double dashspace
  )
  { FILE *psfile = pswr->psfile;
    fprintf(psfile, "%5.3f %5.3f %5.3f setrgbcolor\n", r, g, b);
    fprintf(psfile, "mm %.3f mul setlinewidth\n", width);
    if ((dashlength == 0.0) | (dashspace == 0.0))
      { fprintf(psfile, "[ ] 0 setdash\n"); }
    else
      { fprintf(psfile,
          "[ %.3f mm mul %.3f mm mul ] 0 setdash\n",
          dashlength, dashspace
        );
      }
    fprintf(psfile, "\n");
    fflush(psfile);
  }

void pswr_segment
  ( PSWriter *pswr,
    double xa, double ya,
    double xb, double yb
  )
  { FILE *psfile = pswr->psfile;
    double psxa = pswr->hmin + pswr->xscale * (xa - pswr->xmin);
    double psya = pswr->vmin + pswr->yscale * (ya - pswr->ymin);
    double psxb = pswr->hmin + pswr->xscale * (xb - pswr->xmin);
    double psyb = pswr->vmin + pswr->yscale * (yb - pswr->ymin);
    if (pswr_segment_is_invisible(pswr, psxa, psya, psxb, psyb))
      { return; }
    fprintf(psfile,
      "%6.1f %6.1f  %6.1f %6.1f segd\n",
      psxa, psya, psxb, psyb
    );
    fflush(psfile);
  }

void pswr_curve
  ( PSWriter *pswr,
    double xa, double ya,
    double xb, double yb,
    double xc, double yc,
    double xd, double yd
  )
  { FILE *psfile = pswr->psfile;
    double psxa = pswr->hmin + pswr->xscale * (xa - pswr->xmin);
    double psya = pswr->vmin + pswr->yscale * (ya - pswr->ymin);
    double psxb = pswr->hmin + pswr->xscale * (xb - pswr->xmin);
    double psyb = pswr->vmin + pswr->yscale * (yb - pswr->ymin);
    double psxc = pswr->hmin + pswr->xscale * (xc - pswr->xmin);
    double psyc = pswr->vmin + pswr->yscale * (yc - pswr->ymin);
    double psxd = pswr->hmin + pswr->xscale * (xd - pswr->xmin);
    double psyd = pswr->vmin + pswr->yscale * (yd - pswr->ymin);
    if (pswr_curve_is_invisible(pswr, psxa, psya, psxb, psyb, psxc, psyc, psxd, psyd))
      { return; }
    fprintf(psfile, "%6.1f %6.1f  %6.1f %6.1f  %6.1f %6.1f  %6.1f %6.1f arcd\n",
      psxa, psya, psxb, psyb, psxc, psyc, psxd, psyd
    );
    fflush(psfile);
  }

void pswr_x_coord_line (PSWriter *pswr, double coord)
  { FILE *psfile = pswr->psfile;
    double pscoord = pswr->hmin + pswr->xscale * (coord - pswr->xmin);
    fprintf(psfile, "%6.1f xgrd\n", pscoord);
  }

void pswr_y_coord_line (PSWriter *pswr, double coord)
  { FILE *psfile = pswr->psfile;
    double pscoord = pswr->vmin + pswr->yscale * (coord - pswr->ymin);
    fprintf(psfile, "%6.1f ygrd\n", pscoord);
  }

void pswr_grid_lines(PSWriter *pswr)
  { FILE *psfile = pswr->psfile;
    fprintf(psfile, 
      "%% Grid lines:\n"
      "gsave\n"
      "  initclip\n"
      "  0 1 xn {\n"
      "    xstep mul xmin add xgrd\n"
      "  } for\n"
      "  0 1 yn {\n"
      "    ystep mul ymin add ygrd\n"
      "  } for\n"
      "grestore\n"
      "\n"
    );
    fflush(psfile);
  }

void pswr_frame(PSWriter *pswr)
  { FILE *psfile = pswr->psfile;
    fprintf(psfile, 
      "gsave\n"
      "  initclip\n"
      "  newpath\n"
      "  xmin ymin moveto\n"
      "  xmax ymin lineto\n"
      "  xmax ymax lineto\n"
      "  xmin ymax lineto\n"
      "  xmin ymin lineto\n"
      "  closepath stroke\n"
      "grestore\n"
    );
  }

void pswr_set_fill_color(PSWriter *pswr, double r, double g, double b)
  { /* For now: */
    pswr->fillColor[0] = r;
    pswr->fillColor[1] = g;
    pswr->fillColor[2] = b;
  }

void pswr_rectangle
  ( PSWriter *pswr,
    double xlo, double xhi,
    double ylo, double yhi,
    int fill, int draw
  )
  { FILE *psfile = pswr->psfile;
    double psxlo = pswr->hmin + pswr->xscale * (xlo - pswr->xmin);
    double psxhi = pswr->hmin + pswr->xscale * (xhi - pswr->xmin);
    double psylo = pswr->vmin + pswr->yscale * (ylo - pswr->ymin);
    double psyhi = pswr->vmin + pswr->yscale * (yhi - pswr->ymin);
    if (pswr_rectangle_is_invisible(pswr, psxlo, psxhi, psylo, psyhi))
      { return; }
    fprintf(psfile, "%6.1f %6.1f  %6.1f %6.1f",
      psxlo, psxhi, psylo, psyhi
    );
    if (fill) { pswr_write_color(psfile, pswr->fillColor); }
    fprintf(psfile, " rec%s%s\n", (fill ? "f" : ""), (draw ? "d" : ""));
    fflush(psfile);
  }

void pswr_triangle
  ( PSWriter *pswr,
    double xa, double ya,
    double xb, double yb,
    double xc, double yc,
    int fill, int draw
  )
  { FILE *psfile = pswr->psfile;
    double psxa = pswr->hmin + pswr->xscale * (xa - pswr->xmin);
    double psya = pswr->vmin + pswr->yscale * (ya - pswr->ymin);
    double psxb = pswr->hmin + pswr->xscale * (xb - pswr->xmin);
    double psyb = pswr->vmin + pswr->yscale * (yb - pswr->ymin);
    double psxc = pswr->hmin + pswr->xscale * (xc - pswr->xmin);
    double psyc = pswr->vmin + pswr->yscale * (yc - pswr->ymin);
    if (pswr_triangle_is_invisible(pswr, psxa, psya, psxb, psyb, psxc, psyc))
      { return; }
    fprintf(psfile,
      "%6.1f %6.1f  %6.1f %6.1f  %6.1f %6.1f ",
      psxa, psya, psxb, psyb, psxc, psyc
    );
    if (fill) { pswr_write_color(psfile, pswr->fillColor); }
    fprintf(psfile, " tri%s%s\n", (fill ? "f" : ""), (draw ? "d" : ""));
    fflush(psfile);
  }

void pswr_polygon
  ( PSWriter *pswr,
    double x[], double y[],
    int npoints,
    int fill, int draw
  )
  { FILE *psfile = pswr->psfile;
    double *psx = (double *)malloc(npoints*sizeof(double));
    double *psy = (double *)malloc(npoints*sizeof(double));
    int i;
    /* Map points to device coordinates: */
    for (i=0; i<npoints; i++)
      { psx[i] = pswr->hmin + pswr->xscale * (x[i] - pswr->xmin);
        psy[i] = pswr->vmin + pswr->yscale * (y[i] - pswr->ymin);
      }
    if (! pswr_polygon_is_invisible(pswr, psx, psy, npoints))
      { /* Plot them: */
        if (npoints>6) fprintf(psfile, "\n");
        for (i=0; i<npoints; i++)
          { double psxi = psx[i];
            double psyi = psy[i];
            if ((i % 6) == 0) 
              { if (i>0) fprintf(psfile, "\n"); }
            else
              { fprintf(psfile, "  "); }
            fprintf(psfile, "%6.1f %6.1f", psxi, psyi);
          }
        if (npoints > 6) fprintf(psfile, "\n");
        fprintf(psfile, "  %d", npoints);
        if (fill) { pswr_write_color(psfile, pswr->fillColor); }
        fprintf(psfile, " pol%s%s\n", (fill ? "f" : ""), (draw ? "d" : ""));
        fflush(psfile);
      }
    free(psx); free(psy);
  }

void pswr_circle
  ( PSWriter *pswr,
    double xc, double yc, double rad,
    int fill, int draw
  )
  { FILE *psfile = pswr->psfile;
    double psxc = pswr->hmin + pswr->xscale * (xc - pswr->xmin);
    double psyc = pswr->vmin + pswr->yscale * (yc - pswr->ymin);
    double psrad = pswr->yscale * rad;
    if (pswr_circle_is_invisible(pswr, psxc, psyc, psrad))
      { return; }
    fprintf(psfile, "%6.1f %6.1f  %6.1f", psxc, psyc, psrad); 
    if (fill) { pswr_write_color(psfile, pswr->fillColor); }
    fprintf(psfile, " cir%s%s\n", (fill ? "f" : ""), (draw ? "d" : ""));
    fflush(psfile);
  }

void pswr_dot
  ( PSWriter *pswr,
    double xc, double yc, double rad,
    int fill, int draw
  )
  { FILE *psfile = pswr->psfile;
    double psxc = pswr->hmin + pswr->xscale * (xc - pswr->xmin);
    double psyc = pswr->vmin + pswr->yscale * (yc - pswr->ymin);
    if (pswr_circle_is_invisible(pswr, psxc, psyc, rad))
      { return; }
    fprintf(psfile, "%6.1f %6.1f  %6.1f mm mul", psxc, psyc, rad); 
    if (fill) { pswr_write_color(psfile, pswr->fillColor); }
    fprintf(psfile, " cir%s%s\n", (fill ? "f" : ""), (draw ? "d" : ""));
    fflush(psfile);
  }

void pswr_lune
  ( PSWriter *pswr,
    double xc, double yc, double rad, double tilt,
    int fill, int draw
  )
  { FILE *psfile = pswr->psfile;
    double psxc = pswr->hmin + pswr->xscale * (xc - pswr->xmin);
    double psyc = pswr->vmin + pswr->yscale * (yc - pswr->ymin);
    double psrad = pswr->yscale * rad;
    double pstilt = tilt * 180.0 / 3.1415926;
    if (pswr_lune_is_invisible(pswr, psxc, psyc, psrad, pstilt))
      { return; }
    fprintf(psfile, "%6.1f %6.1f  %6.1f %6.2f",
      psxc, psyc, psrad, pstilt); 
    if (fill) { pswr_write_color(psfile, pswr->fillColor); }
    fprintf(psfile, " lune%s%s\n", (fill ? "f" : ""), (draw ? "d" : ""));
    fflush(psfile);
  }

void pswr_slice
  ( PSWriter *pswr,
    double xc, double yc, double rad, 
    double start, double stop,
    int fill, int draw
  )
  { FILE *psfile = pswr->psfile;
    double psxc = pswr->hmin + pswr->xscale * (xc - pswr->xmin);
    double psyc = pswr->vmin + pswr->yscale * (yc - pswr->ymin);
    double psrad = pswr->yscale * rad;
    double psstart = start * 180.0 / 3.1415926;
    double psstop  = stop  * 180.0 / 3.1415926;
    if (pswr_slice_is_invisible(pswr, psxc, psyc, psrad, psstart, psstop))
      { return; }
    fprintf(psfile, "%6.1f %6.1f  %6.1f  %6.2f %6.2f",
      psxc, psyc, psrad, psstart, psstop); 
    if (fill) { pswr_write_color(psfile, pswr->fillColor); }
    fprintf(psfile, " slice%s%s\n", (fill ? "f" : ""), (draw ? "d" : ""));
    fflush(psfile);
  }

void pswr_grid_cell
  ( PSWriter *pswr, 
    int xi, int yi,
    int fill, int draw
  )
  { FILE *psfile = pswr->psfile;
    fprintf(psfile, "%3d %3d", xi, yi);
    if (fill) { pswr_write_color(psfile, pswr->fillColor); }
    fprintf(psfile, " cel%s%s\n", (fill ? "f" : ""), (draw ? "d" : ""));
    fflush(psfile);
  }
    
/* TEXT PRINTING */

void pswr_set_label_font(PSWriter *pswr, const char *font, double size)
  { FILE *psfile = pswr->psfile;
    pswr_write_label_font_setup_cmds(psfile, font, size);
  }

void pswr_label
  ( PSWriter *pswr, 
    const char *text,
    double x, double y, 
    double xalign, double yalign
  )
  { FILE *psfile = pswr->psfile;
    double psx = pswr->hmin + pswr->xscale * (x - pswr->xmin);
    double psy = pswr->vmin + pswr->yscale * (y - pswr->ymin);
    pswr_write_ps_string(psfile, text, "\\267");
    fprintf(psfile, " ");
    fprintf(psfile, "  %5.3f %5.3f  %6.1f %6.1f  lbsh\n",
      xalign, yalign, psx, psy
    );
    fflush(psfile);
  }

void pswr_add_caption(PSWriter *pswr, const char *txt)
  { FILE *psfile = pswr->psfile;
    fprintf(psfile, "nl ");
    pswr_write_ps_string(psfile, txt, ") shw\nnl (");
    fprintf(psfile, " shw\n");
    fflush(psfile);
  }

/* MISCELLANEOUS */ 

void pswr_comment(PSWriter *pswr, const char *title)
  { FILE *psfile = pswr->psfile;
    fprintf(pswr->psfile, "\n%% [%s]\n", title);
    if (pswr->verbose) { fprintf(stderr, "[%s]\n", title); }
    fflush(psfile);
  }

/* INTERNAL WINDOW SETUP */

void pswr_save_default_window_data
  ( PSWriter *pswr,
    double hmin, double hmax,
    double vmin, double vmax
  )
  { /* Save the actual plot window: */
    pswr->hmin = hmin; pswr->hmax = hmax;
    pswr->vmin = vmin; pswr->vmax = vmax;
    /* The default client ranges are the actual plot ranges: */
    pswr->xmin = hmin; pswr->xmax = hmax;
    pswr->ymin = vmin; pswr->ymax = vmax;
    /* Thus the default scales are trivial: */
    pswr->xscale = 1.0;
    pswr->yscale = 1.0;
    /* Default is only one big cell: */
    pswr->xn = 1;
    pswr->yn = 1;
  }

void pswr_save_window_data
  ( PSWriter *pswr,
    double xmin, double xmax,
    double ymin, double ymax,
    double hmin, double hmax,
    double vmin, double vmax,
    int xn, int yn
  )
  { /* Save the actual plot window: */
    pswr->hmin = hmin; pswr->hmax = hmax;
    pswr->vmin = vmin; pswr->vmax = vmax;
    /* Save the client scales: */
    pswr->xmin = xmin; pswr->xmax = xmax;
    pswr->ymin = ymin; pswr->ymax = ymax;
    /* Compute client-to-file scales: */
    pswr->xscale = (hmax - hmin)/(xmax - xmin);
    pswr->yscale = (vmax - vmin)/(ymax - ymin);
    /* Check for equal scales, allowing for some rounding errors: */
    { double scalesum = fabs(pswr->xscale) + fabs(pswr->yscale);
      double scalediff = fabs(pswr->xscale - pswr->yscale);
      assert(scalediff/scalesum < 0.0001, "unequal scales");
    }
    /* Save client grid size: */
    pswr->xn = xn; pswr->yn = yn;
  }

/* INTERNAL FILE WRITING PROCEDURES */

void pswr_write_eps_file_header
  ( FILE *psfile, 
    const char *date,
    double hmin, double hmax,
    double vmin, double vmax
  )
  { fprintf(psfile, 
      "%%!PS-Adobe-2.0 EPSF-2.0\n"
    );
    fprintf(psfile, 
      "%%%%CreationDate: %s\n", date
    );
    fprintf(psfile, 
      "%%%%BoundingBox: %.0f %.0f %.0f %.0f\n",
      floor(hmin), floor(vmin), ceil(hmax), ceil(vmax)
    );
    fprintf(psfile, 
      "%%%%Pages: 1\n"
      "%%%%EndComments\n"
    );
    
    fprintf(psfile, 
      "/pswr$dict 6400 dict def \n"
      "pswr$dict begin\n"
    );
    pswr_write_proc_defs(psfile);
    fprintf(psfile, 
      "end\n"
    );

    fprintf(psfile, 
      "%%%%EndProlog\n"
    );
  }

void pswr_write_eps_file_trailer(FILE *psfile)
  { fprintf(psfile, "%%%%Trailer\n" ); }

void pswr_write_ps_file_header(FILE *psfile, const char *papersize, const char *date)
  { fprintf(psfile, 
      "%%!PS-Adobe-2.0\n"
      "%%%%Pages: (atend)\n"
    );
    fprintf(psfile, 
      "%%%%DocumentPaperSizes: %s\n", papersize
    );
    fprintf(psfile, 
      "%%%%EndComments\n"
    );
    
    fprintf(psfile, 
      "/pswr$dict 6400 dict def \n"
      "pswr$dict begin\n"
    );
    pswr_write_proc_defs(psfile);
    fprintf(psfile, 
      "end\n"
    );

    fprintf(psfile, 
      "%%%%EndProlog\n"
    );
  }

void pswr_write_page_header(FILE *psfile, const char *pagename, int pageseq)
  { if ((pagename != NULL) && (*pagename != '\000'))
      { fprintf(psfile, "%%%%Page: %d %d\n\n", pageseq, pageseq); }
    else
      { fprintf(psfile, "%%%%Page: %s %d\n\n", pagename, pageseq); }

    fprintf(psfile, 
      "%% Save the standard coord system:\n"
      "/savedstate save def\n"
      "\n"
      "pswr$dict begin\n"
      "\n"
    );
    fflush(psfile);
  }

void pswr_write_page_trailer(FILE *psfile, int eps)
  { if (! eps) { fprintf(psfile, "showpage\n"); }
    fprintf(psfile, 
      "end %% pswr$dict\n"
      "\n"
      "savedstate restore\n"
      "%% Now we are back to the initial Postscript state system.\n"
      "\n"
    );
    fprintf(psfile, 
      "%%%%EndPage\n"
      "\n"
    );
    fflush(psfile);
  }
  
void pswr_write_ps_file_trailer(FILE *psfile, int npages)
  { fprintf(psfile, 
      "%%%%Trailer\n"
    );
    fprintf(psfile, 
      "%%%%Pages: %d\n", npages
    );
  }

void pswr_write_date_and_page_show_cmds
  ( FILE *psfile, 
    const char *date, 
    const char *page
  )
  { 
    fprintf(psfile, 
      "%% Print date:\n"
      "gsave\n"
      "  /Courier findfont\n"
      "  10 scalefont setfont\n"
      "  72 40 moveto\n"
    );
    fprintf(psfile, "  (");
    pswr_write_ps_string(psfile, date, "\\267");
    if ((page != NULL) && (*page != '\000'))
      { fprintf(psfile, "   page ");
        pswr_write_ps_string(psfile, page, "\\267");
      }
    fprintf(psfile, ") show\n");
    fprintf(psfile, 
      "grestore\n"
      "\n"
    );
  }

void pswr_write_page_graphics_setup_cmds(FILE *psfile)
  { fprintf(psfile, 
      "%% Round joints and caps:\n"
      "1 setlinecap 1 setlinejoin\n"
      "\n"
      "%% Black thin lines:\n"
      "0 setlinewidth 0 setgray [ ] 0 setdash\n"
      "\n"
    );

    /* Just in case the client forgets to set it up: */
    pswr_write_label_font_setup_cmds(psfile, "Courier", 8.0);    
  }
  
void pswr_write_window_setup_cmds
  ( FILE *psfile,
    double hmin, double hmax,
    double vmin, double vmax,
    int xn, int yn
  )
  { /* Actual grid cell dimensions (in pt): */
    double xstep = (hmax - hmin)/(double)xn;
    double ystep = (vmax - vmin)/(double)yn;
    
    /* In spite of their names, the Postscript variables
      "xmin,xmax,ymin,ymax" are not client coordinates, 
      but actual plotting coordinates (in pt), namely 
      "hmin,hmax,vmin,vmax". */
      
    fprintf(psfile, "/xmin %f def   %% min plottable x\n", hmin);
    fprintf(psfile, "/xmax %f def   %% max plottable x\n", hmax);
    fprintf(psfile, "/xn %d def     %% grid cells along x axis\n", xn);
    fprintf(psfile, "/xstep %f def  %% x-size of grid cell\n", xstep);

    fprintf(psfile, "/ymin %f def   %% min plottable y\n", vmin);
    fprintf(psfile, "/ymax %f def   %% max plottable y\n", vmax);
    fprintf(psfile, "/yn %d def     %% grid cells along y axis\n", yn);
    fprintf(psfile, "/ystep %f def  %% y-size of grid cell\n", ystep);

    fprintf(psfile, 
      "%% Set clipping path to boundary of plot area:\n"
      "initclip\n"
      "newpath\n"
      "  xmin ymin moveto\n"
      "  xmax ymin lineto\n"
      "  xmax ymax lineto\n"
      "  xmin ymax lineto\n"
      "  xmin ymin lineto\n"
      "clip\n"
      "\n"
    );

    /* Caption cursor. */
    
    fprintf(psfile, 
      "%% Left margin cursor for caption text:\n"
      "/dytext 10 pt mul def\n"
      "/xtext xmin def\n"
      "/ytext ymin def\n"
      "\n"
    );

    fprintf(psfile, 
      "%% Caption font setup:\n"
      "/captionfont\n"
      "  /Courier findfont\n"
      "  dytext scalefont\n"
      "def\n"
      "\n"
    );

    fflush(psfile);
  }

void pswr_write_label_font_setup_cmds(FILE *psfile, const char *font, double size)
  { fprintf(psfile, 
      "%% Label font setup:\n"
      "/labelfont\n"
      "  /%s findfont %.3f pt mul scalefont\n"
      "def\n" 
      "\n",
      font, size
    );
  }  

void pswr_write_color(FILE *psfile, double *clr)
  { fprintf(psfile, "  %5.3f %5.3f %5.3f", clr[0], clr[1], clr[2]); }

void pswr_write_ps_string(FILE *psfile, const char *text, const char *newline)
  { const char *p;
    putc('(', psfile);
    for (p = text; *p != 0; p++)
      { if (*p == '\n')
          { fprintf(psfile, "%s", newline); }
        else if (*p == '(')
          { putc('\\', psfile); putc('(', psfile); }
        else if (*p == ')')
          { putc('\\', psfile); putc(')', psfile); }
        else if (*p == '\t')
          { putc(' ', psfile); putc(' ', psfile); }
        else if (*p == '\\')
          { putc('\\', psfile); putc('\\', psfile); }
        else if ((*p < ' ') || (*p > '~'))
          { fprintf(psfile, "\\%03o", *p); }
        else
          { putc(*p, psfile); }
      }
    fprintf(psfile, ")");
  }

void pswr_write_proc_defs(FILE *psfile)
  { /* Units of measure: */
    
    fprintf(psfile, 
      "%% Units of measure:\n"
      "/pt 1.0 def\n"
      "/in pt 72.0 mul def \n"
      "/mm pt 72.0 25.4 div mul def\n"
      "\n"
    );
  
    fprintf(psfile, 
      "%% Segment draw operator:\n"
      "%%   /xa/ /ya/ /xb/ /yb/ segd --> \n"
      "/segd\n"
      "{\n"
      "  gsave\n"
      "    newpath\n"
      "    moveto\n"
      "    lineto\n"
      "    stroke\n"
      "  grestore\n"
      "} def\n"
      "\n"
    );

    fprintf(psfile, 
      "%% Curve draw operator:\n"
      "%%   /xa/ /ya/  /xb/ /yb/  /xc/ /yc/  /xd/ /yd/ arcd --> \n"
      "/arcd\n"
      "{\n"
      "  gsave\n"
      "    newpath\n"
      "      8 -2 roll moveto curveto\n"
      "    stroke\n"
      "  grestore\n"
      "} def\n"
      "\n"
    );

    fprintf(psfile, 
      "%% Draw an X-value grid line:\n"
      "%%   /x/ xgrd --> \n"
      "/xgrd\n"
      "{\n"
      "  gsave\n"
      "  newpath\n"
      "    dup ymin moveto\n"
      "    ymax lineto\n"
      "    stroke\n"
      "  grestore\n"
      "} def\n"
      "\n"
    );

    fprintf(psfile, 
      "%% Draw an Y-value grid line:\n"
      "%%   /y/ ygrd --> \n"
      "/ygrd\n"
      "{\n"
      "  gsave\n"
      "  newpath\n"
      "    dup xmin exch moveto\n"
      "    xmax exch lineto\n"
      "    stroke\n"
      "  grestore\n"
      "} def\n"
      "\n"
    );

    /* Triangles: */
    
    fprintf(psfile, 
      "%% Triangle fill operator:\n"
      "%%   /xa/ /ya/ /xb/ /yb/ /xc/ /yc/ /r/ /g/ /b/ trif --> \n"
      "/trif\n"
      "{\n"
      "  gsave\n"
      "    setrgbcolor\n"
      "    newpath\n"
      "    moveto\n"
      "    lineto\n"
      "    lineto\n"
      "    closepath\n"
      "    fill\n"
      "  grestore\n"
      "} def\n"
      "\n"
    );

    /* Rectangles: */
    
    fprintf(psfile, 
      "%% Rectangle draw operator:\n"
      "%%   /xlo/ /xhi/ /ylo/ /yhi/ recd --> \n"
      "/recd\n"
      "{\n"
      "  gsave\n"
      "  newpath\n"
      "    3 index 2 index moveto\n"
      "    2 index 2 index lineto\n"
      "    2 index 1 index lineto\n"
      "    3 index 1 index lineto\n"
      "    pop pop pop pop\n"
      "    closepath\n"
      "    stroke\n"
      "  grestore\n"
      "} def\n"
      "\n"
    );

    fprintf(psfile, 
      "%% Rectangle fill operator:\n"
      "%%   /xlo/ /xhi/ /ylo/ /yhi/ /r/ /g/ /b/ recf --> \n"
      "/recf\n"
      "{\n"
      "  gsave\n"
      "    setrgbcolor\n"
      "    newpath\n"
      "    3 index 2 index moveto\n"
      "    2 index 2 index lineto\n"
      "    2 index 1 index lineto\n"
      "    3 index 1 index lineto\n"
      "    pop pop pop pop\n"
      "    closepath\n"
      "    fill\n"
      "  grestore\n"
      "} def\n"
      "\n"
    );

    fprintf(psfile, 
      "%% Rectangle fill and stroke operator:\n"
      "%%   /xlo/ /xhi/ /ylo/ /yhi/ /r/ /g/ /b/ recfd --> \n"
      "/recfd\n"
      "{\n"
      "  gsave\n"
      "    newpath\n"
      "    6 index 5 index moveto\n"
      "    5 index 5 index lineto\n"
      "    5 index 4 index lineto\n"
      "    6 index 4 index lineto\n"
      "    closepath\n"
      "    gsave setrgbcolor fill grestore\n"
      "    stroke\n"
      "    pop pop pop pop\n"
      "  grestore\n"
      "} def\n"
      "\n"
    );

    /* Polygons: */
    
    fprintf(psfile, 
      "%% Polygon draw operator:\n"
      "%%   /x1/ /y1/ .. /xn/ /yn/ /n/ pold --> \n"
      "/pold\n"
      "{\n"
      "  gsave\n"
      "    newpath\n"
      "    3 1 roll moveto\n"
      "    1 sub 1 1  3 2 roll { pop lineto } for\n"
      "    closepath\n"
      "    stroke\n"
      "  grestore\n"
      "} def\n"
      "\n"
    );

    fprintf(psfile, 
      "%% Polygon fill operator:\n"
      "%%   /x1/ /y1/ .. /xn/ /yn/ /n/ /r/ /g/ /b/ polf --> \n"
      "/polf\n"
      "{\n"
      "  gsave\n"
      "    setrgbcolor\n"
      "    newpath\n"
      "    3 1 roll moveto\n"
      "    1 sub 1 1  3 2 roll { pop lineto } for\n"
      "    closepath\n"
      "    fill\n"
      "  grestore\n"
      "} def\n"
      "\n"
    );

    fprintf(psfile, 
      "%% Polygon fill and draw operator:\n"
      "%%   /x1/ /y1/ .. /xn/ /yn/ /n/ /r/ /g/ /b/ polfd --> \n"
      "/polfd\n"
      "{\n"
      "  gsave\n"
      "    newpath\n"
      "    6 4 roll moveto\n"
      "    4 3 roll 1 sub\n"
      "    %% -- /x1/ /y1/ .. /x[n-1]/ /y[n-1]/ /r/ /g/ /b/ /n-1/\n"
      "    1 1  3 2 roll { pop 5 3 roll lineto } for\n"
      "    closepath\n"
      "    gsave setrgbcolor fill grestore\n"
      "    stroke\n"
      "  grestore\n"
      "} def\n"
      "\n"
    );

    /* Circles: */
    
    fprintf(psfile, 
      "%% Circle fill operator:\n"
      "%%   /x/ /y/ /rad/ /r/ /g/ /b/ cirf --> \n"
      "/cirf\n"
      "{\n"
      "  gsave\n"
      "    setrgbcolor\n"
      "    newpath\n"
      "      0 360 arc\n"
      "      closepath\n"
      "    fill\n"
      "  grestore\n"
      "} def\n"
      "\n"
    );

    fprintf(psfile, 
      "%% Circle draw operator:\n"
      "%%   /x/ /y/ /rad/ cird --> \n"
      "/cird\n"
      "{\n"
      "  gsave\n"
      "    newpath\n"
      "      0 360 arc\n"
      "      closepath\n"
      "    stroke\n"
      "  grestore\n"
      "} def\n"
      "\n"
    );

    fprintf(psfile, 
      "%% Circle fill and draw operator:\n"
      "%%   /x/ /y/ /rad/ /r/ /g/ /b/ cirfd --> \n"
      "/cirfd\n"
      "{\n"
      "  gsave\n"
      "    6 3 roll\n"
      "    newpath\n"
      "      0 360 arc\n"
      "      closepath\n"
      "      gsave setrgbcolor fill grestore\n"
      "    stroke\n"
      "  grestore\n"
      "} def\n"
      "\n"
    );

    /* Lunes (lenses): */
    
    fprintf(psfile, 
      "%% Lune fill and draw operator:\n"
      "%%   /xc/ /yc/ /rad/ /tilt/ /r/ /g/ /b/ lunefd --> \n"
      "/lunefd\n"
      "{\n"
      "  gsave\n"
      "    7 3 roll\n"
      "    %% --- r, g, b, xc, yc, rad, tilt\n"
      "    4 2 roll\n"
      "    %% --- r, g, b, rad, tilt, xc, yc\n"
      "    translate\n"
      "    %% --- r, g, b, rad, tilt\n"
      "    rotate\n"
      "    %% --- r, g, b, rad \n"
      "    newpath\n"
      "      dup\n"
      "      %% --- r, g, b, rad, rad\n"
      "      dup neg 0\n"
      "      %% --- r, g, b, rad, rad, -rad, 0\n"
      "      3 2 roll 2 mul\n"
      "      %% --- r, g, b, rad, -rad, 0, 2*rad\n"
      "      -60 60 arc\n"
      "      %% --- r, g, b, rad\n"
      "      dup 0\n"
      "      %% --- r, g, b, rad, rad, 0\n"
      "      3 2 roll 2 mul\n"
      "      %% --- r, g, b, rad, 0, 2*rad\n"
      "      120 240 arc\n"
      "      closepath\n"
      "      gsave setrgbcolor fill grestore\n"
      "    stroke\n"
      "  grestore\n"
      "} def\n"
      "\n"
    );

    /* Pie slices: */
    
    fprintf(psfile, 
      "%% Slice fill and draw operator:\n"
      "%%   /xc/ /yc/ /rad/ /start/ /stop/ /r/ /g/ /b/ slicefd --> \n"
      "/slicefd\n"
      "{\n"
      "  gsave\n"
      "    8 3 roll\n"
      "    %% --- r, g, b, xc, yc, rad, start, stop\n"
      "    newpath\n"
      "      4 index 4 index moveto arc\n"
      "      closepath\n"
      "    gsave setrgbcolor fill grestore\n"
      "    stroke\n"
      "  grestore\n"
      "} def\n"
      "\n"
    );

    /* Cells of the cell grid: */
    
    fprintf(psfile, 
      "%% Cell fill operator:\n"
      "%%   /xi/ /yi/ /r/ /g/ /b/ celf --> \n"
      "/celf\n"
      "{\n"
      "  5 3 roll \n"
      "  %% --- r, g, b, xi, yi\n"
      "  exch dup \n"
      "  %% --- r, g, b, yi, xi, xi\n"
      "  xstep mul xmin add exch 1 add xstep mul xmin add\n"
      "  %% --- r, g, b, yi, xlo, xhi\n"
      "  3 2 roll dup\n"
      "  %% --- r, g, b, xlo, xhi, yi, yi\n"
      "  ystep mul ymin add exch 1 add ystep mul ymin add\n"
      "  %% --- r, g, b, xlo, xhi, ylo, yhi\n"
      "  7 4 roll \n"
      "  recf\n"
      "} def\n"
      "\n"
    );

    /* Labels and captions: */
    
    fprintf(psfile, 
      "%% Label printing operator:\n"
      "%%   /str/ /xa/ /ya/ /xc/ /yc/ lbsh --> \n"
      "/lbsh\n"
      "{\n"
      "  labelfont setfont\n"
      "  newpath moveto\n"
      "    %% --- str, xa, ya\n"
      "  gsave 2 index false charpath flattenpath pathbbox grestore\n"
      "    %% --- str, xa, ya, lox, loy, hix, hiy\n"
      "  3 index 3 index currentpoint \n"
      "    %% --- str, xa, ya, lox, loy, hix, hiy, lox, loy, cx, cy\n"
      "  exch 4 1 roll exch sub\n"
      "  3 1 roll sub exch\n"
      "    %% --- str, xa, ya, lox, loy, hix, hiy, cx-lox, cy-loy\n"
      "  rmoveto\n"
      "    %% --- str, xa, ya, lox, loy, hix, hiy\n"
      "  exch 4 1 roll exch sub \n"
      "  3 1 roll sub exch\n"
      "    %% --- str, xa, ya, dx, dy\n"
      "  exch 4 1 roll mul -1 mul\n"
      "  3 1 roll mul -1 mul exch\n"
      "    %% --- str, -dx*xa, -dy*ya\n"
      "  rmoveto\n"
      "    %% --- str\n"
      "  show\n"
      "} def\n"
      "\n"
    );

    fprintf(psfile, 
      "%% Operator to move to new caption line:\n"
      "%%   nl --> \n"
      "/nl\n"
      "{\n"
      "  /ytext ytext dytext sub def\n"
      "  xtext ytext moveto\n"
      "} def\n"
      "\n"
    );

    fprintf(psfile, 
      "%% Operator to print string at CP without clipping:\n"
      "%%   /s/ shw --> \n"
      "/shw\n"
      "{\n"
      "  gsave\n"
      "    initclip\n"
      "    captionfont setfont show\n"
      "  grestore\n"
      "} def\n"
      "\n"
    );

    fflush(psfile);
  }

/* PAPER SIZES */

void pswr_get_paper_dimensions(const char *papersize, double *xpt, double *ypt)
  { /* US sizes are defined in inches, which are 72 "pt" exactly: */
    /* ISO paper sizes are defined in "mm", and need conversion: */
    double mm = 72.0/25.4; /* One mm in points. */
    if ((strcmp(papersize, "letter") == 0) || (strcmp(papersize, "Letter") == 0))
      { (*xpt) = 612.0; (*ypt) = 792.0; return; /* 8,5 x 11 in, 216 x 279 mm */  }
    if ((strcmp(papersize, "a4") == 0) || (strcmp(papersize, "A4") == 0))
      { (*xpt) = 210 * mm; (*ypt) = 297 * mm; return; }
    if ((strcmp(papersize, "a3") == 0) || (strcmp(papersize, "A3") == 0))
      { (*xpt) = 297 * mm; (*ypt) = 420 * mm; return; }
    if ((strcmp(papersize, "legal") == 0) || (strcmp(papersize, "Legal") == 0))
      { (*xpt) = 612.0; (*ypt) = 1008.0; return; /* 8,5 x 14 in, 216 x 356 mm */ }
    if ((strcmp(papersize, "executive") == 0) || (strcmp(papersize, "Executive") == 0))
      { (*xpt) = 540.0; (*ypt) = 720.0; return; /* 7,5 x 10 in, 190 x 254 mm */ }
    if ((strcmp(papersize, "ledger") == 0) || (strcmp(papersize, "Ledger") == 0)
      || (strcmp(papersize, "tabloid") == 0) || (strcmp(papersize, "Tabloid") == 0))
      { (*xpt) = 792.0; (*ypt) = 1224.0; return; /* 11 x 17 in, 279 x 432 mm */ }
    if ((strcmp(papersize, "a10") == 0) || (strcmp(papersize, "A10") == 0))
      { (*xpt) = 26 * mm; (*ypt) = 37 * mm; return; }
    if ((strcmp(papersize, "a9") == 0) || (strcmp(papersize, "A9") == 0))
      { (*xpt) = 37 * mm; (*ypt) = 52 * mm; return; }
    if ((strcmp(papersize, "a8") == 0) || (strcmp(papersize, "A8") == 0))
      { (*xpt) = 52 * mm; (*ypt) = 74 * mm; return; }
    if ((strcmp(papersize, "a7") == 0) || (strcmp(papersize, "A7") == 0))
      { (*xpt) = 74 * mm; (*ypt) = 105 * mm; return; }
    if ((strcmp(papersize, "a6") == 0) || (strcmp(papersize, "A6") == 0))
      { (*xpt) = 105 * mm; (*ypt) = 148 * mm; return; }
    if ((strcmp(papersize, "a5") == 0) || (strcmp(papersize, "A5") == 0))
      { (*xpt) = 148 * mm; (*ypt) = 210 * mm; return; }
    if ((strcmp(papersize, "a2") == 0) || (strcmp(papersize, "A2") == 0))
      { (*xpt) = 420 * mm; (*ypt) = 594 * mm; return; }
    if ((strcmp(papersize, "a1") == 0) || (strcmp(papersize, "A1") == 0))
      { (*xpt) = 594 * mm; (*ypt) = 841 * mm; return; }
    if ((strcmp(papersize, "a0") == 0) || (strcmp(papersize, "A0") == 0))
      { (*xpt) = 841 * mm; (*ypt) = 1189 * mm; return; }
    if ((strcmp(papersize, "2a0") == 0) || (strcmp(papersize, "2A0") == 0))
      { (*xpt) = 1189 * mm; (*ypt) = 1682 * mm; return; }
    if ((strcmp(papersize, "4a0") == 0) || (strcmp(papersize, "4A0") == 0))
      { (*xpt) = 1682 * mm; (*ypt) = 2378 * mm; return; }
    assert(0, "unkown paper size");
  }
