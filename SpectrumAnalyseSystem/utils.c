/******************************************************
* 
*   PROJECT :   Spectrum Analyse System
* 
*   FILE :  utils.c
* 
******************************************************/

#include <math.h>
#include <string.h>

#include "spectrum.h"
#include "utils.h"
#include "dft.h"

emxArray_real_T* scaled_down_magnitude;
emxArray_real_T* reduced_frequency;
emxArray_real_T* scaled_down_time;

// Read a wave data from a file
bool readWaveData(const char* fileName, SWaveData* waveData) {

    if (waveData == NULL)
        return false;

    SF_INFO _sfinfo;
    SNDFILE* _sndFile = sf_open(fileName, SFM_READ, &_sfinfo);

    if (!_sndFile)
    {
        printf("Error opening the %s\n", fileName);
        return false;
    }

    freeWaveData(waveData);

    waveData->frames = (int)_sfinfo.frames;
    waveData->channels = _sfinfo.channels;
    waveData->sampleRate = _sfinfo.samplerate;
    allocWaveData(waveData);

    sf_readf_float(_sndFile, waveData->datas, waveData->frames);

    sf_close(_sndFile);

    waveData->isData = true;

    return true;
}

/***********************************************************************/
/*void dft(short iwave[],float win[],float *dftmag,int npts1,int npts2,*/
/*                        int fdifcoef){                               */
/***********************************************************************/

/*      DFT.C         D.H. Klatt */

/*      Compute npts2-point dft magnitude spectrum (after M.R. Portnoff) */

/*        Input: npts1 fixed-point waveform samples in "iwave[1,...,npts1]"
                  and a floating-point window in "win[1,...,npts1]"

          If npts1 > npts2, fold waveform re npts2
CHANGE 2-Jan-87:  For first difference preemphasis, set fdifsw = 1  END OUT
          For first difference preemphasis, set fdifcoef = {0,...,100}
           where 0 is no preemphasis, and 100 is exact first difference.
          For no windowing and no preemphasis, fdifcoef = -1 */

          /*        Output: npts2/2 floating-point spectral magnitude-squared
                            samples returned in floating array dftmag[0,...,npts2/2-1]
                    (npts2 must be a power of 2, and npts1 must be even) */


void dft(short iwave[], float win[], float* dftmag, int npts1, int npts2,
    int fdifcoef) {

    float xrsum;  /* this used to be a global*/
    int npts, mn, nskip, nsize, nbut, nhalf;
    float theta, c, s, wr, wi, tempr, dsumr, dsumi, scale, xre, xro;
    float xie, xio;
    float xri, xii, xfdifcoef;
    int i1, i, j, k, n, nrem, itest, isum, nstage, tstage;
    float* xr, * xi;
    float xtemp;

    /*    Multiply waveform by window and place alternate samples
          in xr and xi */

    npts = npts1 >> 1;
    npts2 = npts2 >> 1;
    xr = dftmag;
    xi = dftmag + npts2;


    i1 = 0;
    xfdifcoef = 0.01f * (float)fdifcoef;
    for (i = 0; i < npts2; i++) {
        xr[i] = 0.;
        xi[i] = 0.;
    }
    for (i = 0; i < npts; i++) {
        j = i + i;
        k = j + 1;

        if (fdifcoef >= 0) {
            /*	      Use first difference preemphasis and window function */
            xri = (float)(iwave[j] - (xfdifcoef * (float)iwave[j - 1]));
            xii = (float)(iwave[k] - (xfdifcoef * (float)iwave[j]));
            xr[i1] += xri * win[j];
            xi[i1] += xii * win[k];
        }
        else if (fdifcoef == -1) {
            /*	      No first-difference preemphasis, window contains lpc coefs */
            xr[i1] += win[j];
            xi[i1] += win[k];
        }
        else {
            /*	      No first difference preemphasis, use window function */
            xri = iwave[j];
            xii = iwave[k];
            xr[i1] += xri * win[j];
            xi[i1] += xii * win[k];
        }

        i1++;
        if (i1 >= npts2)    i1 = 0;
    }

    /*    Bit-reverse input sequence */
    i = 1;
    n = npts2 >> 1;
    while (i < (npts2 - 1)) {
        j = n;
        if (i < j) {
            xtemp = xr[j];
            xr[j] = xr[i];
            xr[i] = xtemp;
            xtemp = xi[j];
            xi[j] = xi[i];
            xi[i] = xtemp;
        }
        nrem = n;
        itest = npts2;
        isum = -npts2;
        while (nrem >= 0) {
            isum = isum + itest;
            itest = itest >> 1;
            nrem = nrem - itest;
        }
        n = n + itest - isum;
        i = i + 1;
    }
    /*    End of bit-reversal algorithm */



    /*    Begin FFT */
    nstage = 0;
    nskip = 1;
    while (nskip < npts2) {
    loop:     nstage++;
        tstage = nstage;/* scope problem with -O, d.h.*/
        nsize = nskip;
        nskip = nskip << 1;
        /*        The following loop does butterflies which do not
                  require complex arith */
        for (i = 0; i < npts2; i += nskip) {
            j = i + nsize;
            xtemp = xr[j];
            xr[j] = xr[i] - xr[j];
            xr[i] = xr[i] + xtemp;
            xtemp = xi[j];
            xi[j] = xi[i] - xi[j];
            xi[i] = xi[i] + xtemp;
        }
        if (tstage < 2) {
            goto loop;
        }
        theta = (float)TWOPI / nskip;
        c = (float)cos(theta);
        s = (float)sin(theta);
        wr = 1.0;
        wi = 0.0;

        /*    Do remaining butterflies */
        for (nbut = 2; nbut <= nsize; nbut++) {
            tempr = wr;
            wr = (wr * c) + (wi * s);
            wi = (wi * c) - (tempr * s);
            for (i = nbut - 1; i < npts2; i += nskip) {
                j = i + nsize;
                dsumr = (xr[j] * wr) - (xi[j] * wi);
                dsumi = (xi[j] * wr) + (xr[j] * wi);
                xr[j] = xr[i] - dsumr;
                xi[j] = xi[i] - dsumi;
                xr[i] += dsumr;
                xi[i] += dsumi;
            }
        }
    }

    /*    Set coefficients for scrambled real input */
    scale = 0.5;
    wr = 1.0;
    wi = 0.0;
    nhalf = npts2 >> 1;
    tempr = xr[0];
    xr[0] = tempr + xi[0];
    xi[0] = tempr - xi[0];
    theta = (float)TWOPI / (npts2 + npts2);
    c = (float)cos(theta);
    s = (float)sin(theta);
    for (n = 1; n < nhalf; n++) {
        tempr = wr;
        wr = (wr * c) + (wi * s);
        wi = (wi * c) - (tempr * s);
        mn = npts2 - n;
        xre = (xr[n] + xr[mn]) * scale;
        xro = (xr[n] - xr[mn]) * scale;
        xie = (xi[n] + xi[mn]) * scale;
        xio = (xi[n] - xi[mn]) * scale;
        dsumr = (wr * xie) + (wi * xro);
        dsumi = (wi * xie) - (wr * xro);
        xr[n] = xre + dsumr;
        xi[n] = xio + dsumi;
        xr[mn] = xre - dsumr;
        xi[mn] = dsumi - xio;
    }

    /*    Take magnitude squared */
    xi[0] = 0.;
    xrsum = 0.;
    for (i = 0; i < npts2; i++) {
        xr[i] = 0.000000001f * ((xr[i] * xr[i]) + (xi[i] * xi[i]));
        xrsum = xrsum + xr[i];
    }

}/* end */
/*******************end Dennis' dft.c*************************/


/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
/*		              M K W N D 				 */
/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

/*    Make window */
void mkwnd(XSPECTRO* spectro)

{

    float xi, incr;
    int i, limit;

    /* this shouldn't be needed */

    if ((spectro->sizwin > spectro->sizfft) ||
        //            (spectro->sizwin <= 0) || (spectro->sizfft > 512)) {
        (spectro->sizwin <= 0) || (spectro->sizfft > 4096)) {
        printf("In mkwnd() of quickspec.c, illegal wsize=%d,dftsize=%d",
            spectro->sizwin, spectro->sizfft);
        exit(1);
    }

    /*  printf(
           "\tMake Hamming window; width at base = %d points, pad with %d zeros\n",
          wsize, power2-wsize); */
    xi = 0.;
    incr = (float)TWOPI / (spectro->sizwin);
    limit = spectro->sizwin;
    /*if (spectro->halfwin == 1)    limit = limit >> 1;*/


/*    Compute non-raised cosine */
    for (i = 0; i < limit; i++) {
        if (spectro->params[NW] == 1) {
            spectro->hamm[i] = (1.0f - (float)cos(xi));
            xi += incr;
        }
        else {
            spectro->hamm[i] = 1.;	/* rectangular window */
        }
    }

    /*    Pad with zeros if window duration less than power2 points */
    while (i < spectro->sizfft) {
        spectro->hamm[i++] = 0.;
    }
}/* end mkwnd */

/*************************************************************************/
/*		             M G T O D B				 */
/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

/* Convert long integer to db */
int mgtodb(long nsum, int usergainoffset) {

    static  int dbtable[] = {
             0,  1,  2,  3,  5,  6,  7,  9, 10, 11,
            12, 13, 14, 16, 17, 18, 19, 20, 21, 22,
            23, 24, 25, 26, 27, 28, 29, 30, 31, 32,
            33, 34, 35, 36, 37, 37, 38, 39, 40, 41,
            42, 43, 43, 44, 45, 46, 47, 47, 48, 49,
            50, 50, 51, 52, 53, 53, 54, 55, 56, 56,
            57, 58, 58, 59, 60 };

    int db, temp, dbinc;

    /*    Convert to db-times-10 */
    /*    If sum is 03777777777L, db=660 */
    /*    If sum is halved, db is reduced by 60 */
    db = 1440;

    if (nsum <= 0L) {

        if (nsum < 0L) {

            printf("Error in mgtodb of QUICKSPEC,");
            printf("nsum=%ld is negative, set to 0\n", nsum);
        }

        return(0);
    }

    if (nsum > 03777777777L) {

        printf("Error in mgtodb of QUICKSPEC, nsum=%ld too big, truncate\n",
            nsum);

        nsum = 03777777777L;
    }

    while (nsum < 03777777777L) {

        nsum = nsum + nsum;
        db = db - 60;
    }

    temp = (nsum >> 23) - 077;

    if (temp > 64) {

        temp = 64;
        printf("Error in mgtodb of QUICKSPEC, temp>64, truncate\n");
    }

    dbinc = dbtable[temp];
    db = db + dbinc;
    db = db >> 1;		    /* Routine expects mag, we have mag sq */
    db = db + usergainoffset - 400;/*User offset to spect gain*/
    
    if (db < 0) 
        db = 0;

    return (db);

}/* end mgtodb */


/**********************************************************************/
/*           void   makefbank(XSPECTRO *spectro)                       */
/**********************************************************************/

/* used to take type_spec as a parameter */

