#include <stddef.h>
#include <stdlib.h>
#include "tmwtypes.h"
#include "Nelsonspec_types.h"

emxArray_real_T* emxCreateND_real_T(int numDimensions, int* size);
emxArray_real_T* emxCreateWrapperND_real_T(double* data, int numDimensions, int* size);
emxArray_real_T* emxCreateWrapper_real_T(double* data, int rows, int cols);
emxArray_real_T* emxCreate_real_T(int rows, int cols);
void emxDestroyArray_real_T(emxArray_real_T* emxArray);
void emxInitArray_real_T(emxArray_real_T** pEmxArray, int numDimensions);

void emxEnsureCapacity_creal_T(emxArray_creal_T* emxArray, int oldNumel);
void emxEnsureCapacity_real_T(emxArray_real_T* emxArray, int oldNumel);
void emxFree_creal_T(emxArray_creal_T** pEmxArray);
void emxFree_real_T(emxArray_real_T** pEmxArray);
void emxInit_creal_T(emxArray_creal_T** pEmxArray, int numDimensions);
void emxInit_real_T(emxArray_real_T** pEmxArray, int numDimensions);

void Nelsonspec(emxArray_real_T* b_signal, double Fs, double win_size, double overlap, double fftn, double low, double high, double clip, emxArray_real_T *STFTmag, emxArray_real_T* CIFPos, emxArray_real_T* time,emxArray_real_T* freq);
void Nelsonspec2(const double *b_signal, double Fs, double win_size, double overlap, double fftn, double low, double high, double clip,double numav, emxArray_real_T *STFTmag,emxArray_real_T* CIFPos, emxArray_real_T* time,emxArray_real_T* freq);
void extractElementsPlot(const emxArray_real_T* STFTmag, const emxArray_real_T* plot_these, emxArray_real_T* STFTplot);
 void findIndices(emxArray_real_T* STFTmag, emxArray_real_T* CIFpos, emxArray_real_T* tremap, double clip, double low, double high, emxArray_real_T* plot_these, double t1, double t2);

