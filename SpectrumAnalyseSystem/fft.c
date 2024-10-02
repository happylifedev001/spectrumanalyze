#include <math.h>
#include "rt_nonfinite.h"
#include "Nelsonspec.h"
#include "fft.h"

static void r2br_r2dit_trig_impl(const emxArray_creal_T* x, int unsigned_nRows, const emxArray_real_T* costab, const emxArray_real_T* sintab, emxArray_creal_T* y)
{
	int j;
	int nRowsM2;
	int nRowsD2;
	int nRowsD4;
	int iy;
	int iDelta;
	int ix;
	int ju;
	int i;
	boolean_T tst;
	double temp_re;
	double temp_im;
	int temp_re_tmp;
	double twid_re;
	double twid_im;
	j = x->size[0];
	if (j >= unsigned_nRows) {
		j = unsigned_nRows;
	}

	nRowsM2 = unsigned_nRows - 2;
	nRowsD2 = unsigned_nRows / 2;
	nRowsD4 = nRowsD2 / 2;
	iy = y->size[0];
	y->size[0] = unsigned_nRows;
	emxEnsureCapacity_creal_T(y, iy);
	if (unsigned_nRows > x->size[0]) {
		iDelta = y->size[0];
		iy = y->size[0];
		y->size[0] = iDelta;
		emxEnsureCapacity_creal_T(y, iy);
		for (iy = 0; iy < iDelta; iy++) {
			y->data[iy].re = 0.0;
			y->data[iy].im = 0.0;
		}
	}

	ix = 0;
	ju = 0;
	iy = 0;
	for (i = 0; i <= j - 2; i++) {
		y->data[iy] = x->data[ix];
		iDelta = unsigned_nRows;
		tst = true;
		while (tst) {
			iDelta >>= 1;
			ju ^= iDelta;
			tst = ((ju & iDelta) == 0);
		}

		iy = ju;
		ix++;
	}

	y->data[iy] = x->data[ix];
	if (unsigned_nRows > 1) {
		for (i = 0; i <= nRowsM2; i += 2) {
			temp_re = y->data[i + 1].re;
			temp_im = y->data[i + 1].im;
			y->data[i + 1].re = y->data[i].re - y->data[i + 1].re;
			y->data[i + 1].im = y->data[i].im - y->data[i + 1].im;
			y->data[i].re += temp_re;
			y->data[i].im += temp_im;
		}
	}

	iDelta = 2;
	iy = 4;
	ix = 1 + ((nRowsD4 - 1) << 2);
	while (nRowsD4 > 0) {
		for (i = 0; i < ix; i += iy) {
			temp_re_tmp = i + iDelta;
			temp_re = y->data[temp_re_tmp].re;
			temp_im = y->data[temp_re_tmp].im;
			y->data[temp_re_tmp].re = y->data[i].re - temp_re;
			y->data[temp_re_tmp].im = y->data[i].im - temp_im;
			y->data[i].re += temp_re;
			y->data[i].im += temp_im;
		}

		ju = 1;
		for (j = nRowsD4; j < nRowsD2; j += nRowsD4) {
			twid_re = costab->data[j];
			twid_im = sintab->data[j];
			i = ju;
			nRowsM2 = ju + ix;
			while (i < nRowsM2) {
				temp_re_tmp = i + iDelta;
				temp_re = twid_re * y->data[temp_re_tmp].re - twid_im * y->
					data[temp_re_tmp].im;
				temp_im = twid_re * y->data[temp_re_tmp].im + twid_im * y->
					data[temp_re_tmp].re;
				y->data[temp_re_tmp].re = y->data[i].re - temp_re;
				y->data[temp_re_tmp].im = y->data[i].im - temp_im;
				y->data[i].re += temp_re;
				y->data[i].im += temp_im;
				i += iy;
			}

			ju++;
		}

		nRowsD4 /= 2;
		iDelta = iy;
		iy += iy;
		ix -= iDelta;
	}
}

