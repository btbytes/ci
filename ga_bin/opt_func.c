
/************************************************************/
/* Include files                                            */
/************************************************************/
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include "opt_func.h"

/************************************************************/
/* Function Prototypes for functions with file level scope  */
/************************************************************/
static double f6(double *);
static double sphere(int, double *);
static double rosenbrock(int, double *);
static double rastrigrin(int, double *);
static double griewank(int, double *);

/************************************************************/
/*      Function Definitions                                */    
/************************************************************/
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
double OPT_Function_Routines (int fun_idx,int dim, double *para)
{
    double result;
    switch (fun_idx)
    {
        case F6:
          result = f6(para);
          break;
        case SPHERE:
          result = sphere(dim,para);
          break;
        case ROSENBROCK:
          result = rosenbrock(dim,para);
          break;
        case RASTRIGRIN:
          result = rastrigrin(dim,para);
          break;
        case GRIEWANK:
          result = griewank(dim,para);
          break;
        default:
          break;
    }

    return(result);
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
static double f6 (double *para)
{   /* minimum dynamic [-10,10] */
    double num, denom, f6;
	  double errorf6;

    para[0] *= 10.0;
    para[1] *= 10.0;

	  num=(sin(sqrt((para[0] * para[0]) + (para[1] * para[1]))))  *
		   (sin(sqrt((para[0] * para[0]) + (para[1] * para[1])))) - 0.5;
	  denom=(1.0 + 0.001 * ((para[0] * para[0]) + (para[1] * para[1]))) *
		  (1.0 + 0.001 * ((para[0] * para[0]) + (para[1] * para[1])));
	  f6 = (double) 0.5 - (num/denom);
	  errorf6 = 1 - f6;
	  return errorf6;
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
static double sphere (int dim, double *para)
{   /* minimum dynamic range [-10, 10]*/
    int idx_i;
    double result;
    result = 0.0;
    for (idx_i = 0; idx_i < dim ; idx_i++)
    {
        para[idx_i] *= 10.0;
        result += para[idx_i] * para[idx_i];
    }
    // convert to maximum problem
    return(-result);
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
static double rosenbrock (int dim, double *para)
{   /* minimum  dynamic range [-100 ,100 ] */
    int idx_i;
    double result;
    
    for (idx_i = 0; idx_i < dim; idx_i++)
	  {
        para[idx_i] *= 100.0;
    }

    result=0.0;
	  for (idx_i = 1; idx_i < dim; idx_i++)
	  {
		  result += (100.0 * (para[idx_i] - para[idx_i - 1] * para[idx_i - 1]) * (para[idx_i] - para[idx_i -1 ] * para[idx_i - 1]) + (para[idx_i - 1] - 1) * (para[idx_i - 1] - 1));
	  }
    // convert to maximum problem
	  return(-result);
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
static double rastrigrin (int dim, double *para)
{   /* minimum  dynamic range [-10 ,10 ] */
    int idx_i;
    double result;
    for (idx_i = 0; idx_i < dim; idx_i++)
	  {
        para[idx_i] *= 10.0;
    }

    result=0.0;
    for (idx_i = 0; idx_i < dim; idx_i++)
	  {
        result += para[idx_i] * para[idx_i] - 10.0 * cos(2.0 * 3.141591 * para[idx_i]) + 10.0;
    }
    // convert to maximum problem
    return(-result);
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
static double griewank (int dim, double *para)
{   /* minimum  dynamic range [-600 ,600 ] */
    int idx_i;
    double result_s,result_p;
    for (idx_i = 0; idx_i < dim; idx_i++)
	  {
        para[idx_i] *= 600;
    }

    result_s = 0.0;
    result_p = 1.0;
    for (idx_i = 0; idx_i < dim; idx_i++)
	  {
        result_s += para[idx_i] * para[idx_i];
        result_p *= cos(para[idx_i]/sqrt(idx_i + 1));
    }
    result_s =result_s/4000.0 - result_p +1;
    // convert to maximum problem
    return(-result_s);
}