void makefbank(XSPECTRO* spectro) {

    int np, i1, i3, nsmin, n1000, nfmax;
    float x2, xfmax, fcenter, bwcenter, bwscal, bwscale, xbw1000, xsdftmag, linlogfreq;
    
    float _a, _b, _c, _d, _temp1, _temp2;

    /* Initialization to make filter skirts go down further (to 0.00062)
       put this in main, just do it once at startup
        if (spectro->initskrt == 0) {
        spectro->initskrt++;

        for (np = 100; np<SIZCBSKIRT; np++) {
            cbskrt[np] = 0.975 * cbskrt[np-1];
        }
        printf("\nMax down of cbskrt[] is %f  ",
                             cbskrt[SIZCBSKIRT-1]);
        }
    */

    /*    Highest frequency in input dft magnitude array */
    nfmax = (int)spectro->spers / 2;
    xfmax = (float)nfmax;
    xsdftmag = (float)spectro->nsdftmag;

    _a = 0.0f;
    _b = 0.0f;
    _c = 0.0f;

    if ((spectro->type_spec == LPC_SPECT) || (spectro->type_spec == AVG_SPECT)) {

        /*	    printf("\tCompute freq of each dft mag sample (nfr[%d])",
                 spectro->lenfltr); */
                 /*	  Reset nfr[] */
        i3 = spectro->lenfltr / 2;	/* For rounding up */
        for (i1 = 0; i1 < spectro->lenfltr; i1++) {
            spectro->nfr[i1] = ((i1 * nfmax) + i3) / spectro->lenfltr;
        }
        return;
    }

    if (spectro->type_spec == CRIT_BAND) {
        /*	  Do not include samples below 80 Hz */
        nsmin = ((80 * spectro->nsdftmag) / nfmax) + 1;
        /*	  Determine center frequencies of filters, use frequency spacing of
         *	  equal increments in log(1+(f/flinlog)) */
        linlogfreq = (float)spectro->params[FL];
        _temp1 = 1.0f + ((float)spectro->params[F1] / linlogfreq);
        _temp1 = (float)log(_temp1);
        _temp2 = 1.0f + ((float)spectro->params[F2] / linlogfreq);
        _temp2 = (float)log(_temp2);
        _a = _temp2 - _temp1;
        _b = (_temp1 / _a) - 1.0f;
        _c = (xsdftmag * linlogfreq) / xfmax;
    }
    else {

        nsmin = 1;
    }

    n1000 = (int)((1000.0f * spectro->nsdftmag) / nfmax + 0.5);
    xbw1000 = (float)spectro->params[B1];
    xbw1000 = xbw1000 / n1000;
    bwscal = (360.0f * xfmax) / xsdftmag;		/* 360 is a magic number */

    np = 0;
    spectro->nchan = 0;
    while (spectro->nchan < NFMAX) {

        if (spectro->type_spec == SPECTROGRAM ||
            spectro->type_spec == SPECTO_NO_DFT) {

            /*	      Center frequency in Hz of filter i */
            fcenter = (float)spectro->nchan;
            /*	      Bandwidth in Hz of filter i */
            bwcenter = (float)spectro->params[BS];
        }

        else {
            /*	      Center frequency in Hz of filter i */

            _d = _a * (_b + spectro->nchan + 1.0f);
            fcenter = _c * ((float)exp(_d) - 1.0f);

            /*	      Bandwidth in Hz of filter i */
            bwcenter = xbw1000 * fcenter;
            if (bwcenter < (float)spectro->params[B0]) {
                bwcenter = (float)spectro->params[B0];
            }
        }
        /*	  See if done */
        if (fcenter >= xsdftmag) {
            break;
        }/*	  Center frequency and bandwidth in Hz of filter i */
        spectro->nfr[spectro->nchan] = (int)((fcenter * xfmax) / xsdftmag + 0.5f);
        spectro->nbw[spectro->nchan] = (int)(bwcenter + 0.5);

        /* printf("\nFILTER %d:  f=%4d bw=%3d:\n", nchan, nfr[nchan], nbw[nchan]); */

        /*	  Find weights for each filter */
        spectro->nstart[spectro->nchan] = 0;
        spectro->ntot[spectro->nchan] = 0;

        bwscale = bwscal / (10.0f * spectro->nbw[spectro->nchan]);

        for (i1 = nsmin; i1 < spectro->nsdftmag; i1++) {

            /*	      Let x2 be difference between i1 and filter center frequency */
            x2 = fcenter - i1;

            if (x2 < 0){

                x2 = -x2;
                spectro->sizcbskirt = SIZCBSKIRT;
                /* Go down 22 dB on low side */
            }
            else spectro->sizcbskirt = SIZCBSKIRT >> 1;
            /* Go down 32 dB on high side */

            i3 = (int)(x2 * bwscale);
            if (i3 < spectro->sizcbskirt) {

                /*		  Remember which was first dft sample to be included */
                if (spectro->nstart[spectro->nchan] == 0) {

                    spectro->nstart[spectro->nchan] = i1;
                }

                /*		  Remember pointer to weight for this dft sample */
                spectro->pweight[np] = i3; np++;

                if (np >= NPMAX) {

                    printf("\nFATAL ERROR in MAKECBFB.C: 'NPMAX' exceeded\n");
                    printf("\t(while working on filter %d).  ", spectro->nchan);
                    printf("\tToo many filters or bw's too wide.\n");
                    printf("\tRedimension NPMAX and recompile if necessary.\n");
                    exit(1);
                }
                /*		  Increment counter of number of dft samples in sum */
                spectro->ntot[spectro->nchan]++;
            }
        }
        spectro->nchan++;
    }
    spectro->sizcbskirt = SIZCBSKIRT;
    /*    NCHAN is now set to be the total number of filter channels */
}/* end makefbank */


/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
/*		         Q U I C K S P E C				 */
/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
/* edited to use spectro structure July '93 d. hall*/

int quickspec(XSPECTRO* spectro, short iwave[]){

    float _scratch[1024];
    float floatw;
    static float floatwlast, xfdcoef;
    int _i, iwlast, count;
    int pdft, pdftend, nch, pw;
    long nsum;
    float sum, sumlow;

    int ier = 0;
    double _energy = 0;
    double _pcerror = 0;

    //printf("quickspec:nstart-ntot:\n");

    //for ( _i = 0; _i < spectro->lenfltr; _i++) {

    //    pdft = spectro->nstart[_i];
    //    printf("%d:%d-%d\n", _i + 1, pdft, pdft + spectro->ntot[_i]);
    //}

    //printf("params[FD]=%d\n", spectro->params[FD]);
    //printf("params[NW]=%d\n", spectro->params[NW]);
    //printf("params[NC]=%d\n", spectro->params[NC]);
    //printf("params[SG]=%d\n", spectro->params[SG]);
    //printf("params[B0]=%d\n", spectro->params[B0]);
    //printf("params[B1]=%d\n", spectro->params[B1]);
    //printf("params[BS]=%d\n", spectro->params[BS]);
    //printf("params[F1]=%d\n", spectro->params[F1]);
    //printf("params[F2]=%d\n", spectro->params[F2]);
    //printf("params[FL]=%d\n", spectro->params[FL]);
    //printf("type_spec=%d\n", spectro->type_spec);
    
    //printf("----- dftmag -----\n");
    //for (_i = 0; _i < spectro->lenfltr; _i++) {

    //    if (_i > 0)
    //        printf(",");

    //    printf("%.1f", spectro->dftmag[_i]);
    //}
    //printf("\n");

    //printf("----- fltr -----\n");
    //for (_i = 0; _i < spectro->lenfltr; _i++) {

    //    if (_i > 0)
    //        printf(",");

    //    printf("%.1f", spectro->fltr[_i]);
    //}
    //printf("\n");

    if (spectro->type_spec == DFT_MAG ||
        spectro->type_spec == DFT_MAG_NO_DB) {

        spectro->lenfltr = spectro->sizfft / 2;
        // makefbank(spectro);
    }/*DFT_MAG or DFT_MAG_NO_DB*/

    /*    Make filters */
    else if (spectro->type_spec == CRIT_BAND) {

        spectro->lenfltr = spectro->sizfft / 2;
        makefbank(spectro);
        spectro->lenfltr = spectro->nchan;
    }/*CRIT_BAND*/

    else if ((spectro->type_spec == SPECTROGRAM) ||
        (spectro->type_spec == SPECTO_NO_DFT)) {

        spectro->lenfltr = spectro->sizfft / 2;
        makefbank(spectro);
    }/*SPECTROGRAM or SPECTO_NO_DFT*/

    /*    Make Hamming window  */
    /*does a new window need to be calculated*/
    if (spectro->oldsizwin != spectro->sizwin ||
        spectro->oldwintype != spectro->params[NW]) {

        spectro->oldsizwin = spectro->sizwin;
        spectro->oldwintype = spectro->params[NW];

        /* need to think about different sized ffts */

        mkwnd(spectro);
    }

    //printf("xklspec:quickspec:type_spec=%d,lenfltr=%d\n", spectro->type_spec, spectro->lenfltr);

    /*    Linear Prediction */
    if (spectro->type_spec == LPC_SPECT) {

        spectro->lenfltr = (spectro->sizfft >> 1);

        for (nch = 0; nch < 1024; nch++)
            _scratch[nch] = 0.0;

        spectro->nchan = spectro->lenfltr;

        makefbank(spectro);	/* Actually just set nfr[] */

        /*	  Multiply waveform times window, do first difference, use dftmag[] */
        iwlast = 0;
        xfdcoef = 0.01f * (float)(spectro->params[FD]);
        for (nch = 1; nch < spectro->sizfft; nch++) {

            if (spectro->params[FD] > 0) {

                floatw = (float)iwave[nch];
                floatwlast = iwave[nch - 1];
                /*  Use 0.95 to make unbiased estimate of F1  for synthetic stim */
                spectro->dftmag[nch] = spectro->hamm[nch] *
                    (floatw - (xfdcoef * floatwlast));
            }
            else {

                spectro->dftmag[nch] = spectro->hamm[nch] *
                    (float)(iwave[nch]);
            }
        }

        /* Call Fortran lpc analysis subroutine, coefs in dftmag[256]... */
        pcode(spectro->dftmag, spectro->sizwin, spectro->params[NC],
            &spectro->dftmag[spectro->sizfft],
            _scratch, &_pcerror, &_energy, &ier);

        /*	  Move coefs down 1, make negative */
        for (nch = spectro->params[NC] - 1; nch >= 0; nch--) {

            spectro->dftmag[nch + spectro->sizfft + 1] = -
                (spectro->dftmag[nch + spectro->sizfft]);
        }

        spectro->dftmag[spectro->sizfft] = 1.0f;

        if (ier != 0) {

            printf("Loss of significance, ier=%d\n", ier);
        }

        /*	  Pad acoef with zeros for dft */
        count = (spectro->sizfft << 1);
        for (nch = spectro->params[NC] + spectro->sizfft + 1; nch <= count; nch++) {

            spectro->dftmag[nch] = 0.0f;
        }

        /*	  Call dft routine with firstdifsw=-1 to indicate lpc */
        dft(iwave, &spectro->dftmag[spectro->sizfft], spectro->dftmag,
            spectro->sizwin, spectro->sizfft, -1);


    }/* Linear Prediction*/
    /*    Compute magnitude spectrum */
    else if (spectro->type_spec != SPECTO_NO_DFT) {

        dft(iwave, spectro->hamm, spectro->dftmag,
            spectro->sizwin, spectro->sizfft, spectro->params[FD]);
    }

    /*  DFT now done in all cases, next create filters */

      /*    If DFT magnitude spectrum, just convert to dB */
    if (spectro->type_spec == DFT_MAG) {

        //  Consider each dft sample as an output filter
        // printf("fltr values(%d:%d) : ", spectro->lenfltr, spectro->params[SG]);
        for (_i = 0; _i < spectro->lenfltr; _i++) {

            // Scaled so smaller than cb
            int _nsum = (int)(spectro->dftmag[_i] * 20000.0f);
            int _fltrValue = mgtodb(_nsum, spectro->params[SG]);
            spectro->fltr[_i] = _fltrValue;
            // if (_i > 0)
            //     printf(",");
            // printf("%d", _fltrValue);
            //printf("%d", _nsum);
        }
        // printf("\n");
    } // DFT_MAG
    /*    If LPC: invert, multiply by energy, and convert to dB */
    else if (spectro->type_spec == LPC_SPECT) {

        /*	  Multiply coefs by square root of energy */
        spectro->denergy = _energy * _pcerror;
        spectro->logoffset = (int)(-1450.0 + 100. * log10(spectro->denergy) + 0.5f);
        /*	  Consider each dft sample as an output filter */
        for (nch = 0; nch < spectro->lenfltr; nch++) {

            spectro->denergy = spectro->dftmag[nch];
            spectro->fltr[nch] = (int)(spectro->logoffset - 100.0f *
                                (float)log10(spectro->denergy));
            if (spectro->fltr[nch] < 0)    
                spectro->fltr[nch] = 0;
        }

        return(0);
    }/*LPC_SPEC*/
    /* Sum dft samples to create Critical-band or Spectrogram filter outputs */
    else if ((spectro->type_spec == CRIT_BAND)
        || (spectro->type_spec == SPECTO_NO_DFT)
        || (spectro->type_spec == SPECTROGRAM)) {

        pw = 0;
        for (nch = 0; nch < spectro->lenfltr; nch++) {

            pdft = spectro->nstart[nch];
            pdftend = pdft + spectro->ntot[nch];
            sum = (float)SUMMIN;

            /*            Compute weighted sum of relevent dft samples */
            while (pdft < pdftend) {

                sum += cbskrt[spectro->pweight[pw++]] *
                    spectro->dftmag[pdft++];
            }

            /*	      Add in weak contribution of high-freq channels */
            /*	      Approximate tail of filter skirt by exponential */
            //printf("quickspec:(nch:%d)->pdft/pdftend=%d/%d,sum=%.2f\n", nch, spectro->nstart[nch], spectro->ntot[nch], sum);

            spectro->attenpersamp = (0.4f * spectro->spers) /
                (spectro->nbw[nch] * spectro->nsdftmag);
            spectro->attenpersamp = 1.0f - spectro->attenpersamp;

            if (spectro->attenpersamp > 1.0f) {

                spectro->attenpersamp = 1.0f; 
            }
            else if (spectro->attenpersamp < 0.0f)
            {
                spectro->attenpersamp = 0.0f;
            }

            if (pdftend < spectro->lenfltr) {

                sumlow = 0.;
                pdft = spectro->lenfltr - 1;

                while (pdft >= pdftend) {

                    sumlow = (sumlow + spectro->dftmag[pdft--])
                        * spectro->attenpersamp;

                }

                sum += (sumlow * cbskrt[spectro->sizcbskirt - 1]);
            }

            /*	      Add in weak contribution of low-freq channels   */
            /*	      Approximate tail of filter skirt by exponential */
            pdftend = spectro->nstart[nch];

            if (pdftend > 0) {

                sumlow = 0.0f;
                pdft = 0;
                while (pdft < pdftend) {

                    sumlow =
                        (sumlow + spectro->dftmag[pdft++]) * spectro->attenpersamp;
                }

                sum += (sumlow * cbskrt[(spectro->sizcbskirt - 1) >> 1]);
            }

            if (spectro->type_spec == SPECTO_NO_DFT) {

                sum = sum * 18000.0f;	/* Scaled so bigger than dft */
                nsum = (long)sum;
                spectro->fltr[nch] = mgtodb(nsum, spectro->params[SG]);
            }
            else {

                sum = sum * 18000.0f;	/* Scaled so bigger than dft */
                nsum = (long)sum;
                spectro->fltr[nch] = mgtodb(nsum, spectro->params[SG]);
                /*		  Look for maximum filter output in this frame */
                if (spectro->fltr[nch] > spectro->fltr[spectro->lenfltr + 1]) {

                    spectro->fltr[spectro->lenfltr + 1] = spectro->fltr[nch];
                }
            }
        }
    }

    /*    Compute overall intensity and place in fltr[lenfltr] */
    sum = (float)SUMMIN;
    for (pdft = 0; pdft < spectro->sizfft >> 1; pdft++) {

        sum = sum + spectro->dftmag[pdft];
    }
    nsum = (int)(sum * 10000.0f);	/* Scale factor so synth av=60 about right */

    spectro->fltr[spectro->lenfltr] = mgtodb(nsum, spectro->params[SG]);

    //printf("***** dftmag *****\n");
    //for (_i = 0; _i < spectro->lenfltr; _i++) {

    //    if (_i > 0)
    //        printf(",");

    //    printf("%.1f", spectro->dftmag[_i]);
    //}
    //printf("\n");

    //printf("***** fltr *****\n");
    //for (_i = 0; _i < spectro->lenfltr; _i++) {

    //    if (_i > 0)
    //        printf(",");

    //    printf("%.1f", spectro->fltr[_i]);
    //}
    //printf("\n");


    return(0);
}/* end quickspec */

