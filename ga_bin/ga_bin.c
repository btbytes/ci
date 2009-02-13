/************************************************************/
/* Include files                                            */
/************************************************************/
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <time.h>

#include "ga_bin.h"
#include "fitness.h"
#include "ga_b_ope.h"
#include "ga_math.h"

/************************************************************/
/* Static Variable and Const Variable with File level scope */
/************************************************************/
static time_t	begin;
static time_t finish;

/************************************************************/
/* Enumerations and Structures and Typedefs                 */
/************************************************************/

/************************************************************/
/* Global Variable Declarations                             */
/************************************************************/
ga_binary_data_type ga_data;
ga_env_data_type ga_env_data;

/************************************************************/
/* Function Prototypes for functions with file level scope  */
/************************************************************/
static void ga_read_parameter(char *);
static void ga_allocate_memory(void);
static void ga_store_results(void);
static void ga_free_memory(void);
static void ga_initialization(void);

static void savePara(FILE *);
/************************************************************/
/*      Function Definitions                                */
/************************************************************/
// Listing 4.8 The main() routine of the binary GA implementation

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
void GA_Main_Loop (void)
{
    int c_break(void);

	  /* begin generation  */
	  while ( (++(ga_data.gene_index)) < ga_data.iter_max)
	  {
		  ga_evaluate();
		  ga_selection();
		  ga_crossover();
         	  ga_mutate();
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
void GA_Start_Up (char *datFile)
{
    int idx_i;
    int true;
	  char condition;

    /* get parameters from runFile */
	  begin=time(NULL);

	  ga_read_parameter(datFile);

	  ga_data.indi_length = ga_env_data.dimension  * ga_data.bits_per_para;      /* length of indiviudal */

	  savePara(stderr);        /* display the parameters on the screen */

	  do                               /* check to see whether these parameters OK */
	  {
		  printf("check, then selection, c: continue; q:quit\n");
		  scanf("%c",&condition);
		  switch(condition)
		  {
			  case 'c': true=0;
				  break;
			  case 'C': true=0;
				  break;
			  case 'q': exit(1);
			  case 'Q': exit(1);
			  default: true=1;

		  }
	  } while (true==1);

	  /* assign space to matrix and vector */
    ga_allocate_memory();

	  /* itilialization of population */
	  ga_initialization();

	  /* calculate Gaussian fun. as mutation probability */
	  for (idx_i = 0; idx_i < ga_data.bits_per_para; idx_i++)
		  ga_data.gau[idx_i] = gaussian(sqrt(idx_i));

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
void GA_Clean_Up (void)
{
    ga_store_results();
    ga_free_memory();
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
static void  ga_read_parameter (char *fileName)
{
	  FILE    *fp;

	  if ((fp=fopen(fileName,"rt"))==NULL)
	  {
		  fprintf(stderr,"cannot open %s file \n",fileName);
		  exit(1);
	  }

	  if (fscanf(fp,"%s",ga_env_data.resultFile)==NULL)
	  {
		  fprintf(stderr,"Error reading errorFileName in file %s\n",fileName);
		  exit(1);
	  }

    if (fscanf(fp,"%d",&(ga_env_data.dimension))==NULL)
	  {
		  fprintf(stderr,"Error reading N in file %s\n",fileName);
		  exit(1);
	  }

    if (fscanf(fp,"%d",&(ga_env_data.function))==NULL)
	  {
		  fprintf(stderr,"Error reading function type in file %s\n",fileName);
		  exit(1);
	  }

	  if (fscanf(fp,"%d",&(ga_data.iter_max))==NULL)
	  {
		  fprintf(stderr,"Error reading Iteration in file %s\n",fileName);
		  exit(1);
	  }

	  if (fscanf(fp,"%d",&(ga_data.bits_per_para))==NULL)
	  {
		  fprintf(stderr,"Error reading K in file %s\n",fileName);
		  exit(1);
	  }

	  if (fscanf(fp,"%d",&(ga_data.popu_size))==NULL)
	  {
		  fprintf(stderr,"Error reading popsize in file %s\n",fileName);
		  exit(1);
	  }

	  if (fscanf(fp,"%lf",&(ga_data.crossover_rate))==NULL)
	  {
		  fprintf(stderr,"Error reading c_rate in file %s\n",fileName);
		  exit(1);
	  }

	  if (fscanf(fp,"%lf",&(ga_data.mutation_rate))==NULL)
	  {
		  fprintf(stderr,"Error reading m_rate in file %s\n",fileName);
		  exit(1);
	  }

	  if (fscanf(fp,"%lf",&(ga_data.termination_criterion))==NULL)
	  {
		  fprintf(stderr,"Error reading crition in file %s\n",fileName);
		  exit(1);
	  }

	  if (fscanf(fp,"%d",&(ga_data.mutation_flag))==NULL)
	  {
		  fprintf(stderr,"Error reading flag_m in file %s\n",fileName);
		  exit(1);
	  }

	  if (fscanf(fp,"%d",&(ga_data.c_type))==NULL)
	  {
		  fprintf(stderr,"Error reading flag_c in file %s\n",fileName);
		  exit(1);
	  }

    if (fscanf(fp,"%d",&(ga_data.s_type))==NULL)
	  {
		  fprintf(stderr,"Error reading selection_type in file %s\n",fileName);
		  exit(1);
	  }
	  fclose(fp);

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
static void ga_allocate_memory (void)
{
    int idx_i;
    /* assign space to matrix and vector */
	  ga_data.population = (unsigned char **)calloc(ga_data.popu_size,sizeof(unsigned char *));
	  for (idx_i = 0; idx_i < ga_data.popu_size; idx_i++)
		  ga_data.population[idx_i] = (unsigned char *)calloc(ga_data.indi_length,sizeof(unsigned char));

	  ga_data.fit=(double *)calloc(ga_data.popu_size,sizeof(double));

	  ga_data.gau=(double *)calloc(ga_data.bits_per_para,sizeof(double));
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
static void ga_free_memory (void)
{
    int idx_i;
    /* release the allocated space */
	  free(ga_data.fit);
	  for (idx_i = 0; idx_i < ga_data.popu_size; idx_i++)
		  free(ga_data.population[idx_i]);
	  free(ga_data.population);
	  free(ga_data.gau);
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
static void ga_store_results (void)
{
    int idx_i;
    FILE *fp1;

    if ((fp1=fopen(ga_env_data.resultFile,"w"))==NULL)
	  {
	      fprintf(stderr,"cannot open %s for writing\n",ga_env_data.resultFile);
	      exit(1);
	  }
    savePara(fp1);

	  fprintf(fp1,"generation: %d\n",ga_data.gene_index);
    fprintf(fp1,"best fitness: %lf\n",ga_data.fit[ga_data.best_index]);
    fprintf(fp1,"variance: %lf\n\n",ga_data.fit_variance);
    fprintf(fp1,"fitness values:\n");
    for (idx_i = 0; idx_i < ga_data.popu_size ; idx_i++)
    {
        fprintf(fp1,"fit[%2d]: %lf\n",idx_i,ga_data.fit[idx_i]);
    }

	  fprintf(fp1,"begin time at: %s\n",ctime(&begin));
	  finish=time(NULL);
	  fprintf(fp1,"finish time at: %s\n",ctime(&finish));
	  fclose(fp1);

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
static void savePara (FILE * ff)
{
	  fprintf(ff,"\tresultFile ..........................%s\n",ga_env_data.resultFile);
    fprintf(ff,"\tfunction type .......................%d\n",ga_env_data.function);
    fprintf(ff,"\tinput dim ...........................%d\n",ga_env_data.dimension);
	  fprintf(ff,"\tmax. No. generation..................%d\n",ga_data.iter_max);
	  fprintf(ff,"\tK bits for eachPara..................%d\n",ga_data.bits_per_para);

	  fprintf(ff,"\tpopu_size............................%d\n",ga_data.popu_size);
	  fprintf(ff,"\tindividual length ...................%d\n",ga_data.indi_length);
	  fprintf(ff,"\tcrossover rate ......................%f\n",ga_data.crossover_rate);
	  fprintf(ff,"\tmutation rate  ......................%f\n",ga_data.mutation_rate);
	  fprintf(ff,"\tterm. criterion .....................%f\n",ga_data.termination_criterion);
	  fprintf(ff,"\tflag_m (1:bit position;0:cons) ......%d\n",ga_data.mutation_flag);
	  fprintf(ff,"\tc_type (0:one,1:unif,2:two)..........%d\n",ga_data.c_type);
	  fprintf(ff,"\tselection type ......................%d\n",ga_data.s_type);

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
int c_break(void)
{
	return 0;
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
static void ga_initialization (void)
{
	  int       idx_i;
	  int	    idx_j;
    double 	prob;

    int       stime;
    long      ltime;

    ltime = time(NULL);
    stime = (unsigned) ltime/2;
	  srand(stime);

	  /* generate the initialzied population */

	  for (idx_i = 0; idx_i < ga_data.popu_size; idx_i++)
	  {
		  for (idx_j = 0; idx_j < ga_data.indi_length; idx_j++)
		  {
			  prob = (rand()%1000)/1000.0;
			  if (prob>0.5)
				  ga_data.population[idx_i][idx_j] = 1;
			  else
				  ga_data.population[idx_i][idx_j] = 0;
		  }
	  }
    ga_data.gene_index = -1;            // not start yet
}


/************************************************************/
/*  Debug routines                                          */
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
void debug_print_result(void)
{
    int idx_i;
    FILE *fp1;

    if ((fp1=fopen("debug","a"))==NULL)
	  {
	      fprintf(stderr,"cannot open debug for writing\n");
	      exit(1);
	  }

    fprintf(fp1, "gene %d  best: %d\t%lf\n",ga_data.gene_index,ga_data.best_index,ga_data.fit[ga_data.best_index]);
    fprintf(fp1,"\n fitness values:\n");
    for (idx_i = 0; idx_i < ga_data.popu_size; idx_i++)
    {
        fprintf(fp1,"%lf\n",ga_data.fit[idx_i]);
    }
    fclose(fp1);
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
void debug_selection (int* flag)
{
    int idx_i;
    FILE *fp1;

    if ((fp1=fopen("debug_s","a"))==NULL)
	  {
	      fprintf(stderr,"cannot open debug_s for writing\n");
	      exit(1);
	  }

    fprintf(fp1, "gene %d  best: %d\t%lf\n",ga_data.gene_index,ga_data.best_index,ga_data.fit[ga_data.best_index]);
    fprintf(fp1,"\n flags:\n");
    for (idx_i = 0; idx_i < ga_data.popu_size; idx_i++)
    {
        fprintf(fp1,"%d\n",flag[idx_i]);
    }
    fclose(fp1);
}
