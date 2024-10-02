#include <stddef.h>
#include <stdlib.h>
#include "tmwtypes.h"
#include "Nelsonspec_types.h"

void dobluesteinfft_emx(const emxArray_real_T* x, int N2, int n1, const emxArray_real_T* costab, const emxArray_real_T* sintab, const emxArray_real_T* sintabinv, emxArray_creal_T* y);
void fft_emx(const emxArray_real_T *x, double varargin_1, emxArray_creal_T *y);