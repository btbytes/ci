#include "headfile.h"
#include "mem_loc.h"

/**********************************************************************
		 If you want to allocate a block larger than 64K, you must use
		 farcalloc instead of calloc
**********************************************************************/

/* Memory allocation functions for integer matrix and vector  */

void IVectorAllocate(IVECTOR *ivector, int nCols)
{
	if ((*ivector = (IVECTOR) calloc(nCols, sizeof(long int))) == NULL)
	{
		fprintf(stderr, "Sorry! Not enough memory for vector\n");
		exit(1);
	}
}

/* Allocate space for columns (int cells) for
	dynamic two dimensional matrix[rows][cols]
*/

void IAllocateCols(P_INT imatrix[], int nRows, int nCols)
{
	int  i;
	for (i = 0;  i < nRows;  i++)
		IVectorAllocate(&imatrix[i], nCols);
}

/* Allocate space for a two dimensional dynamic matrix [rows] [cols]
*/

void IMatrixAllocate(IMATRIX *ipmatrix, int nRows, int nCols)
{
	if ( (*ipmatrix  =  (IMATRIX) calloc(nRows,  sizeof(long int) ) )   ==  NULL)
	{
		fprintf(stderr, "Sorry! Not enough memory for matrix\n");
		exit(1);
	}

	IAllocateCols(*ipmatrix, nRows, nCols);
}

/* free space for two dimensional dynamic array */
void IMatrixFree(IMATRIX imatrix,  int nRows)
{
	int   i;
	for (i = 0;  i < nRows;  i++)
		free(imatrix[i]);
	free(imatrix);
}

/* *************** Float routines *************** */

void FVectorAllocate(FVECTOR *fvector, int nCols)
{
	if ((*fvector = (FVECTOR) calloc(nCols, sizeof(float))) == NULL)
	{
		fprintf(stderr, "Sorry! Not enough memory for fvector\n");
		exit(1);
	}
}

/* Allocate space for columns (int cells) for
	dynamic two dimensional matrix[rows][cols]
*/

void FAllocateCols(P_FLOAT fmatrix[], int nRows, int nCols)
{
	int  i;
	for (i = 0;  i < nRows;  i++)
		FVectorAllocate(&fmatrix[i], nCols);
}

/* Allocate space for a two dimensional dynamic matrix [rows] [cols]
*/

void FMatrixAllocate(FMATRIX *fpmatrix, int nRows, int nCols)
{
	if ( (*fpmatrix  =  (FMATRIX) calloc(nRows,  sizeof(float)))   ==  NULL)
	{
		fprintf(stderr, "Sorry! Not enough memory for fmatrix\n");
		exit(1);
	}

	FAllocateCols(*fpmatrix, nRows, nCols);
}

/* free space for two dimensional dynamic array */
void FMatrixFree(FMATRIX fmatrix,  int nRows)
{
	int   i;
	for (i = 0;  i < nRows;  i++)
		free(fmatrix[i]);
	free(fmatrix);
}