/*****************************************************************/
/*          validindex(XSPECTRO *spectro)                         */
/*****************************************************************/
void validindex(XSPECTRO* spectro) {

    // set 0 <= cursor <= totsamp - 1
    // and check for enough samples in new_spectrum
    // char str[100];

    if (spectro->saveindex < 0) {

        spectro->saveindex = 0;
        spectro->savetime = 0.0;
    }
    else if (spectro->saveindex > spectro->totsamp - 1) {

        spectro->saveindex = spectro->totsamp - 1;
        spectro->savetime = (float)spectro->saveindex / spectro->spers * 1000;

        // Show error message when edge of waveform is breached 
        // Used primarily when waveforms are locked together 
        // sprintf(str, "Warning: the value chosen is beyond\nthe boundary of one or more waveforms.");
        //ShowOops(str);
    }
}

/********************************************************************/
/*                    getavg(XSPECTRO *spectro)                     */
/********************************************************************/
void getavg(XSPECTRO* spectro)
{
    int nx, i, index;
    float cumscale, cumspec[256];

    //INFO* info;
    int halfwin;
    int nxxmin, nxxmax;
    int msstep;

    halfwin = (spectro->params[WD] >> 2);

    msstep = (int)(0.001 * spectro->spers + 0.5);

    if (spectro->option == 'k' || spectro->option == 'a') {
        if (spectro->option == 'k') {
            spectro->avgtimes[0] = (int)(spectro->savetime + .5) - spectro->params[KN];
            spectro->avgtimes[1] = (int)(spectro->savetime + .5) + spectro->params[KN];
        }
        /* set up nxxmin for 'a' and 'k'  */

        nxxmin = (int)(spectro->avgtimes[0] * spectro->spers / 1000.0f - halfwin + 0.5f);
        nxxmax = (int)(spectro->avgtimes[1] * spectro->spers / 1000.0f - halfwin + 0.5f);

        /* check to make sure the times are good first */
        for (i = spectro->avgtimes[0]; i <= spectro->avgtimes[1]; i++) {

            index = (int)(i * spectro->spers / 1000.0 + .5) - halfwin;

            if (index < 0 || index >= spectro->totsamp) {
                /* can't do dft */
                printf( "Invalid time: %d.\n", index);
                //writetext(str);
                return;
            }

        }/* all times */

    }/* 'a' and 'k' */

    else if (spectro->option == 'A') {

        for (i = 0; i <= spectro->avgcount; i++) {
            
            index = (int)(spectro->avgtimes[i] * spectro->spers / 1000.0 + .5) - halfwin;

            if (index < 0 || index >= spectro->totsamp) {

                /* can't do dft */
                printf( "Invalid time: %d\n", spectro->avgtimes[i]);
                //writetext(str);
                return;
            }
        }/* all times */

    }/*A*/

    /* done checking now change stuff */

    spectro->sizwin = spectro->params[WD];  /* DFT window*/
    spectro->sizfft = spectro->params[SD]; /* user defined size dft */
    spectro->type_spec = DFT_MAG_NO_DB;    /*AVG_SPEC = DFT_MAG_NO_DB */
    spectro->nsdftmag = spectro->sizfft / 2;

    for (nx = 0; nx < spectro->nsdftmag; nx++) cumspec[nx] = 0.;


    /* now do dfts */
    if (spectro->option == 'k' || spectro->option == 'a') {

        /* Step through waveform at 1 msec as suggested by KNS */
        /*		Change by ANANTHA	*/
        printf("Averaging from %d to %d in 1 msec steps.\n",
            spectro->avgtimes[0], spectro->avgtimes[1]);
        //writetext(str);

        for (i = nxxmin; i <= nxxmax; i += msstep) {
            /* this calls dft routine */
            quickspec(spectro, &spectro->iwave[i]);
            /* Add floating mag sq spectrum into cum spect array */

            for (nx = 0; nx < spectro->lenfltr; nx++) {
                cumspec[nx] += spectro->dftmag[nx];
            }
        }

    }/* 'a' or 'k' */

    else if (spectro->option == 'A') {
        nxxmin = 0;
        nxxmax = 0;
        printf("avgerage times:");
        for (i = 0; i <= spectro->avgcount; i++) {
            nxxmax += halfwin;
            printf("%d ", spectro->avgtimes[i]);
            index = (int)((float)spectro->avgtimes[i] * spectro->spers / 1000.0f + .5f) 
                    - halfwin;
            /* this calls dft routine */
            quickspec(spectro, &spectro->iwave[index]);
            /* Add floating mag sq spectrum into cum spect array */
            for (nx = 0; nx < spectro->lenfltr; nx++) {

                cumspec[nx] += spectro->dftmag[nx];
            }
        }

        printf("\n");
        //writetext(str);
    }/*'A'*/


    /* I don't feel comfortable with the way cumscale is handled
       'A' gives different results that 'k' and 'a'   dh  */

       /* need to check time  to see if there are enough samples for dft*/
       /*DFT stored in savfltr */
    if (nxxmax - nxxmin < halfwin)  nxxmax = nxxmin + halfwin;
    cumscale = 20000.0f * (float)halfwin / (nxxmax - nxxmin);
    spectro->lensavfltr = spectro->lenfltr;
    spectro->savspectrum = 1;
    spectro->history = 1;
    spectro->savsizwin = spectro->sizwin;
    spectro->savspers = spectro->spers;
    for (nx = 0; nx < spectro->lensavfltr; nx++) {
        spectro->savfltr[nx] =
            mgtodb( (long)(cumspec[nx] * cumscale + 0.5), spectro->params[SG]);
    }
    cumscale = (float)halfwin /
        (float)(nxxmax - nxxmin);


    /*	      Smooth avg dft spectrum */
    spectro->spectrum = 1;
    for (nx = 0; nx < spectro->lenfltr; nx++)
        spectro->dftmag[nx] = cumspec[nx] * cumscale;
    spectro->type_spec = SPECTO_NO_DFT;
    /* no dft so iwave index doesn't matter */
    quickspec(spectro, &spectro->iwave[512]);

    if (spectro->option == 'k') {

        spectro->oldindex = spectro->saveindex;
        spectro->oldtime = spectro->savetime;

        spectro->savetime = spectro->savetime +
            (spectro->avgtimes[1] - spectro->avgtimes[0]) / 2;
        spectro->saveindex = (int)(spectro->savetime * spectro->spers / 1000.0f + .5f);
        spectro->savetime = (float)spectro->saveindex / spectro->spers * 1000.0f;
        validindex(spectro);
        if (spectro->oldindex != spectro->saveindex) {
            //for (i = 0; i < 3; i++) {
            //    update(spectro->info[i], spectro, i);
            //}
        }/* new index*/
    }
    //draw_spectrum(spectro, 0, NULL);
    //info = spectro->info[SPECTRUM];

    //XCopyArea(info->display, info->pixmap,
    //    info->win, info->gc,
    //    0, 0, spectro->xr[SPECTRUM], spectro->yb[SPECTRUM], 0, 0);

}/* end getavg */


/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
/*                  G E T F 0 . C       D.H. Klatt   */
/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

/*      On VAX, located in directory [klatt.klspec]     */


