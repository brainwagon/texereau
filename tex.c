/*
 * Mirror Test Data Reduction program - 
 * Version 4.0+
 *
 * Tidied up a bit by Mark VandeWettering, re-indented,
 * removed unreferenced variables, use srand48/drand48,
 * and some such.
 */

#define VERSION "4.0+"

#define DO_GRAPHICS
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#ifndef DOS
#include <sys/ioctl.h>
#include <sys/termios.h>
#endif /*  */
#ifndef MACINTOSH
#include <malloc.h>
#endif /*  */
static char linebuf[1024];

static FILE *input_from;

static const char *GraphFile;

static double Version = -1;

int set_units (const char *u);


#ifdef MACINTOSH

/* need to define strdup() */
char *
strdup (const char *s)
{
    char *p = (char *) malloc (strlen (s) + 1);

    if (p)
	strcpy (p, s);
    return p;
}


#endif /*  */

#ifdef DOS
#include <dos.h>

#ifdef DO_GRAPHICS
#include <graphics.h>
#endif /* DO_GRAPHICS */

#endif /* DOS */

#include <stdarg.h>

#ifdef DOS
#include <conio.h>
#else /*  */
#define far
#include    <string.h>
#include    <ctype.h>
#include    <signal.h>
#define clrscr()
#define min(A, B)    ((A) > (B) ? (B) : (A))
#define max(A, B)    ((A) > (B) ? (A) : (B))
#endif /* DOS */

#ifdef DO_GRAPHICS
int maxx, maxy;


#endif /* DO_GRAPHICS */
int
getch ()
{
    static struct termios cooked;

    static struct termios raw;

    static int beenHere = 0;

    int c;

    int ret;

    if (input_from == stdin) {
	if (!beenHere) {
	    beenHere = 1;
	    tcgetattr (0, &cooked);
	    raw = cooked;
	    raw.c_lflag &= ~(ISIG | ICANON);
	}
	tcsetattr (0, TCSAFLUSH, &raw);
    }
    c = getc (input_from);
    ret = c;
    if (input_from == stdin) {
	tcsetattr (0, TCSAFLUSH, &cooked);
	if (ret == cooked.c_cc[VINTR] || ret == cooked.c_cc[VEOF]) {
	    exit (0);
	}
	if (ret != '\n') {
	    fputc ('\n', stderr);
	}
    }

    else {
	while (c != '\n' && c != EOF) {
	    c = getc (input_from);
	}
    }
    return ret;
}

void
wait_for_key ()
{

#ifndef DOS
    fprintf (stderr, "Hit 'Enter' to continue...");
#endif /*  */

    getch ();
}

#ifndef DOS
FILE *ps;

#ifdef DO_GRAPHICS

#define LEFT_TEXT    0
#define CENTER_TEXT    1
#define RIGHT_TEXT    2
#define TOP_TEXT    2
#define BOTTOM_TEXT    0

#define SOLID_LINE    "[]"
#define DOTTED_LINE    "[1 2]"
#define SOLID_FILL    1
#define BLACK        "0 0 0"
#define WHITE        "1 1 1"
#define LIGHTGRAY    "0.75 0.75 0.75"
#define CYAN        "0 1 1"
#define RED        "1 0 0"
void
setlinestyle (char *style, int a, int w)
{
    fprintf (ps, "%s 0 setdash %d setlinewidth\n", style, w);
} void

line (int x1, int y1, int x2, int y2)
{
    fprintf (ps, "newpath %d %d moveto %d %d lineto stroke\n", x1, maxy - y1,
	     x2, maxy - y2);
} void

displayFile (const char *file)
{
    char buf[1024];

    const char *cmd = getenv ("TEX_PSVIEWER");

    if (!cmd) {
	cmd = "ghostview";
    }
    sprintf (buf, "%s %s", cmd, file);
    if (system (buf) < 0) {
	wait_for_key ();
    }
}

void
closegraph ()
{
    fprintf (ps, "showpage\n");
    fclose (ps);
    ps = 0;
    displayFile (GraphFile);
} static int leftright = LEFT_TEXT;

static int topbottom = BOTTOM_TEXT;

void
settextjustify (int lr, int tb)
{
    leftright = lr;
    topbottom = tb;
}

void
outtextxy (int x, int y, char *buf)
{
    static char *just[] = { "pop 0", "2 div", ""
    };
    char c;

    fputc ('(', ps);
    while ((c = *buf++) != 0) {
	switch (c) {
	case '\b':
	    fputs ("\\b", ps);
	    break;
	case '\f':
	    fputs ("\\f", ps);
	    break;
	case '\n':
	    fputs ("\\n", ps);
	    break;
	case '\r':
	    fputs ("\\r", ps);
	    break;
	case '\t':
	    fputs ("\\t", ps);
	    break;
	case '(':
	    fputs ("\\(", ps);
	    break;
	case ')':
	    fputs ("\\)", ps);
	    break;
	default:
	    if (isprint (c))
		fputc (c, ps);

	    else
		fprintf (ps, "\\%03o", c);
	    break;
	}
    }

    /* note workaround for stringwidth returns 0 height */
    fprintf (ps,
	     ") dup stringwidth pop 10 %s neg %d add exch %s neg %d add exch moveto show\n",
	     just[topbottom], maxy - y, just[leftright], x);
}

void
setcolor (char *color)
{
    fprintf (ps, "%s setrgbcolor\n", color);
} void

setfillstyle (int type, char *color)
{
    setcolor (color);
}


void
bar (int left, int top, int right, int bottom)
{
    fprintf (ps,
	     "newpath %d %d moveto %d %d lineto %d %d lineto %d %d lineto closepath fill\n",
	     left, maxy - top, right, maxy - top, right, maxy - bottom, left,
	     maxy - bottom);
}


void
rectangle (int left, int top, int right, int bottom)
{
    fprintf (ps,
	     "newpath %d %d moveto %d %d lineto %d %d lineto %d %d lineto closepath stroke\n",
	     left, maxy - top, right, maxy - top, right, maxy - bottom, left,
	     maxy - bottom);
}

#endif /* DO_GRAPHICS */

#endif /* DOS */
int
safe_getch ()
{
    int c;

    int ret;

    if (input_from == stdin) {
	return getch ();
    }
    c = getc (input_from);
    if (input_from != stdin) {
	fprintf (stderr, "%c\n", c);
    }
    ret = c;
    while (c != '\n' && c != EOF)
	c = getc (input_from);
    return ret;
}


#define NRMAX 60		/* maximum number of sets of readings */
#define NZ 30			/* maximum number of zones */
#define MAXDIAM 3		/* maximum number of diameters to take readings on */
#define MAX(A,B)                ((A) > (B) ? (A) : (B))
#define MIN(A,B)                ((A) < (B) ? (A) : (B))

#define NEUTRAL                 0
#define ABOVE                   1
#define BELOW                   2
#define TXT                     "mirror.txt"

#define QUIT                    'q'
#define APETURE_FILE            'a'
#define CALC_TABLE              'c'
#define MIRROR_DATA             'd'
#define MIRROR_FILE             'f'
#define KEYBOARD                'k'
#define MONTE_CARLO             'm'
#define GRAPHS                  'g'
#define SAVE_SPECS              's'
#define ZONES                   'p'
#define UNITS                   'u'

struct Matrix
{
    double inner[NZ + 1];	/* boundaries of mask openings */
    double outer[NZ + 1];	/* boundaries of mask openings */
    double hm[NZ];		/* average radius of opening */
    double parab_surf[NZ];	/* expected reading for perfect mirror */
    double hm4f[NZ];		/* hm / 4f */
    double D1[NZ];		/* readings on diameter 1 */
    double D2[NZ];		/* readings on diameter 2 */
    double D3[NZ];		/* readings on diameter 2 */
    double D123[NZ];		/* avg of D1 through D3, set to readings for comp */
    double lamdac[NZ];		/* residual abberation at COC */
    double lamdaf[NZ];		/* abberation * 1e5? */
    double lamdafrho[NZ];	/* abberation / rho */
    double u[NZ];		/* temporary used in "calc" u * 1e6 */
    double wavefront[NZ + 1];	/* wavefront deviations */
};

struct MirrorData
{
    char *comments;
    double dia, roc, rho, cnst;
};

struct best
{
    int i, j, zone;
    double deviation, TrAbbr;
    double a, c;
};

struct Matrix M;
struct MirrorData Specs = { "", 8.0, 96.0, 0., 0. };

int free_specs;

static struct best best;

double nrand (double, double);

static double rdg[NZ][NRMAX];

double green_light = 21.6;

char *units = "inches";

int nzones = 4;

char TesterType = 'f';

char DiaOpt = '1';

int ndia = 1;			/* must match DiaOpt */

int nr;

int nsamp = 1;

int data_valid;

int readings_valid;

int _use_sphere;		/* true if we compare against a sphere */

static char *mirror_file_name = "mirror.dat";

static int mirror_free_it;

static char *reading_file_name = "reading.dat";

static int reading_free_it;


#ifdef DOS
static int far *head_keys = (int far *) 0x0040001aL;

static int far *tail_keys = (int far *) 0x0040001cL;

int
fast_kbhit (void)
{
    if (*head_keys != *tail_keys)
	return (1);

    else
	return (0);
}


#else /*  */
static int break_was_hit;

void
handle_break (int sig)
{
    signal (SIGINT, handle_break);
    if (sig)
	break_was_hit++;
}

int
check_break (void)
{
    int ret = break_was_hit;

    break_was_hit = 0;
    return ret;
}


#endif /* DOS */
char *
myfgets ()
{
    char *tmp = fgets (linebuf, sizeof (linebuf), input_from);

    if (!tmp)
	exit (0);
    if (*tmp)
	tmp[strlen (tmp) - 1] = '\0';
    return tmp;
}

