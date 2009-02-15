#ifndef __MEM_LOC_H__
#define __MEM_LOC_H__

#include "definiti.h"

extern void IVectorAllocate(IVECTOR *ivector, int nCols);
extern void IMatrixFree(IMATRIX imatrix,  int nRows);
extern void IMatrixAllocate(IMATRIX *ipmatrix, int nRows, int nCols);
extern void IAllocateCols(P_INT imatrix[], int nRows, int nCols);

extern void FVectorAllocate(FVECTOR *fvector, int nCols);
extern void FMatrixFree(FMATRIX fmatrix,  int nRows);
extern void FMatrixAllocate(FMATRIX *fpmatrix, int nRows, int nCols);
extern void FAllocateCols(P_FLOAT fmatrix[], int nRows, int nCols);

#endif

