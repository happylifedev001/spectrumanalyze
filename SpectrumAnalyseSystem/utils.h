#pragma once

#ifndef __UTILS_H__
#define __UTILS_H__

/******************************************************
*
*   PROJECT :   Spectrum Analyse System
*
*   FILE :  utils.h
*
******************************************************/

#include "global.h"

#include <sndfile.h>
#include "common.h"
#include "Nelsonspec.h"

#define DIR_LENGTH 600           /* Length in chars of current directory */

/*
 * Some globals used for the spectrum graphics.
 */

#define SPEC_OX .1
#define SPEC_XR .75
#define SPEC_DB 80.0

 /*
  * Spectrum params array indices. See xkl.c.
  */

#define WC 0
#define WD 1
#define WS 2
#define WL 3
#define NC 4
#define FD 5
#define B0 6
#define B1 7
#define BS 8
#define F1 9
#define F2 10
#define FL 11
#define SG 12
#define SD 13
#define NW 14
#define KN 15

#define numSpecParams 16

  /*
   * Postscript types
   */

#define PSNONE -1
#define FULLPAGE 0
#define FOURPAGE 1

extern FILE* PS4_fp;
extern FILE* PSfull_fp;
extern int quadcount;    /* Spectrum counter for 4/pg postscript */

/*
 * Misc globals
 */


#define NUM_WINDOWS 5     /* Number of windows for each waveform */
#define GRAM       2      /* index of spectrogam */
#define REGRAM     4
#define SPECTRUM   3      /* index of drawing widget for spectrum */

extern int lock;                 /* specifies if wave files are locked */

extern char curdir[DIR_LENGTH];  /* current working directory */

#define MELFAC 2595.0375  /* = 1000./log10(1. + 1000. / 700. ) */

#define MINSIZE  450  /* minimum width of window for menu_bar*/

#define AVGLIMIT 100 /* max number of spectra used in 'A' */

#define NPMAX 32000
#define NFMAX  257
#define SIZCBSKIRT 200

extern float cbskrt[200];
extern char cmdstr[800];

typedef struct PARAM {
    int min;
    int max;
    int index; /* position on selection box list = index of spectro->params[] */
} PARAM;

/* Parameter structure */

typedef struct {
    int type;     /* int or float */
    char* key;    /* key to type */
    float min;    /* minimum value */
    float max;    /* maximum value */
    float value;  /* current value */
    char* desc;   /* Description   */
} paramStruct;

enum { FLOAT, INT };

/*
 * Parameters for spectra.
 *
 * Left column is the accelerator list for the selection
 * box that pops up when the user wants to change a parameter.
 * See add_spectro in xspec_util.c to set startup values.
 */

enum {
    SPEC_CB, SPEC_DFT, SPEC_SMOOTH, SPEC_LPC, SPEC_NUMLPC, SPEC_PREEMP,
    SPEC_CB_BW_1, SPEC_CB_BW_1000, SPEC_SMOOTH_BW, SPEC_CB_CF1,
    SPEC_CB_CF2, SPEC_CB_LOG, SPEC_GAIN, SPEC_WINDOW, SPEC_DFT_SIZE,
    SPEC_K_AVG
};

/*
 * Min and max for spectra parameters.
 * See setparam() to change min and max of DFT size.
 */

static int g_mnSpectralParams[] = { 10,10,10,10,2,0,60,60,40,100,120,500,0,0,16,1 };
static int g_mxSpectralParams[] = { 500,500,500,500,20,100,500,500,500,300,500,2000,700,1,4096,50 };
static int g_initSpectralParams[] = { 257, 299, 256, 256, 14, 0, 70, 180, 300, 100, 124, 700, 450, 1, 512, 10 };

                                      // winms, slice, numav, msdelta, specms, minmag, maxmag, xpix, fdifcoef, sec
static int g_initSpectrogramParams[] = {    64,   128,     3,       1,   1300,      5,     25,    1,      100,  0 };

extern PARAM sb_param;

#define XAM(a,b)    (((a) > (b)) ? (a) : (b)) /* for dft library */

/*  LABEL structure:  null-terminated linked list */

#define MAXLABLEN 11         // max displayed label text length plus terminator (ASCIIZ)
#define MAXLABCOM 31		 // max displayed label text length plus terminator (ASCIIZ)
typedef struct LABEL {
    float  offset;              // offset in msecs from start of file
    char   name[MAXLABLEN];     // displayed text
    char	 comment[MAXLABCOM];  // optional comment
    struct LABEL* next;         // next label
} LABEL;