int
get_yn (int def, int val, char *p, ...)
{
    int c;

    va_list args;

    for (;;) {
	va_start (args, p);
	vfprintf (stderr, p, args);
	va_end (args);
	c = tolower (safe_getch ());
	switch (c) {
	case 'y':
	case 'Y':
	    return 'Y';
	case 'n':
	case 'N':
	    return 'N';
	case 'q':
	case EOF:
	    exit (0);
	case '\0':
	    if (def)
		return val;

	    /* FALL THROUGH */
	default:
	    fprintf (stderr, "Please enter 'y' or 'n'\n");
	    break;
	}
    }
}

FILE *
get_file (char *p, char **var, int *free_it, char *mode)
{
    FILE *fp;

    for (;;) {
	fprintf (stderr, p, *var);
	myfgets ();
	if (linebuf[0]) {
	    if (*free_it && *var)
		free (*var);
	    *var = strdup (linebuf);
	    *free_it = 1;
	    break;
	}

	else if (*var)
	    break;
    }
    if (!(fp = fopen (*var, mode))) {
	fprintf (stderr, "Can't open %s for %s.  ", *var,
		 mode[0] == 'r' ? "reading" : "writing");
	wait_for_key ();
    }
    return fp;
}

int
check_version (const char *line, const char *type, double *v)
{
    int len = strlen (type);

    *v = -1;
    if (strncmp (line, "# TEX ", 6)) {
	return 0;
    }
    line += 6;
    if (strncmp (line, type, len) || line[len] != ' ') {
	fprintf (stderr, "ERROR: Wrong type of file. Looking for %s file\n",
		 type);
	return 0;
    }
    line += len + 1;
    sscanf (line, "version %lg", v);
    return 1;
}

double
get_double (int def, double val, char *p, ...)
{
    va_list args;

    double d;

    for (;;) {
	va_start (args, p);
	vfprintf (stderr, p, args);
	va_end (args);
	myfgets ();
	if (!linebuf[0] && def) {
	    if (input_from != stdin) {
		fprintf (stderr, "%g\n", val);
	    }
	    return val;
	}
	if (sscanf (linebuf, "%lg", &d) > 0) {
	    if (input_from != stdin) {
		fprintf (stderr, "%g\n", d);
	    }
	    return d;
	}
    }
}

int
get_int (int def, int val, char *p, ...)
{
    va_list args;

    int d;

    for (;;) {
	va_start (args, p);
	vfprintf (stderr, p, args);
	va_end (args);
	myfgets ();
	if (!linebuf[0] && def) {
	    if (input_from != stdin) {
		fprintf (stderr, "%d\n", val);
	    }
	    return val;
	}
	if (sscanf (linebuf, "%d", &d) > 0) {
	    if (input_from != stdin) {
		fprintf (stderr, "%d\n", d);
	    }
	    return d;
	}
    }
}

char *
get_string (int def, char *val, char *p, ...)
{
    va_list args;

    for (;;) {
	va_start (args, p);
	vfprintf (stderr, p, args);
	va_end (args);
	myfgets ();
	if (!linebuf[0] && def) {
	    if (input_from != stdin) {
		fprintf (stderr, "%s\n", val);
	    }
	    return val;
	}

	else if (linebuf[0]) {
	    if (input_from != stdin) {
		fprintf (stderr, "%s\n", linebuf);
	    }
	    return linebuf;
	}
    }
}

int
get_menu_selection (int def, int val, const char *choices, const char *p, ...)
{
    va_list args;

    for (;;) {
	int c;

	const char *s;

	int quit_seen;

	int first;

	va_start (args, p);
	vfprintf (stderr, p, args);
	va_end (args);
	c = tolower (safe_getch ());
	switch (c) {
	case EOF:
	case 'q':
	    return 'q';
	case '\n':
	    if (def)
		return tolower (val);
	}
	for (s = choices; *s; s++) {
	    if (*s == c)
		return c;
	}
	fprintf (stderr, "INVALID ENTRY\nPlease enter one of ");
	quit_seen = 0;
	first = 1;
	for (s = choices; *s; s++) {
	    if (*s == 'q')
		quit_seen = 1;
	    if (first) {
		fprintf (stderr, "'%c'", *s);
		first = 0;
	    }

	    else if (!s[1]) {
		if (quit_seen)
		    fprintf (stderr, " or '%c'", *s);

		else
		    fprintf (stderr, ", '%c' or 'q'", *s);
	    }

	    else {
		fprintf (stderr, ", '%c'", *s);
	    }
	}
	fprintf (stderr, ".\n\n");
    }
}

void
calc (struct Matrix *tm)
{
    double dev, MinAbr, MaxAbr, m, b, disc;

    double diff, x1, y1, x2, y2, a, c, y, t, maxdev, tdev;

    int i, j, k, type, maxzone;


    /* compute "suitable constant" cnst to be subtracted from the averaged
     * values so that max and min deviations are equal and opposite in sign...
     */
    tdev = Specs.cnst = 0;

    do {
	Specs.cnst += tdev / 150000;
	for (i = 0; i < nzones; i++)
	    tm->lamdaf[i] =
		tm->hm4f[i] * (tm->D123[i] - tm->parab_surf[i] -
			       Specs.cnst) * 100000;
	MinAbr = 1e+10;
	MaxAbr = -MinAbr;
	for (i = 0; i < nzones; i++) {
	    MaxAbr = MAX (MaxAbr, tm->lamdaf[i]);
	    MinAbr = MIN (MinAbr, tm->lamdaf[i]);
	}
	x1 = MinAbr + MaxAbr;
	if (tdev > 0 && fabs (x1) > fabs (tdev)) {
	    fprintf (stderr,
		     "Search for constant failed; program terminated\n");
	    fprintf (stderr, "Check your input for sanity\n");
	    exit (0);
	}
	tdev = x1;
    } while (fabs (tdev) > .001);
    best.TrAbbr = -1e5;
    for (j = 0; j < nzones; j++) {
	tm->lamdac[j] = tm->D123[j] - Specs.cnst - tm->parab_surf[j];	/* residual abberation at COC */

	/* transverse abberation compared with the Airy disk. To be plotted
	 * against average radii (hm) for the upper graph...*/
	tm->lamdafrho[j] = (tm->lamdaf[j] / 100000) / Specs.rho;
	if (fabs (tm->lamdafrho[j]) > best.TrAbbr)
	    best.TrAbbr = fabs (tm->lamdafrho[j]);
	tm->u[j] = -tm->lamdaf[j] * 2 * 10 / Specs.roc;
    }

    /* graph 2 values; wavefront deviations... */
    tm->wavefront[0] = 0;
    for (i = 1; i <= nzones; i++)
	tm->wavefront[i] =
	    tm->wavefront[i - 1] + tm->u[i - 1] * (tm->outer[i - 1] -
						   tm->inner[i - 1]);

    /* find the best reference parabola...
     */
    best.deviation = 1e5;
    for (i = 0; i < nzones; i++) {	/* try parabola thru each pair of points */
	for (j = i + 1; j <= nzones; j++) {
	    x1 = tm->inner[i], y1 = tm->wavefront[i];
	    x2 = tm->inner[j], y2 = tm->wavefront[j];

	    /* parabola equation: y = a * x**2 + c */
	    a = (y2 - y1) / (x2 * x2 - x1 * x1);
	    c = y1 - a * x1 * x1;

	    /* reject this parabola if points are on opposite sides of it */
	    type = NEUTRAL;
	    for (k = 0; k <= nzones; k++) {
		y = a * tm->inner[k] * tm->inner[k] + c;
		diff = y - tm->wavefront[k];
		if (fabs (diff) < 0.0001)
		    continue;
		switch (type) {
		case BELOW:
		    if (diff > 0)
			goto NextPara;
		    break;
		case ABOVE:
		    if (diff < 0)
			goto NextPara;
		    break;
		case NEUTRAL:
		    type = (diff > 0 ? ABOVE : BELOW);
		    break;
		}
	    }

	    /* if the 2 points are consecutive and ( (parabola curves up and
	     * other points are below it) or (parabola curves down and other
	     * points are above) ) */
	    if (j == i + 1 &&
		((a > 0 && type == ABOVE) || (a < 0 && type == BELOW)))
		goto NextPara;

	    /* reject if line segment intersects the parabola */
	    for (k = 0; k < nzones; k++) {
		m = tm->u[k];

		/* segment eqn: y=m*x + b... */
		b = tm->wavefront[k] - tm->u[k] * tm->inner[k];
		disc = m * m - 4 * a * (c - b);
		if (disc < 0)
		    continue;	/* line does not intersect the parabola */
		x1 = (m - sqrt (disc)) / (2 * a);
		x2 = (m + sqrt (disc)) / (2 * a);
		if (x1 > x2) {
		    t = x1;
		    x1 = x2;
		    x2 = t;
		}
		if (x1 > (tm->inner[k] + 0.001)
		    && x2 < (tm->inner[k + 1] - 0.001))
		    goto NextPara;	/* segment intersects parabola */
	    }

	    /* find maximum deviation from this parabola... */
	    maxdev = -1e5;
	    maxzone = -1;
	    for (k = 0; k <= nzones; k++) {
		if (k == i || k == j)
		    continue;
		y1 = a * tm->inner[k] * tm->inner[k] + c;
		dev = fabs (y1 - tm->wavefront[k]);
		if (dev > maxdev) {
		    maxdev = dev;
		    maxzone = k;
		}
	    }
	    if (maxdev < best.deviation) {
		best.deviation = maxdev;	/* best parabola found so far */
		best.i = i;
		best.j = j;
		best.a = a;
		best.c = c;
		best.zone = maxzone;
	    }
	  NextPara:;
	}
    }

    /* subtract ref parabola from wave front */
    for (k = 0; k <= nzones; k++) {
	y1 = best.a * tm->inner[k] * tm->inner[k] + best.c;
	tm->wavefront[k] -= y1;
    }
}