/* EDIT HISTORY:
 * 000001 31-Jan-83 DK  Initial creation
 * 000002  6-Mar-84 DK  Fix serious bug in sorting of acceptable peaks
 * 000003  9-Mar-84 DK  Use comb filter to determine harmonic number of peaks
 * 000004 20-Jul-84 DK  Restrict range to 1500 Hz, select prominent harms, fix
 * 000005 24-Apr-86 DK  Add test to keep 'npeaks' within array size
 */
/*  in xklspec.h 
#define UP  2
#define DOWN    1
#define MAXNPEAKS   50
static int fpeaks[MAXNPEAKS];
*/

/*  Get estimate of fundamental frequency from dft mag spectrum */

int getf0(int dftmag[], int npts, int samrat){

    int direction,npeaks,lasfltr,n;
    int apeaks[MAXNPEAKS];
    int np,np1,kp,dfp;
    int nmax,nmaxa,ampmax;
    int da,da1,da2,df,npaccept;

    direction = DOWN;
    npeaks = 0;
    lasfltr = dftmag[0];
/*    Look for local maxima only up to 1.5 kHz (was 3 kHz) */
    if (samrat > 0) {

        nmax = (npts * 1500) / samrat;
/*    Also find biggest amplitude in range from 0 to 900 Hz */
        nmaxa = (npts * 900) / samrat;
    }
    else 
        printf("In getf0(): somebody walked over samrat, set to zero");

    ampmax = 0;

/*    Begin search for set of local max freqs and ampls */
    for (n=1; n<nmax; n++) {

        if (n < nmaxa) {

            if (dftmag[n] > ampmax)  
                ampmax = dftmag[n];
        }

        if (dftmag[n] > lasfltr) {
        
            direction = UP;
        }
        else if (dftmag[n] < lasfltr) {

            if ((direction == UP) && (npeaks < 49)) {
/*        Interpolate the frequency of a peak by looking at
          rel amp of previous and next filter outputs */
                da1 = dftmag[n-1] - dftmag[n-2];
                da2 = dftmag[n-1] - dftmag[n];

                if (da1 > da2) {

                    da = da1;
                } else {
                
                    da = da2;
                }

                if (da == 0) {

                    da = 1;
                }

                df = da1 - da2;
                df = (df * (samrat>>1)) / da;
                fpeaks[npeaks] = (((n-1) * samrat) + df) / npts;
                apeaks[npeaks] = lasfltr;
                if (npeaks < MAXNPEAKS-1)    
                    npeaks++;
            }
            direction = DOWN;
        }
        lasfltr = dftmag[n];
    }

/*    Remove weaker of two peaks within minf0 Hz of each other */
#if 0
printf("\nInitial candidate set of %d peaks:\n", npeaks);
for (np=0; np<npeaks; np++) {
    printf("\t %d.   f = %4d   a = %3d\n", np+1, fpeaks[np], apeaks[np]);
}
#endif

    for (np=0; np<npeaks-1; np++) {

        if (((np == 0) && (fpeaks[0] < 80))
          || ((np > 0) && ((fpeaks[np] - fpeaks[np-1]) <= 50))) {
        
            if ((np > 0) && (apeaks[np] > apeaks[np-1])) 
                np--;
            npeaks--;
        
            for (kp=np; kp<npeaks; kp++) {

                fpeaks[kp] = fpeaks[kp+1];
                apeaks[kp] = apeaks[kp+1];
            }
        }
    }

/*    Remove initial weak peak before a strong one: */
    if ((apeaks[1] - apeaks[0]) > 200) {
        npeaks--;
        for (kp=0; kp<npeaks; kp++) {
        fpeaks[kp] = fpeaks[kp+1];
        apeaks[kp] = apeaks[kp+1];
        }
    }

/*    Remove weak peak between two strong ones I: */
    for (np=1; np<npeaks-1; np++) {

/*    See if a weak peak that should be ignorred */
        if (((apeaks[np-1] - apeaks[np]) > 40)
         && ((apeaks[np+1] - apeaks[np]) > 40)) {

            npeaks--;
            for (kp=np; kp<npeaks; kp++) {
                fpeaks[kp] = fpeaks[kp+1];
                apeaks[kp] = apeaks[kp+1];
            }
        }
    }

/*    Remove weak peak between two strong ones II: */
    for (np=1; np<npeaks-1; np++) {

/*    See if a weak peak that should be ignorred */
        if ((apeaks[np-1] - apeaks[np]) > 80) {

            np1 = 1;
            while (((np+np1) < npeaks)
              && (fpeaks[np+np1] < (fpeaks[np] + 400))) {

                if ((apeaks[np+np1] - apeaks[np]) > 80) {

                    npeaks--;
                    for (kp=np; kp<npeaks; kp++) {
                        fpeaks[kp] = fpeaks[kp+1];
                        apeaks[kp] = apeaks[kp+1];
                    }
                    np--;
                goto brake1;
                }
                np1++;
            }
        }
brake1:
        np = np;
    }
#if 0
printf("\nFinal candidate set of %d peaks sent to comb filter:\n", npeaks);
for (np=0; np<npeaks; np++) {
    printf("\t %d.   f = %4d   a = %3d\n", np+1, fpeaks[np], apeaks[np]);
}
#endif

    if (npeaks > 1)        dfp = comb_filter(npeaks);
    else if (npeaks > 0)   dfp = fpeaks[0];
    else                   dfp = 0;
#if 0
printf("\nComb filter says f0 = %d\n", dfp);
printf("\nHit <CR> to continue:");
scanf("%c", &char1);
#endif

/*    Zero this estimate of f0 if little low-freq energy in spect */
    if (ampmax < 200) {
        dfp = 0;
    }
/*    Or if f0 high and only a few peaks */
/* couldn't find any other reference to npaccept so*/
/* based on the above comment set it to npeaks d.h.*/
  npaccept = npeaks;
    if ( (npaccept <= 2) && (dfp > 300) )    dfp = 0;
    return(dfp);
}

int comb_filter( int npks){

/* Look for an f0 with most harmonics present, restrict */
/* search to range from f0=60 to f0=400 */

#define NBINS   65
    float fmin_freq,fmax_freq;
    static float f0_hyp[NBINS];
    int nharm,bin,min_freq,max_freq,cntr_freq,max_cntr_freq;
    int max_bin,max_count_harm,harm_found;
    int sum_f,sum_nharm,f0_estimate;
    int np,freq_harm,count_harm;

/* Initialization */
    if (f0_hyp[0] == 0) {

        f0_hyp[0] = 400.;
        for (bin=1; bin<NBINS; bin++) {

    /*    Compute center freq of each bin */
            f0_hyp[bin] = .97f * f0_hyp[bin-1];
        }
    }

    if (npks > 6)    npks = 6;
    max_count_harm = 0;
    for (bin=0; bin<NBINS; bin++) {
    fmin_freq = 0.97f * f0_hyp[bin];
    cntr_freq = (int)f0_hyp[bin];        /* for debugging only */
    fmax_freq = 1.03f * f0_hyp[bin];
    count_harm = 0;
    sum_nharm = 0;
    sum_f = 0;
    for (nharm=1; nharm<=6; nharm++) {

        min_freq = (int) (fmin_freq * (float) nharm);
        max_freq = (int) (fmax_freq * (float) nharm);
        if (min_freq < 2500) {

            harm_found = 0;
            for (np=0; np<npks; np++) {

                freq_harm = fpeaks[np];
                if ((freq_harm > min_freq) && (freq_harm < max_freq)) {
                    if (harm_found == 0) {
                        harm_found++;
                        count_harm++;
                        sum_nharm += nharm;
                        sum_f += freq_harm;
                    }
                }
            }
        }
    }
/*  printf("%d ", count_harm); */
    if (count_harm > max_count_harm) {
        max_count_harm = count_harm;
        max_bin = bin;
        max_cntr_freq = cntr_freq;
        f0_estimate = sum_f / sum_nharm;
    }
    }
/*  printf("\n  Best comb freq = %d, f0 = %d\n",
     max_cntr_freq, f0_estimate); */

/*    Zero this estimate of f0 if the distribution of f0 estimates */
/*    is rather scattered */
    if ((3*max_count_harm) < (2*npks)) {
/*      printf("\nZero f0=%3d", f0_estimate); */
        f0_estimate = 0;
    }
/*  else {
        printf("\n     f0=%3d", f0_estimate);
    }
    printf("  nh=%d np=", max_count_harm);
    for (np=0; np<npks; np++) {   spectro->option = spectro->savoption;
   spectro->history = 0;
        printf("%4d, ", fpeaks[np]);
    } */
    return(f0_estimate);
}

/*****************************************************************/
/*         dofltr(XSPECTRO *spectro,int type, int winsize)         */
/*****************************************************************/
void dofltr(XSPECTRO* spectro, int type, int winsize) {

    /* set up window size and check against dft size */
    /* call quickspec and update fltr if cursor is not too close to end*/
    int firstsamp;

    spectro->sizwin = winsize;
    spectro->sizfft = 256;

    if (spectro->sizwin > spectro->sizfft)
        spectro->sizfft = 512;

    spectro->type_spec = type;
    spectro->nsdftmag = spectro->sizfft / 2;
    firstsamp = spectro->saveindex - spectro->sizwin / 2;

    if (spectro->sizwin <= spectro->sizfft) {

        if (firstsamp >= 0 && firstsamp <= spectro->totsamp - 1) {

            spectro->spectrum = 1;
            quickspec(spectro, &spectro->iwave[firstsamp]);
        }/* new spectrum */
        else {

            spectro->spectrum = 0;
        }/* index too close to end of file for dft */
    }/* window size ok for dft */
    else {
        
        spectro->spectrum = 0;
        //writetext("Window greater that DFT reset parameters.\n");
    }/*spectro->sizwin > spectro->sizfft*/
}/* end dofltr */

