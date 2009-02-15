#include "headfile.h"
#include "definiti.h"
#include "psostate.h"
#include "bp.h"
#include "mem_loc.h"


/**************************************************************/
/* Static Variable and Const Variable with File level scope   */
/**************************************************************/
static int NUM_PSO;                                    // num of PSOs needs to be specified
static int PSO_UPDATE_PBEST_EACH_CYCLE_FLAG;           // needs to be specified, TRUE when PSOs serve as envirnment to the other PSOs 

static BOOLEAN pso_update_pbest_each_cycle_pending;    // 
static PSO_State_Type PSO_current_state;               // current state of the current PSO
static int cur_pso;                                    // current index of PSOs
static int total_cycle_of_PSOs;                        // total cycles of running PSOs
static int pso_cycle_index = 0;                        // index of cycles

static PSO_Type *psos;                                 // pointer to the array of PSOs

static double paper_results[50];                       // paper use only
static int counter;                                    // paper use only   
                
/**************************************************************/
/* Function Prototypes for functions with file level scope    */
/**************************************************************/

static void PSO_update_inertia_weight(void);           // state handlers
static void PSO_evaluate(void);
static void PSO_update_global_best(void);
static void PSO_update_local_best(void);
static void PSO_update_velocity(void);
static void PSO_update_position(void);
static void PSO_goal_reach_judge(void);
static void PSO_next_generation(void);
static void PSO_update_pbest_each_cycle(void);
static void PSO_next_pso(void);
static void PSOs_done(void); 

static void pso_initialize(void);                      // initialization
static void PSO_random_symmetry_initialize(void);
static void PSO_random_asymmetry_initialize(void);

static void simple_min(void);                          // evaluation functions
static void simple_max(void);
static void michalewicz_min(void);
static void michalewicz_max(void);
static void michalewicz_2_min(void);
static void michalewicz_2_max(void);
static void g1_min(void);
static void g1_max(void);
static void g7_min(void);
static void g7_max(void);
static void g9_min(void);
static void g9_max(void);
static void f6(void);
static void sphere(void);
static void rosenbrock(void);
static void rastrigrin(void);
static void griewank(void);

static void constant_iw(void);                         // update inertia weight methods
static void linear_iw(void);
static void fuzzy_two_iw(void);
static void fuzzy_four_iw(void);
static void noise_addition_iw(void);

static void read_pso_parameters(char *dataFile);       // read PSO parameters from I/O file
static void allocate_pso_memory(void);                 // allocate PSOs memory spaces
static void free_pso_memory(void);                     // free PSOs memory spaces
static void pso_store_results(void);                   // write PSO results to I/O file
//static void pso_store_paper_results(void);

static void pso_state_handler(int);                    // PSO state handle routine
static void PSO_initialize_handler(int);               // PSO initialization 
static void evaluate_functions(int);                   // PSO evaluation functions
static void iw_update_methods(int);                    // PSO update inertai weight methods

static void best_solution_values(int);                 // function value and constrant values for best solution at the current generation 
static void g1_best_solution(void);                    // for G1
static void g7_best_solution(void);                    //     G7
static void g9_best_solution(void);                    //     G9

/**************************************************************/
/*      Function Definitions                                  */    
/**************************************************************/      

/**************************************************************/
/*   PSO Start and clean routines and interfaces              */    
/**************************************************************/
/**************************************************************
*     Function:
*
*   Parameters:
*
*      Returns:
*
*  Description:
*
***************************************************************/
void PSO_Start_Up (char *dataFile)
{
    read_pso_parameters(dataFile);
    allocate_pso_memory();            // allocate memory for particles
}

/*************************************************************
*     Function:
*
*   Parameters:
*
*      Returns:
*
*  Description:
*
*************************************************************/
void PSO_Clean_Up (void)
{
    free_pso_memory();                // free memory space of particles
} 