char *couder = { "%!PS+Adobe+2.0\n"
	"\n"
	"% Couder mask generator\n"
	"% Copyright (C) 1997 by Michael Lindner\n"
	"% This code is free, and not warranted to work \n"
	"% You may modify and freely distribute this code provided\n"
	"% (1) you do not charge for it, and\n"
	"% (2) you leave this notice in.\n"
	"\n"
	"% translates inches, cm or mm to points\n"
	"/units 72 def\n"
	"/inches { /units 72 def } def\n"
	"/centimeters { /units 72 2.54 div def } def\n"
	"/millimeters { /units 72 25.4 div def } def\n"
	"\n"
	"% radius of rounded off corners\n"
	"/corner 0.125 72 mul def\n"
	"\n"
	"% maximum angular width (in degrees) of openings\n"
	"/angwidth 20 def\n"
	"\n"
	"% figure out page dimensions\n"
	"clippath pathbbox newpath\n"
	"4 -1 roll exch 4 1 roll 4 copy\n"
	"sub /pagewidth exch def\n"
	"sub /pageheight exch def\n"
	"add 2 div /xcenter exch def\n"
	"add 2 div /ycenter exch def\n"
	"\n"
	"% draw outline of mirror, arrange scale, etc.\n"
	"% if mirror larger than page, try to fit as much as possible\n"
	"/mirror {\n"
	"    units mul\n"
	"    xcenter ycenter translate\n"
	"    90 rotate\n"
	"    dup pageheight gt { dup pageheight sub 2 div 0 translate } if\n"
	"    0.5 setlinewidth\n"
	"    2 div dup 0 0 3 -1 roll 0 360 newpath arc stroke\n"
	"    angwidth sin mul /height exch def\n"
	"    newpath 0 0 1 0 360 arc stroke\n" "} def\n" "\n"
	"% draw 1/2 of center zone\n" "% uses global corner\n"
	"/zonecenter {\n"
	"    outer height lt { /height outer 0.9 mul def } if\n"
	"    newpath 0 0 outer height 2 copy dup mul exch dup mul exch sub sqrt 2 copy exch outer neg 0 moveto lineto atan 0 arcn stroke\n"
	"} def\n" "% find center of circles to round off corners\n"
	"% uses globals corner and height\n" "/findcorners {\n"
	"    dup mul\n"
	"    height corner sub exch 2 copy exch dup mul sub sqrt\n"
	"    exch\n" "    corner exch 1 index dup mul sub sqrt\n"
	"    4 copy\n" "} def\n" "% draw one side of one zone\n"
	"% uses globals inner, outer, corner\n" "/zonepart {\n"
	"    inner height lt { /height inner 0.9 mul def } if\n"
	"    newpath\n" "        inner corner add findcorners\n"
	"        4 copy atan /abot exch def\n"
	"        atan /atop exch def\n"
	"        dup /xbot exch def exch corner 270 abot 180 add arcn\n"
	"        0 0 inner abot atop arc\n"
	"        exch corner atop 180 add 90 arcn\n"
	"        outer corner sub findcorners\n"
	"        4 copy atan /abot exch def\n"
	"        atan /atop exch def\n" "        4 2 roll\n"
	"        dup /xtop exch def exch corner 90 atop arcn\n"
	"        0 0 outer atop abot arcn\n"
	"        exch corner abot 270 arcn\n" "    closepath stroke\n"
	"} def\n" "% draw both sides of zone, handle center zone\n"
	"% sets globals inner and outer\n" "/zone {\n"
	"    units mul /outer exch def\n" "    units mul /inner exch def\n"
	"    inner 0 gt {\n" "        zonepart\n" "        180 rotate\n"
	"        zonepart\n" "    } {\n" "        zonecenter\n"
	"        180 rotate\n" "        zonecenter\n" "    } ifelse\n"
	"    1 -1 scale\n" "} def\n" "\n"
	"% Note that the program works best if you specify zones from the center out.\n"
	"% Don't forget to put a 'showpage' command after the mirror and zone\n"
	"% commands, or it will just generate the page but not print it!\n"
	"\n"
};

void
get_nzones ()
{
    nzones = get_int (1, nzones, "Number of mask zones [%d]: ", nzones);
    if (nzones > NZ) {
	nzones = NZ;
	fprintf (stderr, "Too many zones, changing it to %d\n", nzones);
    }
}

void
enter_mask ()
{
    int i;

    M.inner[0] = get_double (1, 0., "Inside radius of zone 1 [%.3f]: ", 0.);
    for (i = 1; i <= nzones; i++) {
	M.inner[i] = sqrt (M.inner[i - 1] * M.inner[i - 1] +
			   (Specs.dia * Specs.dia / 4. -
			    M.inner[i - 1] * M.inner[i - 1]) / (nzones + 1 -
								i));
	M.outer[i - 1] = M.inner[i] =
	    get_double (1, M.inner[i], "Outside radius of zone %d [%.3f]: ",
			i, M.inner[i]);
	M.hm[i - 1] = (M.outer[i - 1] + M.inner[i - 1]) / 2;	/* average zone width */
	if (_use_sphere)

	    /* longitudinal deviation for a sphere is zero */
	    M.parab_surf[i - 1] = 0;

	else
	    /* theoretical longitudinal deviation (fixed source tester) */
	    M.parab_surf[i - 1] = M.hm[i - 1] * M.hm[i - 1] / Specs.roc;
	M.hm4f[i - 1] = M.hm[i - 1] / (2 * Specs.roc);
    }
}

void
print_mask ()
{
    static char *file_name = "couder.ps";

    static int free_it;

    double diameter;

    double def_diameter;

    int i;

    FILE *fp;

    if (!
	(fp =
	 get_file ("Couder mask file name [%s]: ", &file_name, &free_it,
		   "wt")))
	return;
    fprintf (fp, "%s", couder);
    def_diameter = floor (Specs.dia + 0.5);
    if (def_diameter <= Specs.dia + 0.01) {
	double bevel = 0.125;

	switch (units[0]) {
	case 'm':
	    bevel = 3.;
	    break;
	case 'c':
	    bevel = 0.3;
	    break;
	}
	def_diameter = Specs.dia + bevel;
    }
    diameter =
	get_double (1, def_diameter,
		    "Enter diameter of glass including bevel (NOT optical diameter).\nThis is used to print registration marks to line up the mask\nwith the edge of the glass [%.3f]: ",
		    def_diameter);
    fprintf (fp, "%s\n%lg mirror\n", units, diameter);
    for (i = 0; i < nzones; i++)
	fprintf (fp, "%lg %lg zone\n", M.inner[i], M.outer[i]);
    fprintf (fp, "showpage\n");
    fclose (fp);

#ifndef DOS
    displayFile (file_name);

#endif /*  */
}

void
calc_mask ()
{
    int i;

    double AreaPerZone, center, OR, IR;

    get_nzones ();
    M.inner[0] =
	get_double (1, M.inner[0],
		    "Inside radius of zone 1 (may be 0) [%.3f]: ",
		    M.inner[0]);
    IR = M.inner[0];
    center = IR * IR;
    AreaPerZone = (Specs.dia * Specs.dia / 4 - center) / nzones;
    fprintf (stderr, "\nZone\tIR\tOR\tID\tOD\n");
    for (i = 1; i <= nzones; i++) {
	OR = sqrt (center + i * AreaPerZone);
	fprintf (stderr, "%2d %7.3f %7.3f %7.3f %7.3f\n", i, IR, OR, 2 * IR,
		 2 * OR);
	M.outer[i - 1] = M.inner[i] = OR;
	M.hm[i - 1] = (M.outer[i - 1] + M.inner[i - 1]) / 2;	/* average zone width */
	if (_use_sphere)

	    /* longitudinal deviation for a sphere is zero */
	    M.parab_surf[i - 1] = 0;

	else
	    /* theoretical longitudinal deviation (fixed source tester) */
	    M.parab_surf[i - 1] = M.hm[i - 1] * M.hm[i - 1] / Specs.roc;
	M.hm4f[i - 1] = M.hm[i - 1] / (2 * Specs.roc);
	IR = OR;
    }
}