/*************************************************************/
/*            new_spectrum(spectro)                          */
/*************************************************************/
void newSpectrum(XSPECTRO* spectro) {

    char _option = spectro->option;

    int _i, _firstsamp;

    /* everytime there is a new spectrum set all the spectrum
       cursor values to -1 */

    spectro->specfreq = -1;
    spectro->specamp = -1.0;
    spectro->savspecfreq = -1;
    spectro->savspecamp = -1.0;

    /* if an average is currently being displayed
       switch back to previous mode this allows resize of averaging*/

    if (_option == 'a' || _option == 'A' ||
        _option == 'k') {

        spectro->option = spectro->savoption;
        spectro->history = 0;
    }

    /* based on the spectrum combination */
    /* spectro->fltr is calculated if cursor is not to close to end of file*/

    /* look at the top of xkl.c for info on spectro->params[] */

    /* if the params value is the same for all spectra( ex.fd coeff)
       then it is accessed directly in quickspec otherwise it is
       set in this module (ex. sizwin)
    */

    //printf("------ iwave datas -------- \n");
    //for (_i = 0; _i < spectro->totsamp; _i++) {

    //    if (_i > 0)
    //        printf(", ");
    //    printf("%d", spectro->iwave[_i]);
    //}
    //printf("\n");

    if (_option == 'd') {

        /*DFT magnitude*/
         /* assume that sizwin and sizfft are compatible(setparam)*/
        spectro->sizwin = spectro->params[WD];
        spectro->sizfft = spectro->params[SD];
        spectro->type_spec = DFT_MAG;
        spectro->nsdftmag = (spectro->sizfft >> 1);
        _firstsamp = spectro->saveindex - (spectro->sizwin >> 1);

        if (spectro->sizwin <= spectro->sizfft) {

            if (_firstsamp >= 0 && _firstsamp <= spectro->totsamp - 1) {

                spectro->spectrum = 1;
                quickspec(spectro, &spectro->iwave[_firstsamp]);
            }/* new spectrum */
            else {

                spectro->spectrum = 0;
            }/* index too close to end of file for dft */
        }/* window size ok for dft */
        else {

            spectro->spectrum = 0;
            //writetext("Window greater than DFT, reset parameters.\n");
        }/*spectro->sizwin > spectro->sizfft*/
    }/*d*/

    else if (_option == 's') {

        /*spectrogram-like*/
        if (spectro->history) 
            copysav( spectro);

        dofltr(spectro, SPECTROGRAM, spectro->params[WS]);
    }/*s*/

    else if (_option == 'S') {

        /*spectrogram-like + DFT*/
         /*DFT savfltr*/
        savdft(spectro, spectro->params[WS]);/*make sizwin the same*/
        /*spectrogram-like fltr*/
        dofltr(spectro, SPECTROGRAM, spectro->params[WS]);
    }/*S*/

    else if (_option == 'c') {

        /*critical-band*/
        if (spectro->history) {

            copysav(spectro);
        }/*history*/

        dofltr(spectro, CRIT_BAND, spectro->params[WC]);
    }/*c*/

    else if (_option == 'j') {

        /*critical-band + DFT*/
        /*DFT savfltr*/
        savdft(spectro, spectro->params[WC]);
        /*critical-band fltr*/
        dofltr(spectro, CRIT_BAND, spectro->params[WC]);
    }/*j*/

    else if (_option == 'T') {

        /* put cb in fltr */
        dofltr(spectro, CRIT_BAND, spectro->params[WC]);
        /* if good spectrum,put slope in savfltr */
        if (spectro->spectrum) {

            for (_i = 0; _i < spectro->lenfltr; _i++)
                spectro->savfltr[_i] = 0;

            tilt(&spectro->fltr[13], 24, &spectro->savfltr[13]);

            /* make sure savfltr gets drawn */
            spectro->savspectrum = 1;
            spectro->lensavfltr = spectro->lenfltr;
            spectro->savspectrum = spectro->spectrum;
            spectro->savsizwin = spectro->sizwin;
            spectro->savspers = spectro->spers;

#ifdef SAS_VS2022
            strcpy_s(spectro->savname, 200, spectro->wavename);
#else
            strcpy(spectro->savname, spectro->wavename);
#endif

        }
        else { 
            
            spectro->savspectrum = 0; 
        }
    }/*T*/

    else if (_option == 'l') {

        /*spectrogram-like + DFT*/
         /*DFT savfltr*/
        savdft(spectro, spectro->params[WL]);
        /*linear prediction*/
        dofltr(spectro, LPC_SPECT, spectro->params[WL]);
    }/*l*/

     /* store the value used for this spectrum for postscript*/

    spectro->fd = spectro->params[FD];
}/* end new_spectrum */

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
/*                       T I L T		                         */
/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

/* Replace set of points infltr[] by straight-line approx in outfltr[] */

int tilt(int* infltr, int npts, int* outfltr){

    int i, imin, imax, infltrmax;
    float xsum, ysum, xysum, x2sum;
    float slope;
    float yintc;

    short sloptruncsw;   /* should move this to spectro*/
    sloptruncsw = 0;

    /* Adjust limits to include only peaks in range 1 kH to 5 kHz */
    imin = 0;
    imax = npts;
    /*    Find first local max after 1000 Hz */
    infltrmax = infltr[0];
    for (i = 0; i < imax; i++) {
        if (infltr[i] < infltr[i + 1])    infltrmax = infltr[i + 1] - 60;
        else    break;
    }
    /*    Ignore samples less than max-6 db */
    /* OUT	for (i=0; i<imax; i++) {
            if (infltr[i] < infltrmax)    imin++;
            else    break;
        }
       END OUT */

       /*    Find last local max before 5000 Hz, set infltrmax = peak value */
    infltrmax = infltr[imax - 1];
    for (i = imax - 1; i > imin; i--) {
        if (infltr[i] < infltr[i - 1])    infltrmax = infltr[i - 1];
        else    break;
    }
    infltrmax -= 60;
    /*    Ignore samples less than max-6 db */
    if (sloptruncsw == 1) {

        for (i = imax - 1; i > imin; i--) {

            if (infltrmax > infltr[i])    imax--;
            else    break;
        }
    }

    xsum = 0.0;
    ysum = 0.0;
    xysum = 0.0;
    x2sum = 0.0;

    for (i = imin; i < imax; i++) {

        xsum = xsum + (i + 1);
        ysum = ysum + infltr[i];
        xysum = xysum + (i + 1) * infltr[i];
        x2sum = x2sum + (i + 1) * (i + 1);
    }

    slope = npts * xysum - xsum * ysum;
    slope = slope / (npts * x2sum - xsum * xsum);
    yintc = (ysum - slope * xsum) / npts;

    for (i = imin; i < imax; i++) {

        outfltr[i] = (int)(slope * (i + 1) + yintc + 0.5f);
    }

    i = (int)(100 * slope + 0.5f);
    return(i);	/* Slope as integer value */
}/* end tilt */

/**************************************************************/
/*                savdft(XSPECTRO *spectro, int winsize)      */
/**************************************************************/
void savdft(XSPECTRO* spectro, int winsize) {

    int i, firstsamp;

    /* do dft type spectrum and put it in savfltr (S and j and l)*/
    spectro->sizwin = winsize;
    spectro->sizfft = 256;
    if (spectro->sizwin > spectro->sizfft)
        spectro->sizfft = 512;
    spectro->type_spec = DFT_MAG;
    spectro->nsdftmag = spectro->sizfft / 2;
    firstsamp = spectro->saveindex - spectro->sizwin / 2;
    if (spectro->sizwin <= spectro->sizfft) {
        if (firstsamp >= 0 && firstsamp <= spectro->totsamp - 1) {

            quickspec(spectro, &spectro->iwave[firstsamp]);

            spectro->lensavfltr = spectro->lenfltr;
            spectro->savspectrum = 1;
            spectro->savsizwin = spectro->sizwin;
            spectro->savspers = spectro->spers;

#ifdef SAS_VS2022
            strcpy_s(spectro->savname, 200, spectro->wavename);
#else
            strcpy(spectro->savname, spectro->wavename);
#endif

            /* Rms is in [spectro->lensavfltr] */
            for (i = 0; i <= spectro->lensavfltr; i++)
                spectro->savfltr[i] = spectro->fltr[i];
        }/* new spectrum */
        else {
            spectro->savspectrum = 0;
        }/* index too close to end of file for dft */
    }/* window size ok for dft */
    else {
        spectro->savspectrum = 0;
        // writetext("Window greater that DFT reset parameters.\n");
    }/*spectro->sizwin > spectro->sizfft*/

}/* end savdft*/

/****************************************************************/
/*                 copysav(XSPECTRO *spectro)                   */
/****************************************************************/
void copysav(XSPECTRO* spectro){

    int i;

    spectro->lensavfltr = spectro->lenfltr;
    spectro->savspectrum = spectro->spectrum;
    spectro->savsizwin = spectro->sizwin;
    spectro->savspers = spectro->spers;
    spectro->savfd = spectro->fd;

#ifdef SAS_VS2022
    strcpy_s(spectro->savname, 200, spectro->wavename);
#else
    strcpy(spectro->savname, spectro->wavename);
#endif

    for (i = 0; i <= spectro->lensavfltr; i++) {

        spectro->savfltr[i] = spectro->fltr[i];
        if (spectro->option == 'c')
            spectro->savnfr[i] = spectro->nfr[i];
    }


}/* end copysav*/

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
/*		            G E T F O R M . C		D. Klatt	 */
/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *  */

/*		On VAX, located in directory [klatt.klspec]		*/


/* EDIT HISTORY:
 * 000001 31-Jan-84 DK	Initial creation
 * 000002 24-Mar-84 DK	Fix so hidden formant rejected if too close to peak
 * 000003 23-May-84 DK	Improved peak locating interpolation scheme
 * 000004 17-Apr-86 DK	Improve accuracy of freq estimate for hidden peaks
 */

 /*               most of these are no an SPECTRO structure            */
 /*	int dfltr[257];			Spectral slope */

 /*        #define MAX_INDEX	  7*/
 /*	int nforfreq;			number of formant freqs found   */
 /*	int forfreq[16],foramp[16];	nchan and ampl of max in fltr[] */
 /*	int valleyfreq[8],valleyamp[8];	nchan and ampl of min in fltr[] */

 /*	int nmax_in_d;			number of maxima in deriv       */
 /*	int ncdmax[8],ampdmax[8];	nchan and ampl of max in deriv  */
 /*	int ncdmin[8],ampdmin[8];	nchan and ampl of min in deriv  */

 /*	int hftot;			number of hidden formants found */
 /*	int hiddenf[8],hiddena[8];	freq and ampl of hidden formant */
void getform(XSPECTRO* spectro) {

    int down = 1;
    int up = 2;

    int _i, _count, _index, _direction, _nchb, _nchp, _ff, _df,
        _lasfltr, _lasderiv, _da, _da1, _da2, _nhf;
    long _nsum;

    // Pass 1: Compute spectral slope
    _count = spectro->nchan - 2;
    for ( _i = 2; _i < _count; _i++) {

        // Approximate slope by function involving 5 consecutive channels
        spectro->dfltr[ _i] =
            spectro->fltr[ _i + 2] + spectro->fltr[_i + 1] -
            spectro->fltr[ _i - 1] - spectro->fltr[_i - 2];
    }

    // Pass 2: Find slope max, slope min; half-way between is a formant
    // If max and min of same sign, this is a potential hidden formant
    _direction = down;
    spectro->nmax_in_d = 0;
    _lasderiv = spectro->dfltr[2];
    spectro->hftot = 0;
    _count = spectro->nchan - 2;
    for (_i = 3; _i < _count; _i++) {

        if (spectro->dfltr[ _i] > _lasderiv) {

            if ((_direction == down) && (spectro->nmax_in_d > 0)) {

                // Found max and next min in deriv, see if hidden formant
                if ((spectro->hiddenf[spectro->hftot] = testhidden(spectro)) > 0) {

                    _index = (int)((spectro->hiddenf[spectro->hftot] * spectro->sizfft) / spectro->spers + 0.5f);
                    spectro->hiddena[spectro->hftot] = spectro->fltr[_index];
                    
                    if (spectro->hftot < MAX_INDEX)   
                        spectro->hftot++;
                } // end if ((spectro->hiddenf...
            } // end if ((_direction == ...
            
            _direction = up;
        }
        else if (spectro->dfltr[ _i] < _lasderiv) {

            if ((spectro->nmax_in_d > 0) && (_direction == down)) {
             
                // On way down to valley
                spectro->ampdmin[spectro->nmax_in_d - 1] = spectro->dfltr[ _i];
                spectro->ncdmin[spectro->nmax_in_d - 1] = _i;
            }

            if ((_direction == up) && (spectro->nmax_in_d < 7)) {
                
                spectro->ampdmin[spectro->nmax_in_d] = spectro->dfltr[ _i];
                spectro->ncdmin[spectro->nmax_in_d] = _i;
                
                // Peak in deriv found, compute chan # and value of deriv
                spectro->ampdmax[spectro->nmax_in_d] = _lasderiv;
                spectro->ncdmax[spectro->nmax_in_d] = _i - 1;
                
                if (spectro->nmax_in_d < MAX_INDEX)   
                    spectro->nmax_in_d++;
            } // end if ((_direction == up ...
            
            _direction = down;
        }

        _lasderiv = spectro->dfltr[ _i];
    }

    // Pass 3: Find actual spectral peaks, fold hidden peaks into array
    _direction = down;
    spectro->nforfreq = 0;
    _nhf = 0;
    _lasfltr = spectro->fltr[0];
    _count = spectro->nchan;
    for (_i = 1; _i < _count; _i++) {

        if (spectro->fltr[ _i] > _lasfltr) {

            _direction = up;
        }
        else if (spectro->fltr[_i] < _lasfltr) {

            if ((spectro->nforfreq > 0) && (_direction == down)) {
                
                // On way down to valley
                spectro->valleyamp[spectro->nforfreq - 1] = spectro->fltr[ _i];
            }

            if ((_direction == up) && (spectro->nforfreq < 6)) {

                spectro->valleyamp[spectro->nforfreq] = spectro->fltr[ _i];

                // Peak found, compute frequency
                // Step 1: Work back to filter defining beginning of peak
                
                for (_nchb = _i - 2; _nchb > 0; _nchb--) {

                    if (spectro->fltr[_nchb] < spectro->fltr[_i - 1])  
                        break;
                }

                // Step 2: Compute nearest filter of peak
                _nchp = (_nchb + _i) >> 1;
                _ff = spectro->nfr[_nchp];
                
                // Step 3: Add half a filter if plateau has even # filters
                if (_nchp < ((_nchb + _i + 1) >> 1)) {

                    _ff = (spectro->nfr[_nchp] + spectro->nfr[_nchp + 1]) >> 1;
                }

                // Step 4: Compute interpolation factor
                _da1 = spectro->fltr[_nchp] - spectro->fltr[_nchb];
                _da2 = spectro->fltr[_nchp] - spectro->fltr[_i];
                _da = _da1 + _da1;
                
                if (_da2 > _da1)    
                    _da = _da2 + _da2;

                _nsum = ((_da1 - _da2) * (spectro->nfr[_i - 1] - spectro->nfr[_i - 2]));
                _df = _nsum / _da;
                _ff += _df;

                while ((_nhf < spectro->hftot) && (spectro->hiddenf[_nhf] < _ff)
                    && (spectro->nforfreq < MAX_INDEX)) {

                    // Hidden formant should be inserted here
                    spectro->foramp[spectro->nforfreq] = spectro->hiddena[_nhf];
                    spectro->forfreq[spectro->nforfreq] = -spectro->hiddenf[_nhf];
                    spectro->nforfreq++; 
                    _nhf++;
                } // end while((_hhf ...

                spectro->foramp[spectro->nforfreq] = _lasfltr;
                spectro->forfreq[spectro->nforfreq] = _ff;
                if (spectro->nforfreq < MAX_INDEX)    
                    spectro->nforfreq++;
            }
            
            _direction = down;
        }
        
        _lasfltr = spectro->fltr[_i];
    } // end for _i

    while ((_nhf < spectro->hftot) && (spectro->nforfreq < MAX_INDEX)) {

        // Hidden formant should be inserted here
        spectro->foramp[spectro->nforfreq] = spectro->hiddena[_nhf];
        spectro->forfreq[spectro->nforfreq] = -spectro->hiddenf[_nhf];
        spectro->nforfreq++;
        _nhf++;   
        // minus indic. hidd
    }

    zap_insig_peak(spectro);
}

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
/*		        T E S T H I D D E N				 */
/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

