// NOTE: IN emxArray_ral_T, [0] is number of rows, [1] is numbers of column
// NOTE: when filling the emxArray_ral_T, it is filled column first, first row, then next rows

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <float.h>
#include <math.h>
#include "rt_nonfinite.h"
#include "Nelsonspec.h"
#include "fft.h"
#ifndef M_PI
#define M_PI (3.14159265358979323846)
#endif
emxArray_real_T* emxCreateND_real_T(int numDimensions, int* size)
{
	emxArray_real_T* emx;
	int numEl;
	int i;
	emxInit_real_T(&emx, numDimensions);
	numEl = 1;
	for (i = 0; i < numDimensions; i++) {
		numEl *= size[i];
		emx->size[i] = size[i];
	}

	emx->data = (double*)calloc((unsigned int)numEl, sizeof(double));
	emx->numDimensions = numDimensions;
	emx->allocatedSize = numEl;
	return emx;
}

emxArray_real_T* emxCreateWrapperND_real_T(double* data, int numDimensions, int* size)
{
	emxArray_real_T* emx;
	int numEl;
	int i;
	emxInit_real_T(&emx, numDimensions);
	numEl = 1;
	for (i = 0; i < numDimensions; i++) {
		numEl *= size[i];
		emx->size[i] = size[i];
	}

	emx->data = data;
	emx->numDimensions = numDimensions;
	emx->allocatedSize = numEl;
	emx->canFreeData = false;
	return emx;
}

emxArray_real_T* emxCreateWrapper_real_T(double* data, int rows, int cols)
{
	emxArray_real_T* emx;
	emxInit_real_T(&emx, 2);
	emx->size[0] = rows;
	emx->size[1] = cols;
	emx->data = data;
	emx->numDimensions = 2;
	emx->allocatedSize = rows * cols;
	emx->canFreeData = false;
	return emx;
}

emxArray_real_T* emxCreate_real_T(int rows, int cols)
{
	emxArray_real_T* emx;
	int numEl;
	emxInit_real_T(&emx, 2);
	emx->size[0] = rows;
	numEl = rows * cols;
	emx->size[1] = cols;
	emx->data = (double*)calloc((unsigned int)numEl, sizeof(double));
	emx->numDimensions = 2;
	emx->allocatedSize = numEl;
	return emx;
}

void emxDestroyArray_real_T(emxArray_real_T* emxArray)
{
	emxFree_real_T(&emxArray);
}

void emxInitArray_real_T(emxArray_real_T** pEmxArray, int numDimensions)
{
	emxInit_real_T(pEmxArray, numDimensions);
}

void emxEnsureCapacity_creal_T(emxArray_creal_T* emxArray, int oldNumel)
{
	int newNumel;
	int i;
	void* newData;
	if (oldNumel < 0) {
		oldNumel = 0;
	}

	newNumel = 1;
	for (i = 0; i < emxArray->numDimensions; i++) {
		newNumel *= emxArray->size[i];
	}

	if (newNumel > emxArray->allocatedSize) {
		i = emxArray->allocatedSize;
		if (i < 16) {
			i = 16;
		}

		while (i < newNumel) {
			if (i > 1073741823) {
				i = MAX_int32_T;
			}
			else {
				i <<= 1;
			}
		}

		newData = calloc((unsigned int)i, sizeof(creal_T));
		if (emxArray->data != NULL) {
			memcpy(newData, emxArray->data, sizeof(creal_T) * oldNumel);
			if (emxArray->canFreeData) {
				free(emxArray->data);
			}
		}

		emxArray->data = (creal_T*)newData;
		emxArray->allocatedSize = i;
		emxArray->canFreeData = true;
	}
}

void emxEnsureCapacity_real_T(emxArray_real_T* emxArray, int oldNumel)
{
	int newNumel;
	int i;
	void* newData;
	if (oldNumel < 0) {
		oldNumel = 0;
	}

	newNumel = 1;
	for (i = 0; i < emxArray->numDimensions; i++) {
		newNumel *= emxArray->size[i];
	}

	if (newNumel > emxArray->allocatedSize) {
		i = emxArray->allocatedSize;
		if (i < 16) {
			i = 16;
		}

		while (i < newNumel) {
			if (i > 1073741823) {
				i = MAX_int32_T;
			}
			else {
				i <<= 1;
			}
		}

		newData = calloc((unsigned int)i, sizeof(double));
		if (emxArray->data != NULL) {
			memcpy(newData, emxArray->data, sizeof(double) * oldNumel);
			if (emxArray->canFreeData) {
				free(emxArray->data);
			}
		}

		emxArray->data = (double*)newData;
		emxArray->allocatedSize = i;
		emxArray->canFreeData = true;
	}
}

void emxFree_creal_T(emxArray_creal_T** pEmxArray)
{
	if (*pEmxArray != (emxArray_creal_T*)NULL) {
		if (((*pEmxArray)->data != (creal_T*)NULL) && (*pEmxArray)->canFreeData) {
			free((*pEmxArray)->data);
		}

		free((*pEmxArray)->size);
		free(*pEmxArray);
		*pEmxArray = (emxArray_creal_T*)NULL;
	}
}

void emxFree_real_T(emxArray_real_T** pEmxArray)
{
	if (*pEmxArray != (emxArray_real_T*)NULL) {
		if (((*pEmxArray)->data != (double*)NULL) && (*pEmxArray)->canFreeData) {
			free((*pEmxArray)->data);
		}

		free((*pEmxArray)->size);
		free(*pEmxArray);
		*pEmxArray = (emxArray_real_T*)NULL;
	}
}

void emxInit_creal_T(emxArray_creal_T** pEmxArray, int numDimensions)
{
	emxArray_creal_T* emxArray;
	int i;
	*pEmxArray = (emxArray_creal_T*)malloc(sizeof(emxArray_creal_T));
	emxArray = *pEmxArray;
	emxArray->data = (creal_T*)NULL;
	emxArray->numDimensions = numDimensions;
	emxArray->size = (int*)malloc(sizeof(int) * numDimensions);
	emxArray->allocatedSize = 0;
	emxArray->canFreeData = true;
	for (i = 0; i < numDimensions; i++) {
		emxArray->size[i] = 0;
	}
}

void emxInit_real_T(emxArray_real_T** pEmxArray, int numDimensions)
{
	emxArray_real_T* emxArray;
	int i;
	*pEmxArray = (emxArray_real_T*)malloc(sizeof(emxArray_real_T));
	emxArray = *pEmxArray;
	emxArray->data = (double*)NULL;
	emxArray->numDimensions = numDimensions;
	emxArray->size = (int*)malloc(sizeof(int) * numDimensions);
	emxArray->allocatedSize = 0;
	emxArray->canFreeData = true;
	for (i = 0; i < numDimensions; i++) {
		emxArray->size[i] = 0;
	}
}

static double rt_hypotd_snf(double u0, double u1)
{
  double y;
  double a;
  double b;
  a = fabs(u0);
  b = fabs(u1);
  if (a < b) {
    a /= b;
    y = b * sqrt(a * a + 1.0);
  } else if (a > b) {
    b /= a;
    y = a * sqrt(b * b + 1.0);
  } else if (rtIsNaN(b)) {
    y = b;
  } else {
    y = a * 1.4142135623730951;
  }

  return y;
}

static double rt_remd_snf(double u0, double u1)
{
  double y;
  double b_u1;
  double q;
  if (rtIsNaN(u0) || rtIsNaN(u1) || rtIsInf(u0)) {
    y = rtNaN;
  } else if (rtIsInf(u1)) {
    y = u0;
  } else {
    if (u1 < 0.0) {
      b_u1 = ceil(u1);
    } else {
      b_u1 = floor(u1);
    }

    if ((u1 != 0.0) && (u1 != b_u1)) {
      q = fabs(u0 / u1);
      if (fabs(q - floor(q + 0.5)) <= DBL_EPSILON * q) {
        y = 0.0 * u0;
      } else {
        y = fmod(u0, u1);
      }
    } else {
      y = fmod(u0, u1);
    }
  }

  return y;
}

static double rt_roundd_snf(double u)
{
  double y;
  if (fabs(u) < 4.503599627370496E+15) {
    if (u >= 0.5) {
      y = floor(u + 0.5);
    } else if (u > -0.5) {
      y = u * 0.0;
    } else {
      y = ceil(u - 0.5);
    }
  } else {
    y = u;
  }

  return y;
}


void complexConjugate(const emxArray_creal_T* input, emxArray_creal_T* output) {
    int numRows = input->size[0];
    int numCols = input->size[1];

    // Set the size of the output matrix
    output->size[0] = numRows;
    output->size[1] = numCols;
    emxEnsureCapacity_creal_T(output, numRows * numCols);

    // Compute the complex conjugate of each element
    for (int i = 0; i < numRows; i++) {
        for (int j = 0; j < numCols; j++) {
            output->data[i + output->size[0] * j].re = input->data[i + input->size[0] * j].re;
            output->data[i + output->size[0] * j].im = -input->data[i + input->size[0] * j].im; // Conjugate the imaginary part
        }
    }
}

void elementWiseMultiply(const emxArray_creal_T* A, const emxArray_creal_T* B, emxArray_creal_T* C) {
    int numRows = A->size[0];
    int numCols = A->size[1];

    // Initialize the size of the output matrix
    //emxInit_creal_T(C, 2);
    C->size[0] = numRows;
    C->size[1] = numCols;
    emxEnsureCapacity_creal_T(C, numRows * numCols);

    // Perform element-wise multiplication
    for (int i = 0; i < numRows; i++) {
        for (int j = 0; j < numCols; j++) {
            C->data[i + C->size[0] * j].re = A->data[i + A->size[0] * j].re * B->data[i + B->size[0] * j].re -
                A->data[i + A->size[0] * j].im * B->data[i + B->size[0] * j].im;
            C->data[i + C->size[0] * j].im = A->data[i + A->size[0] * j].re * B->data[i + B->size[0] * j].im +
                A->data[i + A->size[0] * j].im * B->data[i + B->size[0] * j].re;
        }
    }
}

void elementWiseMultiplication(const emxArray_real_T* A, const emxArray_real_T* B, emxArray_real_T* result) {
    // Initialize the size of the output array
    //emxInit_real_T(result, 2);
    result->size[0] = A->size[0];
    result->size[1] = A->size[1];
    emxEnsureCapacity_real_T(result, result->size[0] * result->size[1]);

    // Perform element-wise multiplication
    for (int i = 0; i < A->size[0]; i++) {
        for (int j = 0; j < A->size[1]; j++) {
            result->data[i + result->size[0] * j] = A->data[i + A->size[0] * j] * B->data[i + B->size[0] * j];
        }
    }
}

void complexAngle(const emxArray_creal_T* input, emxArray_real_T* output) {
    int numRows = input->size[0];
    int numCols = input->size[1];

    // Initialize the size of the output matrix
    //emxInit_real_T(output, 2);
    output->size[0] = numRows;
    output->size[1] = numCols;
    emxEnsureCapacity_real_T(output, numRows * numCols);

    // Compute the angle for each complex element
    for (int i = 0; i < numRows; i++) {
        for (int j = 0; j < numCols; j++) {
            output->data[i + output->size[0] * j] = atan2(input->data[i + input->size[0] * j].im,
                input->data[i + input->size[0] * j].re);
        }
    }
}