void
FmtMatrix (int nzones, int nsamp, struct MirrorData *S, struct Matrix *M,
	   FILE * fpt)
{
    int i;

    fprintf (fpt, "\n\tTEXEREAU MIRROR TEST SHEET\n\n");
    fprintf (fpt, "           Comments: %s\n", S->comments);
    fprintf (fpt, "   Optical diameter: %g\n", (double) S->dia);
    fprintf (fpt, "  Readings per zone: %d\n", nsamp);
    fprintf (fpt, "Radius of curvature: %g\n", (double) S->roc);
    fprintf (fpt, "                f/D: %5.2f\n",
	     (double) S->roc / S->dia / 2);
    fprintf (fpt, "   Diffraction disc: %g\n", (double) S->rho);
    fprintf (fpt, "\n 1 %-16s", "ZONE");
    for (i = 1; i <= nzones; i++)
	fprintf (fpt, "    %-5d  ", i);
    fprintf (fpt, "\n 2 %-13s", "h(x)");
    for (i = 1; i <= nzones; i++)
	fprintf (fpt, "%11.4f", M->inner[i]);
    fprintf (fpt, "\n 3 %-13s", "h(m)");
    for (i = 0; i < nzones; i++)
	fprintf (fpt, "%11.4f", M->hm[i]);
    fprintf (fpt, "\n 4 %-13s", "hm**2/R");
    for (i = 0; i < nzones; i++)
	fprintf (fpt, "%11.4f", M->parab_surf[i]);
    fprintf (fpt, "\n 5 %-13s", "hm/4f");
    for (i = 0; i < nzones; i++)
	fprintf (fpt, "%11.4f", M->hm4f[i]);
    fprintf (fpt, "\n 6 %-13s", "D1");
    for (i = 0; i < nzones; i++)
	fprintf (fpt, "%11.4f", M->D1[i]);
    fprintf (fpt, "\n 7 %-13s", "D2");
    for (i = 0; i < nzones; i++)
	fprintf (fpt, "%11.4f", M->D2[i]);
    fprintf (fpt, "\n   %-13s", "D3");
    for (i = 0; i < nzones; i++)
	fprintf (fpt, "%11.4f", M->D3[i]);
    fprintf (fpt, "\n 8 %-13s", "D123");
    for (i = 0; i < nzones; i++)
	fprintf (fpt, "%11.4f", M->D123[i]);
    if (S->cnst <= 0.)
	fprintf (fpt, "\n 9 D123 + %6.4f", (double) fabs (S->cnst));

    else
	fprintf (fpt, "\n 9 D123 - %6.4f", (double) S->cnst);
    for (i = 0; i < nzones; i++)
	fprintf (fpt, "%11.4f", M->D123[i] - S->cnst);
    fprintf (fpt, "\n10 %-13s", "Lamda c");
    for (i = 0; i < nzones; i++)
	fprintf (fpt, "%11.4f", M->lamdac[i]);
    fprintf (fpt, "\n11 %-13s", "Lamda f * 1e5");
    for (i = 0; i < nzones; i++)
	fprintf (fpt, "%11.2f", M->lamdaf[i]);
    fprintf (fpt, "\n12 %-13s", "Lamda f / rho");
    for (i = 0; i < nzones; i++)
	fprintf (fpt, "%11.3f", M->lamdafrho[i]);
    fprintf (fpt, "\n13 %-13s", "u * 1E6");
    for (i = 0; i < nzones; i++)
	fprintf (fpt, "%11.2f", M->u[i]);
    fprintf (fpt, "\n14 %-13s", "Wavefront");
    for (i = 1; i <= nzones; i++)
	fprintf (fpt, "%11.2f", M->wavefront[i]);

#ifndef DO_GRAPHICS
    fprintf (fpt,
	     "\n\nGraph 1: Transverse abberration relative to theoretical\n");
    fprintf (fpt, "\tdiffraction disk radius");
    fprintf (fpt, "\tPlot points:");
    for (i = 0; i < nzones; i++) {
	if (i % 3 == 0)
	    fprintf (fpt, "\n\t\t");
	fprintf (fpt, "(%6.2f,%6.2f) ", (double) M->hm[i],
		 (double) M->lamdafrho[i]);
    } fprintf (fpt,
	       "\n\nGraph 2: Wavefront deviations from spherical wavefront\n");
    fprintf (fpt, "\tPlot points:");
    for (i = 0; i <= nzones; i++) {
	if (i % 3 == 0)
	    fprintf (fpt, "\n\t\t");
	fprintf (fpt, "(%6.2f,%6.2f) ", (double) M->inner[i],
		 (double) M->wavefront[i]);
    }
#endif /*  */
    fprintf (fpt, "\n\tReference parabola: y = %g * x**2 + %g\n", best.a,
	     best.c);

    /* fprintf(fpt, "Best fit ROC: %g\n", 0.5 / best.a); */
    fprintf (fpt,
	     "\nMaximum wavefront error = 1 / %3.1f wave at zone %d\n",
	     green_light / best.deviation, best.zone);
    return;
}

void
PrFile ()
{
    int ch;

    FILE *fpt;

    fpt = fopen (TXT, "r");
    while ((ch = getc (fpt)) != EOF)
	putchar (ch);
    fclose (fpt);
    fprintf (stderr, "Results data above is in file %s\n", TXT);
}

void
readings_invalid (void)
{
    clrscr ();
    fputs ("You must input the readings first.\n", stderr);
    wait_for_key ();
} void

data_invalid (void)
{
    clrscr ();
    fputs ("You must input the mirror constants first.\n", stderr);
    wait_for_key ();
} double

to_fixed (double rdg)
{
    if (TesterType == 'm')
	rdg *= 2.0;
    return rdg;
}

double
from_fixed (double rdg)
{
    if (TesterType == 'm')
	rdg /= 2.0;
    return rdg;
}

int
cmp (const void *v1, const void *v2)
{
    if (*(double *) v1 > *(double *) v2)
	return 1;

    else if (*(double *) v1 == *(double *) v2)
	return 0;

    else
	return -1;
}


/* save aperture format wave error file */
void
save_aper (void)
{
    FILE *fp;

    int i;

    static char *file_name = "aper.dat";

    static int free_it;

    clrscr ();
    fputs ("                 Write an Aperture File\n"
	   "Write a wave error file for input into the Aperture program written\n"
	   "by Dick Suiter.\n\n"
	   "Aperture is available from Willman-Bell, Inc.  It will compute performance\n"
	   "data for a telescope with this or any mirror as compared to a perfect\n"
	   "mirror.\n\n"
	   "It will also simulate a star image as it will appear at various\n"
	   "focus settings for this mirror.  It requires an IBM Compatable with\n"
	   "VGA graphics.\n\n\n", stderr);
    if (!
	(fp =
	 get_file ("Aperture file name [%s]: ", &file_name, &free_it, "wt")))
	return;
    fprintf (fp, "%d\n", nzones);

    /* compute wave error at each outside zone edge */
    for (i = 0; i < nzones; i++) {
	double err;


	/* ref parabola */
	double y = best.a * M.inner[i] * M.inner[i] + best.c;

	double w;

	err = y - (M.wavefront[i]);
	w = err / green_light;
	fprintf (fp, "%6.2lf %7.4lf\n", 2. * M.hm[i] / Specs.dia, w);
    } fclose (fp);
}

/* save mirror constants in a file */
void
save_specs (void)
{
    FILE *fp;

    int i;

    if (!data_valid) {
	data_invalid ();
	return;
    }
    clrscr ();
    if (!
	(fp =
	 get_file ("Save file name [%s]: ", &mirror_file_name,
		   &mirror_free_it, "wt")))
	return;
    fprintf (fp, "# TEX mirror version " VERSION "\n");
    if (Specs.comments)
	fprintf (fp, "%s\n", Specs.comments);

    else
	fprintf (fp, " \n");
    fprintf (fp, "%s\n", units);
    fprintf (fp, "%6.2lf\n", Specs.dia);
    fprintf (fp, "%6.2lf\n", Specs.roc);
    fprintf (fp, "%d\n", nzones);
    fprintf (fp, "%d\n", nsamp);
    fprintf (fp, "%c\n", TesterType);
    fprintf (fp, "%c\n", DiaOpt);
    for (i = 0; i <= nzones; i++)
	fprintf (fp, "%6.3lf\n", M.inner[i]);
    fclose (fp);
}


/* save readings in a file */
void
save_readings (void)
{
    FILE *fp;

    int i;

    int j;

    int k;

    if (!readings_valid) {
	readings_invalid ();
	return;
    }
    clrscr ();
    if (!
	(fp =
	 get_file ("Save file name [%s]: ", &reading_file_name,
		   &reading_free_it, "wt")))
	return;
    fprintf (fp, "# TEX readings version " VERSION "\n");
    fprintf (fp, "%s\n", units);
    fprintf (fp, "%c\n", TesterType);
    fprintf (fp, "%d\n", nzones);
    for (i = 0; i <= nzones; i++) {
	fprintf (fp, "%6.3lf\n", M.inner[i]);
    }
    fprintf (fp, "%d\n", ndia);
    fprintf (fp, "%d\n", nsamp);
    for (i = 0; i < ndia; i++) {
	for (j = 0; j < nzones; j++) {
	    int nr = i * nsamp;

	    for (k = 0; k < nsamp; k++) {
		fprintf (fp, "%.4f\n", from_fixed (rdg[j][nr]));
		nr++;
	    }
	}
    }
    fclose (fp);
}


#ifdef DO_GRAPHICS
/*
 * scale -      for scaling the axis of a plot. given the minimum and maximum
 *              values in the data and a suggested number of intervals, the
 *              returned structure defines a starting/ending value, distance
 *              between tick marks, and the number of intervals (# ticks - 1).
 *              adapted from the UNIX S "pretty" function
 */
struct scalev
{
    int nint;			/* number of intervals */
    int ndecimals;		/* number of places to right of decimal */
    double minval, maxval;	/* first, last value on the axis */
    double incr;		/* distance between tick marks */
};


#define NINT 5
#define ABS(A) ( (A) < 0.0 ? -(A) : (A) )
struct scalev *
scale (double umina, double umaxa, int na)
{

#       define DEL .0002
    static struct scalev s = { 0, 0, 0.0, 0.0, 0.0 };
    static double vint[] = { 1.0, 2.0, 5.0, 10.0 };
    static double sqr[] = { 1.414214, 3.162278, 7.071068 };
    int n, i, nal, m1, m2;

    double umin, umax, dd, a, al, b, fn;

    double fm1, fm2;

    n = MAX (na, 1);
    umin = MIN (umina, umaxa);
    umax = MAX (umina, umaxa);
    if (umin == umax) {
	dd = ABS (umin);
	dd = MAX (0.25 * dd, 1.0e-2);
	umin = umin - dd;
	umax = umax + dd;
	n = 1;
    }
    fn = n;
    a = (umax - umin) / fn;
    al = log10 ((double) a);
    nal = al;
    if (a < 1.0)
	nal--;
    b = a / pow (10.0, (double) nal);
    for (i = 0; i < 3; i++) {
	if (b < sqr[i])
	    break;
    }
    s.incr = vint[i] * pow (10.0, (double) nal);
    if (s.incr < 0.00075)
	s.ndecimals = 4;

    else if (s.incr < 0.0075)
	s.ndecimals = 3;

    else if (s.incr < 0.075)
	s.ndecimals = 2;

    else if (s.incr < 0.75)
	s.ndecimals = 1;

    else
	s.ndecimals = 0;
    fm1 = umin / s.incr;
    m1 = fm1;
    if (fm1 < 0.0)
	m1--;
    dd = (double) m1 + 1.0 - fm1;
    if (ABS (dd) < DEL)
	m1++;
    s.minval = s.incr * (double) m1;
    fm2 = umax / s.incr;
    m2 = fm2 + 1;
    if (fm2 < -1.0)
	m2--;
    dd = fm2 + 1.0 - (double) m2;
    if (ABS (dd) < DEL)
	m2--;
    s.maxval = s.incr * (double) m2;
    if (s.minval > umin)
	s.minval = umin;
    if (s.maxval < umax)
	s.maxval = umax;
    s.nint = ABS (m2 - m1);
    return &s;
}