void dobluesteinfft_emx(const emxArray_real_T* x, int N2, int n1, const emxArray_real_T* costab, const emxArray_real_T* sintab, const emxArray_real_T* sintabinv, emxArray_creal_T* y)
{
	unsigned int sx_idx_1;
	emxArray_creal_T* wwc;
	int nChan;
	int nInt2m1;
	int ihi;
	int idx;
	int rt;
	int nInt2;
	int k;
	int ju;
	double nt_im;
	double nt_re;
	emxArray_creal_T* rwork;
	emxArray_creal_T* fy;
	emxArray_creal_T* fv;
	int nRowsM2;
	int nRowsD2;
	int nRowsD4;
	int b_k;
	int nt_re_tmp;
	int i;
	boolean_T tst;
	double twid_re;
	double fv_re;
	double twid_im;
	double fv_im;
	double wwc_im;
	double b_fv_re;
	sx_idx_1 = (unsigned int)x->size[1];
	emxInit_creal_T(&wwc, 1);
	nChan = (int)sx_idx_1;
	nInt2m1 = (n1 + n1) - 1;
	ihi = wwc->size[0];
	wwc->size[0] = nInt2m1;
	emxEnsureCapacity_creal_T(wwc, ihi);
	idx = n1;
	rt = 0;
	wwc->data[n1 - 1].re = 1.0;
	wwc->data[n1 - 1].im = 0.0;
	nInt2 = n1 << 1;
	for (k = 0; k <= n1 - 2; k++) {
		ju = ((1 + k) << 1) - 1;
		if (nInt2 - rt <= ju) {
			rt += ju - nInt2;
		}
		else {
			rt += ju;
		}

		nt_im = -3.1415926535897931 * (double)rt / (double)n1;
		if (nt_im == 0.0) {
			nt_re = 1.0;
			nt_im = 0.0;
		}
		else {
			nt_re = cos(nt_im);
			nt_im = sin(nt_im);
		}

		wwc->data[idx - 2].re = nt_re;
		wwc->data[idx - 2].im = -nt_im;
		idx--;
	}

	idx = 0;
	ihi = nInt2m1 - 1;
	for (k = ihi; k >= n1; k--) {
		wwc->data[k] = wwc->data[idx];
		idx++;
	}

	ihi = y->size[0] * y->size[1];
	y->size[0] = n1;
	y->size[1] = x->size[1];
	emxEnsureCapacity_creal_T(y, ihi);
	if (n1 > x->size[0]) {
		ihi = y->size[0] * y->size[1];
		emxEnsureCapacity_creal_T(y, ihi);
		ju = y->size[1];
		for (ihi = 0; ihi < ju; ihi++) {
			nInt2 = y->size[0];
			for (rt = 0; rt < nInt2; rt++) {
				y->data[rt + y->size[0] * ihi].re = 0.0;
				y->data[rt + y->size[0] * ihi].im = 0.0;
			}
		}
	}

	emxInit_creal_T(&rwork, 1);
	emxInit_creal_T(&fy, 1);
	emxInit_creal_T(&fv, 1);
	if (0 <= (int)sx_idx_1 - 1) {
		nRowsM2 = N2 - 2;
		nRowsD2 = N2 / 2;
		nRowsD4 = nRowsD2 / 2;
	}

	for (k = 0; k < nChan; k++) {
		rt = x->size[0];
		if (n1 < rt) {
			rt = n1;
		}

		ihi = rwork->size[0];
		rwork->size[0] = n1;
		emxEnsureCapacity_creal_T(rwork, ihi);
		if (n1 > x->size[0]) {
			ju = rwork->size[0];
			ihi = rwork->size[0];
			rwork->size[0] = ju;
			emxEnsureCapacity_creal_T(rwork, ihi);
			for (ihi = 0; ihi < ju; ihi++) {
				rwork->data[ihi].re = 0.0;
				rwork->data[ihi].im = 0.0;
			}
		}

		nInt2 = k * x->size[0];
		for (b_k = 0; b_k < rt; b_k++) {
			nt_re_tmp = (n1 + b_k) - 1;
			nt_re = wwc->data[nt_re_tmp].re;
			nt_im = wwc->data[nt_re_tmp].im;
			rwork->data[b_k].re = nt_re * x->data[nInt2];
			rwork->data[b_k].im = nt_im * -x->data[nInt2];
			nInt2++;
		}

		ihi = rt + 1;
		for (b_k = ihi; b_k <= n1; b_k++) {
			rwork->data[b_k - 1].re = 0.0;
			rwork->data[b_k - 1].im = 0.0;
		}

		nInt2 = rwork->size[0];
		nInt2m1 = N2;
		if (nInt2 < N2) {
			nInt2m1 = nInt2;
		}

		ihi = fy->size[0];
		fy->size[0] = N2;
		emxEnsureCapacity_creal_T(fy, ihi);
		if (N2 > rwork->size[0]) {
			ju = fy->size[0];
			ihi = fy->size[0];
			fy->size[0] = ju;
			emxEnsureCapacity_creal_T(fy, ihi);
			for (ihi = 0; ihi < ju; ihi++) {
				fy->data[ihi].re = 0.0;
				fy->data[ihi].im = 0.0;
			}
		}

		nInt2 = 0;
		ju = 0;
		rt = 0;
		for (i = 0; i <= nInt2m1 - 2; i++) {
			fy->data[rt] = rwork->data[nInt2];
			rt = N2;
			tst = true;
			while (tst) {
				rt >>= 1;
				ju ^= rt;
				tst = ((ju & rt) == 0);
			}

			rt = ju;
			nInt2++;
		}

		fy->data[rt] = rwork->data[nInt2];
		if (N2 > 1) {
			for (i = 0; i <= nRowsM2; i += 2) {
				nt_re = fy->data[i + 1].re;
				nt_im = fy->data[i + 1].im;
				fy->data[i + 1].re = fy->data[i].re - fy->data[i + 1].re;
				fy->data[i + 1].im = fy->data[i].im - fy->data[i + 1].im;
				fy->data[i].re += nt_re;
				fy->data[i].im += nt_im;
			}
		}

		rt = 2;
		nInt2 = 4;
		b_k = nRowsD4;
		nInt2m1 = 1 + ((nRowsD4 - 1) << 2);
		while (b_k > 0) {
			for (i = 0; i < nInt2m1; i += nInt2) {
				nt_re_tmp = i + rt;
				nt_re = fy->data[nt_re_tmp].re;
				nt_im = fy->data[nt_re_tmp].im;
				fy->data[nt_re_tmp].re = fy->data[i].re - nt_re;
				fy->data[nt_re_tmp].im = fy->data[i].im - nt_im;
				fy->data[i].re += nt_re;
				fy->data[i].im += nt_im;
			}

			ju = 1;
			for (idx = b_k; idx < nRowsD2; idx += b_k) {
				twid_re = costab->data[idx];
				twid_im = sintab->data[idx];
				i = ju;
				ihi = ju + nInt2m1;
				while (i < ihi) {
					nt_re_tmp = i + rt;
					nt_re = twid_re * fy->data[nt_re_tmp].re - twid_im * fy->
						data[nt_re_tmp].im;
					nt_im = twid_re * fy->data[nt_re_tmp].im + twid_im * fy->
						data[nt_re_tmp].re;
					fy->data[nt_re_tmp].re = fy->data[i].re - nt_re;
					fy->data[nt_re_tmp].im = fy->data[i].im - nt_im;
					fy->data[i].re += nt_re;
					fy->data[i].im += nt_im;
					i += nInt2;
				}

				ju++;
			}

			b_k /= 2;
			rt = nInt2;
			nInt2 += nInt2;
			nInt2m1 -= rt;
		}

		r2br_r2dit_trig_impl(wwc, N2, costab, sintab, fv);
		ihi = fy->size[0];
		emxEnsureCapacity_creal_T(fy, ihi);
		ju = fy->size[0];
		for (ihi = 0; ihi < ju; ihi++) {
			nt_re = fy->data[ihi].re;
			nt_im = fy->data[ihi].im;
			fv_re = fv->data[ihi].re;
			fv_im = fv->data[ihi].im;
			fy->data[ihi].re = nt_re * fv_re - nt_im * fv_im;
			fy->data[ihi].im = nt_re * fv_im + nt_im * fv_re;
		}

		r2br_r2dit_trig_impl(fy, N2, costab, sintabinv, fv);
		if (fv->size[0] > 1) {
			nt_re = 1.0 / (double)fv->size[0];
			ihi = fv->size[0];
			emxEnsureCapacity_creal_T(fv, ihi);
			ju = fv->size[0];
			for (ihi = 0; ihi < ju; ihi++) {
				fv->data[ihi].re *= nt_re;
				fv->data[ihi].im *= nt_re;
			}
		}

		idx = 0;
		ihi = wwc->size[0];
		for (b_k = n1; b_k <= ihi; b_k++) {
			nt_re = wwc->data[b_k - 1].re;
			fv_re = fv->data[b_k - 1].re;
			nt_im = wwc->data[b_k - 1].im;
			fv_im = fv->data[b_k - 1].im;
			twid_re = wwc->data[b_k - 1].re;
			twid_im = fv->data[b_k - 1].im;
			wwc_im = wwc->data[b_k - 1].im;
			b_fv_re = fv->data[b_k - 1].re;
			rwork->data[idx].re = nt_re * fv_re + nt_im * fv_im;
			rwork->data[idx].im = twid_re * twid_im - wwc_im * b_fv_re;
			idx++;
		}

		for (nInt2 = 0; nInt2 < n1; nInt2++) {
			y->data[nInt2 + y->size[0] * k] = rwork->data[nInt2];
		}
	}

	emxFree_creal_T(&fv);
	emxFree_creal_T(&fy);
	emxFree_creal_T(&rwork);
	emxFree_creal_T(&wwc);
}

