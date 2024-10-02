#include "rt_nonfinite.h"

real_T rtInf;
real_T rtMinusInf;
real_T rtNaN;
real32_T rtInfF;
real32_T rtMinusInfF;
real32_T rtNaNF;

real_T rtGetInf(void)
{
	real_T inf = 0.0;
	uint16_T one = 1U;
	enum {
		LittleEndian,
		BigEndian
	} machByteOrder = (*((uint8_T*)& one) == 1U) ? LittleEndian : BigEndian;
	switch (machByteOrder) {
	case LittleEndian:
	{
		union {
			LittleEndianIEEEDouble bitVal;
			real_T fltVal;
		} tmpVal;

		tmpVal.bitVal.words.wordH = 0x7FF00000U;
		tmpVal.bitVal.words.wordL = 0x00000000U;
		inf = tmpVal.fltVal;
		break;
	}

	case BigEndian:
	{
		union {
			BigEndianIEEEDouble bitVal;
			real_T fltVal;
		} tmpVal;

		tmpVal.bitVal.words.wordH = 0x7FF00000U;
		tmpVal.bitVal.words.wordL = 0x00000000U;
		inf = tmpVal.fltVal;
		break;
	}
	}

	return inf;
}

real32_T rtGetInfF(void)
{
	IEEESingle infF;
	infF.wordL.wordLuint = 0x7F800000U;
	return infF.wordL.wordLreal;
}

real_T rtGetMinusInf(void)
{
	real_T minf = 0.0;
	uint16_T one = 1U;
	enum {
		LittleEndian,
		BigEndian
	} machByteOrder = (*((uint8_T*)& one) == 1U) ? LittleEndian : BigEndian;
	switch (machByteOrder) {
	case LittleEndian:
	{
		union {
			LittleEndianIEEEDouble bitVal;
			real_T fltVal;
		} tmpVal;

		tmpVal.bitVal.words.wordH = 0xFFF00000U;
		tmpVal.bitVal.words.wordL = 0x00000000U;
		minf = tmpVal.fltVal;
		break;
	}

	case BigEndian:
	{
		union {
			BigEndianIEEEDouble bitVal;
			real_T fltVal;
		} tmpVal;

		tmpVal.bitVal.words.wordH = 0xFFF00000U;
		tmpVal.bitVal.words.wordL = 0x00000000U;
		minf = tmpVal.fltVal;
		break;
	}
	}

	return minf;
}

real32_T rtGetMinusInfF(void)
{
	IEEESingle minfF;
	minfF.wordL.wordLuint = 0xFF800000U;
	return minfF.wordL.wordLreal;
}

real_T rtGetNaN(void)
{
	real_T nan = 0.0;
	uint16_T one = 1U;
	enum {
		LittleEndian,
		BigEndian
	} machByteOrder = (*((uint8_T*)& one) == 1U) ? LittleEndian : BigEndian;
	switch (machByteOrder) {
	case LittleEndian:
	{
		union {
			LittleEndianIEEEDouble bitVal;
			real_T fltVal;
		} tmpVal;

		tmpVal.bitVal.words.wordH = 0xFFF80000U;
		tmpVal.bitVal.words.wordL = 0x00000000U;
		nan = tmpVal.fltVal;
		break;
	}

	case BigEndian:
	{
		union {
			BigEndianIEEEDouble bitVal;
			real_T fltVal;
		} tmpVal;

		tmpVal.bitVal.words.wordH = 0x7FFFFFFFU;
		tmpVal.bitVal.words.wordL = 0xFFFFFFFFU;
		nan = tmpVal.fltVal;
		break;
	}
	}

	return nan;
}

real32_T rtGetNaNF(void)
{
	IEEESingle nanF = { { 0 } };

	uint16_T one = 1U;
	enum {
		LittleEndian,
		BigEndian
	} machByteOrder = (*((uint8_T*)& one) == 1U) ? LittleEndian : BigEndian;
	switch (machByteOrder) {
	case LittleEndian:
	{
		nanF.wordL.wordLuint = 0xFFC00000U;
		break;
	}

	case BigEndian:
	{
		nanF.wordL.wordLuint = 0x7FFFFFFFU;
		break;
	}
	}

	return nanF.wordL.wordLreal;
}

void rt_InitInfAndNaN(size_t realSize)
{
  (void)realSize;
  rtNaN = rtGetNaN();
  rtNaNF = rtGetNaNF();
  rtInf = rtGetInf();
  rtInfF = rtGetInfF();
  rtMinusInf = rtGetMinusInf();
  rtMinusInfF = rtGetMinusInfF();
}

boolean_T rtIsInf(real_T value)
{
  return ((value==rtInf || value==rtMinusInf) ? 1U : 0U);
}

boolean_T rtIsInfF(real32_T value)
{
  return(((value)==rtInfF || (value)==rtMinusInfF) ? 1U : 0U);
}

boolean_T rtIsNaN(real_T value)
{
  return (value!=value)? 1U:0U;
}

boolean_T rtIsNaNF(real32_T value)
{
  return (value!=value)? 1U:0U;
}