/* initialize borland graphics */
/*  saves screen max x and y in globals maxx and maxy */
void
open_graph (char *file)
{

#ifdef DOS
    /* request auto detection */
    int gdriver = DETECT, gmode, errorcode;

    errorcode = registerbgidriver (EGAVGA_driver);

    /* initialize graphics mode */
    initgraph (&gdriver, &gmode, "");

    /* read result of initialization */
    errorcode = graphresult ();
    if (errorcode != grOk) {	/* an error occurred */
	fprintf (stderr, "Graphics error: %s\n", grapherrormsg (errorcode));
	fprintf (stderr, "Press any key to halt:");
	exit (1);		/* return with error code */
    }

    /* record screen limits */
    maxx = getmaxx ();
    maxy = getmaxy ();

#else /*  */
    char *file_name = file;

    int free_it = 0;

    maxx = 7.5 * 72;
    maxy = 10 * 72;
    while (!
	   (ps =
	    get_file ("\nPostScipt output file name? [%s] ", &file_name,
		      &free_it, "wt")))
	perror (file_name);
    if (free_it)
	free (file_name);
    fprintf (ps,
	     "%%!PS-Adobe-2.0\n/Helvetica findfont 12 scalefont setfont\n36 36 translate\n");
    GraphFile = file_name;

#endif /*  */
}


/* draw lines between a list of points
 * input is:  pointer to array of x points
 *            pointer to array of y points
 *              number of points
 *            xscale
 *            offset in screen pixels to be added x after scaling
 *            yscale
 *            offset in screen pixels to be added to y after scaling
 */
void
plotxy (double *x, double *y, int num, double xscale, int xoffset,
	double yscale, int yoffset)
{
    int i;

    for (i = 0; i < num - 1; i++) {
	line (xoffset + x[i] * xscale, maxy - (yoffset + y[i] * yscale),
	      xoffset + x[i + 1] * xscale,
	      maxy - (yoffset + y[i + 1] * yscale));
    }
}


/*  Find min and max values of an array of doubles.
 *  input is a pointer to the array, the number of values,
 *  pointers to where min and max results are stored
 *
 *   Returns max - min
 */
double
find_range (double *val, int num, double *maxv, double *minv)
{
    int i;

    double minval = HUGE_VAL;

    double maxval = -HUGE_VAL;

    for (i = 0; i < num; i++) {
	minval = min (minval, val[i]);
	maxval = max (maxval, val[i]);
    }
    *maxv = maxval;
    *minv = minval;
    return (maxval - minval);
}


/* write vertical text */
void
vert_text (int x, int y, char *str)
{
    int i;

    settextjustify (CENTER_TEXT, TOP_TEXT);
    for (i = 0; str[i]; i++) {
	char buf[2];

	buf[0] = str[i];
	buf[1] = 0;
	outtextxy (x, y + 10 * i, buf);
}}

/* make Millies-Lacroix tolerance plot of data in global M */
void
make_ml_graph (int nzones)
{
    double maxdx, maxdy, mindy;

    double yscale;

    double xscale;

    double pointx[20];

    double pointy[20];

    double tmaxy, tminy;

    int xoffset = maxx / 9;	/* left edge of plot window */

    int yoffset;

    int top = maxy / 2 + 2;	/* top of plot window */

    int bottom = maxy - 2;	/* bottom of plot window */

    int right = maxx - 2;	/* right side of plot window */
    char *ytitle = { "Lambda C" };
    char buf[20];

    int i;

    double xstart = M.hm[0];

    double del;

    struct scalev *s;

    maxdx = Specs.dia / 2l;
    xscale = (maxx - 10 - xoffset) / maxdx;

    /* fill plot with blue background */
    setfillstyle (SOLID_FILL, WHITE);

    /* left top right bottom */
    bar (10, top, right, bottom);
    setcolor (LIGHTGRAY);
    rectangle (10, top, right, bottom);
    bottom -= 48;		/* save room for x labels and title */
    top += 12;
    right -= 5;			/* leave a 5 pixel border at left of graph */

    /* calculate tolerance */
    del = (maxdx - M.hm[0]) / 9;
    for (i = 0; i < 20; i++) {
	pointx[i] = xstart + del * (double) i;
	pointy[i] = 2. * Specs.roc * Specs.rho / pointx[i];
    }
    /* find the y range from the tolerance */
    find_range (pointy, 10, &maxdy, &mindy);

    /* find the y range of the measurments */
    find_range (M.lamdac, nzones, &tmaxy, &tminy);

    /* if measurments have the larger values use them */
    if (tmaxy > maxdy)
	maxdy = tmaxy;
    if (tminy < mindy)
	mindy = tminy;

    /* if absolute min is larger than max use it as max */
    if (fabs (tminy) > maxdy)
	maxdy = fabs (tminy);

    /* get y scale label increments */
    s = scale (maxdy, -maxdy, 15);

    /* fit range into plot window */
    yscale = (bottom - top) / (s->maxval - s->minval);
    settextjustify (RIGHT_TEXT, CENTER_TEXT);

    /* grid and lables are black */
    setcolor (BLACK);

    /* x axis will be in middle of plot window */
    yoffset = (top + bottom) / 2;
    line (xoffset, yoffset, right, yoffset);

    /* draw y axis */
    line (xoffset, top, xoffset, bottom);

    /* label y axis and make tick marks */
    for (i = 0; i <= s->nint; i++) {
	double y = s->minval + i * s->incr;

	int ypoint = yoffset - yscale * y;


	/* label every other increment */
	if ((i & 0x1) == 0) {
	    sprintf (buf, "%5.*lf", s->ndecimals, y);
	    outtextxy (xoffset - 3, ypoint, buf);
	    line (xoffset, ypoint, xoffset + 5, ypoint);
	}

	else {
	    line (xoffset, ypoint, xoffset + 3, ypoint);
	}
    }

    /* Vertical title for y */
    /* y title start a middle - 1/2 title length */
    vert_text (xoffset - 44, (top + bottom) / 2 - strlen (ytitle) * 5,
	       ytitle);
    setcolor (BLACK);
    setlinestyle (SOLID_LINE, 0, 3);
    plotxy (M.hm, M.lamdac, nzones, xscale, xoffset, yscale, maxy - yoffset);
    setlinestyle (SOLID_LINE, 0, 1);
    setcolor (CYAN);
    plotxy (pointx, pointy, 10, xscale, xoffset, yscale, maxy - yoffset);
    for (i = 0; i < 10; i++) {
	pointy[i] = -pointy[i];
    }
    plotxy (pointx, pointy, 10, xscale, xoffset, yscale, maxy - yoffset);

    /* draw bottom line of graph */
    setcolor (BLACK);
    line (xoffset, bottom, maxx - 10, bottom);

    /* label x axis */
    settextjustify (CENTER_TEXT, TOP_TEXT);
    bottom += 12;
    setlinestyle (DOTTED_LINE, 0, 1);
    for (i = 0; i < nzones; i++) {
	int x = xoffset + xscale * M.hm[i];

	sprintf (buf, "%6.2lf", M.hm[i]);
	outtextxy (x, bottom, buf);
	line (x, bottom - 2, x, top);
    } bottom += 12;
    outtextxy ((xoffset + right) / 2, bottom, "Zone Center");
    bottom += 12;
    outtextxy ((xoffset + right) / 2, bottom, "Millies-Lacroix Tolerance "
	       "(parabola removed)");
    setlinestyle (SOLID_LINE, 0, 1);
}

/* make surface error graph of data in global M */
void
make_surface_graph (int nzones)
{
    double maxdx, maxdy, mindy;	/* min and max values of data */

    double yscale;

    double xscale;

    double pointy[NZ];		/* temp y points */

    double range;

    int i;

    int xoffset = maxx / 9;	/* leave room on left side for y labels */

    int yoffset;


    /* use the upper half of the screen */
    int bottom = maxy / 2 - 2;	/* bottom edge of plot window */

    int top = 2;		/* top edge of plot window */

    int right = maxx - 2;	/* right side of plot window */

    char *ytitle = "Wavelength";

    char buf[20];


    /* fill plot with blue background */
    setfillstyle (SOLID_FILL, WHITE);

    /* left top right bottom */
    bar (10, top, right, bottom);

    /* draw a white border */
    setcolor (LIGHTGRAY);
    rectangle (10, top, right, bottom);

    /* the radius is the max x value */
    maxdx = Specs.dia / 2.;

    /* leave a 5 pixel border inside plot window */
    right -= 5;

    /* set x scale.  Leave a 10 pixel border on the right side */
    xscale = (right - xoffset) / maxdx;
    for (i = 0; i < nzones + 1; i++)
	pointy[i] = M.wavefront[i];
    range = find_range (pointy, nzones + 1, &maxdy, &mindy);
    if (range < 2.)
	range = 2.;

    /* leave a boader at top of 12 pixels */
    top += 12;

    /* leave a border at the bottom for labels */
    bottom -= 44;

    /* draw x grid zone lines */
    setcolor (BLACK);
    setlinestyle (DOTTED_LINE, 0, 1);
    for (i = 0; i <= nzones; i++)
	line (xoffset + M.inner[i] * xscale, top,
	      xoffset + M.inner[i] * xscale, bottom);
    yscale = (bottom - top) / (2. * range);
    yoffset = (bottom + top) / 2;
    setcolor (BLACK);
    setlinestyle (SOLID_LINE, 0, 3);
    plotxy (M.inner, pointy, nzones + 1, xscale, xoffset, yscale,
	    maxy - yoffset);
    setlinestyle (SOLID_LINE, 0, 1);
    setcolor (BLACK);

    /* draw y axis */
    line (xoffset, yoffset, right, yoffset);

    /* draw top line of graph */
    line (xoffset, top, right, top);

    /* draw bottom line of graph */
    line (xoffset, bottom, right, bottom);

    /* label graph */

    /* y labels */
    settextjustify (RIGHT_TEXT, CENTER_TEXT);
    sprintf (buf, "%6.2lf", range / green_light);
    outtextxy (xoffset - 3, top, buf);
    sprintf (buf, "-%6.2lf", range / green_light);
    outtextxy (xoffset - 3, bottom, buf);
    outtextxy (xoffset - 3, yoffset, "0");

    /* Vertical title for y */
    /* y title start a middle - 1/2 title length */
    vert_text (xoffset - 30, (top + bottom) / 2 - (strlen (ytitle) * 5),
	       ytitle);
    bottom += 10;		/* move bottom to first x label line */

    /* label the x zone values */
    for (i = 0; i < nzones; i++) {

	/* x labels */
	settextjustify (CENTER_TEXT, TOP_TEXT);
	sprintf (buf, "%6.2lf", M.inner[i]);
	outtextxy (xoffset + M.inner[i] * xscale, bottom, buf);
    }

    /* x axis label */
    /* center the following text */
    settextjustify (CENTER_TEXT, TOP_TEXT);
    bottom += 10;		/* move bottom down 1 text line */
    outtextxy (maxx / 2, bottom, "Radius");

    /* title */
    bottom += 10;		/* move bottom douwn 1 text line */
    outtextxy (maxx / 2, bottom,
	       "RELATIVE SURFACE ERROR + surface too high, - surface too low");
}

