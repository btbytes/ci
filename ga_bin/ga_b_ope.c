#include <stdlib.h>
#include <time.h>
#include <stdio.h>

#include "ga_bin.h"
#include "ga_math.h"

/**************************************************************/
/* Function Prototypes for functions with file level scope    */
/**************************************************************/
static int  search(int si);
static void reorder(int *,int,int);

static void onecross(int, int);
static void twocross(int, int);
static void unicross(int, int);

static void roulette_wheel_scaling(void);
static void binary_tournament(void);
static void ranking(void);
static void boltzmann(void);

/************************************************************/
/*      Function Definitions                                */
/************************************************************/
/************************************************************/
/*     mutation operator routines                           */
/************************************************************/


// Listing 4.13 GA mutation operation C source code
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
void ga_mutate (void)
{
	  int idx_i,idx_j;
	  double prob,rate_m;

    for (idx_j = 0; idx_j < ga_data.popu_size; idx_j++)
    {
        if (idx_j != (ga_data.best_index))
        {
	          for (idx_i = 0; idx_i < ga_data.indi_length; idx_i++)
	          {
		          prob = (rand()%1000)/1000.0;

		          if (( ga_data.mutation_flag == 1) )
		          {
			          rate_m = ga_data.mutation_rate * ga_data.gau[idx_i%(ga_data.bits_per_para)];
		          }
		          else
			          rate_m = ga_data.mutation_rate;

		          if (prob <= rate_m)
		          {
			          if ((ga_data.population[idx_j][idx_i]) == 0)
				          ga_data.population[idx_j][idx_i] = 1;
			          else
				          ga_data.population[idx_j][idx_i] = 0;
		          }
	          }
        }
    }
}

/************************************************************/
/*     Crossover operator routines                          */
/************************************************************/