/*
 * The spectro stucture has been modified to accomodate the XKL
 * program.  The structure has all the information needed to draw the
 * two waveform windows, the spectrum window and the spectrogram
 * window. The stucture stores the information about each drawing area
 * widget used in an array of "INFO" stuctures: info[0] has
 * information about the spectrum, info[1] the large waveform window,
 * info[2] the small waveform window and info[3] has information about
 * the spectrogram drawing area widget
 */

typedef struct __XSPECTRO {
    /*******************things from quickspec ****************/
    int type_spec;               /* flag for type of spectrum */

    char option;                 /* which combination of spectra
                                    are displayed*/
    char savoption;              /* used to store mode when
                                    doing a, A or k averaging*/
    int history;                 /* whether the last spectrum
                                    is displayed */
    float histime;               /* time of .spectrum file*/



    int oldsizwin, oldwintype;/* does a new hamming window need
                                 to be calculated */
    int sizfft;		/*Set here, max{usersizfft, sizwin} */

    int sizwin, savsizwin;	/* # pts in time weighting window    */

    int nsdftmag;		/* Set in KLSPEC */

    /* Lock function */

    int locked;

    /*    Arrays from MAKEFBANK.C */

    /* Defines shape of pseudo-Gaus filt */
    float cbskrt[SIZCBSKIRT];
    int nfr[4096];	        /* Center freq (Hz) of output filters */
    int savnfr[4096];	/* Center freq (Hz) of output filters */

    short nbw[NFMAX];	/* Bandwidths (Hz) of output filters */
    short pweight[NPMAX];	/* Weights of indiv. filter coefs    */
    int sizcbskirt;		/* # pts in cbskrt[]		     */
    int nchan;		/* cb		     */
    short nstart[NFMAX], ntot[NFMAX]; /*First sample contributing to filt*/
    int logoffset;
    float attenpersamp;


    /*    From GETFORM.C */

    int dfltr[257];			/* Spectral slope */

#define MAX_INDEX	7
    int nforfreq;			/* number of formant freqs found   */
    int forfreq[16], foramp[16];	/* nchan and ampl of max in fltr[] */
    int valleyfreq[8], valleyamp[8];	/* nchan and ampl of min in fltr[] */

    int nmax_in_d;			/* number of maxima in deriv       */
    int ncdmax[8], ampdmax[8];	/* nchan and ampl of max in deriv  */
    int ncdmin[8], ampdmin[8];	/* nchan and ampl of min in deriv  */

    int hftot;			/* number of hidden formants found */
    int hiddenf[8], hiddena[8];	/* freq and ampl of hidden formant */



    /* spectrum stuff */

    float hamm[4096];
    float dftmag[4096];
    int fltr[4098]; /* spectrum */ /* store RMS in last element */
    int savfltr[4098]; /* last spectrum or dft depending on mode */
    int lenfltr, lensavfltr;
    int fd, savfd; /* spectrum,
                   save the first difference value for c and s */
    int spectrum, savspectrum; /*flag for whether to draw fltr
                                   and savfltr*/
    int savclip; /* in case savfltr needs to be clipped(8k on 5k) */
    float xfltr[2048], yfltr[2048], xsav[2048], ysav[2048];  /* postscript
                                      version of fltr and savfltr */
    int nxxmin, nxxmax;   /* used for 'a' and 'k'
                            start and stop in msec*/
    int avgtimes[AVGLIMIT];    /* array of spectra centers
                           for 'A' in msec*/
    int avgcount;        /* how many spectra in 'A'*/
    double denergy;


    int params[16];            /* wc,wd,ws..etc. */

    float hamm_in_ms[4];     /* store sizwin in ms, convert to samples
                             in setparam and store in params[0-4]*/

                             /***********************spectrogram stuff****************/

    int spectrogram;                     /* spectrogram flag whether or not
                                           spectrogram widget is managed*/
    int respectrogram;                     /* spectrogram flag whether or not
                                           spectrogram widget is managed*/
    int param_active;                    /* is the spectrum paramter selection
                                            box active for this widget */
    int segmode;                         /* whether w to e or the waveform
                                           displayed in the window is played */
    short* startplay;                    /* used for playing or writing section
                                            of waveform*/
    long sampsplay;                      /* number of samples in section
                                            that is played*/
    int swap;                            /* if running on sgi swap bytes
                                           in .wav*/

    char* pix;                           /* data for xim */

    char* repix;

    float* allmag;                       /* dft temporary storage*/

    int xpix, ypix, zpix;                       /* scale factor in x and y */

    int devwidth, devheight;             /* device width and height  */

    int wxmax, wymax;                     /*spectrogram size  */

    int numcol;                          /* shades of grey + white */

    float cinc;                          /* change of intensity
                                            between shades*/
    int tickspace;                       /*spacing so tick marks
                                             don't have to be redrawn*/
    int axisdist, wchar, hchar;            /*axis offset from spectrogram,
                                            character height and width   */

    int sec;                             /* seconds or milliseconds flag*/

    int xgram;                           /* total of pixels in x
                                             stored in .gram file*/
    int   normstate[5];                    /* state of toplevel windows
                                             1 = normal, 0 = icon*/
    char* allgvs;                          /* all the grey indices */

    char* reallgvs;

    int* posti, postcount;                  /*postsript and rle stuff*/

    int ps_window;                           /*which window is requesting
                                               postscript output*/
    int quadcount;                            /* used for putting 4
                                              spectra on a page */
    float gscale;                            /*postscript font scale*/

    float Gscale;                            /* postscript font scale
                                                4 spectra/page*/
    int wwav;                                /* (segtowav)write .. 1:play seg
                                               2:record  3:syn*/
    int donew;                               /* display recorded file in
                                              current display or new one*/
    float maxmag, minmag;                     /* minmag maps to white,
                                                maxmag maps to darkest gray */
    short* iwave;                            /*totsamp samples must convert
                                                (float)(iwave[p]>>4)*/
    int totsamp;                             /*total number of
                                                 samples in iwave file */
    int  slice;                              /*length of each FFT snapshot,
                                                 default 64 samples*/
    float winms;                             /*window in msec
                                                winsamps*1000/spers */
    float savewinms;                         /* save for file swaps */

    int winsamps;                            /*number of samples padded
                                             with 0's if needed passed to fft*/
    int numav;                               /*number of FFTs to
                                                average*/
    int sdelta;                              /* number of new samples in each
                                                spectra*/
    float msdelta;                           /*number of new ms in each
                                                spectrum*/
    float savemsdelta;                       /* save for file swaps*/

    float specms;                            /*suggested number of ms in
                                                  spectrogram*/
    int ovlap;                               /*amount of overlap between
                                                each FFT*/
    int insize;                              /*number of samples in wav file
                                                doesn't have to be power of 2*/
    int specsize;                            /*number of samples in display
                                                doesn't have to be power of 2*/
    int sampsused[5];                          /* the number of samples actually
                                              displayed in window*/
    int  estsize;                            /*(slice-ovlap)*(numav-1)
                                                  + slice*/
    int numests;                             /* insize/estsize (totsamp)*/

    int xmaxests;                            /* number of estimates in window*/

    float  spers;                             /* samples per second
                                                 used in wave file*/
    float savspers;                          /* samples per second
                                                in .fltr file */
    int startmarker, endmarker;             /* sample indices(same for all
                                              windows*/
    int oldstart, oldend;                   /* used to erase markers*/

    float startime[5];                          /*starting time of window
                                                             in msec*/
    float timeoffset;                            /* used in lock function: offset
                            relative to locked waveform */

    int startindex[5];                        /*starting index of data in
                           window*/
    int t0;                                 /*time offset in image coord(spectra)*/

    char fname[200];                          /* preferrence dat file name */

    char wavename[200];                       /* waveform name */

    char savname[200];                       /* name of wavform for
                                              read and write spectrum*/
    char wavefile[200];                       /* directory and file name */

    char segname[200];                       /* new file name for seg,
                                                 syn and record  */
    char grampath[200];                     /* directory for .gram file*/

    char synDefPath[200];                 /* synthesis defaults directory (path to executable) */

    int fdifcoef;                         /* first difference coefficient
                                             between 0 and 100 (spectrogram)*/
    float wavemin, wavemax;               /* waveform min and max value */

    int step;                             /* waveform increment */

    int spratio;                        /* aspect ratio of freq to ms*/

    float wvfactor[2];                       /*for zooming in on waveform*/

    float savetime, oldtime;            /*time from beginning of file
                                      in ms where middle mouse is clicked*/

    int saveindex, oldindex;                /* index from beginning of file
                                       where left mouse is clicked(all windows)*/

    int specfreq, savspecfreq;         /* freq val at cursor location in
                                          spectrum window  */

    int spec_cursor;                  /* position of cursor in spectrum*/

    float savspecamp, specamp;         /* amp val at cursor location in
                                          spectrum window  */

    int defserr;                       /* inappropriate value in defs.dat */

    char tmp_name[500];               /*storage for a file name used for
                                        overwrite popup */

    int localtimemax;		 /* shall we lock onto local max in time? EC*/

    int localfreqmax;		 /* shall we lock onto local max in freq? EC*/

    int clickedWinIndex;		/* index of clicked window */
    float clickedWinTime;			/* offset (msecs) at mouseDn */

    LABEL* labels;                   /* label list */

    bool  useFocusTime;
    float focusTime; // Focus time on wave form window

    float focusTime0;
    int   focusIndex0;
} XSPECTRO, * LPXSPECTRO;