void
draw_histo (int *histo, int size, double *SimVal, int max_sims, int maxval,
	    int ten_percent, int ninty_percent)
{
    double yscale;

    double xscale;

    int i;

    int xoffset = 70;		/* leave room on left side for y labels */


    /* use the upper half of the screen */
    int bottom = maxy / 2 - 20;	/* bottom edge of plot window */

    int top = 2;		/* top edge of plot window */

    int right = maxx - 2;	/* right side of plot window */

    char buf[20];

    double del;

    int space;

    struct scalev *s;

    int low_x = -1;

    int high_x = -1;

    int sum = 0;

    int from_x = -1;

    int from_y = -1;

    double sim50 = SimVal[max_sims / 2];

    for (i = 0; i < size; i++) {
	if (histo[i] > 0) {
	    if (low_x < 0) {
		low_x = i;
	    }
	    high_x = i;
	    sum += histo[i];
	}
    }

    /* fill plot with white background */
    setfillstyle (SOLID_FILL, WHITE);

    /* left top right bottom */
    bar (10, top, right, bottom);

    /* draw a gray border */
    setcolor (LIGHTGRAY);
    rectangle (10, top, right, bottom);
    right -= 15;
    del = ((double) (right - xoffset)) / (high_x + 1 - low_x);
    space = del / 5;
    bottom -= 45;
    top += 18;

    /* get y scale label increments */
    s = scale (0, maxval, 8);
    yscale = (double) (bottom - top) / s->maxval;

    /* draw  axis */
    setcolor (BLACK);
    line (xoffset, top, xoffset, bottom);
    line (xoffset, bottom, right, bottom);
    settextjustify (RIGHT_TEXT, CENTER_TEXT);

    /* label y axis and make tick marks */
    for (i = 1; i <= s->nint; i++) {
	double y = s->minval + i * s->incr;

	int ypoint = bottom - yscale * y;


	/* label every other increment */
	if ((i & 0x1) == 0) {
	    sprintf (buf, "%5.*lf", s->ndecimals, y);
	    outtextxy (xoffset - 10, ypoint, buf);
	    line (xoffset - 5, ypoint, right, ypoint);
	}

	else {
	    line (xoffset - 3, ypoint, xoffset, ypoint);
	}
    }

    /* Vertical title for y */
    strcpy (buf, "Simulations");
    vert_text (xoffset - 40, (top + bottom) / 2 - (strlen (buf) * 6), buf);
    settextjustify (CENTER_TEXT, TOP_TEXT);

    /* draw bar graph */
    for (i = low_x; i <= high_x; i++) {
	int x1 = xoffset + (i - low_x) * del + space;

	int x2 = x1 + 3 * space;


	/*  highlight the middle 80 % */
	if ((i < ten_percent) || (i > ninty_percent))
	    setfillstyle (SOLID_FILL, LIGHTGRAY);

	else
	    setfillstyle (SOLID_FILL, RED);
	bar (x1, bottom - yscale * histo[i], x2, bottom);
	rectangle (x1, bottom - yscale * histo[i], x2, bottom);

	/* label the x axix */
	if (i == low_x || i == high_x || i == ten_percent
	    || i == ninty_percent) {
	    if (i == 0)
		strcpy (buf, ">1");

	    else if (i == size - 1)
		sprintf (buf, "<1/%d", i);

	    else
		sprintf (buf, "1/%d", i);
	    outtextxy ((x1 + x2) / 2, bottom + 10, buf);
	}

	/* draw x axis tick marks */
	line (x1 - space, bottom - 3, x1 - space, bottom + 3);
    }
    bottom += 20;
    settextjustify (CENTER_TEXT, TOP_TEXT);
    setcolor (BLACK);
    outtextxy (right / 2, bottom,
	       "Wave rating. (RED is 80% Confidence section.)");
    outtextxy (right / 2, top - 15, "MONTE CARLO SIMULATION HISTOGRAM");

    /* Confidence graph on bottom half of screen */
    bottom = maxy - 20;		/* bottom edge of plot window */
    top = maxy / 2 + 2;		/* top edge of plot window */
    right = maxx - 2;		/* right side of plot window */

    /* fill plot with white background */
    setfillstyle (SOLID_FILL, WHITE);

    /* left top right bottom */
    bar (10, top, right, bottom);

    /* draw a gray border */
    setcolor (LIGHTGRAY);
    rectangle (10, top, right, bottom);
    right -= 15;
    bottom -= 45;
    top += 18;
    yscale = (double) (bottom - top) / 50;
    xscale = (right - xoffset) / (sim50 - SimVal[0]);

    /* draw  axis */
    setcolor (BLACK);
    line (xoffset, top, xoffset, bottom);
    line (xoffset, bottom, right, bottom);
    settextjustify (RIGHT_TEXT, CENTER_TEXT);

    /* label y axis and make tick marks */
    for (i = 50; i <= 100; i += 5) {
	int ypoint = bottom - yscale * (i - 50);


	/* label every other increment */
	if ((i & 0x1) == 0) {
	    sprintf (buf, "%5d", i);
	    outtextxy (xoffset - 10, ypoint, buf);
	    line (xoffset - 5, ypoint, right, ypoint);
	}

	else {

	    /* setcolor(LIGHTGRAY);
	       line(xoffset -5, ypoint, right, ypoint);
	       setcolor(BLACK); */
	    line (xoffset - 3, ypoint, xoffset, ypoint);
	}
    }

    /* Vertical title for y */
    strcpy (buf, "Confidence");
    vert_text (xoffset - 40, (top + bottom) / 2 - (strlen (buf) * 6), buf);
    settextjustify (CENTER_TEXT, TOP_TEXT);

    /* draw bar graph */
    for (i = 50; i <= 100; i += 5) {
	double val = SimVal[((100 - i) * max_sims) / 100];

	int to_x = xoffset + xscale * (sim50 - val);

	int to_y = bottom - yscale * (i - 50);

	fprintf (stderr, "to_x = %d, (SimVal[%d] = %g, sim50 = %g)\n", to_x,
		 ((100 - i) * max_sims) / 100,
		 SimVal[((100 - i) * max_sims) / 100], sim50);
	if (from_x >= 0) {
	    line (from_x, from_y, to_x, to_y);
	    setcolor (LIGHTGRAY);
	    line (to_x, bottom - 3, to_x, to_y);
	    if (i % 2) {
		line (xoffset, to_y, to_x, to_y);
	    }
	    setcolor (BLACK);
	}
	from_x = to_x;
	from_y = to_y;
	sprintf (buf, "1/%.1f", val);
	outtextxy (from_x, bottom + 10 + (i % 2) * 10, buf);

	/* draw x axis tick marks */
	line (from_x, bottom - 3, from_x, bottom + 3 + (i % 2) * 10);
    }
}


#endif /*  */
char *monte_descr =
    { "The variance of the measurments determines the confidence of the\n"
"results.  This simulation will jitter each mean zone reading by\n"
"plus and minus a random amount based on the standard deviation of your\n"
"readings.  If you entered one set of readings the deviation\n"
"is set at a default of .005.  You can change the deviation settings\n"
"now.  Up to 8000 simulations can be made using these random readings\n"
"If your computer is slow you can reduce the number of simulations.\n"
"The more simulations the more accurate the 80% confidence\n"
"will be.  The results will be displayed in a fractions of wavelength\n"
"wave error histogram and the 80% confidence level will be shown."
};