/************************************************************/
/*      PSO functons                                        */    
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
void PSO_Main_Loop (void)
{
    BOOLEAN running;     

	 pso_initialize();
    // start running PSOs
    while ((pso_cycle_index++) < total_cycle_of_PSOs)
    {   
        running = TRUE;    
        while (running)
        { 
            if (PSO_current_state == PSOS_DONE)
            {
                running = FALSE;                          // end running this cycle of PSO         
            } 
            pso_state_handler(PSO_current_state);         // run state handler             
        }
    }
    pso_store_results();                                  // output results
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
static void read_pso_parameters (char *dataFile)
{
    int idx_i, idx_j;
    FILE  *frun;

    // open the runfile to input parameters
	  if ((frun=fopen(dataFile,"r"))==NULL)
	  {
		  printf("Cant read run file");
		  exit(1);
	  }

    fscanf(frun, "%d",&NUM_PSO);                                      // total number of PSOs
    // allocate memory for array of the PSOs
    psos = malloc(NUM_PSO * sizeof(PSO_Type));
    if (psos == NULL)
    {
        printf("Allocating memory for PSOs failed -- aborting\n");
        exit(1);  
    }

    fscanf(frun, "%d",&PSO_UPDATE_PBEST_EACH_CYCLE_FLAG);             // whether to update pbest before moving to run another pso
    fscanf(frun, "%d",&total_cycle_of_PSOs);                          // total cycles of running PSOs 

	 psos[0].dimension = BP_Get_PSO_Dimension();
    // Read PSOs' parameters from runfile
    for (idx_i=0; idx_i<NUM_PSO;idx_i++ )
	 {
        //read environment data
		  fscanf(frun, "%d",&(psos[idx_i].env_data.opti_type  ));       // optimization type: min or max
		  fscanf(frun, "%d",&(psos[idx_i].env_data.function_type  ));   // evalutiona function
		  fscanf(frun, "%d",&(psos[idx_i].env_data.iw_method  ));       // inertia weight update method
		  fscanf(frun, "%d",&(psos[idx_i].env_data.init_type));         // initialization type: sym/asym
        fscanf(frun, "%f",&(psos[idx_i].env_data.init_range.left  )); // left initialization range
        fscanf(frun, "%f",&(psos[idx_i].env_data.init_range.right  ));// right initialization range
        fscanf(frun, "%f",&(psos[idx_i].env_data.max_velocity  ));    // maximum velocity
        fscanf(frun, "%f",&(psos[idx_i].env_data.max_position  ));    // maximum position
        fscanf(frun, "%d",&(psos[idx_i].env_data.max_generation  ));  // max number of generations

        //read PSO data
        fscanf(frun, "%d",&(psos[idx_i].popu_size ));                 // population size
        //fscanf(frun, "%d",&(psos[idx_i].dimension ));                 // dimension
        fscanf(frun, "%f",&(psos[idx_i].init_inertia_weight ));       // initial inertia weight
        
        // read boundary flag
        fscanf(frun, "%d",&(psos[idx_i].env_data.boundary_flag ));
        if (psos[idx_i].env_data.boundary_flag)
        {
            // allocate memory for boundaries
            FVectorAllocate(&(psos[idx_i].env_data.low_boundaries),  psos[idx_i].dimension);
            FVectorAllocate(&(psos[idx_i].env_data.up_boundaries),   psos[idx_i].dimension);

            //read boundaries
            for (idx_j = 0 ; idx_j < psos[idx_i].dimension ; idx_j++)
            {
                fscanf(frun, "%f",&(psos[idx_i].env_data.low_boundaries[idx_j]));
                fscanf(frun, "%f",&(psos[idx_i].env_data.up_boundaries[idx_j]));
            }  
        }
        psos[idx_i].inertia_weight = psos[idx_i].init_inertia_weight;
    }
    // close runfile
    fclose(frun);
	 if (psos[0].env_data.function_type != BP_EXP_MIN)
	 {
		  printf("not BP explanantion facility problem\n");
        exit(1);
	 }
	 //psos[0].dimension = BP_Get_PSO_Dimension();
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
static void allocate_pso_memory (void)
{
    int idx_i;

    for (idx_i =0 ; idx_i<NUM_PSO ;idx_i++ )
    {
        FVectorAllocate(&(psos[idx_i].pbest_values),          psos[idx_i].popu_size);
        FMatrixAllocate(&(psos[idx_i].velocity_values),       psos[idx_i].popu_size,  psos[idx_i].dimension);
        FMatrixAllocate(&(psos[idx_i].position_values),       psos[idx_i].popu_size,  psos[idx_i].dimension);  
        FMatrixAllocate(&(psos[idx_i].pbest_position_values), psos[idx_i].popu_size,  psos[idx_i].dimension);
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
//static void pso_store_results (void)
//{
//    int idx_i, idx_j, idx_k;
//    FILE  *fout;
//
//    // open file for write
//    if ((fout=fopen("result","w"))==NULL)
//	  {
//		  printf("Cant open file for write");
//		  exit(1);
//	  }
//
//    // output the best position values
//    fprintf(fout,"the position value of the best individual\n");
//    for (idx_j =0;idx_j < NUM_PSO ; idx_j++)
//    {
//        fprintf(fout,"PSO Number %d :\n",idx_j);
//        for (idx_i=0;idx_i< psos[idx_j].dimension; idx_i++)
//        {
//           fprintf(fout,"%d:  %f\n",idx_i,psos[idx_j].pbest_position_values[psos[idx_j].global_best_index][idx_i]);
//        }
//        fprintf(fout,"\n\n");
//    }
//
//    // output fitness values
//    fprintf(fout,"fitness values in pbest\n");
//    
//    for (idx_j =0;idx_j < NUM_PSO ; idx_j++)
//    {
//        fprintf(fout,"PSO Number %d :\n",idx_j);
//        for (idx_i=0;idx_i< psos[idx_j].popu_size; idx_i++)
//        {
//           fprintf(fout,"%d:  %f\n",idx_i,psos[idx_j].pbest_values[idx_i]);
//        }
//        fprintf(fout,"%dth is the best fitness %f\n",psos[idx_j].global_best_index,psos[idx_j].pbest_values[psos[idx_j].global_best_index]);
//    }
//    
//    // output position values
//    for (idx_j =0;idx_j < NUM_PSO ; idx_j++)
//    {
//        fprintf(fout,"PSO Number %d :\n",idx_j);
//
//        for (idx_i=0;idx_i< psos[idx_j].popu_size; idx_i++)
//        {
//             for (idx_k = 0; idx_k < psos[idx_j].dimension; idx_k++ )
//             {
//                  fprintf(fout,"%d:%d  %f\n",idx_i,idx_k,psos[idx_j].pbest_position_values[idx_i][idx_k]);
//             }
//        }
//    }
//    fclose(fout);
//}

/************************************************************
*     Function:
*
*   Parameters:
*
*      Returns:
*
*  Description:  for paper use only
*
*************************************************************/
static void pso_store_results (void)
{
    int idx_i, idx_j;
	 FILE  *fout;
	 float* target;
	 int    dim_t;

	 // open file for write
	 if ((fout=fopen(BP_Get_Result_File_Name(),"w"))==NULL)
	  {
		  printf("Cant open file for write PSO  results");
		  exit(1);
	  }

	 dim_t = BP_Get_Target_Dimension();
	 target = calloc(dim_t,sizeof(float));
    BP_Get_Target(target);

	 // output the best position values
	 //fprintf(fout,"the position value of the best individual\n");
	 for (idx_j =0;idx_j < psos[0].popu_size ; idx_j++)
	 {
		  //fprintf(fout,"PSO Number %d :\n",idx_j);
		  for (idx_i=0;idx_i< psos[0].dimension; idx_i++)
		  {
			  fprintf(fout,"%f\t",psos[0].pbest_position_values[idx_j][idx_i]);
		  }
		  for (idx_i = 0; idx_i < dim_t; idx_i++)
		  {
				 fprintf(fout,"%f\t",target[idx_i]);
		  }
		  fprintf(fout,"%f\t",psos[0].pbest_values[idx_j]);
		  fprintf(fout,"\n");
	 }
    
	 fclose(fout);
	 free(target);
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
static void free_pso_memory (void)
{
    int idx_i;
    for (idx_i =0 ; idx_i<NUM_PSO ;idx_i++ )
    {
        free(psos[idx_i].pbest_values);
        FMatrixFree((psos[idx_i].velocity_values),        psos[idx_i].popu_size  );
        FMatrixFree((psos[idx_i].position_values),        psos[idx_i].popu_size  );
		  FMatrixFree((psos[idx_i].pbest_position_values),  psos[idx_i].popu_size  );
        if (psos[idx_i].env_data.boundary_flag)
        {
           free(psos[idx_i].env_data.low_boundaries );
           free(psos[idx_i].env_data.up_boundaries );
        }
    }
    free(psos);
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
static void pso_initialize (void)
{
    int idx_i,idx_j;
    int stime;
    long ltime;

    // get the current calendar time 
    ltime = time(NULL);
    stime = (unsigned) ltime/2;
    srand(stime);

    for (idx_i = 0; idx_i <NUM_PSO ; idx_i++)
    {
        cur_pso = idx_i;
        for (idx_j = 0; idx_j < (psos[cur_pso].popu_size) ; idx_j++ )
        {
            psos[cur_pso].popu_index = idx_j;
            
            PSO_initialize_handler(psos[cur_pso].env_data.init_type);
        }
    } 

    cur_pso = 0;                                     // done with initialization, move to the first PSO
    PSO_current_state = PSO_EVALUATE;                // move to the EVALUATE state
    psos[cur_pso].popu_index = 0;                    // satrt with the first particle
    psos[cur_pso].gene_index = 0;                    // start from the first generation of the first population
	 pso_cycle_index = 0;                             // start from the first cycle

    psos[0].criterion = BP_Get_Criterion();
}

/*************************************************/
/*      PSO State Handlers                       */    
/*************************************************/ 

/*************************************************
*     Function:
*
*   Parameters:
*
*      Returns:
*
*  Description:
*
**************************************************/
static void pso_state_handler (int state_index)
{
    switch (state_index)
    {
        case PSO_UPDATE_INERTIA_WEIGHT:
          PSO_update_inertia_weight();
          break;
        case PSO_EVALUATE:
          PSO_evaluate();
          break;
        case PSO_UPDATE_GLOBAL_BEST:
          PSO_update_global_best();
          break;
        case PSO_UPDATE_LOCAL_BEST:
          PSO_update_local_best();
          break;
        case PSO_UPDTAE_VELOCITY:
          PSO_update_velocity();
          break;
        case PSO_UPDATE_POSITION:
          PSO_update_position();
          break;
        case PSO_GOAL_REACH_JUDGE:
          PSO_goal_reach_judge();
          break;
        case PSO_NEXT_GENERATION:
          PSO_next_generation();
          break;
        case PSO_UPDATE_PBEST_EACH_CYCLE:
          PSO_update_pbest_each_cycle();
          break;
        case PSO_NEXT_PSO:
          PSO_next_pso();
          break;
        case PSOS_DONE:
          PSOs_done();
          break;
        default:          
          break;
    }
}

/*************************************************
*     Function:
*
*   Parameters:
*
*      Returns:
*
*  Description:
*
**************************************************/
static void PSO_update_inertia_weight (void)
{
    iw_update_methods(psos[cur_pso].env_data.iw_method);
    PSO_current_state = PSO_EVALUATE;                // move to the next state
    psos[cur_pso].popu_index = 0;                    // satrt with the first particle

}

/*************************************************
*     Function:
*
*   Parameters:
*
*      Returns:
*
*  Description:
*
**************************************************/
static void PSO_evaluate (void)
{  
    if ((psos[cur_pso].popu_index) < (psos[cur_pso].popu_size))
    {
        evaluate_functions(psos[cur_pso].env_data.function_type);   
		  PSO_current_state = PSO_UPDATE_LOCAL_BEST;
    }
    else            // done with evaluation, move to the next state
    {
        PSO_current_state = PSO_GOAL_REACH_JUDGE ;
        psos[cur_pso].popu_index = 0;
    }
}

/*************************************************
*     Function:
*
*   Parameters:
*
*      Returns:
*
*  Description:
*
**************************************************/
static void PSO_update_local_best (void)
{
	 int idx_i;

	 // here update local best
	 if ( (psos[cur_pso].env_data.opti_type) == MINIMIZATION)
	 {   // minimization problem
        if ( (pso_cycle_index == 1) && ((psos[cur_pso].gene_index) == 0 ) )
        {
              psos[cur_pso].global_best_index = 0;
              psos[cur_pso].pbest_values[psos[cur_pso].popu_index] =  psos[cur_pso].eva_fun_value;
        }
		  if ((psos[cur_pso].eva_fun_value) < (psos[cur_pso].pbest_values[psos[cur_pso].popu_index] ))
		  {
				psos[cur_pso].pbest_values[psos[cur_pso].popu_index] = psos[cur_pso].eva_fun_value;
				for (idx_i = 0; idx_i < (psos[cur_pso].dimension) ;idx_i++ )
				{
					 (psos[cur_pso].pbest_position_values[psos[cur_pso].popu_index ][idx_i ] ) =  (psos[cur_pso].position_values[psos[cur_pso].popu_index ][idx_i ] );
				}
		  }
	 }
	 else
	 {   // maximization problem
        if ( (pso_cycle_index == 1) && ((psos[cur_pso].gene_index) == 0 ) )
        {
              psos[cur_pso].global_best_index = 0;
              psos[cur_pso].pbest_values[psos[cur_pso].popu_index] =  psos[cur_pso].eva_fun_value;
        } 
		  if ((psos[cur_pso].eva_fun_value) > (psos[cur_pso].pbest_values[psos[cur_pso].popu_index] ))
		  {
				psos[cur_pso].pbest_values[psos[cur_pso].popu_index] = psos[cur_pso].eva_fun_value;
				for (idx_i = 0; idx_i < (psos[cur_pso].dimension) ;idx_i++ )
				{
					 (psos[cur_pso].pbest_position_values[psos[cur_pso].popu_index ][idx_i ] ) =  (psos[cur_pso].position_values[psos[cur_pso].popu_index ][idx_i ] );
				}
		  }
	 }
	 PSO_current_state = PSO_UPDATE_GLOBAL_BEST ;
}

/*************************************************
*     Function:
*
*   Parameters:
*
*      Returns:
*
*  Description:
*
**************************************************/
static void PSO_update_global_best (void)
{
	 // here update global best
    if ( (psos[cur_pso].env_data.opti_type) == MINIMIZATION)
	  {   // minimization problem
		  if ((psos[cur_pso].eva_fun_value) < (psos[cur_pso].pbest_values[psos[cur_pso].global_best_index ] ))
		  {
				psos[cur_pso].global_best_index  =  psos[cur_pso].popu_index;
		  }
	  }
    else
	  {   // maximization problem
        if ((psos[cur_pso].eva_fun_value) > (psos[cur_pso].pbest_values[psos[cur_pso].global_best_index ] ))
		  {
				psos[cur_pso].global_best_index  =  psos[cur_pso].popu_index;
		  }
    }
    
	 PSO_current_state =  PSO_UPDTAE_VELOCITY;
}

/*************************************************
*     Function:
*
*   Parameters:
*
*      Returns:
*
*  Description:
*
**************************************************/
static void PSO_update_velocity (void)
{
   int idx_i;
	// here update velocity
	for (idx_i = 0; idx_i < (psos[cur_pso].dimension) ;idx_i++ )
	{
		 psos[cur_pso].velocity_values[psos[cur_pso].popu_index ][idx_i ] =
				(psos[cur_pso].inertia_weight) *  (psos[cur_pso].velocity_values[psos[cur_pso].popu_index ][idx_i ] )
				+  2*((rand()%1000)/1000.0) * (psos[cur_pso].pbest_position_values[psos[cur_pso].popu_index ][idx_i ] - psos[cur_pso].position_values[psos[cur_pso].popu_index ][idx_i ]  )
				+  2*((rand()%1000)/1000.0) * (psos[cur_pso].pbest_position_values[psos[cur_pso].global_best_index ][idx_i ] - psos[cur_pso].position_values[psos[cur_pso].popu_index ][idx_i ]  );

       if (psos[cur_pso].env_data.boundary_flag)
       {
          if ( fabs(psos[cur_pso].velocity_values[psos[cur_pso].popu_index ][idx_i ] ) > (0.5*((psos[cur_pso].env_data.up_boundaries[idx_i]) - (psos[cur_pso].env_data.low_boundaries[idx_i] ))))
          {
                if ((psos[cur_pso].velocity_values[psos[cur_pso].popu_index ][idx_i ] ) >= 0)
                {
                     psos[cur_pso].velocity_values[psos[cur_pso].popu_index ][idx_i ] = 0.5*((psos[cur_pso].env_data.up_boundaries[idx_i]) - (psos[cur_pso].env_data.low_boundaries[idx_i] ));
                }
                else
                {
                     psos[cur_pso].velocity_values[psos[cur_pso].popu_index ][idx_i ] = -0.5*((psos[cur_pso].env_data.up_boundaries[idx_i]) - (psos[cur_pso].env_data.low_boundaries[idx_i] ));
                }
          }
       }
       else
       {
		    if ( (psos[cur_pso].velocity_values[psos[cur_pso].popu_index ][idx_i ] ) > (psos[cur_pso].env_data.max_velocity  ) )
		    {
				   psos[cur_pso].velocity_values[psos[cur_pso].popu_index ][idx_i ] = psos[cur_pso].env_data.max_velocity;
		    }
		    else if ( (psos[cur_pso].velocity_values[psos[cur_pso].popu_index ][idx_i ] ) < (-(psos[cur_pso].env_data.max_velocity ) ) )
		    {
				   psos[cur_pso].velocity_values[psos[cur_pso].popu_index ][idx_i ] = -(psos[cur_pso].env_data.max_velocity );
		    }
       }
	}

	PSO_current_state =  PSO_UPDATE_POSITION;       // go to the PSO_UPDATE_POSITION state

}

/*************************************************
*     Function:
*
*   Parameters:
*
*      Returns:
*
*  Description:
*
**************************************************/
static void PSO_update_position (void)
{
   int idx_i;
   for (idx_i = 0; idx_i < (psos[cur_pso].dimension) ;idx_i++ )
	 {
		  psos[cur_pso].position_values[psos[cur_pso].popu_index ][idx_i ] +=  psos[cur_pso].velocity_values[psos[cur_pso].popu_index ][idx_i ];
        if (psos[cur_pso].env_data.boundary_flag)
        {
            if ((psos[cur_pso].position_values[psos[cur_pso].popu_index ][idx_i ]) < (psos[cur_pso].env_data.low_boundaries[idx_i] ))
            {
                psos[cur_pso].position_values[psos[cur_pso].popu_index ][idx_i ] =  psos[cur_pso].env_data.low_boundaries[idx_i] + ((psos[cur_pso].env_data.up_boundaries[idx_i] - psos[cur_pso].env_data.low_boundaries[idx_i] ) * ((rand()%1000)/100000.0));  // low boundary + noise
            }
            else  if ((psos[cur_pso].position_values[psos[cur_pso].popu_index ][idx_i ]) > (psos[cur_pso].env_data.up_boundaries[idx_i]))
            {
                psos[cur_pso].position_values[psos[cur_pso].popu_index ][idx_i ] =  psos[cur_pso].env_data.up_boundaries[idx_i] -  ((psos[cur_pso].env_data.up_boundaries[idx_i] - psos[cur_pso].env_data.low_boundaries[idx_i] ) * ((rand()%1000)/100000.0));   // up boundary - noise
            }
        }
	 }
   
	PSO_current_state =  PSO_EVALUATE;       // go back to the PSO_EVALUATE state
	(psos[cur_pso].popu_index)++;
}

/*************************************************
*     Function:
*
*   Parameters:
*
*      Returns:
*
*  Description:
*
**************************************************/
static void PSO_goal_reach_judge (void)
{
    PSO_current_state = PSO_NEXT_GENERATION;
}

/*************************************************
*     Function:
*
*   Parameters:
*
*      Returns:
*
*  Description:
*
**************************************************/
static void PSO_next_generation (void)
{
//printf("best fitness = %f\tbest index = %d\n",psos[cur_pso].pbest_values[psos[cur_pso].global_best_index ], psos[cur_pso].global_best_index);
	 if (((++(psos[cur_pso].gene_index)) < (psos[cur_pso].env_data.max_generation  ) )&&(psos[cur_pso].pbest_values[psos[cur_pso].global_best_index ] > psos[cur_pso].criterion))
    { // next generation of the same population of PSO
		  PSO_current_state = PSO_UPDATE_INERTIA_WEIGHT;
printf("%f\n",psos[0].pbest_values[psos[0].global_best_index]);
    }
    else
    {   
        if ( (++cur_pso ) >= NUM_PSO)
        { // end of the cycle
            cur_pso = 0;                                   // move to the first pso 
        }
        PSO_current_state = PSO_UPDATE_PBEST_EACH_CYCLE;   // move to the next state
        psos[cur_pso].popu_index = 0;
    }    
}

/*************************************************
*     Function:
*
*   Parameters:
*
*      Returns:
*
*  Description:
*
**************************************************/
static void PSO_update_pbest_each_cycle (void)
{
    if (PSO_UPDATE_PBEST_EACH_CYCLE_FLAG)
    {
        pso_update_pbest_each_cycle_pending = TRUE;
        if ((psos[cur_pso].popu_index) < (psos[cur_pso].popu_size))
        {
            evaluate_functions(psos[cur_pso].env_data.function_type); 
            psos[cur_pso].pbest_values[psos[cur_pso].popu_index] = psos[cur_pso].eva_fun_value;      // update pbest
            psos[cur_pso].popu_index++;
        }
        else            // done with evaluation, move to the next state
        {
            PSO_current_state =  PSO_NEXT_PSO;
            pso_update_pbest_each_cycle_pending = FALSE;
        }
    }
    else
    {
        PSO_current_state =  PSO_NEXT_PSO;
    }
}

/*************************************************
*     Function:
*
*   Parameters:
*
*      Returns:
*
*  Description:
*
**************************************************/
static void PSO_next_pso (void)
{
    // next PSO
    if ( cur_pso > 0)
    {
        PSO_current_state = PSO_EVALUATE;       // move to the EVALUATE state for the next pso in the same cycle
    }
    else
    { // all the PSOs have been processed
        PSO_current_state = PSOS_DONE;         // end of the cycle
    }
    psos[cur_pso].popu_index = 0;              // satrt with the first particle
    psos[cur_pso].gene_index = 0;              // satrt with the first particle
}

/*************************************************
*     Function:
*
*   Parameters:
*
*      Returns:
*
*  Description:
*
**************************************************/
static void PSOs_done (void)
{
    PSO_current_state = PSO_EVALUATE;               // if start another cycle, start from PSO_EVALUATE
}
  
/*************************************************/
/*      PSO Evaluation Functions                 */    
/*************************************************/

/*************************************************
*     Function:
*
*   Parameters:
*
*      Returns:
*
*  Description:
*
**************************************************/
static void evaluate_functions (int fun_type)
{
    switch (fun_type)
    {
        case SIMPLE_MIN:
          simple_min();
          break;
        case SIMPLE_MAX:
          simple_max();
          break;
        case MICHALEWICZ_MIN:
          michalewicz_min();
          break;
        case MICHALEWICZ_MAX:
          michalewicz_max();
          break;
        case MICHALEWICZ_2_MIN:
          michalewicz_2_min();
          break;
        case MICHALEWICZ_2_MAX:
          michalewicz_2_max();
          break;
        case G1_MIN:
          g1_min();
          break;
        case G1_MAX:
          g1_max();
          break;
        case G7_MIN:
          g7_min();
          break;
        case G7_MAX:
          g7_max();
          break;
        case G9_MIN:
          g9_min();
          break;
        case G9_MAX:
          g9_max();
          break;
        case F6:
          f6();
          break;
        case SPHERE:
          sphere();
          break;
        case ROSENBROCK:
          rosenbrock();
          break;
        case RASTRIGRIN:
          rastrigrin();
          break;
        case GRIEWANK:
          griewank();
          break;
		  //case BP_MAX:
		  //	 BP_Weights_From_PSO(psos[cur_pso].position_values[psos[cur_pso].popu_index]);
		  //	 psos[cur_pso].eva_fun_value = BP_Main_Loop();
          //printf("psos[cur_pso].eva_fun_value = %f\n",psos[cur_pso].eva_fun_value);
		  //	 break;
        case BP_EXP_MIN:
			 BP_Input_From_PSO(psos[cur_pso].position_values[psos[cur_pso].popu_index]);
			 psos[cur_pso].eva_fun_value = BP_Main_Loop();
        default:
          break;
    }
}

/*************************************************
*     Function:
*
*   Parameters:
*
*      Returns:
*
*  Description:
*
**************************************************/
static void best_solution_values (int fun_type)
{
    switch (fun_type)
    {
        case G1_MIN:
          g1_best_solution();
          break;
        case G7_MIN:
          g7_best_solution();
          break;
        case G9_MIN:
          g9_best_solution();
          break;
        default:
          break;
    }
}

/*************************************************
*     Function:
*
*   Parameters:
*
*      Returns:
*
*  Description:
*
**************************************************/
static void simple_min (void)
{
    int idx_i;
    float x, y,fit_value;
    int env_pso;
    double temp_max;

    env_pso = (cur_pso)?0:1;
    if (pso_update_pbest_each_cycle_pending)
    {
        x = psos[cur_pso].pbest_position_values[psos[cur_pso].popu_index][0];
    }
    else
    {
        x = psos[cur_pso].position_values[psos[cur_pso].popu_index][0];
    }
    for (idx_i = 0; idx_i < (psos[env_pso].popu_size) ; idx_i++ )
    {
        if (pso_update_pbest_each_cycle_pending) 
        {
            y = psos[env_pso].pbest_position_values[idx_i][0];
        }
        else
        {
            y = psos[env_pso].position_values[idx_i][0];
        }
        if (idx_i == 0)
        {
            temp_max = x * y;
            fit_value = temp_max;  
        }
        else
        {
            fit_value = max((x * y),temp_max);
            temp_max = fit_value;  
        }
    }
     
    psos[cur_pso].eva_fun_value = fit_value;
}

/*************************************************
*     Function:
*
*   Parameters:
*
*      Returns:
*
*  Description:
*
**************************************************/
static void simple_max (void)
{
    int idx_i;
    float x, y,fit_value;
    int env_pso;
    double temp_min;

    env_pso = (cur_pso)?0:1;
    if (pso_update_pbest_each_cycle_pending)
    {
        y = psos[cur_pso].pbest_position_values[psos[cur_pso].popu_index][0];
    }
    else
    {
        y = psos[cur_pso].position_values[psos[cur_pso].popu_index][0];
    }
    for (idx_i = 0; idx_i < (psos[env_pso].popu_size) ; idx_i++ )
    {
        if (pso_update_pbest_each_cycle_pending)
        {
            x = psos[env_pso].pbest_position_values[idx_i][0];
        }
        else
        {
            x = psos[env_pso].position_values[idx_i][0];
        }
        if (idx_i == 0)
        {
            temp_min = x * y;
            fit_value = temp_min;  
        }
        else
        {
            fit_value = min((x * y),temp_min);
            temp_min = fit_value;  
        }
    }
     
    psos[cur_pso].eva_fun_value = fit_value;

}

/*************************************************
*     Function:
*
*   Parameters:
*
*      Returns:
*
*  Description:
*
**************************************************/
static void michalewicz_min (void)
{
    int idx_i;
    float x1,x2, y1,y2,fit_value;
    int env_pso;
    double temp_max,temp_value;

    env_pso = (cur_pso)?0:1;
    if (pso_update_pbest_each_cycle_pending)
    {
        x1 = psos[cur_pso].pbest_position_values[psos[cur_pso].popu_index][0];
        x2 = psos[cur_pso].pbest_position_values[psos[cur_pso].popu_index][1];
    }
    else
    {
        x1 = psos[cur_pso].position_values[psos[cur_pso].popu_index][0];
        x2 = psos[cur_pso].position_values[psos[cur_pso].popu_index][1];
    }

    for (idx_i = 0; idx_i < (psos[env_pso].popu_size) ; idx_i++ )
    {
        if (pso_update_pbest_each_cycle_pending)
        {
            y1 = psos[env_pso].pbest_position_values[idx_i][0];
            y2 = psos[env_pso].pbest_position_values[idx_i][1];
        }
        else
        {
            y1 = psos[env_pso].position_values[idx_i][0];
            y2 = psos[env_pso].position_values[idx_i][1];
        }
        temp_value = 100 * pow((x2- x1*x1),2) + pow((1.0-x1),2)  - y1*(x1+x2*x2) - y2*(x1*x1+x2);
        if (idx_i == 0)
        {
            temp_max = temp_value;
            fit_value = (float)temp_max;  
        }
        else
        {
            fit_value = max(temp_value,temp_max);
            temp_max = fit_value;  
        }
 
    }
    
    psos[cur_pso].eva_fun_value = fit_value;
}

/*************************************************
*     Function:
*
*   Parameters:
*
*      Returns:
*
*  Description:
*
**************************************************/
static void michalewicz_max (void)
{
    int idx_i;
    float x1,x2, y1,y2,fit_value;
    int env_pso;
    double temp_min,temp_value;

    env_pso = (cur_pso)?0:1;

    if (pso_update_pbest_each_cycle_pending)
    {
        y1 = psos[cur_pso].pbest_position_values[psos[cur_pso].popu_index][0];
        y2 = psos[cur_pso].pbest_position_values[psos[cur_pso].popu_index][1];
    }
    else
    {
        y1 = psos[cur_pso].position_values[psos[cur_pso].popu_index][0];
        y2 = psos[cur_pso].position_values[psos[cur_pso].popu_index][1];
    }

    for (idx_i = 0; idx_i < (psos[env_pso].popu_size) ; idx_i++ )
    {
        if (pso_update_pbest_each_cycle_pending)
        {
            x1 = psos[env_pso].pbest_position_values[idx_i][0];
            x2 = psos[env_pso].pbest_position_values[idx_i][1];
        }
        else
        {
            x1 = psos[env_pso].position_values[idx_i][0];
            x2 = psos[env_pso].position_values[idx_i][1];
        }
        temp_value = 100 * pow((x2- x1*x1),2) + pow((1.0-x1),2)  - y1*(x1+x2*x2) - y2*(x1*x1+x2);
        if (idx_i == 0)
        {
            temp_min = temp_value;
            fit_value = temp_min;  
        }
        else
        {
            fit_value = min(temp_value,temp_min);
            temp_min = fit_value;  
        }
    }
     
    psos[cur_pso].eva_fun_value = fit_value;

}

/*************************************************
*     Function:
*
*   Parameters:
*
*      Returns:
*
*  Description:
*
**************************************************/
static void michalewicz_2_min (void)
{
    int idx_i;
    float x1,x2, y1,y2,fit_value;
    int env_pso;
    double temp_max,temp_value;

    env_pso = (cur_pso)?0:1;

    if (pso_update_pbest_each_cycle_pending)
    {
        x1 = psos[cur_pso].pbest_position_values[psos[cur_pso].popu_index][0];
        x2 = psos[cur_pso].pbest_position_values[psos[cur_pso].popu_index][1];
    }
    else
    {
        x1 = psos[cur_pso].position_values[psos[cur_pso].popu_index][0];
        x2 = psos[cur_pso].position_values[psos[cur_pso].popu_index][1];
    }

    for (idx_i = 0; idx_i < (psos[env_pso].popu_size) ; idx_i++ )
    {
        if (pso_update_pbest_each_cycle_pending)
        {
            y1 = psos[env_pso].pbest_position_values[idx_i][0];
            y2 = psos[env_pso].pbest_position_values[idx_i][1];
        }
        else
        {
            y1 = psos[env_pso].position_values[idx_i][0];
            y2 = psos[env_pso].position_values[idx_i][1];
        }
        temp_value = pow((x1-2),2) + pow((x2-1),2) +y1*(x1*x1-x2)+y2*(x1+x2-2);
        
        if (idx_i == 0)
        {
            temp_max = temp_value;
            fit_value = temp_max;  
        }
        else
        {
            fit_value = max(temp_value,temp_max);
            temp_max = fit_value;  
        }
 
    }
    
    psos[cur_pso].eva_fun_value = fit_value;
}

/*************************************************
*     Function:
*
*   Parameters:
*
*      Returns:
*
*  Description:
*
**************************************************/
static void michalewicz_2_max (void)
{
    int idx_i;
    float x1,x2, y1,y2,fit_value;
    int env_pso;
    double temp_min,temp_value;

    env_pso = (cur_pso)?0:1;

    if (pso_update_pbest_each_cycle_pending)
    {
        y1 = psos[cur_pso].pbest_position_values[psos[cur_pso].popu_index][0];
        y2 = psos[cur_pso].pbest_position_values[psos[cur_pso].popu_index][1];
    }
    else
    {
        y1 = psos[cur_pso].position_values[psos[cur_pso].popu_index][0];
        y2 = psos[cur_pso].position_values[psos[cur_pso].popu_index][1];
    }

    for (idx_i = 0; idx_i < (psos[env_pso].popu_size) ; idx_i++ )
    {
        if (pso_update_pbest_each_cycle_pending)
        {
            x1 = psos[env_pso].pbest_position_values[idx_i][0];
            x2 = psos[env_pso].pbest_position_values[idx_i][1];
        }
        else
        {
            x1 = psos[env_pso].position_values[idx_i][0];
            x2 = psos[env_pso].position_values[idx_i][1];
        }

        temp_value = pow((x1-2),2) + pow((x2-1),2) +y1*(x1*x1-x2)+y2*(x1+x2-2);
        
        if (idx_i == 0)
        {
            temp_min = temp_value;
            fit_value = temp_min;  
        }
        else
        {
            fit_value = min(temp_value,temp_min);
            temp_min = fit_value;  
        }
    }
     
    psos[cur_pso].eva_fun_value = fit_value;

}

/*************************************************
*     Function:
*
*   Parameters:
*
*      Returns:
*
*  Description:
*
**************************************************/
static void g1_min (void)
{
    int idx_i,idx_j;
    double x[13];
    double y[9];
    double r[9];
    double p[9];

    double fit_value;
    int env_pso;
    double temp_max,temp_value;

    env_pso = (cur_pso)?0:1;
    
    if (pso_update_pbest_each_cycle_pending)
    {
        for (idx_i = 0; idx_i <13 ; idx_i++ )
        {
            x[idx_i]  = psos[cur_pso].pbest_position_values[psos[cur_pso].popu_index][idx_i];
        }
    }
    else
    {
        for (idx_i = 0; idx_i <13 ; idx_i++ )
        {
            x[idx_i]  = psos[cur_pso].position_values[psos[cur_pso].popu_index][idx_i];
        }
    }

    // constrains
    r[0] =  2 * x[0] + 2     * x[1]  + x[9]  + x[10] - 10;
    r[1] =  2 * x[0] + 2     * x[2]  + x[9]  + x[11] - 10;
    r[2] =  2 * x[1] + 2     * x[2]  + x[10] + x[11] - 10;
    r[3] = -8 * x[0] + x[9];
    r[4] = -8 * x[1] + x[10];
    r[5] = -8 * x[2] + x[11];
    r[6] = -2 * x[3] - x[4]  + x[9];
    r[7] = -2 * x[5] - x[6]  + x[10];
    r[8] = -2 * x[7] - x[8]  + x[11];
    
    for (idx_i = 0; idx_i < (psos[env_pso].popu_size) ; idx_i++ )
    {
        if (pso_update_pbest_each_cycle_pending)
        {
            for (idx_j = 0; idx_j <9 ; idx_j++ )
            {
                y[idx_j] = psos[env_pso].pbest_position_values[idx_i][idx_j];
            }
        }
        else
        {
            for (idx_j = 0; idx_j <9 ; idx_j++ )
            {
                y[idx_j] = psos[env_pso].position_values[idx_i][idx_j];
            }
        }

        temp_value = 0.0;
        for (idx_j = 0; idx_j <4 ; idx_j++ )
        {
            temp_value += 5 * (x[idx_j] - x[idx_j] * x[idx_j]);
        }

        for (idx_j = 4; idx_j <13 ; idx_j++ )
        {
            temp_value -= x[idx_j];
        }

        for (idx_j = 0; idx_j <9 ; idx_j++ )
        {
            if ((r[idx_j]) >= (-y[idx_j]/200.0))
            {
                p[idx_j] = y[idx_j] * r[idx_j] + 100 * r[idx_j] * r[idx_j];
            } 
            else
            {
                p[idx_j] = - y[idx_j] * y[idx_j]/400.0;
            }
        }

        
        for (idx_j = 0; idx_j <9 ; idx_j++ )
        {        
            temp_value += p[idx_j];
        }

        if (idx_i == 0)
        {
            temp_max = temp_value;
            fit_value = temp_max;  
        }
        else
        {
            fit_value = max(temp_value,temp_max);
            temp_max = fit_value;  
        }
 
    }
    
    psos[cur_pso].eva_fun_value = fit_value;
}

/*************************************************
*     Function:
*
*   Parameters:
*
*      Returns:
*
*  Description:
*
**************************************************/
static void g1_best_solution (void)
{
    int idx_i,idx_j;
    double x[13];

    double temp_value;
    
    for (idx_i = 0; idx_i <13 ; idx_i++ )
    {
        x[idx_i]  = psos[0].pbest_position_values[psos[0].global_best_index][idx_i];
    }
    counter = 10;
    // constrains
    paper_results[0] =  2 * x[0] + 2     * x[1]  + x[9]  + x[10] - 10;
    paper_results[1] =  2 * x[0] + 2     * x[2]  + x[9]  + x[11] - 10;
    paper_results[2] =  2 * x[1] + 2     * x[2]  + x[10] + x[11] - 10;
    paper_results[3] = -8 * x[0] + x[9];
    paper_results[4] = -8 * x[1] + x[10];
    paper_results[5] = -8 * x[2] + x[11];
    paper_results[6] = -2 * x[3] - x[4]  + x[9];
    paper_results[7] = -2 * x[5] - x[6]  + x[10];
    paper_results[8] = -2 * x[7] - x[8]  + x[11];
    
    temp_value = 0.0;
    for (idx_j = 0; idx_j <4 ; idx_j++ )
    {
        temp_value += 5 * (x[idx_j] - x[idx_j] * x[idx_j]);
    }

    for (idx_j = 4; idx_j <13 ; idx_j++ )
    {
        temp_value -= x[idx_j];
    }
    paper_results[9] = temp_value;
}

/*************************************************
*     Function:
*
*   Parameters:
*
*      Returns:
*
*  Description:
*
**************************************************/
static void g1_max (void)
{
    int idx_i,idx_j;
    double x[13];
    double y[9];
    double r[9];
    double p[9];

    double fit_value;
    int env_pso;
    double temp_min,temp_value;

    env_pso = (cur_pso)?0:1;
    
    if (pso_update_pbest_each_cycle_pending)
    {
        for (idx_i = 0; idx_i <9 ; idx_i++ )
        {
            y[idx_i]  = psos[cur_pso].pbest_position_values[psos[cur_pso].popu_index][idx_i];
        }
    }
    else
    {
        for (idx_i = 0; idx_i <9 ; idx_i++ )
        {
            y[idx_i]  = psos[cur_pso].position_values[psos[cur_pso].popu_index][idx_i];
        }
    } 
    
    for (idx_i = 0; idx_i < (psos[env_pso].popu_size) ; idx_i++ )
    {
        if (pso_update_pbest_each_cycle_pending)
        {
            for (idx_j = 0; idx_j <13 ; idx_j++ )
            {
                x[idx_j] = psos[env_pso].pbest_position_values[idx_i][idx_j];
            }
        }
        else
        {
            for (idx_j = 0; idx_j <13 ; idx_j++ )
            {
                x[idx_j] = psos[env_pso].position_values[idx_i][idx_j];
            }
        } 
        
        // constrains
        r[0] =  2 * x[0] + 2     * x[1]  + x[9]  + x[10] - 10;
        r[1] =  2 * x[0] + 2     * x[2]  + x[9]  + x[11] - 10;
        r[2] =  2 * x[1] + 2     * x[2]  + x[10] + x[11] - 10;
        r[3] = -8 * x[0] + x[9];
        r[4] = -8 * x[1] + x[10];
        r[5] = -8 * x[2] + x[11];
        r[6] = -2 * x[3] - x[4]  + x[9];
        r[7] = -2 * x[5] - x[6]  + x[10];
        r[8] = -2 * x[7] - x[8]  + x[11];
        
        temp_value = 0.0;
        for (idx_j = 0; idx_j <4 ; idx_j++ )
        {
            temp_value += 5 * (x[idx_j] - x[idx_j] * x[idx_j]);
        }

        for (idx_j = 4; idx_j <13 ; idx_j++ )
        {
            temp_value -= x[idx_j];
        }

        for (idx_j = 0; idx_j <9 ; idx_j++ )
        {
            if ((r[idx_j]) >= (-y[idx_j]/200.0))
            {
                p[idx_j] = y[idx_j] * r[idx_j] + 100 * r[idx_j] * r[idx_j];
            } 
            else
            {
                p[idx_j] = - y[idx_j] * y[idx_j]/400.0;
            }
        }

        
        for (idx_j = 0; idx_j <9 ; idx_j++ )
        {        
            temp_value += p[idx_j];
        }
        
        if (idx_i == 0)
        {
            temp_min = temp_value;
            fit_value = temp_min;  
        }
        else
        {
            fit_value = min(temp_value,temp_min);
            temp_min = fit_value;  
        }
 
    }
    
    psos[cur_pso].eva_fun_value = fit_value;
}

/*************************************************
*     Function:
*
*   Parameters:
*
*      Returns:
*
*  Description:
*
**************************************************/
static void g7_min (void)
{
    int idx_i;
    double x1, x2, x3, x4, x5, x6, x7, x8, x9, x10;
	  double y1, y2, y3, y4, y5, y6, y7, y8;
	  double r1, r2, r3, r4, r5, r6, r7, r8;
    double p1, p2, p3, p4, p5, p6, p7, p8;
	  
    double fit_value;
    int env_pso;
    double temp_max,temp_value;

    env_pso = (cur_pso)?0:1;
    
    if (pso_update_pbest_each_cycle_pending)
    {
        x1  = psos[cur_pso].pbest_position_values[psos[cur_pso].popu_index][0];
        x2  = psos[cur_pso].pbest_position_values[psos[cur_pso].popu_index][1];
        x3  = psos[cur_pso].pbest_position_values[psos[cur_pso].popu_index][2];
        x4  = psos[cur_pso].pbest_position_values[psos[cur_pso].popu_index][3];
        x5  = psos[cur_pso].pbest_position_values[psos[cur_pso].popu_index][4];
        x6  = psos[cur_pso].pbest_position_values[psos[cur_pso].popu_index][5];
        x7  = psos[cur_pso].pbest_position_values[psos[cur_pso].popu_index][6];
        x8  = psos[cur_pso].pbest_position_values[psos[cur_pso].popu_index][7];
        x9  = psos[cur_pso].pbest_position_values[psos[cur_pso].popu_index][8];
        x10 = psos[cur_pso].pbest_position_values[psos[cur_pso].popu_index][9];
    }
    else
    {
        x1  = psos[cur_pso].position_values[psos[cur_pso].popu_index][0];
        x2  = psos[cur_pso].position_values[psos[cur_pso].popu_index][1];
        x3  = psos[cur_pso].position_values[psos[cur_pso].popu_index][2];
        x4  = psos[cur_pso].position_values[psos[cur_pso].popu_index][3];
        x5  = psos[cur_pso].position_values[psos[cur_pso].popu_index][4];
        x6  = psos[cur_pso].position_values[psos[cur_pso].popu_index][5];
        x7  = psos[cur_pso].position_values[psos[cur_pso].popu_index][6];
        x8  = psos[cur_pso].position_values[psos[cur_pso].popu_index][7];
        x9  = psos[cur_pso].position_values[psos[cur_pso].popu_index][8];
        x10 = psos[cur_pso].position_values[psos[cur_pso].popu_index][9];
    }

    // restrictions
    r1= -(105- 4*x1- 5*x2+ 3*x7- 9*x8);
    r2= -(-3*pow(x1-2, 2)- 4*pow(x2-3, 2)- 2*x3*x3+ 7*x4+ 120);
    r3= -(-10*x1+ 8*x2+ 17*x7- 2*x8);
    r4= -(-x1*x1- 2*pow(x2-2, 2)+ 2*x1*x2- 14*x5+ 6*x6);
    r5= -(8*x1- 2*x2- 5*x9+ 2*x10+12);
    r6= -(-5*x1*x1- 8*x2- pow(x3-6, 2)+ 2*x4+ 40);
    r7= -(3*x1 -6*x2- 12*pow(x9-8, 2)+ 7*x10);
    r8= -(-0.5*pow(x1-8, 2)- 2*(x2-4)- 3*x5*x5+ x6+ 30);

    for (idx_i = 0; idx_i < (psos[env_pso].popu_size) ; idx_i++ )
    {
        if (pso_update_pbest_each_cycle_pending)
        {
            y1 = psos[env_pso].pbest_position_values[idx_i][0];
            y2 = psos[env_pso].pbest_position_values[idx_i][1];
            y3 = psos[env_pso].pbest_position_values[idx_i][2];
            y4 = psos[env_pso].pbest_position_values[idx_i][3];
            y5 = psos[env_pso].pbest_position_values[idx_i][4];
            y6 = psos[env_pso].pbest_position_values[idx_i][5];
            y7 = psos[env_pso].pbest_position_values[idx_i][6];
            y8 = psos[env_pso].pbest_position_values[idx_i][7];
        }
        else
        {
            y1 = psos[env_pso].position_values[idx_i][0];
            y2 = psos[env_pso].position_values[idx_i][1];
            y3 = psos[env_pso].position_values[idx_i][2];
            y4 = psos[env_pso].position_values[idx_i][3];
            y5 = psos[env_pso].position_values[idx_i][4];
            y6 = psos[env_pso].position_values[idx_i][5];
            y7 = psos[env_pso].position_values[idx_i][6];
            y8 = psos[env_pso].position_values[idx_i][7];
        }

        temp_value =  x1*x1+ x2*x2+ x1*x2- 14*x1 -16*x2+ pow(x3-10, 2)
	               +4*pow(x4-5,2)+ pow(x5-3, 2)+ 2*pow(x6-1, 2)+ 5*x7*x7
	               +7*pow(x8-11, 2)+ 2*pow(x9-10, 2)+ pow(x10-7, 2)+ 45;

        if ((r1) >= (-y1/200.0))
        {
            p1 = y1 * r1 + 100 * r1 * r1;
        }
        else
        {
            p1 = - y1*y1/400.0;  
        }

        if ((r2) >= (-y2/200.0))
        {
            p2 = y2 * r2 + 100 * r2 * r2;
        }
        else
        {
            p2 = - y2*y2/400.0;  
        }

        if ((r3) >= (-y3/200.0))
        {
            p3 = y3 * r3 + 100 * r3 * r3;
        }
        else
        {
            p3 = - y3*y3/400.0;  
        }

        if ((r4) >= (-y4/200.0))
        {
            p4 = y4 * r4 + 100 * r4 * r4;
        }
        else
        {
            p4 = - y4*y4/400.0;  
        }

        if ((r5) >= (-y5/200.0))
        {
            p5 = y5 * r5 + 100 * r5 * r5;
        }
        else
        {
            p5 = - y5*y5/400.0;  
        }

        if ((r6) >= (-y6/200.0))
        {
            p6 = y6 * r6 + 100 * r6 * r6;
        }
        else
        {
            p6 = - y6*y6/400.0;  
        }

        if ((r7) >= (-y7/200.0))
        {
            p7 = y7 * r7 + 100 * r7 * r7;
        }
        else
        {
            p7 = - y7*y7/400.0;  
        }

        if ((r8) >= (-y8/200.0))
        {
            p8 = y8 * r8 + 100 * r8 * r8;
        }
        else
        {
            p8 = - y8*y8/400.0;  
        }

        temp_value += p1 + p2 + p3 + p4 + p5 + p6 + p7 + p8;
        //temp_value += (y1*r1 +y2*r2 +y3*r3 +y4*r4 +y5*r5 +y6*r6 +y7*r7+y8*r8);
         
        if (idx_i == 0)
        {
            temp_max = temp_value;
            fit_value = temp_max;  
        }
        else
        {
            fit_value = max(temp_value,temp_max);
            temp_max = fit_value;  
        }
 
    }
    
    psos[cur_pso].eva_fun_value = fit_value;

}

/*************************************************
*     Function:
*
*   Parameters:
*
*      Returns:
*
*  Description:
*
**************************************************/
static void g7_best_solution (void)
{
    double x1, x2, x3, x4, x5, x6, x7, x8, x9, x10;
	  
    x1  = psos[0].pbest_position_values[psos[0].global_best_index][0];
    x2  = psos[0].pbest_position_values[psos[0].global_best_index][1];
    x3  = psos[0].pbest_position_values[psos[0].global_best_index][2];
    x4  = psos[0].pbest_position_values[psos[0].global_best_index][3];
    x5  = psos[0].pbest_position_values[psos[0].global_best_index][4];
    x6  = psos[0].pbest_position_values[psos[0].global_best_index][5];
    x7  = psos[0].pbest_position_values[psos[0].global_best_index][6];
    x8  = psos[0].pbest_position_values[psos[0].global_best_index][7];
    x9  = psos[0].pbest_position_values[psos[0].global_best_index][8];
    x10 = psos[0].pbest_position_values[psos[0].global_best_index][9];

    // restrictions
    paper_results[0] = -(105- 4*x1- 5*x2+ 3*x7- 9*x8);
    paper_results[1] = -(-3*pow(x1-2, 2)- 4*pow(x2-3, 2)- 2*x3*x3+ 7*x4+ 120);
    paper_results[2] = -(-10*x1+ 8*x2+ 17*x7- 2*x8);
    paper_results[3] = -(-x1*x1- 2*pow(x2-2, 2)+ 2*x1*x2- 14*x5+ 6*x6);
    paper_results[4] = -(8*x1- 2*x2- 5*x9+ 2*x10+12);
    paper_results[5] = -(-5*x1*x1- 8*x2- pow(x3-6, 2)+ 2*x4+ 40);
    paper_results[6] = -(3*x1 -6*x2- 12*pow(x9-8, 2)+ 7*x10);
    paper_results[7] = -(-0.5*pow(x1-8, 2)- 2*(x2-4)- 3*x5*x5+ x6+ 30);

    paper_results[8] =  x1*x1+ x2*x2+ x1*x2- 14*x1 -16*x2+ pow(x3-10, 2)
	           +4*pow(x4-5,2)+ pow(x5-3, 2)+ 2*pow(x6-1, 2)+ 5*x7*x7
	           +7*pow(x8-11, 2)+ 2*pow(x9-10, 2)+ pow(x10-7, 2)+ 45;

    counter = 9;
}

/*************************************************
*     Function:
*
*   Parameters:
*
*      Returns:
*
*  Description:
*
**************************************************/
static void g7_max (void)
{
    int idx_i;
    double x1, x2, x3, x4, x5, x6, x7, x8, x9, x10;
	  double y1, y2, y3, y4, y5, y6, y7, y8;
	  double r1, r2, r3, r4, r5, r6, r7, r8;
    double p1, p2, p3, p4, p5, p6, p7, p8;
    double fit_value;
    int env_pso;
    double temp_min,temp_value;

    env_pso = (cur_pso)?0:1;

    if (pso_update_pbest_each_cycle_pending)
    {
        y1 = psos[cur_pso].pbest_position_values[psos[cur_pso].popu_index][0];
        y2 = psos[cur_pso].pbest_position_values[psos[cur_pso].popu_index][1];
        y3 = psos[cur_pso].pbest_position_values[psos[cur_pso].popu_index][2];
        y4 = psos[cur_pso].pbest_position_values[psos[cur_pso].popu_index][3];
        y5 = psos[cur_pso].pbest_position_values[psos[cur_pso].popu_index][4];
        y6 = psos[cur_pso].pbest_position_values[psos[cur_pso].popu_index][5];
        y7 = psos[cur_pso].pbest_position_values[psos[cur_pso].popu_index][6];
        y8 = psos[cur_pso].pbest_position_values[psos[cur_pso].popu_index][7];
    }
    else
    {
        y1 = psos[cur_pso].position_values[psos[cur_pso].popu_index][0];
        y2 = psos[cur_pso].position_values[psos[cur_pso].popu_index][1];
        y3 = psos[cur_pso].position_values[psos[cur_pso].popu_index][2];
        y4 = psos[cur_pso].position_values[psos[cur_pso].popu_index][3];
        y5 = psos[cur_pso].position_values[psos[cur_pso].popu_index][4];
        y6 = psos[cur_pso].position_values[psos[cur_pso].popu_index][5];
        y7 = psos[cur_pso].position_values[psos[cur_pso].popu_index][6];
        y8 = psos[cur_pso].position_values[psos[cur_pso].popu_index][7];
    }

    for (idx_i = 0; idx_i < (psos[env_pso].popu_size) ; idx_i++ )
    {
        if (pso_update_pbest_each_cycle_pending)
        {
            x1  = psos[env_pso].pbest_position_values[idx_i][0];
            x2  = psos[env_pso].pbest_position_values[idx_i][1];
            x3  = psos[env_pso].pbest_position_values[idx_i][2];
            x4  = psos[env_pso].pbest_position_values[idx_i][3];
            x5  = psos[env_pso].pbest_position_values[idx_i][4];
            x6  = psos[env_pso].pbest_position_values[idx_i][5];
            x7  = psos[env_pso].pbest_position_values[idx_i][6];
            x8  = psos[env_pso].pbest_position_values[idx_i][7];
            x9  = psos[env_pso].pbest_position_values[idx_i][8];
            x10 = psos[env_pso].pbest_position_values[idx_i][9];
        }
        else
        {
            x1  = psos[env_pso].position_values[idx_i][0];
            x2  = psos[env_pso].position_values[idx_i][1];
            x3  = psos[env_pso].position_values[idx_i][2];
            x4  = psos[env_pso].position_values[idx_i][3];
            x5  = psos[env_pso].position_values[idx_i][4];
            x6  = psos[env_pso].position_values[idx_i][5];
            x7  = psos[env_pso].position_values[idx_i][6];
            x8  = psos[env_pso].position_values[idx_i][7];
            x9  = psos[env_pso].position_values[idx_i][8];
            x10 = psos[env_pso].position_values[idx_i][9];
        }

        r1= -(105- 4*x1- 5*x2+ 3*x7- 9*x8);
        r2= -(-3*pow(x1-2, 2)- 4*pow(x2-3, 2)- 2*x3*x3+ 7*x4+ 120);
        r3= -(-10*x1+ 8*x2+ 17*x7- 2*x8);
        r4= -(-x1*x1- 2*pow(x2-2, 2)+2*x1*x2- 14*x5+ 6*x6);
        r5= -(8*x1- 2*x2- 5*x9+ 2*x10+12);
        r6= -(-5*x1*x1- 8*x2- 1*pow(x3-6, 2)+ 2*x4+ 40);
        r7= -(3*x1 -6*x2- 12*pow(x9-8, 2)+ 7*x10);
        r8= -(-0.5*pow(x1-8, 2)- 2*(x2-4)- 3*x5*x5+ x6+ 30);

        temp_value =  x1*x1+ x2*x2+ x1*x2- 14*x1 -16*x2+ pow(x3-10, 2)
	               +4*pow(x4-5,2)+ pow(x5-3, 2)+ 2*pow(x6-1, 2)+ 5*x7*x7
	               +7*pow(x8-11, 2)+ 2*pow(x9-10, 2)+ pow(x10-7, 2)+ 45;

        if ((r1) >= (-y1/200.0))
        {
            p1 = y1 * r1 + 100 * r1 * r1;
        }
        else
        {
            p1 = - y1*y1/400.0;  
        }

        if ((r2) >= (-y2/200.0))
        {
            p2 = y2 * r2 + 100 * r2 * r2;
        }
        else
        {
            p2 = - y2*y2/400.0;  
        }

        if ((r3) >= (-y3/200.0))
        {
            p3 = y3 * r3 + 100 * r3 * r3;
        }
        else
        {
            p3 = - y3*y3/400.0;  
        }

        if ((r4) >= (-y4/200.0))
        {
            p4 = y4 * r4 + 100 * r4 * r4;
        }
        else
        {
            p4 = - y4*y4/400.0;  
        }

        if ((r5) >= (-y5/200.0))
        {
            p5 = y5 * r5 + 100 * r5 * r5;
        }
        else
        {
            p5 = - y5*y5/400.0;  
        }

        if ((r6) >= (-y6/200.0))
        {
            p6 = y6 * r6 + 100 * r6 * r6;
        }
        else
        {
            p6 = - y6*y6/400.0;  
        }

        if ((r7) >= (-y7/200.0))
        {
            p7 = y7 * r7 + 100 * r7 * r7;
        }
        else
        {
            p7 = - y7*y7/400.0;  
        }

        if ((r8) >= (-y8/200.0))
        {
            p8 = y8 * r8 + 100 * r8 * r8;
        }
        else
        {
            p8 = - y8*y8/400.0;  
        }

        temp_value += p1 + p2 + p3 + p4 + p5 + p6 + p7 + p8;
        //temp_value += (y1*r1 +y2*r2 +y3*r3 +y4*r4 +y5*r5 +y6*r6 +y7*r7+y8*r8);
         
        if (idx_i == 0)
        {
            temp_min = temp_value;
            fit_value = temp_min;  
        }
        else
        {
            fit_value = min(temp_value,temp_min);
            temp_min = fit_value;  
        }
 
    }
    
    psos[cur_pso].eva_fun_value = fit_value;
}

/*************************************************
*     Function:
*
*   Parameters:
*
*      Returns:
*
*  Description:
*
**************************************************/
static void g9_min (void)
{
    int idx_i,idx_j;
    double x[7];
    double y[4];
    double r[4];
    double p[4];

    double fit_value;
    int env_pso;
    double temp_max,temp_value;

    env_pso = (cur_pso)?0:1;

    if (pso_update_pbest_each_cycle_pending)
    {
        for (idx_i = 0; idx_i <7 ; idx_i++ )
        {
            x[idx_i]  = psos[cur_pso].pbest_position_values[psos[cur_pso].popu_index][idx_i];
        }
    }
    else
    {
        for (idx_i = 0; idx_i <7 ; idx_i++ )
        {
            x[idx_i]  = psos[cur_pso].position_values[psos[cur_pso].popu_index][idx_i];
        }
    }

    // constrains
    r[0] = 2 * x[0] * x[0] + 3 * pow(x[1],4) + x[2] + 4 * x[3] * x[3] + 5 * x[4]  - 127;
    r[1] = 7 * x[0] + 3 * x[1] + 10 * x[2] * x[2] + x[3] - x[4] - 282;
    r[2] = 23 * x[0] + x[1] * x[1] + 6 * x[5] * x[5] - 8 * x[6] - 196;
    r[3] = 4 * x[0] * x[0] + x[1] * x[1] - 3 * x[0] * x[1] + 2 * x[2] * x[2] + 5 * x[5] - 11 * x[6];

    for (idx_i = 0; idx_i < (psos[env_pso].popu_size) ; idx_i++ )
    {
        if (pso_update_pbest_each_cycle_pending)
        {
            for (idx_j = 0; idx_j <4 ; idx_j++ )
            {
                y[idx_j] = psos[env_pso].pbest_position_values[idx_i][idx_j];
            }
        }
        else
        {
            for (idx_j = 0; idx_j <4 ; idx_j++ )
            {
                y[idx_j] = psos[env_pso].position_values[idx_i][idx_j];
            }
        }

        temp_value = pow((x[0] - 10),2) + 5 * pow((x[1] - 12),2) + pow(x[2],4) + 3 * pow((x[3] - 11),2) 
                     + 10 * pow((x[4]),6) + 7 * pow(x[5],2) + pow(x[6],4) - 4 * x[5] * x[6] - 10 * x[5] - 8 * x[6];
                     
        for (idx_j = 0; idx_j <4 ; idx_j++ )
        {
            if ((r[idx_j]) >= (-y[idx_j]/200.0))
            {
                p[idx_j] = y[idx_j] * r[idx_j] + 100 * r[idx_j] * r[idx_j];
            } 
            else
            {
                p[idx_j] = - y[idx_j] * y[idx_j]/400.0;
            }
            temp_value += p[idx_j];
        }                       
        if (idx_i == 0)
        {
            temp_max = temp_value;
            fit_value = temp_max;  
        }
        else
        {
            fit_value = max(temp_value,temp_max);
            temp_max = fit_value;  
        }
    }
    psos[cur_pso].eva_fun_value = fit_value;
}

/*************************************************
*     Function:
*
*   Parameters:
*
*      Returns:
*
*  Description:
*
**************************************************/
static void g9_best_solution (void)
{
    int idx_i;
    double x[7];

    for (idx_i = 0; idx_i <7 ; idx_i++ )
    {
        x[idx_i]  = psos[0].pbest_position_values[psos[0].global_best_index][idx_i];
    }

    // constrains
    paper_results[0] = 2 * x[0] * x[0] + 3 * pow(x[1],4) + x[2] + 4 * x[3] * x[3] + 5 * x[4]  - 127;
    paper_results[1] = 7 * x[0] + 3 * x[1] + 10 * x[2] * x[2] + x[3] - x[4] - 282;
    paper_results[2] = 23 * x[0] + x[1] * x[1] + 6 * x[5] * x[5] - 8 * x[6] - 196;
    paper_results[3] = 4 * x[0] * x[0] + x[1] * x[1] - 3 * x[0] * x[1] + 2 * x[2] * x[2] + 5 * x[5] - 11 * x[6];

    paper_results[4] = pow((x[0] - 10),2) + 5 * pow((x[1] - 12),2) + pow(x[2],4) + 3 * pow((x[3] - 11),2) 
                 + 10 * pow((x[4]),6) + 7 * pow(x[5],2) + pow(x[6],4) - 4 * x[5] * x[6] - 10 * x[5] - 8 * x[6];
    counter = 5;                 
}

/*************************************************
*     Function:
*
*   Parameters:
*
*      Returns:
*
*  Description:
*
**************************************************/
static void g9_max (void)
{
    int idx_i,idx_j;
    double x[7];
    double y[4];
    double r[4];
    double p[4];

    double fit_value;
    int env_pso;
    double temp_min,temp_value;

    env_pso = (cur_pso)?0:1;

    if (pso_update_pbest_each_cycle_pending)
    {
        for (idx_i = 0; idx_i <4 ; idx_i++ )
        {
            y[idx_i]  = psos[cur_pso].pbest_position_values[psos[cur_pso].popu_index][idx_i];
        }
    }
    else
    {
        for (idx_i = 0; idx_i <4 ; idx_i++ )
        {
            y[idx_i]  = psos[cur_pso].position_values[psos[cur_pso].popu_index][idx_i];
        }
    } 
    
    for (idx_i = 0; idx_i < (psos[env_pso].popu_size) ; idx_i++ )
    {
        if (pso_update_pbest_each_cycle_pending)
        {
            for (idx_j = 0; idx_j <7 ; idx_j++ )
            {
                x[idx_j] = psos[env_pso].pbest_position_values[idx_i][idx_j];
            }
        }
        else
        {
            for (idx_j = 0; idx_j <7 ; idx_j++ )
            {
                x[idx_j] = psos[env_pso].position_values[idx_i][idx_j];
            }
        } 

        // constrains
        r[0] = 2 * x[0] * x[0] + 3 * pow(x[1],4) + x[2] + 4 * x[3] * x[3] + 5 * x[4]  - 127;
        r[1] = 7 * x[0] + 3 * x[1] + 10 * x[2] * x[2] + x[3] - x[4] - 282;
        r[2] = 23 * x[0] + x[1] * x[1] + 6 * x[5] * x[5] - 8 * x[6] - 196;
        r[3] = 4 * x[0] * x[0] + x[1] * x[1] - 3 * x[0] * x[1] + 2 * x[2] * x[2] + 5 * x[5] - 11 * x[6];

        temp_value = pow((x[0] - 10),2) + 5 * pow((x[1] - 12),2) + pow(x[2],4) + 3 * pow((x[3] - 11),2) 
                     + 10 * pow((x[4]),6) + 7 * pow(x[5],2) + pow(x[6],4) - 4 * x[5] * x[6] - 10 * x[5] - 8 * x[6];
                     
        for (idx_j = 0; idx_j <4 ; idx_j++ )
        {
            if ((r[idx_j]) >= (-y[idx_j]/200.0))
            {
                p[idx_j] = y[idx_j] * r[idx_j] + 100 * r[idx_j] * r[idx_j];
            } 
            else
            {
                p[idx_j] = - y[idx_j] * y[idx_j]/400.0;
            }
            temp_value += p[idx_j];
        } 
        if (idx_i == 0)
        {
            temp_min = temp_value;
            fit_value = temp_min;  
        }
        else
        {
            fit_value = min(temp_value,temp_min);
            temp_min = fit_value;  
        }
 
    }
    
    psos[cur_pso].eva_fun_value = fit_value;
}

/*************************************************
*     Function:
*
*   Parameters:
*
*      Returns:
*
*  Description:
*
**************************************************/
static void f6 (void)
{
    /*
	    This is the f6 function as described in the Handbook of
	    Genetic Algorithms, p.8
    */
    double num, denom, f6;

    num = (sin(sqrt((psos[cur_pso].position_values[psos[cur_pso].popu_index][0]*psos[cur_pso].position_values[psos[cur_pso].popu_index][0])+(psos[cur_pso].position_values[psos[cur_pso].popu_index][1]*psos[cur_pso].position_values[psos[cur_pso].popu_index][1]))))  *
		 (sin(sqrt((psos[cur_pso].position_values[psos[cur_pso].popu_index][0]*psos[cur_pso].position_values[psos[cur_pso].popu_index][0])+(psos[cur_pso].position_values[psos[cur_pso].popu_index][1]*psos[cur_pso].position_values[psos[cur_pso].popu_index][1])))) - 0.5;

	  denom = (1.0 + 0.001 * ((psos[cur_pso].position_values[psos[cur_pso].popu_index][0] * psos[cur_pso].position_values[psos[cur_pso].popu_index][0]) + (psos[cur_pso].position_values[psos[cur_pso].popu_index][1]*psos[cur_pso].position_values[psos[cur_pso].popu_index][1]))) *
		  (1.0 + 0.001 * ((psos[cur_pso].position_values[psos[cur_pso].popu_index][0] * psos[cur_pso].position_values[psos[cur_pso].popu_index][0]) + (psos[cur_pso].position_values[psos[cur_pso].popu_index][1]*psos[cur_pso].position_values[psos[cur_pso].popu_index][1])));

	  f6 = (double) 0.5 - (num/denom);

    psos[cur_pso].eva_fun_value = 1 - f6;
	  
}

/*************************************************
*     Function:
*
*   Parameters:
*
*      Returns:
*
*  Description:
*
**************************************************/
static void sphere (void)
{
    /* This is the familiar sphere model  */

	  double result;
	  int idx_i;

	  result=0.0;

	  for (idx_i = 0; idx_i < ( psos[cur_pso].dimension ); idx_i++)
	  {
		  result += psos[cur_pso].position_values[psos[cur_pso].popu_index][idx_i] * psos[cur_pso].position_values[psos[cur_pso].popu_index][idx_i];
	  }

    psos[cur_pso].eva_fun_value = result;
}

/*************************************************
*     Function:
*
*   Parameters:
*
*      Returns:
*
*  Description:
*
**************************************************/
static void rosenbrock (void)
{

	/* this is the Rosenbrock function  */

	int idx_i;
	double result;

	result=0.0;

	for (idx_i = 1; idx_i < ( psos[cur_pso].dimension ); idx_i++)
	{
		result += 100.0*(psos[cur_pso].position_values[psos[cur_pso].popu_index][idx_i] - psos[cur_pso].position_values[psos[cur_pso].popu_index][idx_i - 1] * psos[cur_pso].position_values[psos[cur_pso].popu_index][idx_i - 1]) * (psos[cur_pso].position_values[psos[cur_pso].popu_index][idx_i] - psos[cur_pso].position_values[psos[cur_pso].popu_index][idx_i - 1] * psos[cur_pso].position_values[psos[cur_pso].popu_index][idx_i - 1]) + (psos[cur_pso].position_values[psos[cur_pso].popu_index][idx_i - 1]-1) * (psos[cur_pso].position_values[psos[cur_pso].popu_index][idx_i - 1]-1);
	}
  
	psos[cur_pso].eva_fun_value = fabs(result);
}

/*************************************************
*     Function:
*
*   Parameters:
*
*      Returns:
*
*  Description:
*
**************************************************/
static void rastrigrin (void)
{
	  /* This is the generalized Rastrigrin function  */

	  int idx_i;
	  double result;

	  result=0.0;

	  for (idx_i = 0;idx_i < ( psos[cur_pso].dimension ); idx_i++)
	  {
		  result +=psos[cur_pso].position_values[psos[cur_pso].popu_index][idx_i] * psos[cur_pso].position_values[psos[cur_pso].popu_index][idx_i] - 10.0*cos(2.0*3.141591 * psos[cur_pso].position_values[psos[cur_pso].popu_index][idx_i])+10.0;
	  }
    psos[cur_pso].eva_fun_value = result;
}

/*************************************************
*     Function:
*
*   Parameters:
*
*      Returns:
*
*  Description:
*
**************************************************/
static void griewank (void)
{
	  /* This is the generalized Griewank function */

	  int idx_i;
	  double result_s,result_p;

	  result_s=0.0;
	  result_p=1.0;

	  for (idx_i = 0; idx_i < ( psos[cur_pso].dimension ); idx_i++)
	  {
		  result_s +=psos[cur_pso].position_values[psos[cur_pso].popu_index][idx_i] * psos[cur_pso].position_values[psos[cur_pso].popu_index][idx_i];
		  result_p *=cos(psos[cur_pso].position_values[psos[cur_pso].popu_index][idx_i]/sqrt(idx_i+1));
	  }
	  psos[cur_pso].eva_fun_value = result_s/4000.0 - result_p +1;
}

/*************************************************/
/*      Inertia Weight Update Functions          */    
/*************************************************/

/*************************************************
*     Function:
*
*   Parameters:
*
*      Returns:
*
*  Description:
*
**************************************************/
static void iw_update_methods (int iw_type)
{
    switch (iw_type)
    {
        case CONSTANT_IW:
          constant_iw();
          break;
        case LINEAR_IW:
          linear_iw();
          break;
        case FUZZY_TWO_IW:
          fuzzy_two_iw();
          break;
        case FUZZY_FOUR_IW:
          fuzzy_four_iw();
          break;
        case NOISE_ADDITION_IW:
          noise_addition_iw();
          break;
        default:          
          break;
    }
}

/*************************************************
*     Function:
*
*   Parameters:
*
*      Returns:
*
*  Description:
*
**************************************************/
static void constant_iw (void)
{
    psos[cur_pso].inertia_weight =  psos[cur_pso].init_inertia_weight;
}

/*************************************************
*     Function:
*
*   Parameters:
*
*      Returns:
*
*  Description:
*
**************************************************/
static void linear_iw (void)
{
    int total_gen,cur_index;
    
    total_gen =  total_cycle_of_PSOs * psos[cur_pso].env_data.max_generation;
    cur_index =  pso_cycle_index     * psos[cur_pso].env_data.max_generation + psos[cur_pso].gene_index;

    psos[cur_pso].inertia_weight = ((psos[cur_pso].init_inertia_weight ) - 0.4 ) * ( total_gen - cur_index) / total_gen + 0.4 + ((rand()%600)/1000.0) - 0.3;  
}

/*************************************************
*     Function:
*
*   Parameters:
*
*      Returns:
*
*  Description:
*
**************************************************/
static void fuzzy_two_iw (void)
{

}

/*************************************************
*     Function:
*
*   Parameters:
*
*      Returns:
*
*  Description:
*
**************************************************/
static void fuzzy_four_iw (void)
{

}

/*************************************************
*     Function:
*
*   Parameters:
*
*      Returns:
*
*  Description:
*
**************************************************/
static void noise_addition_iw (void)
{
    psos[cur_pso].inertia_weight =  psos[cur_pso].init_inertia_weight + ((rand()%600)/1000.0) - 0.3 ;
}

/*************************************************/
/*      Initizalization Functions                */    
/*************************************************/

/*************************************************
*     Function:
*
*   Parameters:
*
*      Returns:
*
*  Description:  Symmetry Initialization
*
**************************************************/
static void PSO_initialize_handler (int init_type)
{
    switch (init_type)
    {
        case PSO_RANDOM_SYMMETRY_INITIALIZE:
          PSO_random_symmetry_initialize();
          break;
        case PSO_RANDOM_ASYMMETRY_INITIALIZE:
          PSO_random_asymmetry_initialize();
          break;
        default:           
          break;
    }
}

/*************************************************
*     Function:
*
*   Parameters:
*
*      Returns:
*
*  Description:  Symmetry Initialization
*
**************************************************/
static void PSO_random_symmetry_initialize (void)
{
    int b; 
	  for (b=0;b<(psos[cur_pso].dimension);b++)
	  {
        if (psos[cur_pso].env_data.boundary_flag)
        {
           (psos[cur_pso].position_values)[psos[cur_pso].popu_index][b] = (float)((psos[cur_pso].env_data.up_boundaries[b]) - (psos[cur_pso].env_data.low_boundaries[b] ) ) * ((rand()%1000)/1000.0) + (psos[cur_pso].env_data.low_boundaries[b] );
		     psos[cur_pso].pbest_position_values[psos[cur_pso].popu_index][b] =  (psos[cur_pso].position_values)[psos[cur_pso].popu_index][b];
           psos[cur_pso].velocity_values[psos[cur_pso].popu_index][b] = 0.5* ((psos[cur_pso].env_data.up_boundaries[b]) - (psos[cur_pso].env_data.low_boundaries[b] ) ) * ((rand()%1000)/1000.0); 
        }
        else
        {
	  	     ((psos[cur_pso].position_values)[psos[cur_pso].popu_index][b] ) = (float) (((psos[cur_pso].env_data.init_range.right) - (psos[cur_pso].env_data.init_range.left ))*((rand()%1000)/1000.0) + (psos[cur_pso].env_data.init_range.left ));
		     psos[cur_pso].pbest_position_values[psos[cur_pso].popu_index][b] =  (psos[cur_pso].position_values)[psos[cur_pso].popu_index][b];
		     psos[cur_pso].velocity_values[psos[cur_pso].popu_index][b] = (psos[cur_pso].env_data.max_velocity)*((rand()%1000)/1000.0);
        }
		  if (((rand()%1000)/1000.0) > 0.5)
	      {
	  	      psos[cur_pso].velocity_values[psos[cur_pso].popu_index][b] = -(psos[cur_pso].velocity_values[psos[cur_pso].popu_index][b]  );
	      }
	  }
}

/*************************************************
*     Function:
*
*   Parameters:
*
*      Returns:
*
*  Description:  Asymmetry initialization
*
**************************************************/
static void PSO_random_asymmetry_initialize (void)
{
    int b; 
	  for (b=0;b<(psos[cur_pso].dimension);b++)
	  {
        if (psos[cur_pso].env_data.boundary_flag)
        {
           (psos[cur_pso].position_values)[psos[cur_pso].popu_index][b] = (float)((psos[cur_pso].env_data.up_boundaries[b]) - (psos[cur_pso].env_data.low_boundaries[b] ) ) * ((rand()%1000)/1000.0) + (psos[cur_pso].env_data.low_boundaries[b] );
		     psos[cur_pso].pbest_position_values[psos[cur_pso].popu_index][b] =  (psos[cur_pso].position_values)[psos[cur_pso].popu_index][b];
           psos[cur_pso].velocity_values[psos[cur_pso].popu_index][b] = 0.5* ((psos[cur_pso].env_data.up_boundaries[b]) - (psos[cur_pso].env_data.low_boundaries[b] ) ) * ((rand()%1000)/1000.0); 
        }
        else
        {
	  	     ((psos[cur_pso].position_values)[psos[cur_pso].popu_index][b] ) = (float) (((psos[cur_pso].env_data.init_range.right) - (psos[cur_pso].env_data.init_range.left ))*((rand()%1000)/1000.0) + (psos[cur_pso].env_data.init_range.left ));
		     psos[cur_pso].pbest_position_values[psos[cur_pso].popu_index][b] =  (psos[cur_pso].position_values)[psos[cur_pso].popu_index][b];
		     psos[cur_pso].velocity_values[psos[cur_pso].popu_index][b] = (psos[cur_pso].env_data.max_velocity)*((rand()%1000)/1000.0);
        }
		  if (((rand()%1000)/1000.0) > 0.5)
	      {
	  	      psos[cur_pso].velocity_values[psos[cur_pso].popu_index][b] = -(psos[cur_pso].velocity_values[psos[cur_pso].popu_index][b]  );
	      }
	  }
}