void fft_emx(const emxArray_real_T *x, double varargin_1, emxArray_creal_T *y)
{
  int sz_idx_0;
  emxArray_real_T *costab1q;
  emxArray_real_T *costab;
  emxArray_real_T *sintab;
  emxArray_real_T *sintabinv;
  emxArray_creal_T *rwork;
  boolean_T useRadix2;
  int istart;
  int pmin;
  int pmax;
  int j;
  double temp_re;
  boolean_T exitg1;
  int ix;
  int pow2p;
  int k;
  unsigned int sx_idx_1;
  int nChan;
  int nRowsM2;
  int nRowsD2;
  int nRowsD4;
  int i;
  double temp_im;
  int temp_re_tmp;
  double twid_re;
  double twid_im;
  int ihi;
  sz_idx_0 = (int)varargin_1;
  emxInit_real_T(&costab1q, 2);
  emxInit_real_T(&costab, 2);
  emxInit_real_T(&sintab, 2);
  emxInit_real_T(&sintabinv, 2);
  emxInit_creal_T(&rwork, 1);
  if ((x->size[0] == 0) || (x->size[1] == 0) || (sz_idx_0 == 0)) {
    istart = y->size[0] * y->size[1];
    y->size[0] = sz_idx_0;
    y->size[1] = x->size[1];
    emxEnsureCapacity_creal_T(y, istart);
    if (sz_idx_0 > x->size[0]) {
      istart = y->size[0] * y->size[1];
      emxEnsureCapacity_creal_T(y, istart);
      pmin = y->size[1];
      for (istart = 0; istart < pmin; istart++) {
        pmax = y->size[0];
        for (ix = 0; ix < pmax; ix++) {
          y->data[ix + y->size[0] * istart].re = 0.0;
          y->data[ix + y->size[0] * istart].im = 0.0;
        }
      }
    }
  } else {
    useRadix2 = ((sz_idx_0 > 0) && ((sz_idx_0 & (sz_idx_0 - 1)) == 0));
    pmin = 1;
    if (useRadix2) {
      pmax = sz_idx_0;
    } else {
      if (sz_idx_0 > 0) {
        j = (sz_idx_0 + sz_idx_0) - 1;
        pmax = 31;
        if (j <= 1) {
          pmax = 0;
        } else {
          pmin = 0;
          exitg1 = false;
          while ((!exitg1) && (pmax - pmin > 1)) {
            ix = (pmin + pmax) >> 1;
            pow2p = 1 << ix;
            if (pow2p == j) {
              pmax = ix;
              exitg1 = true;
            } else if (pow2p > j) {
              pmax = ix;
            } else {
              pmin = ix;
            }
          }
        }

        pmin = 1 << pmax;
      }

      pmax = pmin;
    }

    temp_re = 6.2831853071795862 / (double)pmax;
    j = pmax / 2 / 2;
    istart = costab1q->size[0] * costab1q->size[1];
    costab1q->size[0] = 1;
    costab1q->size[1] = j + 1;
    emxEnsureCapacity_real_T(costab1q, istart);
    costab1q->data[0] = 1.0;
    pmax = j / 2 - 1;
    for (k = 0; k <= pmax; k++) {
      costab1q->data[1 + k] = cos(temp_re * (1.0 + (double)k));
    }

    istart = pmax + 2;
    ix = j - 1;
    for (k = istart; k <= ix; k++) {
      costab1q->data[k] = sin(temp_re * (double)(j - k));
    }

    costab1q->data[j] = 0.0;
    if (!useRadix2) {
      j = costab1q->size[1] - 1;
      pmax = (costab1q->size[1] - 1) << 1;
      istart = costab->size[0] * costab->size[1];
      costab->size[0] = 1;
      costab->size[1] = pmax + 1;
      emxEnsureCapacity_real_T(costab, istart);
      istart = sintab->size[0] * sintab->size[1];
      sintab->size[0] = 1;
      sintab->size[1] = pmax + 1;
      emxEnsureCapacity_real_T(sintab, istart);
      costab->data[0] = 1.0;
      sintab->data[0] = 0.0;
      istart = sintabinv->size[0] * sintabinv->size[1];
      sintabinv->size[0] = 1;
      sintabinv->size[1] = pmax + 1;
      emxEnsureCapacity_real_T(sintabinv, istart);
      for (k = 0; k < j; k++) {
        sintabinv->data[1 + k] = costab1q->data[(j - k) - 1];
      }

      istart = costab1q->size[1];
      for (k = istart; k <= pmax; k++) {
        sintabinv->data[k] = costab1q->data[k - j];
      }

      for (k = 0; k < j; k++) {
        costab->data[1 + k] = costab1q->data[1 + k];
        sintab->data[1 + k] = -costab1q->data[(j - k) - 1];
      }

      istart = costab1q->size[1];
      for (k = istart; k <= pmax; k++) {
        costab->data[k] = -costab1q->data[pmax - k];
        sintab->data[k] = -costab1q->data[k - j];
      }
    } else {
      j = costab1q->size[1] - 1;
      pmax = (costab1q->size[1] - 1) << 1;
      istart = costab->size[0] * costab->size[1];
      costab->size[0] = 1;
      costab->size[1] = pmax + 1;
      emxEnsureCapacity_real_T(costab, istart);
      istart = sintab->size[0] * sintab->size[1];
      sintab->size[0] = 1;
      sintab->size[1] = pmax + 1;
      emxEnsureCapacity_real_T(sintab, istart);
      costab->data[0] = 1.0;
      sintab->data[0] = 0.0;
      for (k = 0; k < j; k++) {
        costab->data[1 + k] = costab1q->data[1 + k];
        sintab->data[1 + k] = -costab1q->data[(j - k) - 1];
      }

      istart = costab1q->size[1];
      for (k = istart; k <= pmax; k++) {
        costab->data[k] = -costab1q->data[pmax - k];
        sintab->data[k] = -costab1q->data[k - j];
      }

      sintabinv->size[0] = 1;
      sintabinv->size[1] = 0;
    }

    if (useRadix2) {
      sx_idx_1 = (unsigned int)x->size[1];
      nChan = (int)sx_idx_1;
      istart = y->size[0] * y->size[1];
      y->size[0] = sz_idx_0;
      y->size[1] = x->size[1];
      emxEnsureCapacity_creal_T(y, istart);
      if (sz_idx_0 > x->size[0]) {
        istart = y->size[0] * y->size[1];
        emxEnsureCapacity_creal_T(y, istart);
        pmin = y->size[1];
        for (istart = 0; istart < pmin; istart++) {
          pmax = y->size[0];
          for (ix = 0; ix < pmax; ix++) {
            y->data[ix + y->size[0] * istart].re = 0.0;
            y->data[ix + y->size[0] * istart].im = 0.0;
          }
        }
      }

      if (0 <= (int)sx_idx_1 - 1) {
        nRowsM2 = sz_idx_0 - 2;
        nRowsD2 = sz_idx_0 / 2;
        nRowsD4 = nRowsD2 / 2;
      }

      for (k = 0; k < nChan; k++) {
        ix = k * x->size[0];
        pow2p = x->size[0];
        if (pow2p >= sz_idx_0) {
          pow2p = sz_idx_0;
        }

        istart = rwork->size[0];
        rwork->size[0] = sz_idx_0;
        emxEnsureCapacity_creal_T(rwork, istart);
        if (sz_idx_0 > x->size[0]) {
          pmin = rwork->size[0];
          istart = rwork->size[0];
          rwork->size[0] = pmin;
          emxEnsureCapacity_creal_T(rwork, istart);
          for (istart = 0; istart < pmin; istart++) {
            rwork->data[istart].re = 0.0;
            rwork->data[istart].im = 0.0;
          }
        }

        pmin = 0;
        pmax = 0;
        for (i = 0; i <= pow2p - 2; i++) {
          rwork->data[pmax].re = x->data[ix];
          rwork->data[pmax].im = 0.0;
          j = sz_idx_0;
          useRadix2 = true;
          while (useRadix2) {
            j >>= 1;
            pmin ^= j;
            useRadix2 = ((pmin & j) == 0);
          }

          pmax = pmin;
          ix++;
        }

        rwork->data[pmax].re = x->data[ix];
        rwork->data[pmax].im = 0.0;
        if (sz_idx_0 > 1) {
          for (i = 0; i <= nRowsM2; i += 2) {
            temp_re = rwork->data[i + 1].re;
            temp_im = rwork->data[i + 1].im;
            rwork->data[i + 1].re = rwork->data[i].re - rwork->data[i + 1].re;
            rwork->data[i + 1].im = rwork->data[i].im - rwork->data[i + 1].im;
            rwork->data[i].re += temp_re;
            rwork->data[i].im += temp_im;
          }
        }

        pmax = 2;
        pmin = 4;
        pow2p = nRowsD4;
        ix = 1 + ((nRowsD4 - 1) << 2);
        while (pow2p > 0) {
          for (i = 0; i < ix; i += pmin) {
            temp_re_tmp = i + pmax;
            temp_re = rwork->data[temp_re_tmp].re;
            temp_im = rwork->data[temp_re_tmp].im;
            rwork->data[temp_re_tmp].re = rwork->data[i].re - temp_re;
            rwork->data[temp_re_tmp].im = rwork->data[i].im - temp_im;
            rwork->data[i].re += temp_re;
            rwork->data[i].im += temp_im;
          }

          istart = 1;
          for (j = pow2p; j < nRowsD2; j += pow2p) {
            twid_re = costab->data[j];
            twid_im = sintab->data[j];
            i = istart;
            ihi = istart + ix;
            while (i < ihi) {
              temp_re_tmp = i + pmax;
              temp_re = twid_re * rwork->data[temp_re_tmp].re - twid_im *
                rwork->data[temp_re_tmp].im;
              temp_im = twid_re * rwork->data[temp_re_tmp].im + twid_im *
                rwork->data[temp_re_tmp].re;
              rwork->data[temp_re_tmp].re = rwork->data[i].re - temp_re;
              rwork->data[temp_re_tmp].im = rwork->data[i].im - temp_im;
              rwork->data[i].re += temp_re;
              rwork->data[i].im += temp_im;
              i += pmin;
            }

            istart++;
          }

          pow2p /= 2;
          pmax = pmin;
          pmin += pmin;
          ix -= pmax;
        }

        for (pmax = 0; pmax < sz_idx_0; pmax++) {
          y->data[pmax + y->size[0] * k] = rwork->data[pmax];
        }
      }
    } else {
      dobluesteinfft_emx(x, pmin, (int)varargin_1, costab, sintab, sintabinv, y);
    }
  }

  emxFree_creal_T(&rwork);
  emxFree_real_T(&sintabinv);
  emxFree_real_T(&sintab);
  emxFree_real_T(&costab);
  emxFree_real_T(&costab1q);
}