void
monte (int nr, int nzones)
{
    static double *SimVal;

    int i;

    int j;

    double zone_mean[NZ], zone_stdev[NZ], t, tssq;


#define BUCKETS  51
    int histo[BUCKETS];		/* histogram */

    double minval = HUGE_VAL;

    double maxval = -HUGE_VAL;

    int max_bucket = 0;

    int max_sims = 1000;

    int meter;

    int percent;

    double trdg[NZ][NRMAX];

    struct Matrix tm = M;

    int first;

    int last;

    for (i = 0; i < BUCKETS; i++)
	histo[i] = 0;
    for (i = 0; i < nr; i++)
	for (j = 0; j < nzones; j++)
	    trdg[j][i] = 0.0;

    /*
     * do monte carlo simulation of additional sets of readings to determine
     * what range of accuracy would be produced...
     */
    if (nr > 1) {
	for (i = 0; i < nr; i++) {

	    /* make the mean of all the readings in each set equal to 1.0... */
	    t = 0;
	    for (j = 0; j < nzones; j++)
		t += rdg[j][i];
	    t /= nzones;
	    for (j = 0; j < nzones; j++)
		trdg[j][i] = rdg[j][i] - (t - 1.0);
	}
	for (i = 0; i < nzones; i++) {

	    /* mean & st dev of readings for each zone... */
	    t = tssq = 0;
	    for (j = 0; j < nr; j++) {
		t += trdg[i][j];
		tssq += trdg[i][j] * trdg[i][j];
	    }
	    zone_mean[i] = t / nr;
	    zone_stdev[i] = sqrt ((nr * tssq - t * t) / (nr * (nr - 1)));
	}
    }

    else {			/* the mean is the value and stdev is .005 */
	for (i = 0; i < nzones; i++) {
	    zone_mean[i] = rdg[i][0];
	    zone_stdev[i] = .005;
	}
    }

    do {
	clrscr ();
	fputs ("              monte Carlo Analysis\n", stderr);
	fputs (monte_descr, stderr);
	fputs ("\n\n\n", stderr);
	for (i = 0; i < nzones; i++) {
	    fprintf (stderr, "Zone %d ", i);
	    zone_stdev[i] =
		get_double (1, zone_stdev[i], "deviation [%6.4lf] ",
			    zone_stdev[i]);
	}
    } while (get_yn (0, 0, "Are these correct (y or n)? ") == 'N');

    do {
	max_sims =
	    get_int (1, max_sims, "Number of simulations [%d]: ", max_sims);
	if (max_sims < 20)
	    max_sims = 20;
	if (SimVal)
	    free (SimVal);
	if (!(SimVal = (double *) malloc (max_sims * sizeof (double)))) {
	    fprintf (stderr, "%d is too large, please try a smaller value\n",
		     max_sims);
	}
    } while (!SimVal);
    percent = max_sims / 20;

#ifdef DOS
    fprintf (stderr, "Hit any key to quit early.\n");
    fprintf (stderr, "         %-4d      %4d\n", max_sims / 2, max_sims);
    fputs ("|.........|.........|", stderr);

#else /*  */
    handle_break (0);
    fprintf (stderr,
	     "Hit the interrupt key (usually break or control-C) to quit early.\n");
    fprintf (stderr, "         %-4d      %4d\n", max_sims / 2, max_sims);
    fputs ("|.........|.........|\r", stderr);

#endif /*  */
    fputc ('+', stderr);

    /* simulate max_sims sets of readings, compute accuracy for each... */
    for (i = 0, meter = percent; i < max_sims; i++) {
	for (j = 0; j < nzones; j++)
	    tm.D123[j] = nrand (zone_mean[j], zone_stdev[j]);
	calc (&tm);
	SimVal[i] = green_light / best.deviation;
	if (SimVal[i] < minval)
	    minval = SimVal[i];
	if (SimVal[i] > maxval)
	    maxval = SimVal[i];
	j = SimVal[i];
	if (j >= BUCKETS)
	    j = BUCKETS - 1;
	histo[j]++;

	/* record the largest bucket */
	if (histo[j] > max_bucket)
	    max_bucket = histo[j];
	if (i >= meter) {
	    fputc ('+', stderr);
	    meter += percent;
	}
#ifdef DOS
	if (fast_kbhit ()) {
	    max_sims = i;
	    getch ();
	    break;
	}
#else /*  */
	if (check_break ()) {
	    max_sims = i;
	    break;
	}
#endif /*  */
    }

#ifndef DOS
    fputc ('\n', stderr);

#endif /*  */
/* #ifndef DO_GRAPHICS */
    first = last = -1;
    for (i = 1; i < BUCKETS - 1; i++) {
	if (histo[i] > 0) {
	    if (first < 0) {
		first = i;
	    }
	    last = i;
	}
    }
    for (i = first; i <= last; i++) {
	if (i == 0) {
	    fprintf (stderr, "\n> 1/%-2d :  %d", i, histo[i]);
	}

	else if (i == BUCKETS - 1) {
	    fprintf (stderr, "\n< 1/%-2d :  %d", i, histo[i]);
	}

	else {
	    fprintf (stderr, "\n 1/%-2d :  %d", i, histo[i]);
	}
    }

/* #endif */
    qsort (SimVal, max_sims, sizeof (SimVal[0]), cmp);
    i = 10. * (double) max_sims / 100.;
    j = 89. * (double) max_sims / 100.;

/* #ifndef DO_GRAPHICS */
    fprintf (stderr, "\n80%% Confidence Limits: 1/%-.1f to 1/%-.1f\n",
	     SimVal[i], SimVal[j]);
    wait_for_key ();

/* #else */

    /* turn i and j into histogram indexes */
    i = SimVal[i];
    j = SimVal[j];
    if (j >= BUCKETS)
	j = BUCKETS - 1;
    open_graph ("monte.ps");
    draw_histo (histo, BUCKETS, SimVal, max_sims, max_bucket, i, j);

#ifdef DOS
    wait_for_key ();

#endif /*  */
    closegraph ();

/* #endif */
}

void
make_data_table (void)
{
    FILE *fpt;

    int i;

    int j;

    Specs.rho = 1.22 * green_light * 1e-6 * Specs.roc / Specs.dia / 2.;
    calc (&M);
    if ((fpt = fopen (TXT, "w")) == NULL) {
	fprintf (stderr, "Open failed for file %s\n", TXT);
	return;
    }
    FmtMatrix (nzones, nsamp, &Specs, &M, fpt);
    if (nr > 1) {
	struct Matrix tm = M;

	fprintf (fpt, "\nResults for individual sets of readings:\n");
	for (i = 0; i < nr; i++) {
	    for (j = 0; j < nzones; j++)
		tm.D123[j] = rdg[j][i];
	    calc (&tm);
	    fprintf (fpt, "\tReading set %d: 1 / %4.1lf lamda, "
		     "Max Trans Abberation=%2.2f\n", i + 1,
		     green_light / best.deviation, (double) best.TrAbbr);
    }}
    fclose (fpt);
} void

input_mirror_data (void)
{
    char *comment_line = 0;

    static const char *mirror_menu =
	{ "                            INPUT MIRROR CONSTANTS\n" "\n"
"   f)  Input from file.\n" "   k)  Input from keyboard.\n" "   q)    abort\n" "\n"
"   Mirror constants of:\n" "      Comments\n" "      Diameter\n"
"      Radius of Curvature\n" "      Number of Zones\n" "      Readings per Zone\n"
"      Tester type (moving or fixed light source).\n" "      Number of diameters\n"
"      Center zone inside radius\n" "      Zone outside radiuses\n\n"
"   Constants are entered from the keyboard or a file.\n"
"   The file format is identical to the keyboard input one line\n" "   at a time.\n"
"   You will be able to save the mirror constants in a file later on. \n"
"\nSelect 'f' for file input, 'k' for keyboard input, or 'q' to abort.\n"
    };
    static const char *mirror_choices = "fk";


    /* data entry... */
    fprintf (stderr, "Texereau Mirror Test Program\n\n");
    switch (get_menu_selection (0, 0, mirror_choices, mirror_menu)) {
    case MIRROR_FILE:
	clrscr ();
	if (!
	    (input_from =
	     get_file ("\nMirror data file name? [%s] ", &mirror_file_name,
		       &mirror_free_it, "rt"))) {
	    input_from = stdin;
	    return;
	}
	myfgets ();
	if (check_version (linebuf, "mirror", &Version)) {
	    comment_line = 0;
	}

	else {
	    comment_line = linebuf;
	}
	break;
    case 'q':
	return;
    case KEYBOARD:
	input_from = stdin;
	break;
    }

    do {
	if (comment_line) {
	    fprintf (stderr, "Comments [%s]: %s\n", Specs.comments,
		     comment_line);
	}

	else {
	    comment_line =
		get_string (1, Specs.comments, "Comments [%s]: ",
			    Specs.comments);
	}
	if (Specs.comments && free_specs)
	    free (Specs.comments);
	Specs.comments = strdup (comment_line);
	comment_line = 0;
	free_specs = 1;
	if (Version > 0) {
	    char ustr[2];

	    ustr[1] = '\0';
	    set_units (get_string (0, "", "Units (cm, mm, in): "));
	}
	Specs.dia =
	    get_double (1, Specs.dia, "Optical diameter of mirror [%.2f]: ",
			Specs.dia);
	Specs.roc =
	    get_double (1, Specs.roc, "Radius of curvature [%.2f]: ",
			Specs.roc);
	get_nzones ();
	nsamp = get_int (1, nsamp, "Readings per zone [%d]: ", nsamp);
	if (nsamp > NRMAX / MAXDIAM) {
	    nsamp = NRMAX / MAXDIAM;
	    fprintf (stderr,
		     "Too many readings per zone, changing it to %d\n",
		     nsamp);
	}
	switch (TesterType =
		get_menu_selection (1, TesterType, "fm",
				    "Is the light source fixed, or does it move with the knifedge (f or m) [%c]: ",
				    TesterType)) {
	case 'f':
	    break;
	case 'm':
	    fprintf (stderr,
		     "Moving source tester; readings will be shown doubled\n");
	    break;
	default:
	    return;
	}
	switch (DiaOpt =
		get_menu_selection (1, DiaOpt, "123",
				    "Readings taken on 1, 2 or 3 diameters [%c]: ",
				    DiaOpt)) {
	case '1':
	    ndia = 1;
	    break;
	case '2':
	    ndia = 2;
	    break;
	case '3':
	    ndia = 3;
	    break;
	default:
	    return;
	}
	enter_mask ();

	/* switch to keyboard input  for corrections */
	if (input_from != stdin) {
	    fclose (input_from);
	    input_from = stdin;
	}
    } while (get_yn (0, 0, "\nAre the above values correct (y or n)?: ") ==
	     'N');
    data_valid = 1;
}

