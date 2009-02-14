#include "headfile.h"
#include "definiti.h"
#include "sofm.h"
#include "mem_loc.h"

#define MAX_NUM_CHARS 100
#define NEIGHBORHODD_DISTANCE 1

#define SAVE_CLUSTER  1
/**************************************************************/
/* Static Variable and Const Variable with File level scope   */
/**************************************************************/
 static SOFM_Type                sofm;
 static SOFM_Pattern_Set_Type    patset;
 static SOFM_State_Type          sofm_cur_state;

 #if  SAVE_CLUSTER
 static IMATRIX                  cluster_result;
 #endif

 static char                     pat_file_name[MAX_NUM_CHARS];
 static float                    *target_out;
 static IMATRIX                  test_result;
 static SOFM_2D_Size_Type        ite_per_update_neighbor;
 static SOFM_2D_Size_Type        slab_out_size;
/**************************************************************/
/* Function Prototypes for functions with file level scope    */
/**************************************************************/
static void read_sofm_parameters(char *dataFile);       // read SOFM parameters from I/O file
static void allocate_sofm_memory(void);                 // allocate SOFM memory spaces
static void free_sofm_memory(void);                     // free SOFM memory spaces
static void store_sofm_results(void);                   // write SOFM results to I/O file

static void sofm_initialize(void); 
static void sofm_initialize_weights(void);
static void sofm_state_handler(int);                    // SOFM state handle routine

static void sofm_get_pattern(void);
static void sofm_weight_normalization(void);
static void sofm_feedforward_input(void);
static void sofm_feedforward_output(void); 
static void sofm_winning_neuron(void); 
static void sofm_update_neighborhood(void);
//static void sofm_weight_step_change(void);
static void sofm_weight_change(void);
static void sofm_next_pattern(void);
static void sofm_next_iteration(void);
static void sofm_update_learning_rate(void);
static void sofm_update_conscience_factor(void);
static void sofm_training_done(void);
static void sofm_recall_done(void);

static float activate_function(float, int);
static float nn_linear_function(float);
static float nn_gausian_function(float);
static float nn_sigmoid_function(float);

static void print_net_parameters(void);
static void update_recall_result(void);

static float neighbor_func(int,int);
static float chef_hat(int);
static float mexican_hat(int);
static float stovepipe_hat(int);

/**************************************************************/
/*      Function Definitions                                  */    
/**************************************************************/      


/**************************************************************/
/*   SOFM Start and clean routines and interfaces               */    
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

