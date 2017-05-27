/* pswraux.h -- internal prototypes for pswr.c. */
/* Last edited on 2003-01-08 20:57:03 by stolfi */

#ifndef pswraux_H
#define pswraux_H

#include <stdio.h>
#include <pswr.h>

/* INTERNAL SCALING/CLIPPING PROCEDURES */

void pswr_save_default_window_data
  ( PSWriter *pswr,
    double hmin, double hmax,
    double vmin, double vmax
  );
  /* Saves the given data as the current plot rectangle
    ("pswr->hmin,pswr->hmax,pswr->vmin,pswr->vmax"), and also as the current
    client coordinate ranges ("pswr->xmin,pswr->xmax,pswr->ymin,pswr->ymax"). Sets
    the current cell grid ("pswr->xn,pswr->yn") to a single cell. */
    
void pswr_save_window_data
  ( PSWriter *pswr,
    double xmin, double xmax,
    double ymin, double ymax,
    double hmin, double hmax,
    double vmin, double vmax,
    int xn, int yn
  );
  /* Saves the given data as the current plot rectangle
    ("pswr->hmin,pswr->hmax,pswr->vmin,pswr->vmax"), current client coordinate
    ranges ("pswr->xmin,pswr->xmax,pswr->ymin,pswr->ymax"), and current cell grid
    ("pswr->xn,pswr->yn"). */
    
/* FILE PREMBLES AND POSTAMBLES */

/* The file preamble (produced by both pswr_write_eps_file_header and
  pswr_write_ps_file_header) creates a Postscript dictionary
  "pswr$dict" that contains definitions for special Postscript
  operators and some state variables, such as the plotting rectangle
  and grid size. This dictionary is closed before the end of the
  preamble, andmust be re-opened at each page. */

void pswr_write_eps_file_header
  ( FILE *psfile, 
    const char *date,
    double hmin, double hmax,
    double vmin, double vmax
  );
  /* Writes the file preamble for an encapsulated Postscript file. The
    figure's "%%BoundingBox" will be set to the given values (in pt),
    rounded outwards to integers. Must be followed by
    pswr_write_page_header to complete the setup. */

void pswr_write_eps_file_trailer(FILE *psfile);
  /* Writes the file postamble for an encapsulated Postscript file
    (just the "%%Trailer" comment).  Must be preceded by
    pswr_write_page_trailer. */

void pswr_write_ps_file_header(FILE *psfile, const char *papersize, const char *date);
  /* Writes the file preamble (including operator definitions) for a
    non-encapsulated Postscript document file, that specifies the
    given paper size, ending with "%%EndProlog". Should be followed by
    pswr_write_page_header. */
    
void pswr_write_ps_file_trailer(FILE *psfile, int npages);
  /* Writes the trailer for a non-encapsulated Postscript document.
    The trailer is a "%%Trailer" line, plus the directive
    "%%Pages: N" where "N" is the given page count.  Must be preceded by
    pswr_write_page_trailer. */

/* PAGE PREAMBLES AND POSTAMBLES */

void pswr_write_page_header(FILE *psfile, const char *pagename, int pageseq);
  /* Writes the preamble for a new page of a Postscript file (EPS or
    non-EPS; if the former, may be called only once). The page
    preamble includes the "%%Page" directive, and commands that save
    the current Postscript state (with "save"), and open the
    "pswr$dict" dictionary. */
    
void pswr_write_page_trailer(FILE *psfile, int eps);
  /* Writes the postamble for a page in a Postscript file (EPS or
    non-EPS). 
    
    The page postamble includes a "showpage" command, if "eps" is
    false; then commands that close the "pswr$dict" dictionary, and
    restore the Postcript state that was saved when the page was
    started. */

/* MISCELLANEOUS WRITING PROCEDURES */

void pswr_write_proc_defs(FILE *psfile);
  /* Writes to "psfile" the definition of the operators assumed
    by other plotting and text writing commands. */
  
void pswr_write_date_and_page_show_cmds
  ( FILE *psfile, 
    const char *date, 
    const char *page
  );
  /* Writes to "pswr->psfile" some commands that print the 
    given date and page number near the bottom of the
    current page. */

void pswr_write_page_graphics_setup_cmds(FILE *psfile);
  /* Writes to "pswr->psfile" some commands that setup some graphics
    variables, suitable for the beginning of a new page or figure. */
  
void pswr_write_window_setup_cmds
  ( FILE *psfile,
    double hmin, double hmax,
    double vmin, double vmax,
    int xn, int yn
  );
  /* Writes to "psfile" some commands that setup the Postscript
    variables "xmin,xmax,ymin,ymax" to the values
    "hmin,hmax,vmin,vmax", and set the clip path to that rectangle.
    Those commands also set the Postscript variables "xn,yn" that
    define the cell grid; "xtext,ytext", which define
    the start of the current caption line; and 
    "dytext" that defines the caption interline
    spacing (in pt).  Finally, they set the "captionfont"
    varriable to the caption font, properly scaled. */

void pswr_write_label_font_setup_cmds(FILE *psfile, const char *font, double size);
  /* Writes to "psfile" commands that create an instance of the font
    named "font", scaled by "size" pt, and save it in the Postscript
    variable "labelfont" (which is used by ps_label). */

void pswr_write_ps_string(FILE *psfile, const char *text, const char *newline);
  /* Writes the given text out  to /psfile/, in Postscript form, properly
    quoting any special chars and parentheses. 
    
    Replaces any embedded '\n' by the given /newline/ string --- which
    could be, for example, ") show mynl (". */

void pswr_write_color(FILE *psfile, double *clr);
  /* Writes the color value "clr[0..2]" to the Postscript file,
    with 3 decimals, preceded by spaces.  */

#endif