/* DFT_SPEC is different in makefbank & quickspec*/

#define DFT_SPEC        0
#define AVG_SPEC        4
#define TWOPI           6.283185307

#define DFT_MAG		0
#define CRIT_BAND	1
#define SPECTROGRAM	2
#define LPC_SPECT	3
#define AVG_SPECT       4
#define DFT_MAG_NO_DB	4
#define SPECTO_NO_DFT	5
#define MAX_LPC_COEFS	20

/*
 * for getf0
 */

#define UP	        2
#define DOWN	        1
#define MAXNPEAKS	50
static int fpeaks[MAXNPEAKS];

/*
 * Minimum value allowed as input to log conversion
 * to prevent quantization noise problems
 */

#define SUMMIN  .001

// Read a wave data from a file
bool readWaveData(const char* fileName, SWaveData * waveData);
/*****************************************************************/
/*          validindex(XSPECTRO *spectro)                         */
/*****************************************************************/
void validindex(XSPECTRO* spectro);

int quickspec(XSPECTRO* spectro, short iwave[]);
/********************************************************************/
/*                    getavg(XSPECTRO *spectro)                     */
/********************************************************************/
void getavg(XSPECTRO* spectro);
int getf0(int dftmag[], int npts, int samrat);
int comb_filter( int npks);
/*****************************************************************/
/*         dofltr(XSPECTRO *spectro,int type, int winsize)         */
/*****************************************************************/
void dofltr(XSPECTRO* spectro, int type, int winsize);
/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
/*                       T I L T		                         */
/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

