#include <math.h>

/************************************************************/
/* Constants and Macros                                     */
/************************************************************/
#define PI 3.1415

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
double average(double *vec,int dim)
{
	  int	idx_i;
	  double aver;

	  aver=0.0;
	  for (idx_i = 0; idx_i < dim; idx_i++)
		  aver += vec[idx_i];

	  aver =aver/dim;
	  return(aver);
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
double gaussian(double xx)
{
    double resu;        
    
    resu = -0.5 * xx * xx;        
    resu = exp(resu);
    resu = resu/sqrt(2 * PI);

    return(resu);
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
int maximum(double *vec,int dim)
{
	  int idx_i;
	  double max;
	  int inx;

	  inx = 0;
	  max = vec[0];
	  for (idx_i = 1; idx_i < dim; idx_i++)
	  {
		  if (vec[idx_i] > max)
		  {
			  max = vec[idx_i];
			  inx = idx_i;
		  }
	  }
	  return(inx);
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
double minimial(double *vec,int dim)
{
	  int idx_i;
	  double mini;

	  mini = vec[0];
	  for (idx_i = 1; idx_i < dim; idx_i++)
	  {
		  if (vec[idx_i] < mini)
			  mini = vec[idx_i];
	  }
	  return(mini);
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
double variance (double *vec,double ave,int len)
{
	  int idx_i;
	  double sum,tem;

	  sum=0.0;
	  for (idx_i = 0; idx_i < len; idx_i++)
	  {
	  	  tem = vec[idx_i] - ave;
	  	  tem = tem * tem;
	  	  sum += tem;
	  }
	  sum = sum/len;
	  sum = sqrt(sum);
	  return(sum);
}