/* Found loc max in deriv followed by loc min in deriv, see if hidden formant */
int testhidden(XSPECTRO* spectro) {

    int nchpeak;	/* channel number of potential hidden peak */
    int amphidpeak;	/* amplitude of hidden peak = (max+min)/2 in deriv */
    int delamp, delslope, nchvar, f, a, denom, foffset;

    int NCH_OF_PEAK_IN_DERIV, AMP_OF_PEAK_IN_DERIV;
    int NCH_OF_VALLEY_IN_DERIV, AMP_OF_VALLEY_IN_DERIV;

    NCH_OF_PEAK_IN_DERIV = spectro->ncdmax[spectro->nmax_in_d - 1];
    AMP_OF_PEAK_IN_DERIV = spectro->ampdmax[spectro->nmax_in_d - 1];
    NCH_OF_VALLEY_IN_DERIV = spectro->ncdmin[spectro->nmax_in_d - 1];
    AMP_OF_VALLEY_IN_DERIV = spectro->ampdmin[spectro->nmax_in_d - 1];

    /*    A real peak is where derivative passes down through zero. */
    /*    A hidden peak is where local max and succeeding local min of derivative */
    /*    both are positive, or both are negative */
    if ((AMP_OF_PEAK_IN_DERIV <= 0)
        || (AMP_OF_VALLEY_IN_DERIV >= 0)) {

        /*	   OUT printf("ap=%d av=%d fp=%d fv=%d\n",
                 AMP_OF_PEAK_IN_DERIV, AMP_OF_VALLEY_IN_DERIV,
                 NCH_OF_PEAK_IN_DERIV, NCH_OF_VALLEY_IN_DERIV); END OUT */

                 /*	  See if diff in slope not simply noise */
        delslope = AMP_OF_PEAK_IN_DERIV - AMP_OF_VALLEY_IN_DERIV;
        if (delslope < 25) {
            return(-1);
        }

        /*	  Tentative channel hidden peak = mean of peak & valley locs in deriv */
        nchpeak = (NCH_OF_PEAK_IN_DERIV + NCH_OF_VALLEY_IN_DERIV) >> 1;

        /*	  Find amplitude of nearest formant peak (local spectral max) */
        if (AMP_OF_PEAK_IN_DERIV < 0) {
            /*	      Nearest peak is previous peak, find it */
            nchvar = nchpeak;
            while ((spectro->fltr[nchvar - 1] >= spectro->fltr[nchvar]) && (nchvar > 0)) {
                nchvar--;
            }
            delamp = spectro->fltr[nchvar] - spectro->fltr[nchpeak];
            foffset = -80;
        }
        else {
            /*	      Nearest peak is next peak, find it */
            nchvar = nchpeak;
            while ((spectro->fltr[nchvar + 1] >= spectro->fltr[nchvar]) && (nchvar < 127)) {
                nchvar++;
            }
            delamp = spectro->fltr[nchvar] - spectro->fltr[nchpeak];
            foffset = 120;	/* hidden formant is actually between min
                       and next max */
        }

        /*	  See if amp of hidden formant not too weak relative to near peak */
        if (delamp < 80) {
            /*	      Exact location of hidden peak is halfway from max to min in deriv */
            amphidpeak = (AMP_OF_PEAK_IN_DERIV + AMP_OF_VALLEY_IN_DERIV) >> 1;
            nchpeak = NCH_OF_PEAK_IN_DERIV;
            if (spectro->dfltr[nchpeak] > amphidpeak) {
                while (((spectro->dfltr[nchpeak] - amphidpeak) >= 0)
                    && (nchpeak <= NCH_OF_VALLEY_IN_DERIV)) {
                    nchpeak++;
                }
                nchpeak--;
            }
            a = spectro->dfltr[nchpeak] - amphidpeak;
            denom = spectro->dfltr[nchpeak] - spectro->dfltr[nchpeak + 1];
            if (denom > 0) {
                f = spectro->nfr[nchpeak]
                    + ((a * (spectro->nfr[nchpeak + 1] - spectro->nfr[nchpeak])) / denom);
            }
            else {
                f = spectro->nfr[nchpeak];
            }

            return(f + foffset);
        }
    }

    return(-1);
}/* end testhidden */

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
/*		        Z A P - I N S I G - P E A K			 */
/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

/*	  Eliminate insignificant peaks (last one always significant) */
/*	  Must be down 6 dB from peaks on either side		      */
/*	  And valley on either side must be less than 2 dB	      */

/*	  Also eliminate weaker of two formant peaks within 210 Hz of */
/*	  each other, unless not enough formants in F1-F3 range	      */

void zap_insig_peak(XSPECTRO* spectro) {

    int nf, n, xxx, fcur, fnext;

    for (nf = 0; nf < spectro->nforfreq - 1; nf++) {

        if ((nf == 0) || (spectro->foramp[nf - 1] > spectro->foramp[nf] + 60)) {
            if (spectro->foramp[nf + 1] > spectro->foramp[nf] + 60) {
                if ((nf == 0) || (spectro->valleyamp[nf - 1] > spectro->foramp[nf] - 20)) {
                    if (spectro->valleyamp[nf] > spectro->foramp[nf] - 20) {
                        for (n = nf; n < spectro->nforfreq; n++) {
                            spectro->forfreq[n] = spectro->forfreq[n + 1];
                            spectro->foramp[n] = spectro->foramp[n + 1];
                        }
                        spectro->nforfreq--;
                    }
                }
            }
        }

        /*	  Zap weaker of 2 close peaks (hidden peak is alway weaker) */
        if (nf < spectro->nforfreq - 1) {
            fcur = spectro->forfreq[nf];
            if (fcur < 0)    fcur = -fcur;

            fnext = spectro->forfreq[nf + 1];
            if (fnext < 0)    fnext = -fnext;

            if ((fnext - fcur) < 210) {

                if (spectro->foramp[nf] > spectro->foramp[nf + 1])    xxx = 1;
                else    xxx = 0;

                for (n = nf + xxx; n < spectro->nforfreq - 1; n++) {
                    spectro->forfreq[n] = spectro->forfreq[n + 1];
                    spectro->foramp[n] = spectro->foramp[n + 1];
                }
                spectro->nforfreq--;
                nf--;
            }
        }
    }
}/* end zap_insig_peak */

/***************************************************************/
/*                  findsdelta(spectro)                       */
/***************************************************************/
void findsdelta(XSPECTRO* spectro){

    /*find number of samples for Hamming window from window in ms*/
    spectro->winsamps = (int)(spectro->winms * spectro->spers / 1000.0f + 0.5f);
    /* default to fft size if window is larger*/
    if (spectro->winsamps > spectro->slice) {

        spectro->winsamps = spectro->slice;
        spectro->winms = (float)spectro->slice * 1000.0f / spectro->spers;
    }

    spectro->sdelta = (int)(spectro->msdelta * spectro->spers / 1000.0f + 0.5f);

    if ((spectro->winsamps - spectro->sdelta) > 0)
        spectro->ovlap = spectro->winsamps - spectro->sdelta;
    else
        spectro->ovlap = 11;

}/* end findsdelta */

/**************************************************************/
/*                   findypix(XSPECTRO *spectro)               */
/**************************************************************/
void  findypix(XSPECTRO* spectro) {

    /*called before calculation */
    /* set up ypix based on xpix only */
    /* change xpix if it forces ypix off screen*/
    /* this code tries to preserve a standard(lspecto) aspect ratio
    lspecto displays 650 ms in the length of the y axis
    */

    /* calculate all dfts */

    spectro->specsize = spectro->totsamp;

    spectro->xmaxests = (spectro->specsize - spectro->winsamps) /
        ((spectro->winsamps - spectro->ovlap) * spectro->numav);


    spectro->sampsused[GRAM] = spectro->winsamps + spectro->xmaxests *
        ((spectro->winsamps - spectro->ovlap) * spectro->numav);

    spectro->sampsused[REGRAM] = spectro->winsamps + spectro->xmaxests *
        ((spectro->winsamps - spectro->ovlap) * spectro->numav);

    spectro->wxmax = spectro->xmaxests * spectro->xpix;

    float _temp = (float)spectro->wxmax * spectro->spratio / ((float)spectro->sampsused[REGRAM] / spectro->spers * 1000);
    spectro->wymax = (int)( _temp + 0.5f);

    /*make sure it is less than devheight*/
    if (spectro->wymax > spectro->devheight) {
        spectro->wymax = spectro->devheight;
        spectro->ypix = spectro->wymax / (spectro->slice / 2);

        /* change xpix */
        spectro->wxmax = (int)(((float)spectro->sampsused[GRAM] / spectro->spers * 1000) /
            spectro->spratio * spectro->wymax + 0.5f);

        /* change xpix */
        spectro->wxmax = (int)(((float)spectro->sampsused[REGRAM] / spectro->spers * 1000) /
            spectro->spratio * spectro->wymax + 0.5f);

    }
    else {
        spectro->ypix = spectro->wymax / (spectro->slice / 2);
    }

}/* findypix */