void mod2Pi(const emxArray_real_T* input, emxArray_real_T* output) {
    int numRows = input->size[0];
    int numCols = input->size[1];

    // Initialize the size of the output matrix
    // emxInit_real_T(output, 2);
    output->size[0] = numRows;
    output->size[1] = numCols;
    emxEnsureCapacity_real_T(output, numRows * numCols);

    // Compute the modulus of each element with respect to 2*pi
    for (int i = 0; i < numRows; i++) {
        for (int j = 0; j < numCols; j++) {
            output->data[i + output->size[0] * j] = fmod(input->data[i + input->size[0] * j], 2 * M_PI);
        }
    }
}

void modNeg2Pi(const emxArray_real_T* input, emxArray_real_T* output) {
    int numRows = input->size[0];
    int numCols = input->size[1];

    // Initialize the size of the output matrix
    //emxInit_real_T(output, 2);
    output->size[0] = numRows;
    output->size[1] = numCols;
    emxEnsureCapacity_real_T(output, numRows * numCols);

    // Compute the modulus of each element with respect to -2*pi
    for (int j = 0; j < numCols; j++) {
        for (int i = 0; i < numRows; i++) {
            double val = input->data[i + input->size[0] * j];
            double neg2Pi = -2 * M_PI;
            output->data[i + output->size[0] * j] = fmod(val + neg2Pi, neg2Pi);
            //printf("in=%.4f \tout=%.4f\n", val, output->data[i + output->size[0] * j]);
            //printf("result=%.4f   neg=%.4f\n",fmod(-2.6820 + neg2Pi, neg2Pi), neg2Pi);

        }
    }
}


void scalarMultiply(const emxArray_real_T* input, double scalar, emxArray_real_T* output) {
    int numRows = input->size[0];
    int numCols = input->size[1];

    // Initialize the size of the output matrix
    //emxInit_real_T(output, 2);
    output->size[0] = numRows;
    output->size[1] = numCols;
    emxEnsureCapacity_real_T(output, numRows * numCols);

    // Perform element-wise multiplication of each element by the scalar value
    for (int i = 0; i < numRows; i++) {
        for (int j = 0; j < numCols; j++) {
            output->data[i + output->size[0] * j] = input->data[i + input->size[0] * j] * scalar;
        }
    }
}

void computeF(emxArray_real_T* f, double Fs, int lowindex, int highindex, double fftn) {
    int length = highindex - lowindex + 1;

    // Initialize the size of the output array
    //emxInit_real_T(&f, 2);
    f->size[0] = 1;
    f->size[1] = length;
    emxEnsureCapacity_real_T(f, length);

    // Compute each element of f
    for (int i = 0; i < length; i++) {
        f->data[i] = Fs * (lowindex - 1 + i) / fftn;
    }
}

void computeSTFTmagAbs(const emxArray_creal_T* STFTpos, emxArray_real_T* STFTmag) {
    // Initialize the size of the output array
    //emxInit_real_T(STFTmag, 2);
    //STFTmag->size[0] = STFTpos->size[0];
    //STFTmag->size[1] = STFTpos->size[1];
    //emxEnsureCapacity_real_T(STFTmag, STFTpos->size[0] * STFTpos->size[1]);

    // Compute the absolute value of each complex element
    for (int i = 0; i < STFTpos->size[0]; i++) {
        for (int j = 0; j < STFTpos->size[1]; j++) {
            double realPart = STFTpos->data[i + STFTpos->size[0] * j].re;
            double imagPart = STFTpos->data[i + STFTpos->size[0] * j].im;
            STFTmag->data[i + STFTmag->size[0] * j] = sqrt(realPart * realPart + imagPart * imagPart);
        }
    }
}

double maxVal(const emxArray_real_T* array) {
    double max = array->data[0];
    for (int i = 1; i < array->size[0] * array->size[1]; i++) {
        if (array->data[i] > max) {
            max = array->data[i];
        }
    }
    return max;
}

// Function to compute STFTmagDec = STFTmag ./ max(max(STFTmag))
void computeSTFTmagNorm(const emxArray_real_T* STFTmagAbs, emxArray_real_T* STFTmagNorm) {
    // Compute the maximum value of STFTmag
    double maxValSTFTmagAbs = maxVal(STFTmagAbs);
    //printf("maxVal: %.4f", maxValSTFTmagAbs);
    
    // Compute STFTmagDec = STFTmag ./ maxValSTFTmag
    for (int i = 0; i < STFTmagAbs->size[0]; i++) {
        for (int j = 0; j < STFTmagAbs->size[1]; j++) {
            STFTmagNorm->data[i + STFTmagNorm->size[0] * j] = STFTmagAbs->data[i + STFTmagAbs->size[0] * j] / maxValSTFTmagAbs;
        }
    }
}

void computeLogMagnitude(emxArray_real_T* STFTmagNorm, emxArray_real_T* STFTmag) {
    // Compute 20*log10(STFTmag) for each element
    for (int i = 0; i < STFTmagNorm->size[0]; i++) {
        for (int j = 0; j < STFTmagNorm->size[1]; j++) {
            double val = STFTmagNorm->data[i + STFTmagNorm->size[0] * j];
            STFTmag->data[i + STFTmag->size[0] * j] = 20 * log10(val);
        }
    }
}

void hanningWindow(double len, emxArray_real_T* window) {
    // Compute the Hanning window
    for (int i = 0; i < len; i++) {
        window->data[i] = 0.5 * (1 - cos(2 * M_PI * i / (len - 1)));
        printf("%.4f\n", window->data[i]);
    }
}

void generateOffset(double step, double len, double win_size, emxArray_real_T* offset) {
    // Compute the length of the offset array
    double length = (len - win_size) / step + 1;

    // Initialize the size of the output array
    //emxInit_real_T(&offset, 2);
    offset->size[0] = 1;
    offset->size[1] = (int)length;
    emxEnsureCapacity_real_T(offset, (int)length);

    //printf("generate %d %d\n", offset->size[1], offset->size[0]);

    // Compute the offset values
    double value = 0;
    for (int i = 0; i < (int)length; i++) {
        offset->data[i] = value;
        value += step;
    }
}

void replicateArray(const emxArray_real_T* source, int numCopies, emxArray_real_T* destination) {
    destination->size[0] = source->size[0];
    destination->size[1] = numCopies;
    emxEnsureCapacity_real_T(destination, numCopies * source->size[0]);
    //printf("%d %d\n", destination->size[0], destination->size[1]);
    for (int i = 0; i < source->size[0]; i++) {
        for (int j = 0; j < numCopies; j++) {
            destination->data[i + destination->size[0] * j] = source->data[i];
        }
    }
}

void extractElements(const emxArray_real_T* signal, const emxArray_real_T* idx, emxArray_real_T* idxSignal) {
    // Initialize the size of the output array
    idxSignal->size[0] = idx->size[0];
    idxSignal->size[1] = idx->size[1];
    emxEnsureCapacity_real_T(idxSignal, idx->size[0] * idx->size[1]);

    // Extract elements from signal using indices in idx
    for (int i = 0; i < idx->size[0]; i++) {
        for (int j = 0; j < idx->size[1]; j++) {
            int index = (int)idx->data[i + idx->size[0] * j] - 1; // MATLAB uses 1-based indexing
            idxSignal->data[i + idxSignal->size[0] * j] = signal->data[index];
        }
    }
}

void extractElementsPlot(const emxArray_real_T* STFTmag, const emxArray_real_T* plot_these, emxArray_real_T* STFTplot) {
    // Extract elements from STFTmag using indices in plot_these
    for (int i = 0; i < plot_these->size[0]; i++) {
        int index = (int)plot_these->data[i]; // MATLAB uses 1-based indexing
        STFTplot->data[i] = STFTmag->data[index];
    }
}

// Function to find indices of elements satisfying the given conditions
void findIndices(emxArray_real_T* STFTmag, emxArray_real_T* CIFpos, emxArray_real_T* tremap, double clip, double low, double high, emxArray_real_T* plot_these, double t1, double t2) {
    // Initialize the size of the output array
    int count = 0; // Count of elements satisfying conditions
    
    // Iterate over elements and apply conditions
    for (int j = 0; j < STFTmag->size[1]; j++) {
        for (int i = 0; i < STFTmag->size[0]; i++) {
            if (STFTmag->data[i + STFTmag->size[0] * j] >= clip && low <= CIFpos->data[i + CIFpos->size[0] * j] && CIFpos->data[i + CIFpos->size[0] * j] <= high && t1 <= tremap->data[i + tremap->size[0] * j] && tremap->data[i + tremap->size[0] * j] <= t2) {
                plot_these->size[0]++;
                emxEnsureCapacity_real_T(plot_these, plot_these->size[0]);
                plot_these->data[count] = (int)(i + CIFpos->size[0] * j);
                count++;
            }
        }
    }
}