/* Replace set of points infltr[] by straight-line approx in outfltr[] */
int tilt(int* infltr, int npts, int* outfltr);
/*************************************************************/
/*            new_spectrum(spectro)                          */
/*************************************************************/
void newSpectrum(XSPECTRO* spectro);
/**************************************************************/
/*                savdft(XSPECTRO *spectro, int winsize)      */
/**************************************************************/
void savdft(XSPECTRO* spectro, int winsize);
/****************************************************************/
/*                 copysav(XSPECTRO *spectro)                   */
/****************************************************************/
void copysav(XSPECTRO* spectro);
void getform(XSPECTRO* spectro);
/* Found loc max in deriv followed by loc min in deriv, see if hidden formant */
int testhidden(XSPECTRO* spectro);
/*	  Eliminate insignificant peaks (last one always significant) */
/*	  Must be down 6 dB from peaks on either side		      */
/*	  And valley on either side must be less than 2 dB	      */

/*	  Also eliminate weaker of two formant peaks within 210 Hz of */
/*	  each other, unless not enough formants in F1-F3 range	      */
void zap_insig_peak(XSPECTRO* spectro);
/***************************************************************/
/*                  findsdelta(spectro)                        */
/***************************************************************/
void findsdelta(XSPECTRO* spectro);
/**************************************************************/
/*                   findypix(XSPECTRO *spectro)               */
/**************************************************************/
void  findypix(XSPECTRO* spectro);
/***************************************************************/
/*                  calculate_spectra(spectro)                 */
/***************************************************************/
void calculate_spectra(XSPECTRO* spectro);
/***************************************************************/
/*                  calculate_spectrogram(spectro)                 */
/***************************************************************/
void calculate_spectrogram(XSPECTRO* spectro);
/****************************************************************/
/*                 win_size(XSPECTRO *spectro)                   */
/****************************************************************/
void win_size(XSPECTRO* spectro);
/***************************************************************/
/*                 remapgray(XSPECTRO *spectro)                 */
/***************************************************************/
void remapgray(XSPECTRO* spectro);

#endif