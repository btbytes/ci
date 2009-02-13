/************************************************************/
/* Include files                                            */
/************************************************************/
#include <math.h>
#include <stdio.h>
#include <stdlib.h>

#include "ga_bin.h"
#include "ga_math.h"
#include "opt_func.h"

/************************************************************/
/* Function Prototypes for functions with file level scope  */
/************************************************************/
static void  get_parameter(int, double *);

/************************************************************/
/*      Function Definitions                                */
/************************************************************/

// Listing 4.9 The ga_evaluate() routine
/************************************************************
*     Function:
*
*   Parameters:
*
*      Returns:
*
*  Description:
*
*************************************************************/
void ga_evaluate (void)
{
	int idx_i;
	double *para;       /* pointer to the parameters */

  /* allocate memory space for the weight matrix */
	para = (double *)calloc(ga_env_data.dimension,sizeof(double));

	/* fitness calculation */
	for ( idx_i = 0; idx_i < ga_data.popu_size; idx_i++)
	{
		/* convert binary vector to real value parameters */
		get_parameter(idx_i,para);
		/* get fitness  */
		ga_data.fit[idx_i] = OPT_Function_Routines(ga_env_data.function,ga_env_data.dimension,para);
	}

	free(para);

  ga_data.best_index = maximum(ga_data.fit,ga_data.popu_size);
  ga_data.fit_mean = average(ga_data.fit, ga_data.popu_size);
  ga_data.fit_variance = variance(ga_data.fit,ga_data.fit_mean, ga_data.popu_size);
}

/************************************************************
*     Function:
*
*   Parameters:
*
*      Returns:
*
*  Description:
*
*************************************************************/
static void get_parameter (int p_index, double *para)
{
	  int idx_i,idx_s;
	  int tem;
	  double max;

	  /* get the maximum value (ga_data.bits_per_para) bits can represent */
	  max = 0.0;
	  for (idx_i = 0; idx_i < (ga_data.bits_per_para); idx_i++)
		  max += pow(2.0,idx_i);

	  /* calculate parameters */
	  tem = 0;
	  for (idx_i = 0; idx_i < ga_env_data.dimension; idx_i++)
	  {
		  para[idx_i] = 0.0;
		  for (idx_s = 0; idx_s < ga_data.bits_per_para; idx_s++)
		  {
		    para[idx_i] += (pow(2.0,idx_s)) * ga_data.population[p_index][tem];
		    tem++;
		  }
		  para[idx_i] = (((2.0 * para[idx_i])/max) - 1.0);    /* dynamic range (-1.0 1.0) */
	  }
}