void Nelsonspec(emxArray_real_T* b_signal, double Fs, double win_size, double
                overlap, double fftn, double low, double high, double clip,
                emxArray_real_T *STFTmag, emxArray_real_T* CIFPos_mag, emxArray_real_T* time, emxArray_real_T* freq)
{
    //---------------------------------------------------variable KALEEM
  emxArray_real_T *y;
  int i0;
  int loop_ub;
  emxArray_real_T *window;
  int nx;
  int k;
  double d;
  emxArray_real_T *offset;
  emxArray_real_T* time_offset;
  double ndbl;
  double cdiff;
  double apnd;
  int i1;
  emxArray_real_T *b_y;
  emxArray_real_T *r0;

  emxArray_real_T* r0_cif;
  emxArray_real_T* r1_cif;
  emxArray_creal_T* STFTfreqdelpos;
  emxArray_creal_T* STFTfreqdelpos2;
  emxArray_creal_T* conjSTFTdelpos;
  emxArray_creal_T* conjSTFTfreqdelpos;
  emxArray_creal_T* STFTdelpos;
  emxArray_creal_T* C;
  emxArray_real_T* angleC;
  emxArray_real_T* angleL;
  emxArray_real_T* argC;
  emxArray_real_T* argL;
  emxArray_real_T* CIFpos;
  emxArray_real_T* LGDpos;
  emxArray_real_T* time_t;
  emxArray_creal_T* L;

  emxArray_real_T *r1;
  emxArray_creal_T *STFT;
  double d0;
  emxArray_creal_T *STFTpos;
  emxArray_creal_T* STFTdelpos2;
  int m;
  unsigned int unnamed_idx_1;
  boolean_T exitg1;
  (void)clip;
  //---------------------------------------------------variable KALEEM

  int len = b_signal->size[0] * b_signal->size[1];
  printf("signal size: %d\n", len);


  //emxInit_real_T(&window, 1);
  //window->size[0] = win_size;
  //emxEnsureCapacity_real_T(window, win_size);
  //hanningWindow(win_size, window);
  //FILE* fpWindow = fopen("window2.txt", "wb"); // store file KALEEM
  //for (int i = 0; i < window->size[0]; i++) {
  //    fprintf(fpWindow, "%.4f\n", window->data[i]); // store file KALEEM
  //}
  //fclose(fpWindow); // store file KALEEM


  //--------------------------------------------------------------------------- Hanning Start KALEEM
  emxInit_real_T(&y, 2);
  if (rtIsNaN(win_size)) {
    i0 = y->size[0] * y->size[1];
    y->size[0] = 1;
    y->size[1] = 1;
    emxEnsureCapacity_real_T(y, i0);
    y->data[0] = rtNaN;
  } else if (win_size < 1.0) {
    y->size[0] = 1;
    y->size[1] = 0;
  } else if (rtIsInf(win_size) && (1.0 == win_size)) {
    i0 = y->size[0] * y->size[1];
    y->size[0] = 1;
    y->size[1] = 1;
    emxEnsureCapacity_real_T(y, i0);
    y->data[0] = rtNaN;
  } else {
    i0 = y->size[0] * y->size[1];
    y->size[0] = 1;
    loop_ub = (int)floor(win_size - 1.0);
    y->size[1] = loop_ub + 1;
    emxEnsureCapacity_real_T(y, i0);
    for (i0 = 0; i0 <= loop_ub; i0++) {
      y->data[i0] = 1.0 + (double)i0;
    }
  }
 
  emxInit_real_T(&window, 1);
  i0 = window->size[0];
  window->size[0] = y->size[1];
  emxEnsureCapacity_real_T(window, i0);
  loop_ub = y->size[1];
  for (i0 = 0; i0 < loop_ub; i0++) {
    window->data[i0] = 6.2831853071795862 * y->data[i0] / (win_size + 1.0);
  }

  nx = window->size[0];
  for (k = 0; k < nx; k++) {
    window->data[k] = cos(window->data[k]);
  }

  FILE* fpWindow = fopen("window.txt", "wb"); // store file KALEEM
  i0 = window->size[0];
  emxEnsureCapacity_real_T(window, i0);
  loop_ub = window->size[0];
  for (i0 = 0; i0 < loop_ub; i0++) {
    window->data[i0] = 0.5 * (1.0 - window->data[i0]);
    fprintf(fpWindow, "%.4f\n", window->data[i0]); // store file KALEEM
    //printf("%.4f\n", window->data[i0]);
  }
  fclose(fpWindow); // store file KALEEM
  
  //--------------------------------------------------------------------------- Hanning End KALEEM, Result OK

  
  double step = win_size - overlap;
  //--------------------------------------------------------------------------- step KALEEM, Result OK
  
  //printf("DATA: %.4f ; %d ; %.4f\n", step, len, win_size);
  emxInit_real_T(&offset, 2);
  generateOffset(step, len, win_size, offset);
  //printf("offset size: %d\n", offset->size[1]);
  FILE* fpOffset2 = fopen("offset2.txt", "wb"); // store file KALEEM
  for (int i = 0; i < offset->size[0] * offset->size[1]; i++) {
      fprintf(fpOffset2, "%.4f\n", offset->data[i]); // store file KALEEM
  }
  fclose(fpOffset2); // store file KALEEM
  //--------------------------------------------------------------------------- offset KALEEM, Result OK

  //printf("offset size: %d\n", offset->size[1]);
  emxArray_real_T* WW;
  emxInit_real_T(&WW, 2);
  replicateArray(window, offset->size[1], WW);
  FILE* fpWW = fopen("WW.txt", "wb"); // store file KALEEM
  for (int i = 0; i < WW->size[1]; i++) {
      for (int j = 0; j < WW->size[0]; j++) {
          fprintf(fpWW, "%.4f\n", WW->data[j + WW->size[0] * i]); // store file KALEEM
      }
  }
  fclose(fpWW); // store file KALEEM

  emxArray_real_T* idx;
  emxInit_real_T(&idx, 2);
  idx->size[0] = win_size;
  idx->size[1] = offset->size[1];
  emxEnsureCapacity_real_T(idx, win_size * offset->size[1]);
  for (int i = 0; i < idx->size[0]; i++) {
      for (int j = 0; j < idx->size[1]; j++) {
          idx->data[i + idx->size[0] * j] = i + 1 + offset->data[j];
      }
  }
  FILE* fpidx = fopen("fpidx.txt", "wb"); // store file KALEEM
  for (int i = 0; i < idx->size[1]; i++) {
      for (int j = 0; j < idx->size[0]; j++) {
          fprintf(fpidx, "%.4f\n", idx->data[j + idx->size[0] * i]); // store file KALEEM
      }
  }
  fclose(fpidx); // store file KALEEM



  
  // Extract elements from signal using indices in idx
  emxArray_real_T* idxSignal;
  emxInit_real_T(&idxSignal, 2);
  extractElements(b_signal, idx, idxSignal);
  FILE* fpidxSignal = fopen("fpidxSignal.txt", "wb"); // store file KALEEM
  for (int i = 0; i < idxSignal->size[1]; i++) {
      for (int j = 0; j < idxSignal->size[0]; j++) {
          fprintf(fpidxSignal, "%.4f\n", idxSignal->data[j + idxSignal->size[0] * i]); // store file KALEEM
      }
  }
  fclose(fpidxSignal); // store file KALEEM



  // Extract elements from signal using indices in idx
  emxArray_real_T* idxPlusOne;
  emxInit_real_T(&idxPlusOne, 2);
  idxPlusOne->size[0] = idx->size[0];
  idxPlusOne->size[1] = idx->size[1];
  emxEnsureCapacity_real_T(idxPlusOne, idx->size[0] * idx->size[1]);
  for (int i = 0; i < idxPlusOne->size[0]; i++) {
      for (int j = 0; j < idxPlusOne->size[1]; j++) {
          idxPlusOne->data[i + idxPlusOne->size[0] * j] = idx->data[i + idx->size[0] * j] + 1;
      }
  }
  FILE* fpidxPlusOne = fopen("fpidxPlusOne.txt", "wb"); // store file KALEEM
  for (int i = 0; i < idxPlusOne->size[1]; i++) {
      for (int j = 0; j < idxPlusOne->size[0]; j++) {
          fprintf(fpidxPlusOne, "%.4f\n", idxPlusOne->data[j + idxPlusOne->size[0] * i]); // store file KALEEM
      }
  }
  fclose(fpidxPlusOne); // store file KALEEM


  emxArray_real_T* idxSignalPlusOne;
  emxInit_real_T(&idxSignalPlusOne, 2);
  extractElements(b_signal, idxPlusOne, idxSignalPlusOne);
  FILE* fpidxSignalPlusOne = fopen("fpidxSignalPlusOne.txt", "wb"); // store file KALEEM
  for (int i = 0; i < idxSignalPlusOne->size[1]; i++) {
      for (int j = 0; j < idxSignalPlusOne->size[0]; j++) {
          fprintf(fpidxSignalPlusOne, "%.4f\n", idxSignalPlusOne->data[j + idxSignalPlusOne->size[0] * i]); // store file KALEEM
      }
  }
  fclose(fpidxSignalPlusOne); // store file KALEEM


  emxArray_real_T* S;
  emxInit_real_T(&S, 2);
  elementWiseMultiplication(idxSignalPlusOne, WW, S);
  FILE* fpS = fopen("fpS.txt", "wb"); // store file KALEEM
  for (int i = 0; i < S->size[1]; i++) {
      for (int j = 0; j < S->size[0]; j++) {
          fprintf(fpS, "%.4f\n", S->data[j + S->size[0] * i]); // store file KALEEM
      }
  }
  fclose(fpS); // store file KALEEM

  

  emxArray_real_T* Sdel;
  emxInit_real_T(&Sdel, 2);
  elementWiseMultiplication(idxSignal, WW, Sdel);
  FILE* fpSdel = fopen("fpSdel.txt", "wb"); // store file KALEEM
  for (int i = 0; i < Sdel->size[1]; i++) {
      for (int j = 0; j < Sdel->size[0]; j++) {
          fprintf(fpSdel, "%.4f\n", Sdel->data[j + Sdel->size[0] * i]); // store file KALEEM
      }
  }
  fclose(fpSdel); // store file KALEEM





  if (window->size[0] < 1) {
    y->size[0] = 1;
    y->size[1] = 0;
  } else {
    i0 = window->size[0];
    i1 = y->size[0] * y->size[1];
    y->size[0] = 1;
    loop_ub = (int)((double)i0 - 1.0);
    y->size[1] = loop_ub + 1;
    emxEnsureCapacity_real_T(y, i1);
    for (i0 = 0; i0 <= loop_ub; i0++) {
      y->data[i0] = 1.0 + (double)i0;
    }
  }

  emxInit_real_T(&b_y, 2);
  k = offset->size[1];
  nx = window->size[0];
  i0 = b_y->size[0] * b_y->size[1];
  b_y->size[0] = y->size[1];
  b_y->size[1] = k;
  emxEnsureCapacity_real_T(b_y, i0);
  loop_ub = y->size[1];
  for (i0 = 0; i0 < loop_ub; i0++) {
    for (i1 = 0; i1 < k; i1++) {
      b_y->data[i0 + b_y->size[0] * i1] = y->data[i0];
    }
  }

  emxInit_real_T(&r0, 2);
  emxInit_real_T(&r0_cif, 2);
  i0 = r0->size[0] * r0->size[1];
  r0->size[0] = nx;
  r0->size[1] = offset->size[1];
  // do for r0_clif as well
  r0_cif->size[0] = nx;
  r0_cif->size[1] = offset->size[1];
  emxEnsureCapacity_real_T(r0, i0);
  emxEnsureCapacity_real_T(r0_cif, i0);
  for (i0 = 0; i0 < nx; i0++) {
    loop_ub = offset->size[1];
    for (i1 = 0; i1 < loop_ub; i1++) {
      r0->data[i0 + r0->size[0] * i1] = offset->data[i1];
      r0_cif->data[i0 + r0_cif->size[0] * i1] = offset->data[i1];
    }
  }

  emxInit_real_T(&r1, 2);
  emxInit_real_T(&r1_cif, 2);
  i0 = r1->size[0] * r1->size[1];
  r1->size[0] = b_y->size[0];
  r1->size[1] = b_y->size[1];

  r1_cif->size[0] = b_y->size[0];
  r1_cif->size[1] = b_y->size[1];
  // do r1_cif as well
  emxEnsureCapacity_real_T(r1, i0);
  emxEnsureCapacity_real_T(r1_cif, i0);
  loop_ub = b_y->size[0] * b_y->size[1];
  for (i0 = 0; i0 < loop_ub; i0++) {
    r1->data[i0] = b_signal->data[(int)((b_y->data[i0] + r0->data[i0]) + 1.0) - 1];
    r1_cif->data[i0] = b_signal->data[(int)((b_y->data[i0] + r0_cif->data[i0])) - 1];
  }

  k = offset->size[1];
  i0 = b_y->size[0] * b_y->size[1];
  b_y->size[0] = window->size[0];
  b_y->size[1] = k;
  emxEnsureCapacity_real_T(b_y, i0);
  loop_ub = window->size[0];
#if 0
  //emxFree_real_T(&offset);
#else
  //emxFree_real_T(&offset);
#endif
  for (i0 = 0; i0 < loop_ub; i0++) {
    for (i1 = 0; i1 < k; i1++) {
      b_y->data[i0 + b_y->size[0] * i1] = window->data[i0];
    }
  }

  emxFree_real_T(&window);
  i0 = r0->size[0] * r0->size[1];
  r0->size[0] = r1->size[0];
  r0->size[1] = r1->size[1];
  emxEnsureCapacity_real_T(r0, i0);
  loop_ub = r1->size[0] * r1->size[1];
  for (i0 = 0; i0 < loop_ub; i0++) {
    r0->data[i0] = r1->data[i0] * b_y->data[i0];
    r0_cif->data[i0] = r1_cif->data[i0] * b_y->data[i0];
  }
#if 0
  //emxFree_real_T(&b_y);
#else
  emxFree_real_T(&b_y);
#endif
  emxFree_real_T(&r1);
  emxInit_creal_T(&STFT, 2);
  emxInit_creal_T(&STFTfreqdelpos, 2);
  emxInit_creal_T(&STFTdelpos, 2);
  emxInit_real_T(&CIFpos, 2);
  emxInit_real_T(&LGDpos, 2);
  emxInit_real_T(&time_t, 2);

  ;
  
  FILE* fpr0_cif = fopen("fpr0_cif.txt", "wb"); // store file KALEEM
  for (int i = 0; i < r0_cif->size[1]; i++) {
      for (int j = 0; j < r0_cif->size[0]; j++) {
          fprintf(fpr0_cif, "%.4f\n", r0_cif->data[j + r0_cif->size[0] * i]); // store file KALEEM
      }
  }
  fclose(fpr0_cif); // store file KALEEM


  fft_emx(S, fftn, STFT); // STFT = fft(S,fftn) in Matlab?
  
  //printf("%d, %d\n", r0->size[0], r0->size[1]);
  FILE* fpr0 = fopen("fpr0.txt", "wb"); // store file KALEEM
  for (int j = 0; j < r0->size[0]; j++) {
      for (int i = 0; i < r0->size[1]; i++) {
          //printf("%.4f\t", r0->data[i + r0->size[0] * j]); // Accessing the element at row i, column j
          fprintf(fpr0, "%.4f\n", r0->data[i + r0->size[0] * j]); // store file KALEEM
      }
  }
  fclose(fpr0); // store file KALEEM


  fft_emx(r0_cif, fftn, STFTfreqdelpos);
  fft_emx(Sdel, fftn, STFTdelpos);

  


  // dummy calculation, so that proper memory is allocated for CIF pos
  emxFree_real_T(&r0);


  //validate
  int s1 = STFT->size[0]; // rows
  int s2 = STFT->size[1]; // column
  //--------------------------------------------------- checking STFT content start, Result OK
    //printf("s1: %d, s2: %d\n", s1, s2);
    FILE* fpSTFTreal = fopen("fpSTFTreal.txt", "wb"); // store file KALEEM
    for (int j = 0; j < s2; j++) {
        for (int i = 0; i < s1; i++) {
            //printf("%.4f\t %.4f\n", STFT->data[i + s1 * j].re, STFT->data[i + s1 * j].im); // Accessing the element at row i, column j
            fprintf(fpSTFTreal, "%.4f\n", STFT->data[i + s1 * j].re); // store file KALEEM
        }
    }
    fclose(fpSTFTreal); // store file KALEEM
    FILE* fpSTFTimag = fopen("fpSTFTimag.txt", "wb"); // store file KALEEM
    for (int j = 0; j < s2; j++) {
        for (int i = 0; i < s1; i++) {
            //printf("%.4f\t %.4f\n", STFT->data[i + s1 * j].re, STFT->data[i + s1 * j].im); // Accessing the element at row i, column j
            fprintf(fpSTFTimag, "%.4f\n", STFT->data[i + s1 * j].im); // store file KALEEM -> NOTE: IN MATLAB, IMAGINARY CAN BE -0, IN C IS 0
        }
    }
    fclose(fpSTFTimag); // store file KALEEM
    //--------------------------------------------------- checking STFT content end, Result OK


    int s3 = STFTdelpos->size[0]; // rows
    int s4 = STFTdelpos->size[1]; // column
    //--------------------------------------------------- checking STFT content start, Result OK
    //printf("s3: %d, s4: %d\n", s3, s4);
    FILE* fpSTFTdelreal = fopen("fpSTFTdelreal.txt", "wb"); // store file KALEEM
    for (int j = 0; j < s4; j++) {
        for (int i = 0; i < s3; i++) {
            //printf("%.4f\t %.4f\n", STFT->data[i + s1 * j].re, STFT->data[i + s1 * j].im); // Accessing the element at row i, column j
            fprintf(fpSTFTdelreal, "%.4f\n", STFTdelpos->data[i + s3 * j].re); // store file KALEEM
        }
    }
    fclose(fpSTFTdelreal); // store file KALEEM
    FILE* fpSTFTdelimag = fopen("fpSTFTdelimag.txt", "wb"); // store file KALEEM
    for (int j = 0; j < s4; j++) {
        for (int i = 0; i < s3; i++) {
            //printf("%.4f\t %.4f\n", STFT->data[i + s1 * j].re, STFT->data[i + s1 * j].im); // Accessing the element at row i, column j
            fprintf(fpSTFTdelimag, "%.4f\n", STFTdelpos->data[i + s3 * j].im); // store file KALEEM -> NOTE: IN MATLAB, IMAGINARY CAN BE -0, IN C IS 0
        }
    }
    fclose(fpSTFTdelimag); // store file KALEEM
    //--------------------------------------------------- checking STFT content end, Result OK

  //printf("DEBUG\n");
  int t = 0;
  /*for (int j = 0; j < s2; j++)
  {
      if (j == 0)
          t = s1 - 1;
      else
          t = ((s1 - 1) * (j + 1)) + j;
      
      STFTfreqdelpos->data[j].re = STFT->data[t].re;
      STFTfreqdelpos->data[j].im = STFT->data[t].im;
      //printf("%d\t %.4f\t %.4f\n", j, STFTfreqdelpos->data[j].re, STFTfreqdelpos->data[j].im);
  }

  //printf("next line\n");
  int cnt = s2;
  for (int j = 0; j < s1 - 1; j++)
  {
      for (int i = 0; i < s2; i++)
      {
          t = s1 * i + j;
          STFTfreqdelpos->data[cnt].re = STFT->data[t].re;
          STFTfreqdelpos->data[cnt].im = STFT->data[t].im;
          //printf("%d\t %lf\t %lf\n", i, STFTfreqdelpos->data[cnt].re, STFTfreqdelpos->data[cnt].im);
          cnt++;
      }
  }*/

  //--------------------------------------------------- checking STFT content start, Result OK
  // Copy elements from STFT to STFTfreqdel
  for (int j = 0; j < STFT->size[1]; j++) {
      for (int i = 0; i < ((int)fftn)-1; i++) { // Cast fftn to int
          STFTfreqdelpos->data[i + 1 + (int)fftn * j].re = STFT->data[i + (int)fftn * j].re; // Copy real part from STFT(fftn,:) to STFTfreqdel
          STFTfreqdelpos->data[i + 1 + (int)fftn * j].im = STFT->data[i + (int)fftn * j].im; // Copy imaginary part from STFT(fftn,:) to STFTfreqdel
      }
      for (int i = (int)fftn-1; i < STFT->size[0]; i++) { // Cast fftn to int
          STFTfreqdelpos->data[(STFT->size[0] - 1 - i) + (int)fftn * j].re = STFT->data[i + (int)fftn * j].re; // Copy real part from STFT(1:fftn-1,:) to STFTfreqdel
          STFTfreqdelpos->data[(STFT->size[0] - 1 - i) + (int)fftn * j].im = STFT->data[i + (int)fftn * j].im; // Copy imaginary part from STFT(1:fftn-1,:) to STFTfreqdel
      }
  }

  int s5 = STFTfreqdelpos->size[0]; // rows
  int s6 = STFTfreqdelpos->size[1]; // column
  //printf("s5: %d, s6: %d\n", s5, s6);
  FILE* fpSTFTfreqdelposreal = fopen("fpSTFTfreqdelposreal.txt", "wb"); // store file KALEEM
  for (int j = 0; j < s6; j++) {
      for (int i = 0; i < s5; i++) {
          //printf("%.4f\t %.4f\n", STFT->data[i + s1 * j].re, STFT->data[i + s1 * j].im); // Accessing the element at row i, column j
          fprintf(fpSTFTfreqdelposreal, "%.4f\n", STFTfreqdelpos->data[i + s5 * j].re); // store file KALEEM
      }
  }
  fclose(fpSTFTfreqdelposreal); // store file KALEEM
  FILE* fpSTFTfreqdelposimag = fopen("fpSTFTfreqdelposimag.txt", "wb"); // store file KALEEM
  for (int j = 0; j < s6; j++) {
      for (int i = 0; i < s5; i++) {
          //printf("%.4f\t %.4f\n", STFT->data[i + s1 * j].re, STFT->data[i + s1 * j].im); // Accessing the element at row i, column j
          fprintf(fpSTFTfreqdelposimag, "%.4f\n", STFTfreqdelpos->data[i + s5 * j].im); // store file KALEEM -> NOTE: IN MATLAB, IMAGINARY CAN BE -0, IN C IS 0
      }
  }
  fclose(fpSTFTfreqdelposimag); // store file KALEEM
  //--------------------------------------------------- checking STFT content end, Result OK

  //if (rt_remd_snf(fftn, 2.0) == 1.0) {
  if(fmod(fftn, 2.0) == 1.0){
    d0 = (fftn - 1.0) / 2.0;
  } else {
    d0 = fftn / 2.0;
  }

  //printf("%f", d0); // Matlab ret_n result OK

  cdiff = Fs * (d0 - 1.0) / fftn; // matlab> high > Fs * (ret_n-1)/fftn
  if (high > cdiff) {
    high = cdiff;
  }

  //cdiff = rt_roundd_snf(low / Fs * fftn);
  cdiff = round(low / Fs * fftn);
  if (cdiff == 0.0) {
    cdiff = 1.0; // Matlab> lowindex = 1;
  }

  //printf("%f", cdiff); // Matlab lowindes result OK

  //ndbl = rt_roundd_snf(high / Fs * fftn);
  ndbl = round(high / Fs * fftn);
  
  //printf("%f", ndbl); // Matlab highindex result OK

  if (cdiff > ndbl) {
    i0 = -1;
    i1 = -1;
  } else {
    i0 = (int)cdiff - 2;
    i1 = (int)ndbl - 1;
  }

  //--------------------------------------------------- checking STFT content start, Result OK
  emxInit_creal_T(&STFTpos, 2);
  loop_ub = STFT->size[1];
  nx = STFTpos->size[0] * STFTpos->size[1];
  m = i1 - i0;
  STFTpos->size[0] = m;
  STFTpos->size[1] = loop_ub;
  emxEnsureCapacity_creal_T(STFTpos, nx);

  FILE* fpSTFTposreal = fopen("fpSTFTposreal.txt", "wb"); // store file KALEEM
  FILE* fpSTFTposimag = fopen("fpSTFTposimag.txt", "wb"); // store file KALEEM
  for (i1 = 0; i1 < loop_ub; i1++) {
    for (nx = 0; nx < m; nx++) {
      STFTpos->data[nx + STFTpos->size[0] * i1] = STFT->data[((i0 + nx) + STFT->size[0] * i1) + 1];
      fprintf(fpSTFTposreal, "%.4f\n", STFTpos->data[nx + STFTpos->size[0] * i1].re); // store file KALEEM
      fprintf(fpSTFTposimag, "%.4f\n", STFTpos->data[nx + STFTpos->size[0] * i1].im); // store file KALEEM
    }
  }
  fclose(fpSTFTposreal); // store file KALEEM
  fclose(fpSTFTposimag); // store file KALEEM

  //--------------------------------------------------- checking STFT content end, Result OK


  ///////////////// istirahat
  //--------------------------------------------------- checking STFT content start, Result OK
  if (cdiff > ndbl) {
      i0 = -1;
      i1 = -1;
  }
  else {
      i0 = (int)cdiff - 2;
      i1 = (int)ndbl - 1;
  }

  emxInit_creal_T(&STFTdelpos2, 2);
  loop_ub = STFTdelpos->size[1];
  nx = STFTdelpos2->size[0] * STFTdelpos2->size[1];
  m = i1 - i0;
  STFTdelpos2->size[0] = m;
  STFTdelpos2->size[1] = loop_ub;
  emxEnsureCapacity_creal_T(STFTdelpos2, nx);

  FILE* fpSTFTdelpos2real = fopen("fpSTFTdelpos2real.txt", "wb"); // store file KALEEM
  FILE* fpSTFTdelpos2imag = fopen("fpSTFTdelpos2imag.txt", "wb"); // store file KALEEM
  for (i1 = 0; i1 < loop_ub; i1++) {
      for (nx = 0; nx < m; nx++) {
          STFTdelpos2->data[nx + STFTdelpos2->size[0] * i1] = STFTdelpos->data[((i0 + nx) + STFTdelpos->size[0] * i1) + 1];
          fprintf(fpSTFTdelpos2real, "%.4f\n", STFTdelpos2->data[nx + STFTdelpos2->size[0] * i1].re); // store file KALEEM
          fprintf(fpSTFTdelpos2imag, "%.4f\n", STFTdelpos2->data[nx + STFTdelpos2->size[0] * i1].im); // store file KALEEM
      }
  }
  fclose(fpSTFTdelpos2real); // store file KALEEM
  fclose(fpSTFTdelpos2imag); // store file KALEEM
  
  //--------------------------------------------------- checking STFT content end, Result OK

  //--------------------------------------------------- checking STFT content start, Result OK
  if (cdiff > ndbl) {
      i0 = -1;
      i1 = -1;
  }
  else {
      i0 = (int)cdiff - 2;
      i1 = (int)ndbl - 1;
  }

  emxInit_creal_T(&STFTfreqdelpos2, 2);
  loop_ub = STFTfreqdelpos->size[1];
  nx = STFTfreqdelpos2->size[0] * STFTfreqdelpos2->size[1];
  m = i1 - i0;
  STFTfreqdelpos2->size[0] = m;
  STFTfreqdelpos2->size[1] = loop_ub;
  emxEnsureCapacity_creal_T(STFTfreqdelpos2, nx);

  FILE* fpSTFTfreqdelpos2real = fopen("fpSTFTfreqdelpos2real.txt", "wb"); // store file KALEEM
  FILE* fpSTFTfreqdelpos2imag = fopen("fpSTFTfreqdelpos2imag.txt", "wb"); // store file KALEEM
  for (i1 = 0; i1 < loop_ub; i1++) {
      for (nx = 0; nx < m; nx++) {
          STFTfreqdelpos2->data[nx + STFTfreqdelpos2->size[0] * i1] = STFTfreqdelpos->data[((i0 + nx) + STFTfreqdelpos->size[0] * i1) + 1];
          fprintf(fpSTFTfreqdelpos2real, "%.4f\n", STFTfreqdelpos2->data[nx + STFTfreqdelpos2->size[0] * i1].re); // store file KALEEM
          fprintf(fpSTFTfreqdelpos2imag, "%.4f\n", STFTfreqdelpos2->data[nx + STFTfreqdelpos2->size[0] * i1].im); // store file KALEEM
      }
  }
  fclose(fpSTFTfreqdelpos2real); // store file KALEEM
  fclose(fpSTFTfreqdelpos2imag); // store file KALEEM

  //--------------------------------------------------- checking STFT content end, Result OK


  // Compute the complex conjugate of STFTdelpos
  emxInit_creal_T(&conjSTFTdelpos, 2);
  complexConjugate(STFTdelpos2, conjSTFTdelpos);

  FILE* fpconjSTFTdelposreal = fopen("fpconjSTFTdelposreal.txt", "wb"); // store file KALEEM
  FILE* fpconjSTFTdelposimag = fopen("fpconjSTFTdelposimag.txt", "wb"); // store file KALEEM
  //printf("s5: %d, s6: %d\n", s5, s6);
  int s15 = conjSTFTdelpos->size[0]; // rows
  int s16 = conjSTFTdelpos->size[1]; // column
  for (int j = 0; j < s16; j++) {
      for (int i = 0; i < s15; i++) {
          fprintf(fpconjSTFTdelposreal, "%.4f\n", conjSTFTdelpos->data[i + s15 * j].re); // store file KALEEM -> NOTE: IN MATLAB, IMAGINARY CAN BE -0, IN C IS 0
          fprintf(fpconjSTFTdelposimag, "%.4f\n", conjSTFTdelpos->data[i + s15 * j].im); // store file KALEEM -> NOTE: IN MATLAB, IMAGINARY CAN BE -0, IN C IS 0
      }
  }
  fclose(fpconjSTFTdelposreal); // store file KALEEM
  fclose(fpconjSTFTdelposimag); // store file KALEEM


  // Compute the complex conjugate of STFTdelpos
  emxInit_creal_T(&conjSTFTfreqdelpos, 2);
  complexConjugate(STFTfreqdelpos2, conjSTFTfreqdelpos);

  FILE* fpconjSTFTfreqdelposreal = fopen("fpconjSTFTfreqdelposreal.txt", "wb"); // store file KALEEM
  FILE* fpconjSTFTfreqdelposimag = fopen("fpconjSTFTfreqdelposimag.txt", "wb"); // store file KALEEM
  //printf("s5: %d, s6: %d\n", s5, s6);
  s15 = conjSTFTdelpos->size[0]; // rows
  s16 = conjSTFTdelpos->size[1]; // column
  for (int j = 0; j < s16; j++) {
      for (int i = 0; i < s15; i++) {
          fprintf(fpconjSTFTfreqdelposreal, "%.4f\n", conjSTFTfreqdelpos->data[i + s15 * j].re); // store file KALEEM -> NOTE: IN MATLAB, IMAGINARY CAN BE -0, IN C IS 0
          fprintf(fpconjSTFTfreqdelposimag, "%.4f\n", conjSTFTfreqdelpos->data[i + s15 * j].im); // store file KALEEM -> NOTE: IN MATLAB, IMAGINARY CAN BE -0, IN C IS 0
      }
  }
  fclose(fpconjSTFTfreqdelposreal); // store file KALEEM
  fclose(fpconjSTFTfreqdelposimag); // store file KALEEM

  emxInit_creal_T(&C, 2);
  elementWiseMultiply(STFTpos, conjSTFTdelpos, C);
  FILE* fpCreal = fopen("fpCreal.txt", "wb"); // store file KALEEM
  FILE* fpCimag = fopen("fpCimag.txt", "wb"); // store file KALEEM
  //printf("s5: %d, s6: %d\n", s5, s6);
  s15 = C->size[0]; // rows
  s16 = C->size[1]; // column
  for (int j = 0; j < s16; j++) {
      for (int i = 0; i < s15; i++) {
          fprintf(fpCreal, "%.4f\n", C->data[i + s15 * j].re); // store file KALEEM -> NOTE: IN MATLAB, IMAGINARY CAN BE -0, IN C IS 0
          fprintf(fpCimag, "%.4f\n", C->data[i + s15 * j].im); // store file KALEEM -> NOTE: IN MATLAB, IMAGINARY CAN BE -0, IN C IS 0
      }
  }
  fclose(fpCreal); // store file KALEEM
  fclose(fpCimag); // store file KALEEM


  emxInit_creal_T(&L, 2);
  elementWiseMultiply(STFTpos, conjSTFTfreqdelpos, L);
  FILE* fpLreal = fopen("fpLreal.txt", "wb"); // store file KALEEM
  FILE* fpLimag = fopen("fpLimag.txt", "wb"); // store file KALEEM
  //printf("s5: %d, s6: %d\n", s5, s6);
  s15 = L->size[0]; // rows
  s16 = L->size[1]; // column
  for (int j = 0; j < s16; j++) {
      for (int i = 0; i < s15; i++) {
          fprintf(fpLreal, "%.4f\n", L->data[i + s15 * j].re); // store file KALEEM -> NOTE: IN MATLAB, IMAGINARY CAN BE -0, IN C IS 0
          fprintf(fpLimag, "%.4f\n", L->data[i + s15 * j].im); // store file KALEEM -> NOTE: IN MATLAB, IMAGINARY CAN BE -0, IN C IS 0
      }
  }
  fclose(fpLreal); // store file KALEEM
  fclose(fpLimag); // store file KALEEM


  emxInit_real_T(&angleC, 2);
  complexAngle(C, angleC); // is the order correct?
  FILE* fpangleC = fopen("fpangleC.txt", "wb"); // store file KALEEM
  s15 = C->size[0]; // rows
  s16 = C->size[1]; // column
  for (int j = 0; j < s16; j++) {
      for (int i = 0; i < s15; i++) {
          fprintf(fpangleC, "%.4f\n", angleC->data[i + angleC->size[0] * j]); // store file KALEEM
      }
  }
  fclose(fpangleC); // store file KALEEM


  

  emxInit_real_T(&argC, 2);
  mod2Pi(angleC, argC);// is the order correct?
  FILE* fpargC = fopen("fpargC.txt", "wb"); // store file KALEEM
  s15 = angleC->size[0]; // rows
  s16 = angleC->size[1]; // column
  for (int j = 0; j < s16; j++) {
      for (int i = 0; i < s15; i++) {
          fprintf(fpargC, "%.4f\n", argC->data[i + argC->size[0] * j]); // store file KALEEM
      }
  }
  fclose(fpargC); // store file KALEEM

  int delay = 1;

  scalarMultiply(argC, Fs / delay, CIFpos);
  FILE* fpCIFpostmp = fopen("fpCIFpostmp.txt", "wb"); // store file KALEEM
  s15 = CIFpos->size[0]; // rows
  s16 = CIFpos->size[1]; // column
  for (int j = 0; j < s16; j++) {
      for (int i = 0; i < s15; i++) {
          fprintf(fpCIFpostmp, "%.4f\n", CIFpos->data[i + CIFpos->size[0] * j]); // store file KALEEM
      }
  }
  fclose(fpCIFpostmp); // store file KALEEM

  

// Perform element-wise division of tmp by (2 * pi)
  FILE* fpCIFpos = fopen("fpCIFpos.txt", "wb"); // store file KALEEM
  s15 = CIFpos->size[0]; // rows
  s16 = CIFpos->size[1]; // column
  CIFPos_mag->size[0] = s15;
  CIFPos_mag->size[1] = s16;
  emxEnsureCapacity_real_T(CIFPos_mag, s15 * s16);

  for (int i = 0; i < s16; i++) {
      for (int j = 0; j < s15; j++) {
          //printf("%.4f ", CIFpos->data[i + CIFpos->size[0] * j]);
          CIFpos->data[j + CIFpos->size[0] * i] /= (2 * M_PI);
          CIFPos_mag->data[j + CIFPos_mag->size[0] * i] = CIFpos->data[j + CIFpos->size[0] * i];
          //printf(" > %.4f \n", CIFpos->data[i + CIFpos->size[0] * j]);
          fprintf(fpCIFpos, "%.4f\n", CIFpos->data[j + CIFpos->size[0] * i]); // store file KALEEM
      }
  }
  fclose(fpCIFpos); // store file KALEEM
  
 
  emxInit_real_T(&angleL, 2);
  complexAngle(L, angleL); // is the order correct?
  FILE* fpangleL = fopen("fpangleL.txt", "wb"); // store file KALEEM
  s15 = L->size[0]; // rows
  s16 = L->size[1]; // column
  for (int j = 0; j < s16; j++) {
      for (int i = 0; i < s15; i++) {
          fprintf(fpangleL, "%.4f\n", angleL->data[i + angleL->size[0] * j]); // store file KALEEM
      }
  }
  fclose(fpangleL); // store file KALEEM


  emxInit_real_T(&argL, 2);
  modNeg2Pi(angleL, argL);// is the order correct?
  FILE* fpargL = fopen("fpargL.txt", "wb"); // store file KALEEM
  s15 = angleL->size[0]; // rows
  s16 = angleL->size[1]; // column
  for (int j = 0; j < s16; j++) {
      for (int i = 0; i < s15; i++) {
          fprintf(fpargL, "%.4f\n", argL->data[i + argL->size[0] * j]); // store file KALEEM
      }
  }
  fclose(fpargL); // store file KALEEM

  
  scalarMultiply(argL, fftn / Fs, LGDpos);
  FILE* fpLGDpostmp = fopen("fpLGDpostmp.txt", "wb"); // store file KALEEM
  s15 = LGDpos->size[0]; // rows
  s16 = LGDpos->size[1]; // column
  for (int j = 0; j < s16; j++) {
      for (int i = 0; i < s15; i++) {
          fprintf(fpLGDpostmp, "%.4f\n", LGDpos->data[i + LGDpos->size[0] * j]); // store file KALEEM
      }
  }
  fclose(fpLGDpostmp); // store file KALEEM

  // Perform element-wise division of tmp by (2 * pi)
  FILE* fpLGDpos = fopen("fpLGDpos.txt", "wb"); // store file KALEEM
  s15 = LGDpos->size[0]; // rows
  s16 = LGDpos->size[1]; // column
  for (int i = 0; i < s16; i++) {
      for (int j = 0; j < s15; j++) {
          LGDpos->data[j + LGDpos->size[0] * i] *=  (-1);
          LGDpos->data[j + LGDpos->size[0] * i] /= (2 * M_PI);
          fprintf(fpLGDpos, "%.4f\n", LGDpos->data[j + LGDpos->size[0] * i]); // store file KALEEM
      }
  }
  fclose(fpLGDpos); // store file KALEEM



 // t = (offset + win_size / 2) . / Fs;
  int s0_offset = offset->size[0]; // 1 size
  int s1_offset = offset->size[1];
  emxInit_real_T(&time_offset, 2);
  time_offset->size[0] = s0_offset;
  time_offset->size[1] = s1_offset;
  emxEnsureCapacity_real_T(time_offset, s0_offset * s1_offset);
  for (int i = 0; i < s0_offset * s1_offset; i++)
  {
      time_offset->data[i] = ((offset->data[i] + (win_size/2)) ) / Fs;
  }

  FILE* fptime_offset = fopen("fptime_offset.txt", "wb"); // store file KALEEM
  s15 = time_offset->size[0]; // rows
  s16 = time_offset->size[1]; // column
  for (int i = 0; i < s16; i++) {
      for (int j = 0; j < s15; j++) {
          fprintf(fptime_offset, "%.4f\n", time_offset->data[j + time_offset->size[0] * i]); // store file KALEEM
      }
  }
  fclose(fptime_offset); // store file KALEEM



  
  // for a = 1:highindex-lowindex+1 di MATLAB
  int tmp_cnt = 0;
  int size_0 = LGDpos->size[0];
  int size_1 = LGDpos->size[1];

  //printf("%d %d\n", size_0, size_1);
  time_t->size[0] = size_0;
  time_t->size[1] = size_1;
  emxEnsureCapacity_real_T(time_t, size_0 * size_1);
  time->size[0] = size_0;
  time->size[1] = size_1;
  emxEnsureCapacity_real_T(time, size_0 * size_1);
  //time_t is treemap in MATLAB
  FILE* fptime_t = fopen("fptime_t.txt", "wb"); // store file KALEEM
  for (int i = 0; i < size_1; i++) {
      for (int j = 0; j < size_0; j++) {
          time_t->data[j + time_t->size[0] * i] = LGDpos->data[j + LGDpos->size[0] * i] + time_offset->data[i] - (((win_size / 2) - 1) / Fs);
          time->data[j + time->size[0] * i] = LGDpos->data[j + LGDpos->size[0] * i] + time_offset->data[i] - (((win_size / 2) - 1) / Fs);
          fprintf(fptime_t, "%.4f\n", time_t->data[j + time_t->size[0] * i]); // store file KALEEM
      }
  }
  fclose(fptime_t); // store file KALEEM
 

  // Compute frequency
  computeF(freq, Fs, cdiff, ndbl, fftn);

  FILE* fpfreq = fopen("fpfreq.txt", "wb"); // store file KALEEM
  for (int i = 0; i < freq->size[1]; i++) {
      for (int j = 0; j < freq->size[0]; j++) {
          fprintf(fpfreq, "%.4f\n", freq->data[j + freq->size[0] * i]); // store file KALEEM
      }
  }
  fclose(fpfreq); // store file KALEEM


  emxArray_real_T* STFTmagAbs;
  emxInit_real_T(&STFTmagAbs, 2);
  STFTmagAbs->size[0] = STFTpos->size[0];
  STFTmagAbs->size[1] = STFTpos->size[1];
  emxEnsureCapacity_real_T(STFTmagAbs, STFTpos->size[0] * STFTpos->size[1]);
  computeSTFTmagAbs(STFTpos, STFTmagAbs);

  FILE* fpSTFTmagAbs = fopen("fpSTFTmagAbs.txt", "wb"); // store file KALEEM
  for (int i = 0; i < STFTmagAbs->size[1]; i++) {
      for (int j = 0; j < STFTmagAbs->size[0]; j++) {
          fprintf(fpSTFTmagAbs, "%.4f\n", STFTmagAbs->data[j + STFTmagAbs->size[0] * i]); // store file KALEEM
      }
  }
  fclose(fpSTFTmagAbs); // store file KALEEM


  emxArray_real_T* STFTmagNorm;
  emxInit_real_T(&STFTmagNorm, 2);
  STFTmagNorm->size[0] = STFTmagAbs->size[0];
  STFTmagNorm->size[1] = STFTmagAbs->size[1];
  emxEnsureCapacity_real_T(STFTmagNorm, STFTmagAbs->size[0] * STFTmagAbs->size[1]);
  computeSTFTmagNorm(STFTmagAbs, STFTmagNorm);

  FILE* fpSTFTmagNorm = fopen("fpSTFTmagNorm.txt", "wb"); // store file KALEEM
  for (int i = 0; i < STFTmagNorm->size[1]; i++) {
      for (int j = 0; j < STFTmagNorm->size[0]; j++) {
          fprintf(fpSTFTmagNorm, "%.4f\n", STFTmagNorm->data[j + STFTmagNorm->size[0] * i]); // store file KALEEM
      }
  }
  fclose(fpSTFTmagNorm); // store file KALEEM



  // Compute 20*log10(STFTmag)
  STFTmag->size[0] = STFTmagNorm->size[0];
  STFTmag->size[1] = STFTmagNorm->size[1];
  emxEnsureCapacity_real_T(STFTmag, STFTmagNorm->size[0] * STFTmagNorm->size[1]);
  computeLogMagnitude(STFTmagNorm, STFTmag);
  FILE* fpSTFTmag = fopen("fpSTFTmag.txt", "wb"); // store file KALEEM
  for (int i = 0; i < STFTmag->size[1]; i++) {
      for (int j = 0; j < STFTmag->size[0]; j++) {
          fprintf(fpSTFTmag, "%.4f\n", STFTmag->data[j + STFTmag->size[0] * i]); // store file KALEEM
      }
  }
  fclose(fpSTFTmag); // store file KALEEM

  // Find indices of elements satisfying the conditions
  emxArray_real_T* plot_these;
  emxInit_real_T(&plot_these, 1);

/*
  findIndices(STFTmag, CIFpos, time, clip, low, high, plot_these, time_offset->data[0], time_offset->data[time_offset->size[1] - 1]);
  /*FILE* fpplot_these = fopen("fpplot_these.txt", "wb"); // store file KALEEM
  for (int i = 0; i < plot_these->size[0]; i++) {
      fprintf(fpplot_these, "%.0f\n", plot_these->data[i] + 1); // store file KALEEM
  }
  fclose(fpplot_these); // store file KALEEM


  // Initialize the STFTplot array
  emxArray_real_T* STFTplot;
  emxInit_real_T(&STFTplot, 1);
  STFTplot->size[0] = plot_these->size[0];
  emxEnsureCapacity_real_T(STFTplot, STFTplot->size[0]);
  extractElementsPlot(STFTmag, plot_these, STFTplot);


  // Initialize the CIFplot array
  emxArray_real_T* CIFplot;
  emxInit_real_T(&CIFplot, 1);
  CIFplot->size[0] = plot_these->size[0];
  emxEnsureCapacity_real_T(CIFplot, CIFplot->size[0]);
  extractElementsPlot(CIFpos, plot_these, CIFplot);

  // Initialize the tremapPlot array
  emxArray_real_T* tremapPlot;
  emxInit_real_T(&tremapPlot, 1);
  tremapPlot->size[0] = plot_these->size[0];
  emxEnsureCapacity_real_T(tremapPlot, tremapPlot->size[0]);
  extractElementsPlot(time, plot_these, tremapPlot);
  */

}