/***************************************************************/
/*                  calculate_spectra(spectro)                 */
/***************************************************************/
void calculate_spectra(XSPECTRO* spectro){

    bool _bFileWrite = false;

    int        i, j, m, k, l, index;
    float      a, * mag;
    double     tempflt;
    COMPLEX* samp;
    float      t, fq, xinc, yinc;
    unsigned long gv; /* gray value index*/
    float fd;         /* convert the fdifcoef to float between 0 and 1*/

    int f, fdj, fdk;
    int halfslice, halfwin;
    float xri, xii;
    int gvsindex, totgvs;

    //if (cmdtextw) {
    //    writetext("calculating...\n");
    //    XSync(XtDisplay(cmdtextw), False);
    //}
    //else {
    //    printf("calculating...\n");
    //}

    halfslice = spectro->slice >> 1;
    halfwin = spectro->winsamps / 2;
    spectro->insize = spectro->totsamp;
    /*  Calculate the number of samples in each estimation and the number of estimations in the data set */
    spectro->estsize = (spectro->winsamps - spectro->ovlap) * (spectro->numav - 1) + spectro->winsamps;
    spectro->numests = (spectro->insize - spectro->winsamps) / ((spectro->winsamps - spectro->ovlap) * spectro->numav);

    t = 0.0;  /* for drawing only */

    /* pre-emphasis*/

    fd = 0.01f * (float)spectro->fdifcoef;

    /*allocate space for all the spectra (1/2 slice * numests) */
    if (spectro->allmag)
        free(spectro->allmag);
    spectro->allmag = (float*)malloc(sizeof(float) * spectro->slice / 2 * spectro->numests);

    mag = (float*)malloc(sizeof(float) * spectro->slice);
    samp = (COMPLEX*)malloc(sizeof(COMPLEX) * spectro->slice);
    if (!mag || !samp) {
        printf("\n  Memory allocation error.\n");
        exit(1);
    }

    /* store gray values */
    totgvs = (int)((float)spectro->slice / 2.0f * spectro->numests * spectro->xpix * spectro->ypix + 0.5f);
    printf("total gvs: %d\n", totgvs);
    printf("allmag allocation: %lu\n", sizeof(float) * spectro->slice / 2 * spectro->numests);
    printf("spectro->winsamp: %d\n", spectro->winsamps);

    if (spectro->allgvs)
        free(spectro->allgvs);

    /* make background zeros */
#ifdef _REPLACED_WITH_CALLOC
    spectro->allgvs = (char*)malloc(totgvs);
    /***debug**/
    for (i = 0; i < totgvs; i++)
        spectro->allgvs[i] = (char)0;
#endif
    spectro->allgvs = (char*)calloc(sizeof(char), totgvs);

    gvsindex = 0;

    /* put #points in DFT in window title
    sprintf(str, "%s, %.1fms win.", spectro->wavename, spectro->winms);
    */

    FILE * _fp1 = NULL;
    FILE * _fp2 = NULL;

    if (_bFileWrite) {

#ifdef SAS_VS2022

        fopen_s( &_fp1, "DATA_GV1.txt", "wb");
        fopen_s( &_fp2, "DATA_GV2.txt", "wb");
#else

        _fp1 = fopen("DATA_GV1.txt", "wb");
        _fp2 = fopen("DATA_GV2.txt", "wb");
#endif
    }

    for (i = 0; i < spectro->numests; i++) { // in this test, value is 1121
        for (k = 0; k < spectro->slice; k++) { // in this test, value is 128
            mag[k] = 0;
        }
        for (j = 0; j < spectro->numav; j++) {
            if (fd > 0.0) {
                /*Use first difference preemphasis by D.Klatt*/
                index = i * (spectro->winsamps - spectro->ovlap) * spectro->numav + j * (spectro->winsamps - spectro->ovlap);
                if (index == 0) {
                    for (f = 0; f < spectro->winsamps; f++) {
                        samp[f].real = (float)(spectro->iwave[f] >> 4);
                        samp[f].imag = 0.0;
                    }
                }/*index = 0*/
                else {
                    k = 0;
                    for (f = 0; f < halfwin; f++) {
                        fdj = f + f;
                        fdk = fdj + 1;
                        xri = (float)(spectro->iwave[index + fdj] >> 4) - fd * (float)(spectro->iwave[index + fdj - 1] >> 4);
                        xii = (float)(spectro->iwave[index + fdk] >> 4) - fd * (float)(spectro->iwave[index + fdj] >> 4);

                        samp[k].real = xri;
                        samp[k].imag = 0.0;
                        samp[k + 1].real = xii;
                        samp[k + 1].imag = 0.0;
                        k += 2;
                    }
                }/*don't use iwave[-1]*/
            }/* preemphasis */
            else {
                for (k = 0; k < spectro->winsamps; k++) {
                    /* index = i*spectro->estsize +*/

                    /*debug dhall*/
                    index = i * (spectro->winsamps - spectro->ovlap) * spectro->numav + j * (spectro->winsamps - spectro->ovlap) + k;
                    samp[k].real = (float)(spectro->iwave[index] >> 4);
                    samp[k].imag = 0;
                }
            }/* no preemphasis */

            if (spectro->slice > spectro->winsamps) {
                /*pad with zeros like lspecto*/
                for (k = spectro->winsamps; k < spectro->slice; k++) {
                    samp[k].real = 0.0;
                    samp[k].imag = 0.0;
                }
            }/* padding */

            ham(samp, spectro->winsamps);
            m = ilog2(spectro->slice);
            fft(samp, m);
            a = (float)spectro->slice * spectro->slice;
            for (k = 0; k < spectro->slice; k++) {
                tempflt = samp[k].real * samp[k].real;
                tempflt += samp[k].imag * samp[k].imag;
                tempflt = tempflt / a;
                mag[k] += (float)tempflt;
            }
        }
        fq = 0.0; /* for drawing */
        /* Take log after averaging the magnitudes.  */
        for (k = 0; k < spectro->slice / 2; k++) {

            mag[k] = mag[k] / spectro->numav; // KALEEM: di sini magnitude dibagi dengan numav
            mag[k] = (float)(10 * log10(XAM(mag[k], 1.e-14))); // KALEEM: dan di menggunakan operasi log
            // printf("mag: %f\n", mag[k]);
            /* store all spectra in one array*/

            spectro->allmag[i * spectro->slice / 2 + k] = mag[k];
            /*DRAWING STUFF*/
            if (k > 0 && i > 0) {
                yinc = (spectro->allmag[(i - 1) * spectro->slice / 2 + k] - spectro->allmag[(i - 1) * spectro->slice / 2 + k - 1]) 
                            / (float)(spectro->ypix);
                xinc = (spectro->allmag[i * spectro->slice / 2 + k - 1] - spectro->allmag[(i - 1) * spectro->slice / 2 + k - 1])
                            / (float)(spectro->xpix);

                for (j = 0; j < spectro->xpix; j++) {
                    for (l = 0; l < spectro->ypix; l++) {
                        if ((spectro->allmag[(i - 1) * spectro->slice / 2 + k - 1] + j * xinc + l * yinc) > spectro->minmag) {
                            if ((spectro->allmag[(i - 1) * spectro->slice / 2 + k - 1] + j * xinc + l * yinc) >= spectro->maxmag) {
                                gv = spectro->numcol;
                            }
                            else {
                                gv = (unsigned long)(
                                    (spectro->allmag[(i - 1) * spectro->slice / 2 + k - 1] - spectro->minmag
                                        + j * xinc + l * yinc) / spectro->cinc + 2);
                            }
                            gvsindex = (int)(t)*spectro->ypix * halfslice + (int)fq;
                            spectro->allgvs[gvsindex] = (char)gv;

                            if (_bFileWrite) {

#ifdef SAS_VS2022

                                fprintf_s( _fp1, "%lu\n", gv);
                                fprintf_s( _fp2, "%f\n", spectro->allmag[(i - 1) * spectro->slice / 2 + k - 1]);
#else

                                fprintf(_fp1, "%lu\n", gv);
                                fprintf(_fp2, "%f\n", spectro->allmag[(i - 1) * spectro->slice / 2 + k - 1]);
#endif
                            }
                            //printf("1mag %f\n",mag[k]);
                        }/* only draw if not white */
                        fq += 1.0;
                    }/*ypix freq*/
                    t += 1.0;
                    fq -= (float)spectro->ypix;
                }/*xpix time*/
                t -= (float)spectro->xpix;
                fq += (float)spectro->ypix;
            }/*wait till second spectrum to draw*/
        }/*k*/
        if (i > 0)
            t += (float)spectro->xpix;
    }/* end numests */

    if (_bFileWrite) {

        fclose( _fp1);
        fclose( _fp2);
    }

    /*  free(mag); free(samp); */
    spectro->xgram = spectro->numests * spectro->xpix;
    spectro->wymax = spectro->slice / 2 * spectro->ypix;
    printf("xgram: %d\twymax: %d\n", spectro->xgram, spectro->wymax);
    printf("total sample: %d\n", spectro->totsamp);
    printf("end calculate_spectra..............\n");
}/*end calculate_spectra*/