void SOFM_Start_Up (char *dataFile)
{
    read_sofm_parameters(dataFile);
    allocate_sofm_memory();            // allocate memory for SOFM 
    sofm_initialize();
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
void SOFM_Clean_Up (void)
{
    store_sofm_results();
    free_sofm_memory();                // free memory space of SOFM
}


/************************************************************/
/*      SOFM  functons                                        */    
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
void SOFM_Main_Loop (void)
{
    BOOLEAN running;

    running = TRUE;
    while (running)
    {
        if ((sofm_cur_state == SOFM_TRAINING_DONE) || (sofm_cur_state == SOFM_RECALL_DONE) )
        {
            running = FALSE;
        }
        sofm_state_handler(sofm_cur_state);
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
static void read_sofm_parameters (char *dataFile)
{
    FILE  *frun;

    // open the runfile to input parameters
	  if ((frun=fopen(dataFile,"r"))==NULL)
	  {
		  printf("Cant read run file");
		  exit(1);
	  }
  
    // read SOFM's parameters from run file             
       // read SOFM's environment data
    fscanf(frun, "%d",&(sofm.env.operation_mode));     // training or recall
    fscanf(frun, "%d",&(sofm.env.train_mode));         // training mode if in training operation mode
    fscanf(frun, "%f",&(sofm.env.eta));                // learning rate
    fscanf(frun, "%f",&(sofm.env.shrink));             // learning rate shrinking coefficient
    fscanf(frun, "%f",&(sofm.env.gama));               // bias factor
    fscanf(frun, "%f",&(sofm.env.beta));               // beta
    fscanf(frun, "%f",&(sofm.env.criterion));          // training error criterion for termination
    fscanf(frun, "%d",&(sofm.env.max_ite));            // maximum number of generations
    fscanf(frun, "%d",&(sofm.env.max_tra_pat));        // total number of training patterns
    fscanf(frun, "%d",&(sofm.env.conscience));         // 0: no conscience, 1: conscience
    fscanf(frun, "%d",&(sofm.env.neighbor.width));     // initial width of neighborhood
    fscanf(frun, "%d",&(sofm.env.neighbor.height));    // initial height of neighborhood
    
       // read SOFM's Arch
    fscanf(frun, "%d",&(slab_out_size.height));        // out slab height
    fscanf(frun, "%d",&(slab_out_size.width));         // out slab width
    fscanf(frun, "%d",&(sofm.env.neighbor_function));  // neighborhood function

    // read in/out pattern parameters
    fscanf(frun, "%d",&(patset.size));                 // number of pattern
       // read pattern input dimension
    fscanf(frun, "%d",&(patset.dim_in));               // input dimension
       // read pattern output dimension
    fscanf(frun, "%d",&(patset.dim_out));              // output dimension
       // read pattern file name
    fscanf(frun, "%s", pat_file_name);                 // pattern file name

    fclose(frun);

    print_net_parameters();
}

static void print_net_parameters (void)
{
    // test
    printf( "%d\n",(sofm.env.operation_mode));       // training or recall
    printf( "%d\n",(sofm.env.train_mode));
    printf( "%f\n",(sofm.env.eta));                  // learning rate
    printf( "%f\n",(sofm.env.shrink));               // learning rate shrinking coefficient
    printf( "%f\n",(sofm.env.gama));                 // bias factor
    printf( "%f\n",(sofm.env.beta));                 //
    printf( "%f\n",(sofm.env.criterion));            // training error criterion for termination
    printf( "%d\n",(sofm.env.max_ite));              // maximum number of generations
    printf( "%d\n",(sofm.env.max_tra_pat));          // total number of training patterns
    printf( "%d\n",(sofm.env.conscience));
    printf( "%d\n",(sofm.env.neighbor.width));       // initial width of neighborhood
    printf( "%d\n",(sofm.env.neighbor.height));      // initial height of neighborhood
    printf( "%d\n",(slab_out_size.height));
    printf( "%d\n",(slab_out_size.width));
    printf( "%d\n\n",(sofm.env.neighbor_function));
    printf( "%d\n",(patset.size));                 // number of pattern
     // read pattern input dimension
    printf( "%d\n",(patset.dim_in));               // input dimension
     // read pattern output dimension
    printf( "%d\n",(patset.dim_out));              // output dimension
     // read pattern file name
    printf( "%s\n", pat_file_name);
}

static void update_recall_result(void)
{
	 int idx_out;

	 if (patset.dim_out > 0)
	 {
		  for (idx_out = 0; idx_out < patset.dim_out; idx_out++)
		  {   // loop throught the neurons of the output layer
				if (target_out[idx_out] > 0.1)
				{
					 #if  SAVE_CLUSTER
					 cluster_result[sofm.env.cur_pat][2] = idx_out;
					 #endif
					 test_result[sofm.env.winner.height * sofm.arch.slabs[1].size.width + sofm.env.winner.width][idx_out]++;
				}
		  }
	 }
	 else
	 {
		  test_result[sofm.env.winner.height * sofm.arch.slabs[1].size.width + sofm.env.winner.width][0]++;
    }
	 #if  SAVE_CLUSTER
	 cluster_result[sofm.env.cur_pat][0] = sofm.env.winner.height;
	 cluster_result[sofm.env.cur_pat][1] = sofm.env.winner.width;
	 #endif
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
static void allocate_sofm_memory (void)
{
    int idx,idx_h;

    // allocate memory for SOFM net
    sofm.arch.size = 2;
    sofm.arch.slabs = calloc(sofm.arch.size,sizeof(NN_Slab_Arch_Type));

    //allocate memory for input layer
	 sofm.arch.slabs[0].size.width       = patset.dim_in;
    sofm.arch.slabs[0].size.height      = 1;
    sofm.arch.slabs[0].neurons    = (Neuron_Type**)calloc(sofm.arch.slabs[0].size.height,sizeof(Neuron_Type));
    for (idx_h = 0; idx_h < sofm.arch.slabs[0].size.height; idx_h++)
    {
        sofm.arch.slabs[0].neurons[idx_h] = (Neuron_Type*)calloc(sofm.arch.slabs[0].size.width,sizeof(Neuron_Type));
    }

    sofm.arch.slabs[0].slab_type =  NN_INPUT_LAYER;
       // specify and allocate memory for neurons of input layer
    for (idx = 0 ; idx < sofm.arch.slabs[0].size.width ; idx++)
    {
        for (idx_h = 0 ; idx_h < sofm.arch.slabs[0].size.height ; idx_h++)
        {
            sofm.arch.slabs[0].neurons[idx_h][idx].neuron_function = NN_LINEAR_FUNCTION;
		      FVectorAllocate(&(sofm.arch.slabs[0].neurons[idx_h][idx].delta_w),1);
		      FVectorAllocate(&(sofm.arch.slabs[0].neurons[idx_h][idx].w),1);
        }
    }

    // allocate memory for output layer
    sofm.arch.slabs[sofm.arch.size - 1].size.width       = slab_out_size.width; 
    sofm.arch.slabs[sofm.arch.size - 1].size.height      = slab_out_size.height; 

    sofm.arch.slabs[sofm.arch.size - 1].neurons    = (Neuron_Type**)calloc(sofm.arch.slabs[sofm.arch.size - 1].size.height,sizeof(Neuron_Type));
    for (idx_h = 0; idx_h < sofm.arch.slabs[sofm.arch.size - 1].size.height; idx_h++)
    {
        sofm.arch.slabs[sofm.arch.size - 1].neurons[idx_h] = (Neuron_Type*)calloc(sofm.arch.slabs[sofm.arch.size - 1].size.width,sizeof(Neuron_Type));
    }
    sofm.arch.slabs[sofm.arch.size - 1].slab_type = NN_OUTPUT_LAYER;
       // specify and allocate memory for neurons of output layer
    for (idx =0 ; idx < sofm.arch.slabs[sofm.arch.size - 1].size.width ; idx++)
    {
        for (idx_h = 0 ; idx_h < sofm.arch.slabs[sofm.arch.size - 1].size.height ; idx_h++)
        {
		      sofm.arch.slabs[sofm.arch.size - 1].neurons[idx_h][idx].neuron_function = NN_LINEAR_FUNCTION;
		      sofm.arch.slabs[sofm.arch.size - 1].neurons[idx_h][idx].c_f = 1.0/(sofm.arch.slabs[sofm.arch.size - 1].size.height * sofm.arch.slabs[sofm.arch.size - 1].size.width);
            sofm.arch.slabs[sofm.arch.size - 1].neurons[idx_h][idx].b_v = 0.0;
            sofm.arch.slabs[sofm.arch.size - 1].neurons[idx_h][idx].w_s = 0;    // no winner at beginning
		      FVectorAllocate(&(sofm.arch.slabs[sofm.arch.size - 1].neurons[idx_h][idx].delta_w),sofm.arch.slabs[sofm.arch.size - 2].size.width);
            FVectorAllocate(&(sofm.arch.slabs[sofm.arch.size - 1].neurons[idx_h][idx].w),sofm.arch.slabs[sofm.arch.size - 2].size.width);
        }
    }
      
    // allocate memory for pattern set
	 FMatrixAllocate(&(patset.patterns),patset.size,(patset.dim_in + patset.dim_out));

    #if  SAVE_CLUSTER
	 // allocate memory for storing cluster information for each pattern
	 if (sofm.env.operation_mode == NN_RECALL)
	 {   // allocate for test result storage
		  if (patset.dim_out == 0)
		  {
            IMatrixAllocate(&(cluster_result),patset.size,2);
		  }
		  else
		  {
		      IMatrixAllocate(&(cluster_result),patset.size,3);
		  }
	 }
	 #endif

	 // allocate memory for target output
	 if (patset.dim_out > 0)
	 {
		  target_out = calloc(patset.dim_out,sizeof(float));
    }
    if (sofm.env.operation_mode == NN_RECALL)
	 {   // allocate for test result storage
		  if (patset.dim_out == 0)
		  {
            IMatrixAllocate(&(test_result),(sofm.arch.slabs[1].size.height * sofm.arch.slabs[1].size.width),1); 
		  }
		  else
		  {
		      IMatrixAllocate(&(test_result),(sofm.arch.slabs[1].size.height * sofm.arch.slabs[1].size.width),patset.dim_out);
		  }
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
static void free_sofm_memory (void)
{
    int idx,idx_h;
    if (sofm.env.operation_mode == NN_RECALL)
    {   // allocate for test result storage
		  IMatrixFree(test_result,(sofm.arch.slabs[1].size.height * sofm.arch.slabs[1].size.width));
        #if SVAE_CLUSTER
		  ImatrixFree(cluster_result,patset.size);
        #endif
    }
    // free memory for pattern set
    FMatrixFree(patset.patterns,patset.size);

    // free memory for SOFM net
       // free memory for output layer
    for (idx =0 ; idx < sofm.arch.slabs[sofm.arch.size - 1].size.width ; idx++)
    {
       for (idx_h = 0 ; idx_h < sofm.arch.slabs[sofm.arch.size - 1].size.height ; idx_h++)
       {
           free(sofm.arch.slabs[sofm.arch.size - 1].neurons[idx_h][idx].w);
           free(sofm.arch.slabs[sofm.arch.size - 1].neurons[idx_h][idx].delta_w);
       }
    }
    for (idx =0 ; idx < sofm.arch.slabs[sofm.arch.size - 1].size.height ; idx++)
    {
       free(sofm.arch.slabs[sofm.arch.size - 1].neurons[idx]);
    }
    free(sofm.arch.slabs[sofm.arch.size - 1].neurons);

       // free memory for input layer 
    for (idx =0 ; idx < sofm.arch.slabs[0].size.width ; idx++)
    {
       for (idx_h = 0 ; idx_h < sofm.arch.slabs[0].size.height ; idx_h++)
       {
           free(sofm.arch.slabs[0].neurons[idx_h][idx].w);
           free(sofm.arch.slabs[0].neurons[idx_h][idx].delta_w);
       }
    }
    for (idx =0 ; idx < sofm.arch.slabs[0].size.height ; idx++)
    {
       free(sofm.arch.slabs[0].neurons[idx]);
    }
    free(sofm.arch.slabs[0].neurons);   
       
    free(sofm.arch.slabs);
	 if (patset.dim_out >0)
	 {
	     free(target_out);
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
static void sofm_initialize (void)
{
    int idx_i,idx_j;
    int stime;
    long ltime;
    FILE  *frun;

    // get the current calendar time 
    ltime = time(NULL);
    stime = (unsigned) ltime/2;
    srand(stime);
   
    // sofm read patterns from file
    // open the pattern file to obtain in/out patterns
	  if ((frun=fopen(pat_file_name,"r"))==NULL)
	  {
		  printf("Cant read pattern file");
		  exit(1);
	  }

    for (idx_i = 0; idx_i < patset.size; idx_i++)
    {
        for (idx_j = 0; idx_j < (patset.dim_in + patset.dim_out) ; idx_j++)
        {
				fscanf(frun, "%f",&(patset.patterns[idx_i][idx_j]));
		  }
    }
	  fclose(frun);

    sofm.env.winner.height = 0;
    sofm.env.winner.width  = 0;

    ite_per_update_neighbor.height = sofm.env.max_ite/(sofm.env.neighbor.height + 1);
    ite_per_update_neighbor.width  = sofm.env.max_ite/(sofm.env.neighbor.width + 1);

    // sofm randomly initialize weights
    sofm_initialize_weights();

    // sofm initial setting
    sofm_cur_state = SOFM_GET_PATTERN;
    sofm.env.cur_ite = 0;
    sofm.env.cur_pat = 0;
    sofm.env.pat_counter = 0;
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
static void sofm_initialize_weights (void)
{
    int idx_neuron_w,idx_neuron_h,idx_weight;
    float prob;
    int idx_cn_h, idx_cn_w,idx_pn;
    FILE  *fout;

    if (sofm.env.operation_mode == NN_TRAINING)
    {
        // initialize weight for the neurons in the output layer
        for (idx_weight = 0; idx_weight < (sofm.arch.slabs[sofm.arch.size - 2].size.width); idx_weight++ )
        {
            for (idx_neuron_h = 0; idx_neuron_h < (sofm.arch.slabs[sofm.arch.size - 1].size.height); idx_neuron_h++)
            {
                for (idx_neuron_w = 0; idx_neuron_w < (sofm.arch.slabs[sofm.arch.size - 1].size.width); idx_neuron_w++)
                {
                    prob = ((rand()%1000)/1000.0) * 0.2;
                    sofm.arch.slabs[sofm.arch.size - 1].neurons[idx_neuron_h][idx_neuron_w].w[idx_weight]       =  prob + 0.4;       // (0.4, 0.6)
                    sofm.arch.slabs[sofm.arch.size - 1].neurons[idx_neuron_h][idx_neuron_w].delta_w[idx_weight] = 0.0;
                }
            }
		  }
    }
    else
    {   // RECALL operation mode
        // read in weights from file
        // open file for write
        if ((fout=fopen("SOFM_res.txt","r"))==NULL)
	      {
		      printf("Cant open file for reading SOFM training  results");
		      exit(1);
	      }
        for (idx_cn_h = 0; idx_cn_h < (sofm.arch.slabs[1].size.height) ; idx_cn_h++)
        {   
            for (idx_cn_w = 0; idx_cn_w < (sofm.arch.slabs[1].size.width) ; idx_cn_w++)
            {   // loop through the neurons in the current layer            
                for (idx_pn = 0; idx_pn < (sofm.arch.slabs[0].size.width) ; idx_pn++)
                {   // loop through the connect weights of the current neurons
                    fscanf(fout,"%f",&(sofm.arch.slabs[1].neurons[idx_cn_h][idx_cn_w].w[idx_pn]));
                }
            }
        }
        fclose(fout);
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
static void  store_sofm_results (void)
{
    // store weights
    int idx_cn_h,idx_cn_w, idx_pn;
    FILE  *fout;

    // open file for write

    if (sofm.env.operation_mode == NN_TRAINING)
	  {
        if ((fout=fopen("SOFM_res.txt","w"))==NULL)
		  {
				printf("Cant open file for writing SOFM weights");
				exit(1);
		  }
        for (idx_cn_h = 0; idx_cn_h < (sofm.arch.slabs[1].size.height) ; idx_cn_h++)
        {   
            for (idx_cn_w = 0; idx_cn_w < (sofm.arch.slabs[1].size.width) ; idx_cn_w++)
            {   // loop through the neurons in the current layer            
                for (idx_pn = 0; idx_pn < (sofm.arch.slabs[0].size.width) ; idx_pn++)
                {   // loop through the connect weights of the current neurons
                    fprintf(fout,"%f\n",(sofm.arch.slabs[1].neurons[idx_cn_h][idx_cn_w].w[idx_pn]));
                }
                fprintf(fout,"\n");
            }
        }

        fclose(fout);
    }
    else // RECALL 
    {
        if ((fout=fopen("SOFM_test.txt","w"))==NULL)
        {
        	printf("Cant open file for writing SOFM test results");
        	exit(1);
        }
        fprintf(fout,"\n");
		  if (patset.dim_out >0)
		  {
				for (idx_pn = 0; idx_pn < patset.dim_out; idx_pn++ )
				{
					 fprintf(fout,"\tClass %d",idx_pn);
		      }
		  }
        fprintf(fout,"\n-----------------------------------------------------\n");
        
        for (idx_cn_h = 0; idx_cn_h < sofm.arch.slabs[1].size.height; idx_cn_h++ )
        {
            for (idx_cn_w = 0; idx_cn_w < sofm.arch.slabs[1].size.width; idx_cn_w++ )
            {
					 fprintf(fout,"%2.2d %2.2d\t", idx_cn_h, idx_cn_w);
					 if (patset.dim_out == 0)
					 {
                    fprintf(fout,"%d\t",test_result[idx_cn_h * sofm.arch.slabs[1].size.width + idx_cn_w][0] );
					 }
					 else
					 {
						  for (idx_pn = 0; idx_pn < patset.dim_out; idx_pn++ )
						  {
								fprintf(fout,"%d\t",test_result[idx_cn_h * sofm.arch.slabs[1].size.width + idx_cn_w][idx_pn] );
					     }
					 }
                fprintf(fout,"\n");
            }
        }
		  fprintf(fout,"\n");
        #if SAVE_CLUSTER
		  fprintf(fout,"\n\--------------------------------------------------------\n");
		  fprintf(fout,"\tcluster number for each pattern\n");
		  if (patset.dim_out == 0)
		  {
            fprintf(fout,"\nindex\tcluster height\tcluster width \n");
		  }
		  else
		  {
				fprintf(fout,"\nindex\tcluster height\tcluster width\ttarget \n");
		  }
		  for (idx_cn_h = 0; idx_cn_h < patset.size; idx_cn_h++)
		  {
				if (patset.dim_out == 0)
				{
                fprintf(fout,"%d\t%d\t%d\n",idx_cn_h, cluster_result[idx_cn_h][0], cluster_result[idx_cn_h][1] );
				}
				else
				{
				    fprintf(fout,"%d\t%d\t%d\t\t%d\n",idx_cn_h, cluster_result[idx_cn_h][0], cluster_result[idx_cn_h][1],cluster_result[idx_cn_h][2] );
				}
		  }
		  fprintf(fout,"\n");
        #endif
        fclose(fout);
    }  
}

/************************************************************/
/*      SOFM  State Handlers                                  */
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
static void sofm_state_handler (int state_index)
{
   switch (state_index)
   {
       case SOFM_GET_PATTERN:
          sofm_get_pattern();
			break;
       case SOFM_WEIGHT_NORMALIZATION:
          sofm_weight_normalization();
          break;
		 case SOFM_FEEDFORWARD_INPUT:
			sofm_feedforward_input();
          break;
       case SOFM_FEEDFORWARD_OUTPUT:
          sofm_feedforward_output();
          break;
       case SOFM_WINNING_NEURON:
          sofm_winning_neuron();
          break;
       case SOFM_UPDATE_NEIGHBORHOOD:
          sofm_update_neighborhood();
          break;
       //case SOFM_WEIGHT_STEP_CHANGE:
       //   sofm_weight_step_change();
       //   break;
       case SOFM_WEIGHT_CHANGE:
          sofm_weight_change();
          break;
       case SOFM_NEXT_PATTERN:
          sofm_next_pattern();
          break;
       case SOFM_NEXT_ITERATION:
          sofm_next_iteration();
          break;
       case SOFM_UPDATE_LEARNING_RATE:
          sofm_update_learning_rate();
          break;
       case SOFM_UPDATE_CONSCIENCE_FACTOR:
          sofm_update_conscience_factor();
          break;
       case SOFM_TRAINING_DONE:
          sofm_training_done();
          break;
       case SOFM_RECALL_DONE:
          sofm_recall_done();
          break;
       default:
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
static void sofm_get_pattern (void)
{
   int idx;

	 for (idx = 0; idx < (sofm.arch.slabs[0].size.width); idx++)
	 {
		  sofm.arch.slabs[0].neurons[0][idx].in = patset.patterns[sofm.env.cur_pat][idx];
	 }
	 if (patset.dim_out > 0)
	 {
		  for (idx = 0; idx < patset.dim_out; idx++ )
		  {
				target_out[idx] = patset.patterns[sofm.env.cur_pat][patset.dim_in + idx];
				//printf("%d: %f\n",sofm.env.cur_pat, target_out[idx]);
	     }
	 }
	if (sofm.env.operation_mode == NN_TRAINING)
   {     
	     sofm_cur_state = SOFM_WEIGHT_NORMALIZATION;
   }
   else
   {
       sofm_cur_state = SOFM_FEEDFORWARD_INPUT;
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
static void sofm_weight_normalization (void)
{
   int idx_cn_h,idx_cn_w, idx_pn;
   double sum;
   float temp_f;

   for (idx_cn_h = 0; idx_cn_h < (sofm.arch.slabs[1].size.height) ; idx_cn_h++)
   {   
       for (idx_cn_w = 0; idx_cn_w < (sofm.arch.slabs[1].size.width) ; idx_cn_w++)
       {   // loop through neurons in the output layer
           sum = 0.0;
           for (idx_pn = 0; idx_pn < (sofm.arch.slabs[0].size.width) ; idx_pn++ )
           {   // loop through all the weights connected to this neuron
               sum += sofm.arch.slabs[1].neurons[idx_cn_h][idx_cn_w].w[idx_pn] * sofm.arch.slabs[1].neurons[idx_cn_h][idx_cn_w].w[idx_pn];
           }
           sum = sqrt(sum);
           if (sum > 0.0)
           {
              for (idx_pn = 0; idx_pn < (sofm.arch.slabs[0].size.width) ; idx_pn++ )
              {   // loop through all the weights connected to this neuron
                  temp_f = sofm.arch.slabs[1].neurons[idx_cn_h][idx_cn_w].w[idx_pn]/sum;
                  sofm.arch.slabs[1].neurons[idx_cn_h][idx_cn_w].w[idx_pn] = temp_f;
              }
           }
       }
   }
   sofm_cur_state = SOFM_FEEDFORWARD_INPUT;
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
static void sofm_feedforward_input(void)
{
	 int idx;

   for (idx = 0; idx < (sofm.arch.slabs[0].size.width); idx++)
	 {
		  sofm.arch.slabs[0].neurons[0][idx].out = sofm.arch.slabs[0].neurons[0][idx].in;
	 }

	 sofm_cur_state = SOFM_FEEDFORWARD_OUTPUT;
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
static void sofm_feedforward_output (void)
{
	  int idx_out_h, idx_out_w, idx_prev;
	  double sum, temp_f;
	  for (idx_out_h = 0; idx_out_h < (sofm.arch.slabs[1].size.height ); idx_out_h++)
	  {   
	      for (idx_out_w = 0; idx_out_w < (sofm.arch.slabs[1].size.width ); idx_out_w++)
	      {   // loop throught the neurons of the output layer
	          sum = 0.0;
		      for ( idx_prev = 0; idx_prev < (sofm.arch.slabs[0].size.width ); idx_prev++)
		      {   // loop through the neurons of the input layer
                temp_f = (sofm.arch.slabs[0].neurons[0][idx_prev].out - sofm.arch.slabs[1].neurons[idx_out_h][idx_out_w].w[idx_prev] );
			      sum += (temp_f * temp_f);
		      }
            temp_f = sqrt(sum); 
		      sofm.arch.slabs[1].neurons[idx_out_h][idx_out_w].in = temp_f;
		      sofm.arch.slabs[1].neurons[idx_out_h][idx_out_w].out =  activate_function(temp_f,sofm.arch.slabs[1].neurons[idx_out_h][idx_out_w].neuron_function);
        }
	  }
    sofm_cur_state = SOFM_WINNING_NEURON;
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
static void sofm_winning_neuron (void)
{
    int idx_h, idx_w;
    SOFM_2D_Size_Type temp_w;
    float min_v = 1000.0;

    for (idx_h = 0; idx_h < (sofm.arch.slabs[1].size.height); idx_h++)
    {   
        for (idx_w = 0; idx_w < (sofm.arch.slabs[1].size.width); idx_w++)
        {   // loop through the neurons in output layer
            if ((sofm.arch.slabs[1].neurons[idx_h][idx_w].out - sofm.arch.slabs[1].neurons[idx_h][idx_w].b_v) < min_v)
            {
                min_v = sofm.arch.slabs[1].neurons[idx_h][idx_w].out - sofm.arch.slabs[1].neurons[idx_h][idx_w].b_v;
                temp_w.height = idx_h;
                temp_w.width  = idx_w;
            }
        }
    }
    sofm.arch.slabs[1].neurons[sofm.env.winner.height][sofm.env.winner.width].w_s = 0;    // clear the old winner status
    sofm.env.winner.height = temp_w.height;                               // assign new winner
    sofm.env.winner.width = temp_w.width;
    sofm.arch.slabs[1].neurons[sofm.env.winner.height][sofm.env.winner.width].w_s = 1;    // set new winner status

    if (sofm.env.operation_mode == NN_TRAINING)
    {
        sofm_cur_state = SOFM_UPDATE_NEIGHBORHOOD;
    }
    else
    {   // recall
        update_recall_result();
        sofm_cur_state = SOFM_NEXT_PATTERN;
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
static void sofm_next_pattern (void)
{
    if (sofm.env.operation_mode == NN_TRAINING)
    {
        if (sofm.env.train_mode == SOFM_RANDOM_MODE)
        {   // random training
	          sofm.env.cur_pat = rand()%(sofm.env.max_tra_pat);     // next random pattern index
        }
        else
        {   // sequential training
            if (++sofm.env.cur_pat >= sofm.env.max_tra_pat)
            {
                sofm.env.cur_pat = 0;
            }
        }
        if ((++sofm.env.pat_counter) <sofm.env.max_tra_pat)          
        {   // add other termination criterion here
            sofm_cur_state = SOFM_UPDATE_LEARNING_RATE;
        }
        else   
        {
            sofm_cur_state = SOFM_NEXT_ITERATION;
        }
    }
    else  // recall
    {
        if ((++sofm.env.cur_pat) < patset.size) 
        {   
            sofm_cur_state = SOFM_GET_PATTERN;
        }
        else
        {
            sofm_cur_state = SOFM_RECALL_DONE;
        }
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
static void sofm_next_iteration (void)
{
    sofm.env.pat_counter = 0;
    if ((++sofm.env.cur_ite) < sofm.env.max_ite)          
    {   // add other termination criterion here
        sofm_cur_state = SOFM_UPDATE_LEARNING_RATE;
    }
    else   
    {
        sofm.env.pat_counter = 0;
        sofm_cur_state = SOFM_TRAINING_DONE;
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
static void sofm_update_neighborhood (void)
{
    static int temp_c;

    temp_c = sofm.env.cur_ite/ite_per_update_neighbor.height;
    sofm.env.cur_neighbor.height =  sofm.env.neighbor.height - temp_c;
//printf("temp_c = %d\theight = %d\n",temp_c, sofm.env.cur_neighbor.height);
    temp_c = sofm.env.cur_ite/ite_per_update_neighbor.width;
    sofm.env.cur_neighbor.width =  sofm.env.neighbor.width - temp_c;
//printf("temp_c = %d\theight = %d\n",temp_c, sofm.env.cur_neighbor.height);
    if (sofm.env.cur_neighbor.height < 0)
    {
        sofm.env.cur_neighbor.height = 0;
    }
    if (sofm.env.cur_neighbor.width < 0)
    {
        sofm.env.cur_neighbor.width = 0;
    }
    sofm_cur_state = SOFM_WEIGHT_CHANGE;
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
static void sofm_weight_change (void)
{
    int idx_pn, idx_h, idx_w;
    int n_h,n_w;

    for (idx_pn = 0; idx_pn < (sofm.arch.slabs[0].size.width) ; idx_pn++)
    {   // loop through the connect weights of the current neurons
        for (idx_h = -(sofm.env.cur_neighbor.height); idx_h <= (sofm.env.cur_neighbor.height ) ; idx_h++ )
        {
            n_h = sofm.env.winner.height + idx_h;
            if (n_h < 0)
            {
                n_h += sofm.arch.slabs[1].size.height;
            }
            else if (n_h >= sofm.arch.slabs[1].size.height)
            {
                n_h -= sofm.arch.slabs[1].size.height;
            }
            for (idx_w = -(sofm.env.cur_neighbor.width); idx_w <= (sofm.env.cur_neighbor.width ) ; idx_w++ )
            {
                n_w = sofm.env.winner.width  + idx_w;
                if (n_w < 0)
                {
                    n_w += sofm.arch.slabs[1].size.width;
                }
                else if (n_w >= sofm.arch.slabs[1].size.width)
                {
                    n_w -= sofm.arch.slabs[1].size.width;
                }
                sofm.arch.slabs[1].neurons[n_h][n_w].delta_w[idx_pn] = sofm.arch.slabs[0].neurons[0][idx_pn].out - sofm.arch.slabs[1].neurons[n_h][n_w].w[idx_pn];
                sofm.arch.slabs[1].neurons[n_h][n_w].delta_w[idx_pn] *= (sofm.env.eta * neighbor_func(idx_h,idx_w));

                sofm.arch.slabs[1].neurons[n_h][n_w].w[idx_pn] += sofm.arch.slabs[1].neurons[n_h][n_w].delta_w[idx_pn];
            }
        }
        
        
    }
    sofm_cur_state = SOFM_NEXT_PATTERN;
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
static float neighbor_func (int height, int width)
{
    int temp_i;
    float result;

    temp_i = (height > width)?height:width;
    switch (sofm.env.neighbor_function)
    {
        case CHEF_HAT:
          result = chef_hat(temp_i);
          break;
        case MEXICAN_HAT:
          result = mexican_hat(temp_i);
          break;
        case STOVEPIPE_HAT:
          result = stovepipe_hat(temp_i);
          break;
        default:
          printf("need to specify neighborhood function\n");
          exit(1);
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
static float chef_hat (int distance)
{
    if (distance <= NEIGHBORHODD_DISTANCE)
        return(1.0);
    else
        return(0.0);
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
static float mexican_hat (int distance)
{
    printf("Mexican hat function is not implemeted\n");
    exit(1);
    return(1.0);
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
static float stovepipe_hat (int distance)
{
    printf("Stovepipe hat function is not implemeted\n");
    exit(1);
    return(1.0);
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
static void sofm_update_learning_rate (void)
{
    sofm.env.eta *= sofm.env.shrink;
    sofm_cur_state = SOFM_UPDATE_CONSCIENCE_FACTOR;
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
static void sofm_update_conscience_factor (void)
{
    int idx_h, idx_w;     
    float temp_f;
    if (sofm.env.conscience == SOFM_CONSCIENCE)
    {
        for (idx_h = 0; idx_h < (sofm.arch.slabs[1].size.height); idx_h++)
        {   
            for (idx_w = 0; idx_w < (sofm.arch.slabs[1].size.width); idx_w++)
            {   // loop through the neurons in output layer
                temp_f  = sofm.arch.slabs[1].neurons[idx_h][idx_w].c_f;
                sofm.arch.slabs[1].neurons[idx_h][idx_w].c_f = temp_f + sofm.env.beta * (sofm.arch.slabs[1].neurons[idx_h][idx_w].w_s - temp_f);
                sofm.arch.slabs[1].neurons[idx_h][idx_w].b_v = sofm.env.gama * (1.0/(sofm.arch.slabs[1].size.height * sofm.arch.slabs[1].size.width) - sofm.arch.slabs[1].neurons[idx_h][idx_w].c_f ); 
            }
        }
    }
    sofm_cur_state = SOFM_GET_PATTERN;
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
static void sofm_training_done (void)
{
    sofm_weight_normalization();
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
static void sofm_recall_done (void)
{

}

/************************************************************/
/*      neuron activation functions                         */
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
static float activate_function(float sum, int index)
{
	 float res;
	 switch (index)
	 {
		  case NN_LINEAR_FUNCTION:
				 res = nn_linear_function(sum);
				 break;
		  case NN_GAUSIAN_FUNCTION:
				 res = nn_gausian_function(sum);
				 break;
		  case NN_SIGMOID_FUNCTION:
				 res = nn_sigmoid_function(sum);
				 break;
		  default:
             break;
	 }
	 return (res);
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
static float nn_linear_function (float sum)
{
	 float res;

    res = sum;
	 return (res);
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
static float nn_gausian_function (float sum)
{
	 float res;
   res = sum;      // No Guassian in SOFM
	 return (res);
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
static float nn_sigmoid_function (float sum)
{
	 float res;
   res = sum;     // No sigmoid in SOFM
	 return (res);
}