void Nelsonspec2(const double *b_signal, double Fs, double win_size, double
                overlap, double fftn, double low, double high, double clip,double numav,
                emxArray_real_T *STFTmag,emxArray_real_T* CIFPos_mag, emxArray_real_T* time,emxArray_real_T* freq)
{
  emxArray_real_T *y;
  int i0;
  int loop_ub;
  emxArray_real_T *window;
  int nx;
  int k;
  double d;
  emxArray_real_T *offset;
  emxArray_real_T* time_offset;
  double ndbl;
  double cdiff;
  double apnd;
  int i1;
  emxArray_real_T *b_y;
  emxArray_real_T *r0;
#if 1
  emxArray_real_T* r0_cif;
  emxArray_real_T* r1_cif;
  emxArray_creal_T* STFTfreqdelpos;
  emxArray_creal_T* STFTdelpos;
  emxArray_creal_T* CIFpos;
  emxArray_creal_T* LGDpos;
  emxArray_creal_T* time_t;
  emxArray_creal_T* L;


#endif
  emxArray_real_T *r1;
  emxArray_creal_T *STFT;
  double d0;
  emxArray_creal_T *STFTpos;
  int m;
  unsigned int unnamed_idx_1;
  boolean_T exitg1;
  (void)clip;
  emxInit_real_T(&y, 2);
  if (rtIsNaN(win_size)) {
    i0 = y->size[0] * y->size[1];
    y->size[0] = 1;
    y->size[1] = 1;
    emxEnsureCapacity_real_T(y, i0);
    y->data[0] = rtNaN;
  } else if (win_size < 1.0) {
    y->size[0] = 1;
    y->size[1] = 0;
  } else if (rtIsInf(win_size) && (1.0 == win_size)) {
    i0 = y->size[0] * y->size[1];
    y->size[0] = 1;
    y->size[1] = 1;
    emxEnsureCapacity_real_T(y, i0);
    y->data[0] = rtNaN;
  } else {
    i0 = y->size[0] * y->size[1];
    y->size[0] = 1;
    loop_ub = (int)floor(win_size - 1.0);
    y->size[1] = loop_ub + 1;
    emxEnsureCapacity_real_T(y, i0);
    for (i0 = 0; i0 <= loop_ub; i0++) {
      y->data[i0] = 1.0 + (double)i0;
    }
  }

  emxInit_real_T(&window, 1);
  i0 = window->size[0];
  window->size[0] = y->size[1];
  emxEnsureCapacity_real_T(window, i0);
  loop_ub = y->size[1];
  for (i0 = 0; i0 < loop_ub; i0++) {
    window->data[i0] = 6.2831853071795862 * y->data[i0] / (win_size + 1.0);
  }

  nx = window->size[0];
  for (k = 0; k < nx; k++) {
    window->data[k] = cos(window->data[k]);
  }

  i0 = window->size[0];
  emxEnsureCapacity_real_T(window, i0);
  loop_ub = window->size[0];
  for (i0 = 0; i0 < loop_ub; i0++) {
    window->data[i0] = 0.5 * (1.0 - window->data[i0]);
  }

  d = (double)window->size[0] - overlap;
  emxInit_real_T(&offset, 2);
  emxInit_real_T(&time_offset, 2);
  if (rtIsNaN(d)) {
    i0 = offset->size[0] * offset->size[1];
    offset->size[0] = 1;
    offset->size[1] = 1;
    emxEnsureCapacity_real_T(offset, i0);
    offset->data[0] = rtNaN;
  } else if ((d == 0.0) || ((0 < 33706 - window->size[0]) && (d < 0.0)) ||
             ((33706 - window->size[0] < 0) && (d > 0.0))) {
    offset->size[0] = 1;
    offset->size[1] = 0;
  } else if (rtIsInf(d)) {
    i0 = offset->size[0] * offset->size[1];
    offset->size[0] = 1;
    offset->size[1] = 1;
    emxEnsureCapacity_real_T(offset, i0);
    offset->data[0] = 0.0;
  } else if (floor(d) == d) {
    cdiff = (33707.0 - (double)window->size[0]) - 1.0;
    i0 = offset->size[0] * offset->size[1];
    offset->size[0] = 1;
    //xiang,ma 2024.01.30
    loop_ub = (int)floor(cdiff / (d*numav));
    offset->size[1] = loop_ub + 1;
    emxEnsureCapacity_real_T(offset, i0);
    for (i0 = 0; i0 <= loop_ub; i0++) {
      offset->data[i0] = d * (double)i0;
    }
  } else {
    ndbl = floor(((33707.0 - (double)window->size[0]) - 1.0) / d + 0.5);
    apnd = ndbl * d;
    if (d > 0.0) {
      cdiff = apnd - ((33707.0 - (double)window->size[0]) - 1.0);
    } else {
      cdiff = ((33707.0 - (double)window->size[0]) - 1.0) - apnd;
    }

    if (fabs(cdiff) < 4.4408920985006262E-16 * fabs((33707.0 - (double)
          window->size[0]) - 1.0)) {
      ndbl++;
      apnd = (33707.0 - (double)window->size[0]) - 1.0;
    } else if (cdiff > 0.0) {
      apnd = (ndbl - 1.0) * d;
    } else {
      ndbl++;
    }

    if (ndbl >= 0.0) {
      loop_ub = (int)ndbl;
    } else {
      loop_ub = 0;
    }

    i0 = offset->size[0] * offset->size[1];
    offset->size[0] = 1;
    offset->size[1] = loop_ub;
    emxEnsureCapacity_real_T(offset, i0);
    if (loop_ub > 0) {
      offset->data[0] = 0.0;
      if (loop_ub > 1) {
        offset->data[loop_ub - 1] = apnd;
        nx = (loop_ub - 1) / 2;
        for (k = 0; k <= nx - 2; k++) {
          cdiff = (1.0 + (double)k) * d;
          offset->data[1 + k] = cdiff;
          offset->data[(loop_ub - k) - 2] = apnd - cdiff;
        }

        if (nx << 1 == loop_ub - 1) {
          offset->data[nx] = apnd / 2.0;
        } else {
          cdiff = (double)nx * d;
          offset->data[nx] = cdiff;
          offset->data[nx + 1] = apnd - cdiff;
        }
      }
    }
  }

  if (window->size[0] < 1) {
    y->size[0] = 1;
    y->size[1] = 0;
  } else {
    i0 = window->size[0];
    i1 = y->size[0] * y->size[1];
    y->size[0] = 1;
    loop_ub = (int)((double)i0 - 1.0);
    y->size[1] = loop_ub + 1;
    emxEnsureCapacity_real_T(y, i1);
    for (i0 = 0; i0 <= loop_ub; i0++) {
      y->data[i0] = 1.0 + (double)i0;
    }
  }

  emxInit_real_T(&b_y, 2);
  k = offset->size[1];
  nx = window->size[0];
  i0 = b_y->size[0] * b_y->size[1];
  b_y->size[0] = y->size[1];
  b_y->size[1] = k;
  emxEnsureCapacity_real_T(b_y, i0);
  loop_ub = y->size[1];
  for (i0 = 0; i0 < loop_ub; i0++) {
    for (i1 = 0; i1 < k; i1++) {
      b_y->data[i0 + b_y->size[0] * i1] = y->data[i0];
    }
  }

  emxInit_real_T(&r0, 2);
  emxInit_real_T(&r0_cif, 2);
  i0 = r0->size[0] * r0->size[1];
  r0->size[0] = nx;
  r0->size[1] = offset->size[1];
  // do for r0_clif as well
  r0_cif->size[0] = nx;
  r0_cif->size[1] = offset->size[1];
  emxEnsureCapacity_real_T(r0, i0);
  emxEnsureCapacity_real_T(r0_cif, i0);
  for (i0 = 0; i0 < nx; i0++) {
    loop_ub = offset->size[1];
    for (i1 = 0; i1 < loop_ub; i1++) {
      r0->data[i0 + r0->size[0] * i1] = offset->data[i1];
      r0_cif->data[i0 + r0_cif->size[0] * i1] = offset->data[i1];
    }
  }

  emxInit_real_T(&r1, 2);
  emxInit_real_T(&r1_cif, 2);
  i0 = r1->size[0] * r1->size[1];
  r1->size[0] = b_y->size[0];
  r1->size[1] = b_y->size[1];

  r1_cif->size[0] = b_y->size[0];
  r1_cif->size[1] = b_y->size[1];
  // do r1_cif as well
  emxEnsureCapacity_real_T(r1, i0);
  emxEnsureCapacity_real_T(r1_cif, i0);
  loop_ub = b_y->size[0] * b_y->size[1];
  for (i0 = 0; i0 < loop_ub; i0++) {
    r1->data[i0] = b_signal[(int)((b_y->data[i0] + r0->data[i0]) + 1.0) - 1];
    r1_cif->data[i0] = b_signal[(int)((b_y->data[i0] + r0_cif->data[i0])) - 1];
  }

  k = offset->size[1];
  i0 = b_y->size[0] * b_y->size[1];
  b_y->size[0] = window->size[0];
  b_y->size[1] = k;
  emxEnsureCapacity_real_T(b_y, i0);
  loop_ub = window->size[0];
#if 0
  //emxFree_real_T(&offset);
#else
  //emxFree_real_T(&offset);
#endif
  for (i0 = 0; i0 < loop_ub; i0++) {
    for (i1 = 0; i1 < k; i1++) {
      b_y->data[i0 + b_y->size[0] * i1] = window->data[i0];
    }
  }

  emxFree_real_T(&window);
  i0 = r0->size[0] * r0->size[1];
  r0->size[0] = r1->size[0];
  r0->size[1] = r1->size[1];
  emxEnsureCapacity_real_T(r0, i0);
  loop_ub = r1->size[0] * r1->size[1];
  for (i0 = 0; i0 < loop_ub; i0++) {
    r0->data[i0] = r1->data[i0] * b_y->data[i0];
    r0_cif->data[i0] = r1_cif->data[i0] * b_y->data[i0];
  }
#if 0
  //emxFree_real_T(&b_y);
#else
  emxFree_real_T(&b_y);
#endif
  emxFree_real_T(&r1);
  emxInit_creal_T(&STFT, 2);
  emxInit_creal_T(&STFTfreqdelpos, 2);
  emxInit_creal_T(&STFTdelpos, 2);
  emxInit_creal_T(&CIFpos, 2);
  emxInit_creal_T(&LGDpos, 2);
  emxInit_creal_T(&time_t, 2);
  printf("  fft calculations started \n");
  fft_emx(r0,fftn,STFT);
  printf(" 1a ffts calculations \n");
  fft_emx(r0_cif,fftn,STFTfreqdelpos);
  printf(" 2 ffts calculations \n");
  fft_emx(r0_cif, fftn, STFTdelpos);
  printf(" 3 ffts calculations \n");
  
  
  // dummy calculation, so that proper memory is allocated for CIF pos
  fft_emx(r0, fftn, CIFpos);
  fft_emx(r0, fftn, LGDpos);
  fft_emx(r0, fftn, time_t);
  emxFree_real_T(&r0);
   emxFree_real_T(&r0_cif);
  //validate
  int s1 = STFT->size[0];
  int s2 = STFT->size[1];
  printf("DEBUG\n");
  int t = 0;
  for (int j = 0; j < s2; j++)
  {
      if (j == 0)
          t = s1 - 1;
      else
          t = ((s1 - 1) * (j + 1)) + j;
      
      STFTfreqdelpos->data[j * s1].re = STFT->data[t].re;
      STFTfreqdelpos->data[j * s1].im = STFT->data[t].im;
      //printf("%d\t %lf\t %lf\n", j,STFTfreqdelpos->data[j].re, STFTfreqdelpos->data[j].im);
  }

  //printf("next line\n");
  int cnt = s2;
  for (int j = 0; j < s1 - 1; j++)
  {
      for (int i = 0; i < s2; i++)
      {
          t = s1 * i + j;
          STFTfreqdelpos->data[t + 1].re = STFT->data[t].re;
          STFTfreqdelpos->data[t + 1].im = STFT->data[t].im;
         // printf("%d\t %lf\t %lf\n", i, STFTfreqdelpos->data[cnt].re, STFTfreqdelpos->data[cnt].im);
          cnt++;
      }
  }
 
  if (rt_remd_snf(fftn, 2.0) == 1.0) {
    d0 = (fftn - 1.0) / 2.0;
  } else {
    d0 = fftn / 2.0;
  }

  cdiff = Fs * (d0 - 1.0) / fftn;
  if (high > cdiff) {
    high = cdiff;
  }

  cdiff = rt_roundd_snf(low / Fs * fftn);
  if (cdiff == 0.0) {
    cdiff = 1.0;
  }

  ndbl = rt_roundd_snf(high / Fs * fftn);
  if (cdiff > ndbl) {
    i0 = -1;
    i1 = -1;
  } else {
    i0 = (int)cdiff - 2;
    i1 = (int)ndbl - 1;
  }

  emxInit_creal_T(&STFTpos, 2);
  loop_ub = STFT->size[1];
  nx = STFTpos->size[0] * STFTpos->size[1];
  m = i1 - i0;
  STFTpos->size[0] = m;
  STFTpos->size[1] = loop_ub;
  emxEnsureCapacity_creal_T(STFTpos, nx);
  for (i1 = 0; i1 < loop_ub; i1++) {
    for (nx = 0; nx < m; nx++) {
      STFTpos->data[nx + STFTpos->size[0] * i1] = STFT->data[((i0 + nx) +
        STFT->size[0] * i1) + 1];
    }
  }
  /*
  here STFTPos is available, so start computing CIFPos as well
  below is the matlab code
  C = STFTpos .* conj(STFTdelpos);

  L = STFTpos .* conj(STFTfreqdelpos);
  argC = mod(angle(C),2*pi);
  CIFpos = ((Fs/delay).* argC)./(2.*pi);
  */
  /*
 Find C
  1. compute STFTdelpos
  2. compute conj of STFTdelpos
  3. multiply STFTpos with step2 ---> C computed
  */
  // compute STFTdel


  /*
  Compute C:
        1. first conj(STFTdelpos)
        2. STFTpos . * conj(STFTdelpos)
   */
   // computing conj(STFTdelpos)
  int size_0 = STFTdelpos->size[0];
  int size_1 = STFTdelpos->size[1];
  for (int i = 0; i < size_0 * size_1; i++)
  {
      //conj(STFTdelpos)
      double im1 = -STFTdelpos->data[i].im;
      STFTdelpos->data[i].im = im1;
     // conj(STFTfreqdelpos);
      double im2 = -STFTfreqdelpos->data[i].im;
      STFTfreqdelpos->data[i].im = im2;
  }
  //compute STFTpos . * conj(STFTdelpos)
  printf(" %lf\t %lf\n", STFTpos->data[0].re, STFTpos->data[0].im);
  printf(" %lf\t %lf\n", STFTpos->data[1].re, STFTpos->data[1].im);
  printf(" %lf\t %lf\n", STFTpos->data[2].re, STFTpos->data[2].im);
  printf(" %lf\t %lf\n", STFTpos->data[3].re, STFTpos->data[3].im);

  printf("\n");

  printf(" %lf\t %lf\n", STFTdelpos->data[0].re, STFTdelpos->data[0].im);
  printf(" %lf\t %lf\n", STFTdelpos->data[1].re, STFTdelpos->data[1].im);
  printf(" %lf\t %lf\n", STFTdelpos->data[2].re, STFTdelpos->data[2].im);
  printf(" %lf\t %lf\n", STFTdelpos->data[3].re, STFTdelpos->data[3].im);
  size_0 = STFTpos->size[0];
  size_1 = STFTpos->size[1];
   t = 0;
#if 0
  for (int j = 0; j < s2 ; j++)
  {
      for (int i = 0; i < s1; i++)
      {
          t = s1 * i + j;
          double im_delpos_im = STFTdelpos->data[t].im;
          double im_pos_im = STFTpos->data[t].im;

          double im = im_delpos_im * im_pos_im;
          printf("%lf\n", im);
          double im_delpos_re = STFTdelpos->data[t].re;
          double im_pos_re = STFTpos->data[t].re;

          double re = im_delpos_re * im_pos_re;
          printf("%lf\n", re);
          STFTdelpos->data[t].im = im;
          STFTdelpos->data[t].re = re;
          // printf("%d\t %lf\t %lf\n", i, STFTfreqdelpos->data[cnt].re, STFTfreqdelpos->data[cnt].im);
         
      }
  }
#endif
#if 1
  /*
  * (a+bi) .* (c + di)
  * = (a*c) + (a * di) + (bi * c) - (b*d)
  */
  for (int i = 0; i < size_0 * size_1; i++)
  {
      double im_delpos_im = STFTdelpos->data[i].im;
      double im_pos_im = STFTpos->data[i].im;

      double im_delpos_re = STFTdelpos->data[i].re;
      double im_pos_re = STFTpos->data[i].re;


      // b*d
      double re_2 = (im_delpos_im * im_pos_im);
      //a*c
      double re_1 = im_delpos_re * im_pos_re;
      //a*di
      double im_1 = im_delpos_re * im_pos_im;
      double im_2 = im_delpos_im * im_pos_re;
    //  printf("%lf\n", re_1 - re_2);
     // printf("%lf\n", im_1 + im_2);
      STFTdelpos->data[i].im = im_1 + im_2;
      STFTdelpos->data[i].re = re_1 - re_2;
  }
#endif
  //compute STFTpos . * conj(STFTfreqdelpos)

  for (int i = 0; i < size_0 * size_1; i++)
  {
      
      double re1 = STFTfreqdelpos->data[i].re * STFTpos->data[i].re;
      double re2 = STFTfreqdelpos->data[i].im * STFTpos->data[i].im;
      
      STFTfreqdelpos->data[i].re = re1 - re2;

      double im1 = STFTfreqdelpos->data[i].re * STFTpos->data[i].im;
      double im2 = STFTfreqdelpos->data[i].im * STFTpos->data[i].re;
      STFTfreqdelpos->data[i].im = im1 + im2;
  }
  int delay = 1;

  for (int i = 0; i < size_0 * size_1; i++)
  {
      CIFpos->data[i].re = 0.0;
      CIFpos->data[i].im = 0.0;
  }

  for (int i = 0; i < size_0 * size_1; i++)
  {
      LGDpos->data[i].re = 0.0;
      LGDpos->data[i].im = 0.0;
  }

  for (int i = 0; i < size_0 * size_1; i++)
  {
      time_t->data[i].re = 0.0;
      time_t->data[i].im = 0.0;
  }
  for (int i = 0; i < size_0 * size_1; i++)
  {
      double x = STFTdelpos->data[i].re;
      double y = STFTdelpos->data[i].im;
      double angle = (double)atan2(y,x);
      //if(i >= 3365)
     // printf("angle = %lf\n", angle);
      double mod_v = fmod(angle, (2 * M_PI));
      double temp = (mod_v * (Fs / delay));
      temp = temp / (2 * M_PI);
      CIFpos->data[i].re =  temp;
      CIFpos->data[i].im = 0.0;
  }

  for (int i = 0; i < size_0 * size_1; i++)
  {
      double angle = (double)atan2(STFTfreqdelpos->data[i].im, STFTfreqdelpos->data[i].re);
      double mod_v = fmod(angle, (-2 * M_PI));
      double temp = (mod_v * (fftn / Fs));
      temp = temp / (2 * M_PI);
      LGDpos->data[i].re = -temp;
      LGDpos->data[i].im = 0.0;
  }

  // compute t
  double step = win_size - overlap;
 // t = (offset + win_size / 2) . / Fs;
  int s0_offset = offset->size[0];
  int s1_offset = offset->size[1];
  time_offset->size[0] = s0_offset;
  time_offset->size[1] = s1_offset;
  emxEnsureCapacity_real_T(time_offset, s0_offset * s1_offset);
  for (int i = 0; i < s0_offset * s1_offset; i++)
  {
     // printf("%lf", offset->data[i]);
      time_offset->data[i] = ((offset->data[i] + win_size) / 2) / Fs;
  }
  int tmp_cnt = 0;
  for (int i = 0; i < size_0 * size_1; i++)
  {
      time_t->data[i].re = (LGDpos->data[i].re + time_offset->data[tmp_cnt] - (win_size / 2 - 1)) / Fs;
      if (tmp_cnt == size_1)
          tmp_cnt = 0;
  }
  i0 = STFT->size[1];
  nx = m * i0;
  i0 = STFT->size[1];
  i1 = STFTmag->size[0] * STFTmag->size[1];
  STFTmag->size[0] = m;
  STFTmag->size[1] = i0;
  CIFPos_mag->size[0] = m;
  CIFPos_mag->size[1] = i0;
  time->size[0] = m;
  time->size[1] = i0;
  emxEnsureCapacity_real_T(STFTmag, i1);
  for (k = 0; k < nx; k++) {
    STFTmag->data[k] = rt_hypotd_snf(STFTpos->data[k].re, STFTpos->data[k].im);
  }

  emxFree_creal_T(&STFTpos);
  emxFree_creal_T(&STFT);
  m = STFTmag->size[0];
  loop_ub = STFTmag->size[1];
  unnamed_idx_1 = (unsigned int)STFTmag->size[1];
  i0 = y->size[0] * y->size[1];
  y->size[0] = 1;
  y->size[1] = (int)unnamed_idx_1;
  emxEnsureCapacity_real_T(y, i0);
  if (STFTmag->size[1] >= 1) {
    for (nx = 0; nx < loop_ub; nx++) {
      y->data[nx] = STFTmag->data[STFTmag->size[0] * nx];
      for (k = 2; k <= m; k++) {
        cdiff = y->data[nx];
        ndbl = STFTmag->data[(k + STFTmag->size[0] * nx) - 1];
        if ((!rtIsNaN(ndbl)) && (rtIsNaN(cdiff) || (cdiff < ndbl))) {
          y->data[nx] = STFTmag->data[(k + STFTmag->size[0] * nx) - 1];
        }
      }
    }
  }

  loop_ub = y->size[1];
  if (y->size[1] <= 2) {
    if (y->size[1] == 1) {
      cdiff = y->data[0];
    } else if ((y->data[0] < y->data[1]) || (rtIsNaN(y->data[0]) && (!rtIsNaN
                 (y->data[1])))) {
      cdiff = y->data[1];
    } else {
      cdiff = y->data[0];
    }
  } else {
    if (!rtIsNaN(y->data[0])) {
      nx = 1;
    } else {
      nx = 0;
      k = 2;
      exitg1 = false;
      while ((!exitg1) && (k <= y->size[1])) {
        if (!rtIsNaN(y->data[k - 1])) {
          nx = k;
          exitg1 = true;
        } else {
          k++;
        }
      }
    }

    if (nx == 0) {
      cdiff = y->data[0];
    } else {
      cdiff = y->data[nx - 1];
      i0 = nx + 1;
      for (k = i0; k <= loop_ub; k++) {
        if (cdiff < y->data[k - 1]) {
          cdiff = y->data[k - 1];
        }
      }
    }
  }

  emxFree_real_T(&y);
  i0 = STFTmag->size[0] * STFTmag->size[1];
  i1 = STFTmag->size[0] * STFTmag->size[1];
  emxEnsureCapacity_real_T(STFTmag, i1);
  loop_ub = i0 - 1;
  for (i0 = 0; i0 <= loop_ub; i0++) {
    //xiang,ma
    STFTmag->data[i0] /= cdiff;
   // STFTmag->data[i0] /= numav;
  }

  nx = STFTmag->size[0] * STFTmag->size[1];
  for (k = 0; k < nx; k++) {
    STFTmag->data[k] = log10(STFTmag->data[k]);
  }

  i0 = STFTmag->size[0] * STFTmag->size[1];
  i1 = STFTmag->size[0] * STFTmag->size[1];
  emxEnsureCapacity_real_T(STFTmag, i1);
  emxEnsureCapacity_real_T(CIFPos_mag, i1);
  emxEnsureCapacity_real_T(time, i1);
  loop_ub = i0 - 1;
  for (i0 = 0; i0 <= loop_ub; i0++) {
    STFTmag->data[i0] *= 20.0;
    CIFPos_mag->data[i0] = CIFpos->data[i0].re;
    time->data[i0] = time_t->data[i0].re;
  }
}