/***************************************************************/
/*                  calculate_spectrogram(spectro)                 */
/***************************************************************/
void calculate_spectrogram(XSPECTRO* spectro){

    int        i, j, m, k, l, index;
    float      a, * mag;
    double     tempflt;
    COMPLEX* samp;
    float      t, fq, xinc, yinc, zinc;
    unsigned long gv; /* gray value index*/
    float fd;         /* convert the fdifcoef to float between 0 and 1*/

    int f, fdj, fdk;
    int halfslice, halfwin;
    float xri, xii;
    char str[800];
    int gvsindex, totgvs;
    int y, fmax, inc;

    printf("Debug_fjr.......................................\n");
    halfslice = spectro->slice >> 1;
    halfwin = spectro->winsamps / 2;

    spectro->insize = spectro->totsamp;

    /*  Calculate the number of samples in each estimation and the number of estimations in the data set */
    spectro->estsize = (spectro->winsamps - spectro->ovlap) * (spectro->numav - 1) + spectro->winsamps;
    spectro->numests = (spectro->insize - spectro->winsamps) / ((spectro->winsamps - spectro->ovlap) * spectro->numav);

    t = 0.0;  /* for drawing only */

    /* pre-emphasis*/
    fd = 0.01 * (float)spectro->fdifcoef;

    /*allocate space for all the spectra (1/2 slice * numests) */
    if (spectro->allmag)
        free(spectro->allmag);
    spectro->allmag = (float*)malloc(sizeof(float) * spectro->slice / 2 * spectro->numests);

    mag = (float*)malloc(sizeof(float) * spectro->slice);
    samp = (COMPLEX*)malloc(sizeof(COMPLEX) * spectro->slice);
    if (!mag || !samp) {
        printf("\n  Memory allocation error.\n");
        exit(1);
    }

    /* store gray values */
    totgvs = spectro->slice / 2.0 * spectro->numests * spectro->xpix * spectro->ypix;

    if (spectro->reallgvs)
        free(spectro->reallgvs);

    /* make background zeros */
#ifdef _REPLACED_WITH_CALLOC
    spectro->allgvs = (char*)malloc(totgvs);
    /***debug**/
    for (i = 0; i < totgvs; i++)
        spectro->allgvs[i] = (char)0;
#endif
    spectro->reallgvs = (char*)calloc(sizeof(char), totgvs);

    gvsindex = 0;

    emxArray_real_T* STFTmag;
    emxInitArray_real_T(&STFTmag, 2);

    emxArray_real_T* CIFPos;
    emxInitArray_real_T(&CIFPos, 2);

    emxArray_real_T* time;
    emxInitArray_real_T(&time, 2);

    emxArray_real_T* freq;
    emxInitArray_real_T(&freq, 2);
    double* signal2 = (double*)malloc(spectro->totsamp * sizeof(double));

    emxArray_real_T* signal;
    emxInitArray_real_T(&signal, 2);

    signal->size[0] = 1;
    signal->size[1] = spectro->totsamp;
    emxEnsureCapacity_real_T(signal, spectro->totsamp);

    for (int n = 0; n < spectro->totsamp; n++) {

        signal->data[n] = (double)spectro->iwave[n];
        signal2[n] = (double)spectro->iwave[n] / 32767;
    }

    printf("before Nelsonspec\n");
    int ovlap = spectro->ovlap;
    printf("spectro->winsamps = %d\n", spectro->winsamps);
    printf("spectro->slice (fft) = %d\n", spectro->slice);
    printf("spectro->ovlap= %d\n", spectro->ovlap);
    printf("spectro->numav= %d\n", spectro->numav);
    printf("totgvs = %d\n", totgvs);
    printf("numsets = %d\n", spectro->numests);

    Nelsonspec(signal, 10000, spectro->winsamps, spectro->ovlap, spectro->slice, 0, 5000, -30, STFTmag, CIFPos, time, freq);

    int maxIndex = STFTmag->size[0] * STFTmag->size[1];
    FILE* fp = fopen("FINAL_OUTPUT_STFTmag_C.txt", "wb");
    for (int i = 0; i < maxIndex; i++)
        fprintf(fp, "%.4f\n", STFTmag->data[i]);
    fclose(fp);

    maxIndex = CIFPos->size[0] * CIFPos->size[1];
    FILE* fp1 = fopen("FINAL_OUTPUT_CIFpos_C.txt", "wb");
    for (int i = 0; i < maxIndex; i++)
        fprintf(fp1, "%.4f\n", CIFPos->data[i]);
    fclose(fp1);

    maxIndex = time->size[0] * time->size[1];
    FILE* fp2 = fopen("FINAL_OUTPUT_time_C.txt", "wb");
    for (int i = 0; i < maxIndex; i++)
        fprintf(fp2, "%.4f\n", time->data[i]);
    fclose(fp2);

    maxIndex = freq->size[0] * freq->size[1];
    FILE* fp3 = fopen("FINAL_OUTPUT_freq_C.txt", "wb");
    for (int i = 0; i < maxIndex; i++)
        fprintf(fp3, "%.4f\n", freq->data[i]);
    fclose(fp3);

    // Find indices of elements satisfying the conditions
    emxArray_real_T* plot_these;
    emxInit_real_T(&plot_these, 1);

    findIndices(STFTmag, CIFPos, time, -30, 0, 5000, plot_these, 0.0032, 3.3672);
    FILE* fpplot_these = fopen("fpplot_these.txt", "wb"); // store file KALEEM
    for (int i = 0; i < plot_these->size[0]; i++)
        fprintf(fpplot_these, "%.0f\n", plot_these->data[i] + 1); // store file KALEEM
    fclose(fpplot_these); // store file KALEEM

    // Initialize the STFTplot array
    emxArray_real_T* STFTplot;
    emxInit_real_T(&STFTplot, 1);
    STFTplot->size[0] = plot_these->size[0];
    emxEnsureCapacity_real_T(STFTplot, STFTplot->size[0]);
    extractElementsPlot(STFTmag, plot_these, STFTplot); // Z vector
    FILE* fpSTFTplot = fopen("fpSTFTplot.txt", "wb"); // store file KALEEM
    for (int i = 0; i < STFTplot->size[0]; i++)
        fprintf(fpSTFTplot, "%.4f\n", STFTplot->data[i]); // store file KALEEM
    fclose(fpSTFTplot); // store file KALEEM

    // Initialize the CIFplot array
    emxArray_real_T* CIFplot;
    emxInit_real_T(&CIFplot, 1);
    CIFplot->size[0] = plot_these->size[0];
    emxEnsureCapacity_real_T(CIFplot, CIFplot->size[0]);
    extractElementsPlot(CIFPos, plot_these, CIFplot);
    FILE* fpCIFplot = fopen("fpCIFplot.txt", "wb"); // store file KALEEM
    for (int i = 0; i < CIFplot->size[0]; i++) {
        fprintf(fpCIFplot, "%.4f\n", CIFplot->data[i]); // store file KALEEM
    }
    fclose(fpCIFplot); // store file KALEEM

    // Initialize the tremapPlot array
    emxArray_real_T* tremapPlot;
    emxInit_real_T(&tremapPlot, 1);
    tremapPlot->size[0] = plot_these->size[0];
    emxEnsureCapacity_real_T(tremapPlot, tremapPlot->size[0]);
    extractElementsPlot(time, plot_these, tremapPlot);
    FILE* fptremap = fopen("fpfptremap.txt", "wb"); // store file KALEEM
    for (int i = 0; i < tremapPlot->size[0]; i++) {
        fprintf(fptremap, "%.4f\n", tremapPlot->data[i]); // store file KALEEM
    }
    fclose(fptremap); // store file KALEEM

    //--------------------------- KALEEM test plot data start

    printf("after Nelsonspec\n");
    int is1 = STFTmag->size[0];
    int is2 = STFTmag->size[1];

    printf("Number of time by nelsons = %d\n", tremapPlot->size[0]);

    printf("spectro->numests = %d\n", spectro->numests);
    printf("spectro->slice = %d\n", spectro->slice);
    printf("spectro->ypix = %d\n", spectro->ypix);
    printf("spectro->xpix = %d\n", spectro->xpix);

    printf("is1 = %d\n", is1);
    printf("is2 = %d\n", is2);

    printf("spectro->minmag = %f\n", spectro->minmag);
    printf("spectro->maxmag = %f\n", spectro->maxmag);
    //---------------------------------------------------------------------------- Nelson end
    FILE* fp40 = fopen("DATA_GV2.txt", "wb");

    // Assuming you have your reduced time and frequency arrays ready as emxArray_real_T pointers
    //emxArray_real_T* reduced_frequency;
    emxInit_real_T(&reduced_frequency, 1);
    reduced_frequency->size[0] = CIFplot->size[0];
    emxEnsureCapacity_real_T(reduced_frequency, spectro->numests);

    // Find the maximum value in the original frequency array
    double max_frequency = CIFplot->data[0];
    for (int i = 1; i < CIFplot->size[0]; i++) {

        if (CIFplot->data[i] > max_frequency)
            max_frequency = CIFplot->data[i];
    }

    // Scale down the frequency values to fit within the target range (0 - TARGET_FREQUENCY_MAX)
    for (int i = 0; i < CIFplot->size[0]; i++)
        reduced_frequency->data[i] = (CIFplot->data[i] / max_frequency) * (spectro->slice / 2 * spectro->ypix);

    FILE* downsampleFreq = fopen("downsampleFreq.txt", "wb"); // store file KALEEM
    for (int i = 0; i < reduced_frequency->size[0]; i++)
        fprintf(downsampleFreq, "%.4f\n", reduced_frequency->data[i]); // store file KALEEM
    fclose(downsampleFreq); // store file KALEEM

    emxInit_real_T(&scaled_down_magnitude, 1);
    scaled_down_magnitude->size[0] = CIFplot->size[0];
    emxEnsureCapacity_real_T(scaled_down_magnitude, STFTmag->size[0]);

    // Find the maximum value in the original frequency array
    double lowest_magnitude = STFTplot->data[0];
    for (int i = 0; i < STFTplot->size[0]; i++) {

        if (STFTplot->data[i] < lowest_magnitude)
            lowest_magnitude = STFTplot->data[i];
    }

    // Scale down the frequency values to fit within the target range (0 - TARGET_FREQUENCY_MAX)
    for (int i = 0; i < STFTplot->size[0]; i++)
        scaled_down_magnitude->data[i] = (STFTplot->data[i] / lowest_magnitude) * 9;

    //emxArray_real_T* scaled_down_time;
    emxInit_real_T(&scaled_down_time, 1);
    scaled_down_time->size[0] = tremapPlot->size[0];
    emxEnsureCapacity_real_T(scaled_down_time, tremapPlot->size[0]);

    // Find the maximum value in the original frequency array
    double longest_time = tremapPlot->data[0];
    for (int i = 0; i < tremapPlot->size[0]; i++) {

        if (tremapPlot->data[i] > longest_time)
            longest_time = tremapPlot->data[i];
    }

    // Scale down the frequency values to fit within the target range (0 - TARGET_FREQUENCY_MAX)
    for (int i = 0; i < tremapPlot->size[0]; i++)
        scaled_down_time->data[i] = (tremapPlot->data[i] / longest_time) * (spectro->numests * spectro->xpix);

    for (i = 0; i <= (spectro->slice / 2.0) * spectro->numests * spectro->xpix * spectro->ypix; i++) {

        if (i >= reduced_frequency->size[0]) {
            printf("dibreak\n");
            break;
        }
        //-----------------------------------------------------------------
        if (round(reduced_frequency->data[i]) > 0) {

            gv = (9 - scaled_down_magnitude->data[i]) * 1;

            int _pos = 1 + ((int)round(scaled_down_time->data[i]) * (spectro->slice / 2 * spectro->ypix))
                + (int)round(reduced_frequency->data[i]);
            
            if (_pos < totgvs)
                spectro->reallgvs[ _pos] = (char)gv;
        }
    }/* end numests */

    fclose(fp40);

    /*  free(mag); free(samp); */
    spectro->xgram = spectro->numests * spectro->xpix;
    spectro->wymax = spectro->slice / 2 * spectro->ypix;
    printf("xgram: %d\twymax: %d\n", spectro->xgram, spectro->wymax);
    printf("total sample: %d\n", spectro->totsamp);
    printf("end of calculate_spectrogram.....\n");
}/*end calculate_spectrogram*/

/****************************************************************/
/*                 win_size(XSPECTRO *spectro)                   */
/****************************************************************/
void win_size(XSPECTRO* spectro) {

    printf("win_size.................................\n");
    if (spectro->specms > (float)spectro->totsamp / spectro->spers * 1000.0) {
        spectro->specsize = spectro->totsamp;
        spectro->specms = spectro->specsize / spectro->spers * 1000;
    }
    else
    {
        spectro->specsize = spectro->specms * spectro->spers / 1000;
    }

    /* calculate how many estimates will be displayed in window */

    spectro->xmaxests = (spectro->specsize - spectro->winsamps) /
        ((spectro->winsamps - spectro->ovlap) * spectro->numav);


    spectro->wxmax = spectro->xpix * (spectro->xmaxests);

    /* pixmap is limited */
    if (spectro->wxmax > spectro->devwidth - 100) {
        spectro->wxmax = spectro->devwidth - 100;


        spectro->xmaxests = spectro->wxmax / spectro->xpix;

        spectro->specsize = spectro->xmaxests *
            ((spectro->winsamps - spectro->ovlap) * spectro->numav) +
            spectro->winsamps;

        spectro->specms = spectro->specsize / spectro->spers * 1000;

    }/* spectrogram wider that screen */

    spectro->sampsused[GRAM] = spectro->winsamps + spectro->xmaxests *
        ((spectro->winsamps - spectro->ovlap) * spectro->numav);
    spectro->sampsused[REGRAM] = spectro->winsamps + spectro->xmaxests *
        ((spectro->winsamps - spectro->ovlap) * spectro->numav);

    //findxryb(spectro);

}/* end find_win_size*/

/***************************************************************/
/*                 remapgray(XSPECTRO *spectro)                 */
/***************************************************************/
void remapgray(XSPECTRO* spectro) {

    findsdelta(spectro);
    findypix(spectro);
    printf("calculate_spectra......calling 1\n");
    calculate_spectra(spectro);
    //win_size(spectro);
}