// Listing 4.12 The ga_crossover() routine
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
void ga_crossover(void)
{
	  int idx_i,idx_j;
	  int *sg;               /* sg[popsize];	 to store the flag of beibg chosen or not */
	  int r_no,kid1,kid2;    /* rno: no. of remaining unchosen individuals */
						     /* k1: first chosen kid; k2 : second */
	  double prob;

	  sg=(int *)calloc(ga_data.popu_size,sizeof(int));

	  /* initialization */
	  /* at the begining, all have not been selected */
	  /* sg[i] store i, and r_no=popsize */
	  for (idx_i = 0; idx_i < ga_data.popu_size; idx_i++)
	  {
		  sg[idx_i] = idx_i;
	  }
	  r_no = ga_data.popu_size;

	  /* begin crossover among population */
	  for (idx_i = 0; idx_i < (ga_data.popu_size/2 + 1); idx_i++)
	  {   /* two kids are chosen each time */
		  if (r_no >= 2) 	                       /* at least two indivuals remain unchosen */
		  {
			  idx_j = search(r_no);	               /* find the first kid */
			  kid1 = sg[idx_j];                    /* index to the first kid */
			  r_no = r_no - 1;                     /* update number of remaining unchosen */
			  reorder(sg,r_no,idx_j);	           /* reorder the sign vector */
			  idx_j = search(r_no); 		       /* find the second kid */
			  kid2 = sg[idx_j];                    /* index to the second kid */
			  r_no = r_no-1;                       /* update number of remaining unchosen */
			  reorder(sg,r_no,idx_j);	           /* reorder the sign vector */

			  prob = (rand()%1000)/1000.0;
			  if (prob <= ga_data.crossover_rate)  /* probability for crossover */
			  {
				  if((kid1 != ga_data.best_index) && (kid2 != ga_data.best_index))
				  {   /* keep  the best */
				      // Listing 4.2 Example of a crossover index
					  switch(ga_data.c_type)
					  {
						  case ONE_POINT_CROSSOVER:
							  onecross(kid1,kid2);
							  break;
						  case UNIFORM_CROSSOVER:
							  unicross(kid1,kid2);
							  break;
						  default:
							  twocross(kid1,kid2);
                            break;
					  }
				  }

			  }
		  }
	  }
	  free(sg);
}/* end of crossover.c */

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
static void onecross (int kid1,int kid2)
{
	  int     cro_pos,                       /* the crossover-position */
			  idx_i;
	  int     tmp;                           /* used for swapping alleles */

	  cro_pos = rand()%(ga_data.indi_length); /* choose the first crossover point */

	  /* crossover through exchanging information */
	  for (idx_i = cro_pos; idx_i < ga_data.indi_length; idx_i++)
	  {
		  tmp = ga_data.population[kid1][idx_i];
		  ga_data.population[kid1][idx_i] = ga_data.population[kid2][idx_i];
		  ga_data.population[kid2][idx_i] = tmp;
	  }
} /* end 1cross */

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
static void twocross (int kid1, int kid2)
{
	  int 	fir,		                /* first crossover-position */
			sec,		                /* second crossover-position */
		    idx_i;
	  int	tmp;		                /* used for swapping values */

	  fir = rand()%(ga_data.indi_length);	/* choose the first crossover point */
	  sec = rand()%(ga_data.indi_length);	/* choose the second crossover point */
	  sec = (2 * sec)%ga_data.indi_length;

    /* sort crossover points */
	  if (fir >= sec)
	  {
		  tmp = fir;
		  fir = sec;
		  sec = tmp;
	  }
	  /* crossover through exchanging information */
	  for (idx_i = fir; idx_i < sec; idx_i++)
	  {
		  tmp = ga_data.population[kid1][idx_i];
		  ga_data.population[kid1][idx_i] = ga_data.population[kid2][idx_i];
		  ga_data.population[kid2][idx_i] = tmp;
	  }
} /* end 2cross */

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
static void unicross (int kid1,int kid2)
{
	  int     idx_i;
	  double  prob;
	  int     tmp;            /* used for swapping alleles */

	  /* crossover through exchanging information */
	  for (idx_i = 0; idx_i < ga_data.indi_length; idx_i++)
	  {
		  prob = (rand()%1000)/1000.0;
		  if (prob > 0.5)
		  {
            tmp = ga_data.population[kid1][idx_i];
            ga_data.population[kid1][idx_i] = ga_data.population[kid2][idx_i];
            ga_data.population[kid2][idx_i] = tmp;
		  }
	  }/* end for */
} /* end uniform crossover */

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
static int search (int si)	/* number of unchosen individuals */
{
    int re;

    re = rand()%(si);
    return(re);
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
static void reorder (int *vec,int si,int ind)
{
    int i;

    if (ind<si)
    {
        for (i=ind;i<si;i++)
        {
    	      *(vec+i)=*(vec+i+1);
        }
    }
}

/************************************************************/
/*   selection operator  routines                           */
/************************************************************/

// The ga_selection() routine
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
void ga_selection (void)
{
    switch (ga_data.s_type)
    {
        case ROULETTE_WHEEL_SCALING:
          roulette_wheel_scaling();
          break;
        case BINARY_TOURNAMENT:
          binary_tournament();
          break;
        case RANKING:
          ranking();
          break;
        case BOLTZMANN:
          boltzmann();
          break;
        default:
          binary_tournament();
          break;
    }
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
static void roulette_wheel_scaling (void)
{
    int idx_i,idx_j,idx_k;
	  double *Prob;	    /* probability vector of individuals to be selected */
	  double Sum;
	  double SumProb;
	  double prob;
	  int *flag;		/* information for selected times */
	  int no;
	  double	mini,max;
    double    *temp_fit;

	  Prob = (double *)calloc(ga_data.popu_size,sizeof(double));
	  flag = (int *)calloc(ga_data.popu_size,sizeof(int));
    temp_fit = (double *)calloc(ga_data.popu_size,sizeof(double));

    /* shift the fitness with minimal fitness 0.1 of the dynamic range */
    mini = minimial(ga_data.fit,ga_data.popu_size);
    max = maximum(ga_data.fit,ga_data.popu_size);
    for (idx_i = 0; idx_i < ga_data.popu_size ; idx_i++)
    {
        temp_fit[idx_i] = ga_data.fit[idx_i] - mini + 0.1 * (max - mini);
    }

	  /* calculation the sum of the fitness */
	  Sum = 0.0;
	  for (idx_i = 0; idx_i < ga_data.popu_size; idx_i++)
	  {
		  Sum += temp_fit[idx_i];
	  }

	  /* calculate the probability vector of individuals */
	  SumProb = 0.0;
	  for (idx_i = 0; idx_i < ga_data.popu_size; idx_i++)
	  {
		  Prob[idx_i]  = temp_fit[idx_i]/Sum + SumProb;
		  SumProb += temp_fit[idx_i]/Sum;
	  }

	  /* generate new population */

	  /* set all flags to be zero, means no one has been selected */
	  for ( idx_i = 0; idx_i < ga_data.popu_size; idx_i++)
	  {
		  flag[idx_i]=0;
	  }

	  flag[ga_data.best_index] = 1;  /* keep the best */

	  /* set the flags for all individuals */
	  for (idx_i = 0; idx_i < (ga_data.popu_size - 1); idx_i++)
	  {
		  /* turn wheel to see which individual is selected */
		  prob = (rand()%1000)/1000.0;
		  idx_k = 0;
		  for (idx_j = 0; idx_j < ga_data.popu_size; idx_j++)
		  {
			  if (prob > Prob[idx_j])
				  idx_k = idx_j + 1 ;
		  }
		  flag[idx_k] += 1;
	  }

	  /* form the new population */
	  for (idx_i = 0; idx_i < ga_data.popu_size; idx_i++)
	  {
		  if (flag[idx_i] == 0)
		  {
			  no = 0;
			  for (idx_j = 0; idx_j < ga_data.popu_size; idx_j++)
			  {
				  if (flag[idx_j] > 1)
				  {
					  idx_k  = idx_j;
					  no = no + 1;
					  break;
				  }
			  }
			  if (no == 0)
			  {
				  printf("something wrong in selection \n");
				  exit(1);
			  }
			  flag[idx_k] = flag[idx_k] - 1;


			  /* copy the selected individual to new individual */
			  for (idx_j = 0; idx_j < ga_data.indi_length; idx_j++)
				  ga_data.population[idx_i][idx_j] = ga_data.population[idx_k][idx_j];

			  /* get the fitness for the new individual */
			  flag[idx_i] += 1;
		  }
	  }
	  /* check the selection */
	  for (idx_i = 0; idx_i < ga_data.popu_size; idx_i++)
	  {
		  if (flag[idx_i] != 1)
		  {
			  printf("something wrong with selection \n");
			  exit(1);
		  }
	  }
	  free(Prob);
	  free(flag);
    free(temp_fit);
}


// Listing 4.11 Binary tournament selection operator
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
static void binary_tournament (void)
{
    int idx_i,idx_j,idx_k;
    int kid_1,kid_2;
    int *flag;		/* information for selected times */
	  int no;

	  flag = (int *)calloc(ga_data.popu_size,sizeof(int));

	  for ( idx_i = 0; idx_i < ga_data.popu_size; idx_i++)
	  {   /* set all flags to be zero, means no one has been selected */
	      flag[idx_i] = 0;
	  }

	  flag[ga_data.best_index] = 1;  /* keep the best */

	  for (idx_i = 0; idx_i < (ga_data.popu_size - 1); idx_i++)
	  {   /* set the flags for all individuals */
        kid_1 = rand()%(ga_data.popu_size);
        kid_2 = rand()%(ga_data.popu_size);

        if ((ga_data.fit[kid_1]) > (ga_data.fit[kid_2]))
        {
            flag[kid_1] +=1;
        }
        else
            flag[kid_2] += 1;
	  }

	  for (idx_i = 0; idx_i < ga_data.popu_size; idx_i++)
	  {   /* form the new population */
	      if (flag[idx_i] == 0)
	      {
		      no = 0;
		      for (idx_j = 0; idx_j < ga_data.popu_size; idx_j++)
		      {
			      if (flag[idx_j] > 1)
			      {
				      idx_k  = idx_j;
				      no = no + 1;
				      break;
			      }
		      }
		      if (no == 0)
		      {
			      printf("something wrong in selection \n");
			      exit(1);
		      }
		      flag[idx_k] = flag[idx_k] - 1;


		      /* copy the selected individual to new individual */
		      for (idx_j = 0; idx_j < ga_data.indi_length; idx_j++)
		      {   /* copy the selected individual to new individual */
			      ga_data.population[idx_i][idx_j] = ga_data.population[idx_k][idx_j];
            }

		      flag[idx_i] += 1;
	      }
	  }

	  for (idx_i = 0; idx_i < ga_data.popu_size; idx_i++)
	  {   /* check the selection */
	      if (flag[idx_i] != 1)
	      {
		      printf("something wrong with selection \n");
		      exit(1);
	      }
	  }
	  free(flag);
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
static void ranking (void)
{

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
static void boltzmann (void)
{

}
