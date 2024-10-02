#pragma once

#include "global.h"

/***********************************************************************
  code from Embreee and Kimble

spec_dft - Discrete Fourier Transform

This function performs a straight DFT of N points on an array of
complex numbers whose first member is pointed to by Datain.  The
output is placed in an array pointed to by Dataout.

void spec_dft(COMPLEX *Datain, COMPLEX *Dataout, int N)

*************************************************************************/
void spec_dft(COMPLEX* Datain, COMPLEX* Dataout, int N);

/***********************************************************************

idft - Inverse Discrete Fourier Transform

This function performs an inverse DFT of N points on an array of
complex numbers whose first member is pointed to by Datain.  The
output is placed in an array pointed to by Dataout.
It returns nothing.

void idft(COMPLEX *Datain, COMPLEX *Dataout, int N)

*************************************************************************/
void idft(COMPLEX* Datain, COMPLEX* Dataout, int N);
/**************************************************************************

fft - In-place radix 2 decimation in time FFT

Requires pointer to complex array, x and power of 2 size of FFT, m
(size of FFT = 2**m).  Places FFT output on top of input COMPLEX array.

void fft(COMPLEX *x, int m)

*************************************************************************/
void fft(COMPLEX* x, int m);
/**************************************************************************

ifft - In-place radix 2 decimation in time inverse FFT

Requires pointer to complex array, x and power of 2 size of FFT, m
(size of FFT = 2**m).  Places inverse FFT output on top of input
frequency domain COMPLEX array.

void ifft(COMPLEX *x, int m)

*************************************************************************/
void ifft(COMPLEX* x, int m);
/************************************************************

rfft - trig recombination real input FFT

Requires real array pointed to by x, pointer to complex
output array, y and the size of real FFT in power of
2 notation, m (size of input array and FFT, N = 2**m).
On completion, the COMPLEX array pointed to by y
contains the lower N/2 + 1 elements of the spectrum.

void rfft(float *x, COMPLEX *y, int m)

***************************************************************/
void rfft(float* x, COMPLEX* y, int m);
/*************************************************************************

ham - Hamming window

Scales both real and imaginary parts of input array in-place.
Requires COMPLEX pointer and length of input array.

void ham(COMPLEX *x, int n)

*************************************************************************/
void ham(COMPLEX* x, int n);
/**************************************************************************

ilog2 - base 2 logarithm

Returns base 2 log such that i = 2**ans where ans = ilog2(i).
if ilog2(i) is between two values, the larger is returned.

int ilog2(unsigned int x)

*************************************************************************/
int ilog2(unsigned int x);