void
input_readings ()
{
    int nresp;

    double t;

    int i;

    int j;

    int k;

    if (!data_valid) {
	data_invalid ();
	return;
    }

    /* read in the knife edge readings... */
    do {			/* loop until readings are acceptable */
	nresp = 0;
	for (i = 0; i < ndia; i++) {
	    fprintf (stderr, "\n\n");
	    if (DiaOpt != '1')
		fprintf (stderr, "Enter readings taken on radius %d\n",
			 i + 1);
	    for (j = 0; j < nzones; j++) {
		fprintf (stderr, "\tZone %d\n", j + 1);
		t = 0.;
		nr = i * nsamp;
		for (k = 0; k < nsamp; k++) {
		    t += rdg[j][nr] =
			to_fixed (get_double
				  (1, from_fixed (rdg[j][nr]),
				   "\t\tReading #%d [%.4f] ", k + 1,
				   from_fixed (rdg[j][nr])));
		    nr++;
		}
		switch (i) {
		case 0:
		    M.D1[j] = t / nsamp;

		    /* make average come out right... */
		    /* not strictly necessary, since code below handles this */
		    M.D2[j] = t / nsamp;
		    M.D3[j] = t / nsamp;
		    break;
		case 1:
		    M.D2[j] = t / nsamp;

		    /* make average come out right... */
		    /* not strictly necessary, since code below handles this */
		    M.D3[j] = (M.D1[j] + M.D2[j]) / 2;
		    break;
		default:
		    M.D3[j] = t / nsamp;
		    break;
		}
	    }
	}
	input_from = stdin;	/* make sure we get confirmation from KBD */
    } while (get_yn (0, 0, "\nAre the above readings correct (y or n)?: ") ==
	     'N');
    for (i = 0; i < nzones; i++) {

	/* average the D1 through D3 readings */
	switch (ndia) {
	case 1:
	    M.D123[i] = M.D1[i];
	    break;
	case 2:
	    M.D123[i] = (M.D1[i] + M.D2[i]) / 2;
	    break;
	case 3:
	    M.D123[i] = (M.D1[i] + M.D2[i] + M.D3[i]) / 3;
	    break;
	}
    }
    readings_valid = 1;
}

void
read_readings ()
{
    char *str;

    FILE *fp;

    clrscr ();
    if (!
	(fp =
	 get_file ("Readings file name [%s]: ", &reading_file_name,
		   &reading_free_it, "rt")))
	return;
    input_from = fp;
    myfgets ();
    if (!check_version (linebuf, "readings", &Version)) {
	return;
    }
    str = get_string (0, "", "Units: ");
    if (strcmp (str, units)) {
	fprintf (stderr,
		 "ERROR: readings in different units (%s) from mirror data (%s)\nPlease reenter mirror data in %s and try again\n",
		 str, units, str);
	fclose (fp);
	return;
    }

    else if (!set_units (str)) {
	fclose (fp);
	return;
    }
    str = get_string (0, "", "Fixed or moving source tester: ");
    switch (str[0]) {
    case 'f':
	TesterType = 'f';
	break;
    case 'm':
	TesterType = 'm';
	fprintf (stderr,
		 "Moving source tester; readings will be shown doubled\n");
	break;
    default:
	fprintf (stderr, "ERROR: Invalid tester type: %s\n", str);
	fclose (fp);
	return;
    }
    get_nzones ();
    enter_mask ();
    switch (DiaOpt =
	    get_menu_selection (1, DiaOpt, "123",
				"Readings taken on 1 or 2 diameters [%c]: ",
				DiaOpt)) {
    case '1':
	ndia = 1;
	break;
    case '2':
	ndia = 2;
	break;
    case '3':
	ndia = 3;
	break;
    default:
	return;
    }
    nsamp = get_int (1, nsamp, "Readings per zone [%d]: ", nsamp);
    if (nsamp > NRMAX / MAXDIAM) {
	nsamp = NRMAX / MAXDIAM;
	fprintf (stderr, "Too many readings per zone, changing it to %d\n",
		 nsamp);
    }
    input_readings ();
    fclose (fp);
    input_from = stdin;
}

void
calc_wavefront (struct Matrix *m, double coc, double *dmin, double *dmax,
		double *trab)
{
    int i;

    double minval = +HUGE_VAL;

    double maxval = -HUGE_VAL;

    double maxtrab = -HUGE_VAL;

    double tedge;

    m->wavefront[0] = 0;
    for (i = 0; i <= nzones; i++) {
	m->lamdac[i] = m->D123[i] - m->parab_surf[i] - coc;
	m->lamdafrho[i] =
	    m->lamdac[i] * m->hm[i] / (2. * Specs.roc * Specs.rho);
	if (fabs (m->lamdafrho[i]) > maxtrab)
	    maxtrab = fabs (m->lamdafrho[i]);
	m->u[i] = -m->lamdafrho[i] * Specs.rho * 1e6 / (Specs.roc / 2.);
	if (i > 0)
	    m->wavefront[i] =
		(m->outer[i - 1] - m->inner[i - 1]) * m->u[i - 1] +
		m->wavefront[i - 1];
	if (m->wavefront[i] > maxval)
	    maxval = m->wavefront[i];
	if (m->wavefront[i] < minval)
	    minval = m->wavefront[i];
    }
    tedge =
	m->lamdafrho[nzones - 2] + (m->inner[nzones - 1] -
				    m->hm[nzones -
					  2]) * (m->lamdafrho[nzones - 1] -
						 m->lamdafrho[nzones -
							      2]) /
	(m->hm[nzones - 1] - m->hm[nzones - 2]);
    if (tedge > maxtrab)
	maxtrab = tedge;
    *dmax = maxval;
    *dmin = minval;
    *trab = maxtrab;
}

int
set_units (const char *u)
{
    switch (u[0]) {
    case 'c':
	green_light = 54.87;
	units = "centimeters";
	return 1;
    case 'm':
	green_light = 548.7;
	units = "millimeters";
	return 1;
    case 'i':
	green_light = 21.6;
	units = "inches";
	return 1;
    default:
	fprintf (stderr, "ERROR: unknown value for units: %s\n", u);
	return 0;
    }
}

int
main (int argc, char **argv)
{
    char ustr[2];

    static const char *main_screen =
	{ "            FOUCAULT DATA REDUCTION  Version " VERSION "\n\n"
          "   Copyright (c) 1997-2004 by Michael Lindner and Larry Phillips\n"
          "   Additional modifications by Mark VandeWettering\n\n"
          "   [d]  input mirror data\n" 
          "   [r]  input Foucault test readings\n"
          "   [c]  calculate mirror results\n"
#ifdef DO_GRAPHICS
	  "   [g]  graph mirror results\n"
#endif /*  */
	  "   [m]  Monte Carlo analysis\n"
	  "   [s]  save data specs in a file\n"
	  "   [a]  output file for Aperture program\n"
	  "   [u]  select units of measurements\n"
	  "   [p]  generate PostScript file to print Couder mask\n"
	  "\n" 
          "   [q]  quit\n"
    };
    static const char *main_choices = "acdgmprsu";

    input_from = stdin;
    if (argc > 1 && strcmp (argv[1], "-m") == 0) {
	input_mirror_data ();
	calc_mask ();
	return 0;
    }
    if (argc > 1 && strcmp (argv[1], "-s") == 0)
	_use_sphere = 1;
    for (;;) {
	switch (get_menu_selection (0, 0, main_choices, main_screen)) {
	case MIRROR_DATA:
	    input_mirror_data ();
	    break;
	case 'r':
	    switch (get_menu_selection
		    (0, 0, "fk",
		     "   f)  Input from file.\n"
		     "   k)  Input from keyboard.\n" "   q)    abort\n")) {
	    case 'f':
		read_readings ();
		break;
	    case 'k':
		input_readings ();
		break;
	    }
	    if (readings_valid) {
		make_data_table ();
		PrFile ();
		wait_for_key ();
	    }
	    break;
	case CALC_TABLE:
	    if (!readings_valid)
		readings_invalid ();

	    else {
		make_data_table ();
		PrFile ();
		wait_for_key ();
	    }
	    break;
	case MONTE_CARLO:
	    if (!readings_valid)
		readings_invalid ();

	    else
		monte (nr, nzones);
	    break;

#ifdef DO_GRAPHICS
	case GRAPHS:
	    if (!readings_valid)
		readings_invalid ();

	    else {
		open_graph ("tex.ps");
		make_surface_graph (nzones);
		make_ml_graph (nzones);

#ifdef DOS
		wait_for_key ();

#endif /*  */
		closegraph ();
	    }
	    break;

#endif /*  */
	case APETURE_FILE:
	    if (!readings_valid)
		readings_invalid ();

	    else
		save_aper ();
	    break;
	case ZONES:
	    if (!data_valid)
		data_invalid ();

	    else {
		print_mask ();
	    }
	    break;
	case SAVE_SPECS:
	    if (!data_valid)
		data_invalid ();

	    else {
		static const char *save_screen =
		    "    m) save mirror data\n" 
		    "    r) save readings\n"
		    "    q) quit\n";
		static const char *save_choices = "mr";

		switch (get_menu_selection (0, 0, save_choices, save_screen)) {
		case 'm':
		    save_specs ();
		    break;
		case 'r':
		    save_readings ();
		    break;
		}
	    }
	    break;
	case UNITS:
	    ustr[0] =
		get_menu_selection (0, 0, "cmi",
				    "C for centimeters, M for millimeters, or I for Inches: ");
	    ustr[1] = '\0';
	    set_units (ustr);
	    break;
	case QUIT:
	case EOF:
	    return 0;
	}
    }
    return 0;
}

/* 
 * Use the Box-Muller transform to generate gaussian random numbers.
 */

double
nrand (double mean, double stdev)
{
    double u1, u2 ;
    double u ;

    u1 = drand48() ;
    u2 = drand48() ;

    u = sqrt(-2.0 * log(u1)) * cos(2.0 * M_PI * u2) ;
    /* u = sqrt(-2.0 * log(u1)) * sin(2.0 * M_PI * u2) ; */

    return mean + u * stdev ;
}
