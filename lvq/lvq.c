#include "headfile.h"
#include "definiti.h"
#include "lvq.h"
#include "mem_loc.h"

#define MAX_NUM_CHARS 100

#define SAVE_CLUSTER  1

/**************************************************************/
/* Static Variable and Const Variable with File level scope   */
/**************************************************************/
 static LVQ_Type                lvq;
 static LVQ_Pattern_Set_Type    patset;
 static LVQ_State_Type          lvq_cur_state;

 #if  SAVE_CLUSTER
 static IMATRIX                 cluster_result;
 #endif

 static char                    pat_file_name[MAX_NUM_CHARS];
 static float                   *target_out;
 static IMATRIX                 test_result;
/**************************************************************/
/* Function Prototypes for functions with file level scope    */
/**************************************************************/
static void read_lvq_parameters(char *dataFile);       // read LVQ parameters from I/O file
static void allocate_lvq_memory(void);                 // allocate LVQ memory spaces
static void free_lvq_memory(void);                     // free LVQ memory spaces
static void store_lvq_results(void);                   // write LVQ results to I/O file

static void lvq_initialize(void);
static void lvq_initialize_weights(void);
static void lvq_state_handler(int);                    // LVQ state handle routine

static void lvq_get_pattern(void);
static void lvq_weight_normalization(void);
static void lvq_feedforward_input(void);
static void lvq_feedforward_output(void);
static void lvq_winning_neuron(void);
static void lvq_weight_step_change(void);
static void lvq_weight_change(void);
static void lvq_next_pattern(void);
static void lvq_next_iteration(void);
static void lvq_update_learning_rate(void);
static void lvq_update_conscience_factor(void);
static void lvq_training_done(void);
static void lvq_recall_done(void);

static float activate_function(float, int);
static float nn_linear_function(float);
static float nn_gausian_function(float);
static float nn_sigmoid_function(float);

static void print_net_parameters(void);
static void update_recall_result(void);

/**************************************************************/
/*      Function Definitions                                  */
/**************************************************************/


/**************************************************************/
/*   LVQ Start and clean routines and interfaces               */
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

void LVQ_Start_Up (char *dataFile)
{
    read_lvq_parameters(dataFile);
    allocate_lvq_memory();            // allocate memory for LVQ
    lvq_initialize();
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
void LVQ_Clean_Up (void)
{
    store_lvq_results();
    free_lvq_memory();
}


/************************************************************/
/*      LVQ  functons                                        */
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
void LVQ_Main_Loop (void)
{
    BOOLEAN running;

    running = TRUE;
    while (running)
    {
        if ((lvq_cur_state == LVQ_TRAINING_DONE) || (lvq_cur_state == LVQ_RECALL_DONE) )
        {
            running = FALSE;
        }
        lvq_state_handler(lvq_cur_state);
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
static void read_lvq_parameters (char *dataFile)
{
    FILE  *frun;

    // open the runfile to input parameters
	  if ((frun=fopen(dataFile,"r"))==NULL)
	  {
		  printf("Cant read run file");
		  exit(1);
	  }

    // read LVQ's parameters from run file
       // read LVQ's environment data
    fscanf(frun, "%d",&(lvq.env.operation_mode));      // training or recall
    fscanf(frun, "%d",&(lvq.env.train_mode));          // training mode if in training operation mode
    fscanf(frun, "%f",&(lvq.env.eta));                 // learning rate
    fscanf(frun, "%f",&(lvq.env.shrink));              // learning rate shrinking coefficient
    fscanf(frun, "%f",&(lvq.env.gama));                // bias factor
    fscanf(frun, "%f",&(lvq.env.beta));                // beta
    fscanf(frun, "%f",&(lvq.env.criterion));           // training error criterion for termination
    fscanf(frun, "%d",&(lvq.env.max_ite));             // maximum number of generations
    fscanf(frun, "%d",&(lvq.env.max_tra_pat));         // total number of training patterns
    fscanf(frun, "%d",&(lvq.env.conscience));          // 0: no conscience, 1: conscience
    fscanf(frun, "%d",&(lvq.env.no_clusters));         // number of clusters

       // read LVQ's Arch

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
    printf( "%d\n",(lvq.env.operation_mode));       // training or recall
    printf( "%d\n",(lvq.env.train_mode));
    printf( "%f\n",(lvq.env.eta));                  // learning rate
    printf( "%f\n",(lvq.env.shrink));               // learning rate shrinking coefficient
    printf( "%f\n",(lvq.env.gama));                 // bias factor
    printf( "%f\n",(lvq.env.beta));                 //
    printf( "%f\n",(lvq.env.criterion));            // training error criterion for termination
    printf( "%d\n",(lvq.env.max_ite));              // maximum number of generations
    printf( "%d\n",(lvq.env.max_tra_pat));          // total number of training patterns
    printf( "%d\n",(lvq.env.conscience));
    printf( "%d\n",(lvq.env.no_clusters));

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

    for (idx_out = 0; idx_out < patset.dim_out; idx_out++)
	  {   // loop throught the output of the target pattern
        if (target_out[idx_out] > 0.1)
		  {
				#if  SAVE_CLUSTER
				cluster_result[lvq.env.cur_pat][0] = idx_out;
				#endif
				test_result[lvq.env.winner][idx_out]++;
        }
	  }

	 #if  SAVE_CLUSTER
	 cluster_result[lvq.env.cur_pat][1] = lvq.env.winner;
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
static void allocate_lvq_memory (void)
{
    int idx;

    // allocate memory for LVQ net
    lvq.arch.size = 2;
    lvq.arch.layers = calloc(lvq.arch.size,sizeof(NN_Layer_Arch_Type));

    //allocate memory for input layer
    lvq.arch.layers[0].size       = patset.dim_in;
    lvq.arch.layers[0].neurons    = calloc(patset.dim_in,sizeof(Neuron_Type));
    lvq.arch.layers[0].layer_type =  NN_INPUT_LAYER;
       // specify and allocate memory for neurons of input layer
    for (idx =0 ; idx < lvq.arch.layers[0].size ; idx++)
    {
        lvq.arch.layers[0].neurons[idx].neuron_function = NN_LINEAR_FUNCTION;
		  FVectorAllocate(&(lvq.arch.layers[0].neurons[idx].delta_w),1);
		  FVectorAllocate(&(lvq.arch.layers[0].neurons[idx].w),1);
    }

    // allocate memory for output layer
    lvq.arch.layers[lvq.arch.size - 1].size       = lvq.env.no_clusters;
    lvq.arch.layers[lvq.arch.size - 1].neurons    = calloc(lvq.env.no_clusters,sizeof(Neuron_Type));
    lvq.arch.layers[lvq.arch.size - 1].layer_type = NN_OUTPUT_LAYER;
       // specify and allocate memory for neurons of output layer
    for (idx =0 ; idx < lvq.arch.layers[lvq.arch.size - 1].size ; idx++)
    {
		  lvq.arch.layers[lvq.arch.size - 1].neurons[idx].neuron_function = NN_LINEAR_FUNCTION;
		  lvq.arch.layers[lvq.arch.size - 1].neurons[idx].c_f = 1.0/lvq.env.no_clusters;
        lvq.arch.layers[lvq.arch.size - 1].neurons[idx].b_v = 0.0;
        lvq.arch.layers[lvq.arch.size - 1].neurons[idx].w_s = 0;    // no winner at beginning
		  FVectorAllocate(&(lvq.arch.layers[lvq.arch.size - 1].neurons[idx].delta_w),lvq.arch.layers[lvq.arch.size - 2].size);
        FVectorAllocate(&(lvq.arch.layers[lvq.arch.size - 1].neurons[idx].w),lvq.arch.layers[lvq.arch.size - 2].size);
    }

    // allocate memory for pattern set
	 FMatrixAllocate(&(patset.patterns),patset.size,(patset.dim_in + patset.dim_out));

	 #if  SAVE_CLUSTER
	 // allocate memory for storing cluster information for each pattern
    if (lvq.env.operation_mode == NN_RECALL)
    {   // allocate for test result storage
		  IMatrixAllocate(&(cluster_result),patset.size,2);
	 }
	 #endif

    // allocate memory for target output
    target_out = calloc(patset.dim_out,sizeof(float));

    if (lvq.env.operation_mode == NN_RECALL)
    {   // allocate for test result storage
        IMatrixAllocate(&(test_result),lvq.env.no_clusters,patset.dim_out);
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
static void free_lvq_memory (void)
{
    int idx;
    if (lvq.env.operation_mode == NN_RECALL)
    {   // allocate for test result storage
		  IMatrixFree(test_result,lvq.env.no_clusters);
		  #if SVAE_CLUSTER
		  ImatrixFree(cluster_result,patset.size);
        #endif
    }
    // free memory for pattern set
    FMatrixFree(patset.patterns,patset.size);

    // free memory for LVQ net
       // free memory for output layer
    for (idx =0 ; idx < lvq.arch.layers[lvq.arch.size - 1].size ; idx++)
    {
       free(lvq.arch.layers[lvq.arch.size - 1].neurons[idx].w);
       free(lvq.arch.layers[lvq.arch.size - 1].neurons[idx].delta_w);
    }
    free(lvq.arch.layers[lvq.arch.size - 1].neurons);

       // free memory for input layer
    for (idx =0 ; idx < lvq.arch.layers[0].size ; idx++)
    {
        free(lvq.arch.layers[0].neurons[idx].w);
        free(lvq.arch.layers[0].neurons[idx].delta_w);
    }
    free(lvq.arch.layers[0].neurons);

    free(lvq.arch.layers);

    free(target_out);
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
static void lvq_initialize (void)
{
    int idx_i,idx_j;
    int stime;
    long ltime;
    FILE  *frun;

    // get the current calendar time
    ltime = time(NULL);
    stime = (unsigned) ltime/2;
    srand(stime);

    // lvq read patterns from file
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

    lvq.env.winner = 0;
    // lvq randomly initialize weights
    lvq_initialize_weights();

    // lvq initial setting
    lvq_cur_state = LVQ_GET_PATTERN;
    lvq.env.cur_ite = 0;
    lvq.env.cur_pat = 0;
    lvq.env.pat_counter = 0;
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
static void lvq_initialize_weights (void)
{
    int idx_layer,idx_neuron,idx_weight;
    float prob;
    int idx_cn, idx_pn;
    FILE  *fout;

    if (lvq.env.operation_mode == NN_TRAINING)
    {
        // initialize weight for the neurons in the output layer
        for (idx_weight = 0; idx_weight < (lvq.arch.layers[lvq.arch.size - 2].size); idx_weight++ )
        {
            for (idx_neuron = 0; idx_neuron < (lvq.arch.layers[lvq.arch.size - 1].size); idx_neuron++)
            {
                prob = (rand()%1000)/1000.0;
                lvq.arch.layers[lvq.arch.size - 1].neurons[idx_neuron].w[idx_weight]       =  prob;       // (0,1)
                lvq.arch.layers[lvq.arch.size - 1].neurons[idx_neuron].delta_w[idx_weight] = 0.0;
            }
		  }
    }
    else
    {   // RECALL operation mode
        // read in weights from file
        // open file for write
        if ((fout=fopen("LVQ_res.txt","r"))==NULL)
	      {
		      printf("Cant open file for reading LVQ training  results");
		      exit(1);
	      }
        for (idx_layer = 1; idx_layer < (lvq.arch.size) ; idx_layer++ )
        {   // loop through the layers
            for (idx_cn = 0; idx_cn < (lvq.arch.layers[idx_layer].size) ; idx_cn++)
            {   // loop through the neurons in the current layer
                for (idx_pn = 0; idx_pn < (lvq.arch.layers[idx_layer - 1].size) ; idx_pn++)
                {   // loop through the connect weights of the current neurons
                    fscanf(fout,"%f",&(lvq.arch.layers[idx_layer].neurons[idx_cn].w[idx_pn]));
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
static void  store_lvq_results (void)
{
    // store weights
    int idx_layer, idx_cn, idx_pn;
    FILE  *fout;

    // open file for write

    if (lvq.env.operation_mode == NN_TRAINING)
	  {
        if ((fout=fopen("LVQ_res.txt","w"))==NULL)
		  {
				printf("Cant open file for writing LVQ weights");
				exit(1);
		  }
        for (idx_layer = 1; idx_layer < (lvq.arch.size) ; idx_layer++ )
        {   // loop through the layers
            for (idx_cn = 0; idx_cn < (lvq.arch.layers[idx_layer].size) ; idx_cn++)
            {   // loop through the neurons in the current layer
                for (idx_pn = 0; idx_pn < (lvq.arch.layers[idx_layer - 1].size) ; idx_pn++)
                {   // loop through the connect weights of the current neurons
                    fprintf(fout,"%f\n",lvq.arch.layers[idx_layer].neurons[idx_cn].w[idx_pn]);
                }
                fprintf(fout,"\n");
            }
        }
        fclose(fout);
    }
    else // RECALL
    {
        if ((fout=fopen("LVQ_test.txt","w"))==NULL)
        {
        	printf("Cant open file for writing LVQ test results");
        	exit(1);
        }
        fprintf(fout,"\n");

        for (idx_pn = 0; idx_pn < patset.dim_out; idx_pn++ )
        {
            fprintf(fout,"\tClass %d",idx_pn);
        }
        fprintf(fout,"\n-----------------------------------------------------\n");

		  for (idx_cn = 0; idx_cn < lvq.env.no_clusters; idx_cn++ )
		  {
            fprintf(fout,"%d\t", idx_cn);
            for (idx_pn = 0; idx_pn < patset.dim_out; idx_pn++ )
            {
                fprintf(fout,"%d\t",test_result[idx_cn][idx_pn]);
            }
            fprintf(fout,"\n");
        }
		  fprintf(fout,"\n");
		  #if SAVE_CLUSTER
		  fprintf(fout,"\n\--------------------------------------------------------\n");
		  fprintf(fout,"\tcluster number for each pattern\n");
		  fprintf(fout,"\nindex\ttarget\tcluster no. \n");
		  for (idx_cn = 0; idx_cn < patset.size; idx_cn++)
		  {
		      fprintf(fout,"%d\t%d\t%d\n",idx_cn, cluster_result[idx_cn][0], cluster_result[idx_cn][1]);
		  }
		  fprintf(fout,"\n");
        #endif
		  fclose(fout);
	 }
}

/************************************************************/
/*      LVQ  State Handlers                                  */
/************************************************************/

// Listing 6.10 Main part of the LVQ state machine
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
static void lvq_state_handler (int state_index)
{
   switch (state_index)
   {
       case LVQ_GET_PATTERN:
          lvq_get_pattern();
			break;
       case LVQ_WEIGHT_NORMALIZATION:
          lvq_weight_normalization();
          break;
		 case LVQ_FEEDFORWARD_INPUT:
			lvq_feedforward_input();
          break;
       case LVQ_FEEDFORWARD_OUTPUT:
          lvq_feedforward_output();
          break;
       case LVQ_WINNING_NEURON:
          lvq_winning_neuron();
          break;
       case LVQ_WEIGHT_STEP_CHANGE:
          lvq_weight_step_change();
          break;
       case LVQ_WEIGHT_CHANGE:
          lvq_weight_change();
          break;
       case LVQ_NEXT_PATTERN:
          lvq_next_pattern();
          break;
       case LVQ_NEXT_ITERATION:
          lvq_next_iteration();
          break;
       case LVQ_UPDATE_LEARNING_RATE:
          lvq_update_learning_rate();
          break;
       case LVQ_UPDATE_CONSCIENCE_FACTOR:
          lvq_update_conscience_factor();
          break;
       case LVQ_TRAINING_DONE:
          lvq_training_done();
          break;
       case LVQ_RECALL_DONE:
          lvq_recall_done();
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
static void lvq_get_pattern (void)
{
   int idx;

	 for (idx = 0; idx < (lvq.arch.layers[0].size); idx++)
	 {
		  lvq.arch.layers[0].neurons[idx].in = patset.patterns[lvq.env.cur_pat][idx];
	 }
   for (idx = 0; idx < patset.dim_out; idx++ )
   {
		  target_out[idx] = patset.patterns[lvq.env.cur_pat][patset.dim_in + idx];
		  //printf("%d: %f\n",lvq.env.cur_pat, target_out[idx]);
   }
   if (lvq.env.operation_mode == NN_TRAINING)
   {
	     lvq_cur_state = LVQ_WEIGHT_NORMALIZATION;
   }
   else
   {
       lvq_cur_state = LVQ_FEEDFORWARD_INPUT;
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
static void lvq_weight_normalization (void)
{
   int idx_cn, idx_pn;
   double sum;
   float temp_f;

   for (idx_cn = 0; idx_cn < (lvq.arch.layers[1].size) ; idx_cn++)
   {   // loop through neurons in the output layer
       sum = 0.0;
       for (idx_pn = 0; idx_pn < (lvq.arch.layers[0].size) ; idx_pn++ )
       {   // loop through all the weights connected to this neuron
           sum += lvq.arch.layers[1].neurons[idx_cn].w[idx_pn] * lvq.arch.layers[1].neurons[idx_cn].w[idx_pn];
       }
       sum = sqrt(sum);
       if (sum > 0.0)
       {
          for (idx_pn = 0; idx_pn < (lvq.arch.layers[0].size) ; idx_pn++ )
          {   // loop through all the weights connected to this neuron
              temp_f = lvq.arch.layers[1].neurons[idx_cn].w[idx_pn]/sum;
              lvq.arch.layers[1].neurons[idx_cn].w[idx_pn] = temp_f;
          }
       }
   }
   lvq_cur_state = LVQ_FEEDFORWARD_INPUT;
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
static void lvq_feedforward_input(void)
{
	 int idx;

   for (idx = 0; idx < (lvq.arch.layers[0].size); idx++)
	 {
		  lvq.arch.layers[0].neurons[idx].out = lvq.arch.layers[0].neurons[idx].in;
	 }

	 lvq_cur_state = LVQ_FEEDFORWARD_OUTPUT;
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
static void lvq_feedforward_output (void)
{
	  int idx_out, idx_prev;
	  double sum, temp_f;
	  for (idx_out = 0; idx_out < (lvq.arch.layers[1].size ); idx_out++)
	  {   // loop throught the neurons of the output layer
	      sum = 0.0;
		  for ( idx_prev = 0; idx_prev < (lvq.arch.layers[0].size ); idx_prev++)
		  {   // loop through the neurons of the input layer
            temp_f = (lvq.arch.layers[0].neurons[idx_prev].out - lvq.arch.layers[1].neurons[idx_out].w[idx_prev] );
			  sum += (temp_f * temp_f);
		  }
        temp_f = sqrt(sum);
		  lvq.arch.layers[1].neurons[idx_out].in = temp_f;
		  lvq.arch.layers[1].neurons[idx_out].out =  activate_function(temp_f,lvq.arch.layers[1].neurons[idx_out].neuron_function);
	  }
    lvq_cur_state = LVQ_WINNING_NEURON;
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
static void lvq_winning_neuron (void)
{
    int idx, temp_w;
    float min_v = 1000.0;

    for (idx = 0; idx < (lvq.arch.layers[1].size); idx++)
    {   // loop through the neurons in output layer
        if ((lvq.arch.layers[1].neurons[idx].out - lvq.arch.layers[1].neurons[idx].b_v) < min_v)
        {
            min_v = lvq.arch.layers[1].neurons[idx].out - lvq.arch.layers[1].neurons[idx].b_v;
            temp_w = idx;
        }
    }
    lvq.arch.layers[1].neurons[lvq.env.winner].w_s = 0;    // clear the old winner status
    lvq.env.winner = temp_w;                               // assign new winner
    lvq.arch.layers[1].neurons[lvq.env.winner].w_s = 1;    // set new winner status

    if (lvq.env.operation_mode == NN_TRAINING)
    {
        lvq_cur_state = LVQ_WEIGHT_STEP_CHANGE;
    }
    else
    {   // recall
        update_recall_result();
        lvq_cur_state = LVQ_NEXT_PATTERN;
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
static void lvq_next_pattern (void)
{
    if (lvq.env.operation_mode == NN_TRAINING)
    {
        if (lvq.env.train_mode == LVQ_RANDOM_MODE)
        {   // random training
	          lvq.env.cur_pat = rand()%(lvq.env.max_tra_pat);     // next random pattern index
        }
        else
        {   // sequential training
            if (++lvq.env.cur_pat >= lvq.env.max_tra_pat)
            {
                lvq.env.cur_pat = 0;
            }
        }
        if ((++lvq.env.pat_counter) <lvq.env.max_tra_pat)
        {   // add other termination criterion here
            lvq_cur_state = LVQ_UPDATE_LEARNING_RATE;
        }
        else
        {
            lvq_cur_state = LVQ_NEXT_ITERATION;
        }
    }
    else  // recall
    {
        if ((++lvq.env.cur_pat) < patset.size)
        {
            lvq_cur_state = LVQ_GET_PATTERN;
        }
        else
        {
            lvq_cur_state = LVQ_RECALL_DONE;
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
static void lvq_next_iteration (void)
{
    lvq.env.pat_counter = 0;
    if ((++lvq.env.cur_ite) < lvq.env.max_ite)
    {   // add other termination criterion here
        lvq_cur_state = LVQ_UPDATE_LEARNING_RATE;
    }
    else
    {
        lvq.env.pat_counter = 0;
        lvq_cur_state = LVQ_TRAINING_DONE;
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
static void lvq_weight_step_change (void)
{
    int idx_pn;

    for (idx_pn = 0; idx_pn < (lvq.arch.layers[0].size) ; idx_pn++)
    {   // loop through the connect weights of the current neurons
        lvq.arch.layers[1].neurons[lvq.env.winner].delta_w[idx_pn] = lvq.arch.layers[0].neurons[idx_pn].out - lvq.arch.layers[1].neurons[lvq.env.winner].w[idx_pn];
        lvq.arch.layers[1].neurons[lvq.env.winner].delta_w[idx_pn] *= lvq.env.eta;
    }
    lvq_cur_state = LVQ_WEIGHT_CHANGE;
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
static void lvq_weight_change (void)
{
    int idx_pn;

    for (idx_pn = 0; idx_pn < (lvq.arch.layers[0].size) ; idx_pn++)
    {   // loop through the connect weights of the current neurons
        lvq.arch.layers[1].neurons[lvq.env.winner].w[idx_pn] += lvq.arch.layers[1].neurons[lvq.env.winner].delta_w[idx_pn];
    }

    lvq_cur_state =  LVQ_NEXT_PATTERN;
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
static void lvq_update_learning_rate (void)
{
    lvq.env.eta *= lvq.env.shrink;
    lvq_cur_state = LVQ_UPDATE_CONSCIENCE_FACTOR;
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
static void lvq_update_conscience_factor (void)
{
    int idx;
    float temp_f;
    if (lvq.env.conscience == LVQ_CONSCIENCE)
    {
        for (idx = 0; idx < (lvq.arch.layers[1].size); idx++)
        {   // loop through the neurons in output layer
            temp_f  = lvq.arch.layers[1].neurons[idx].c_f;
            lvq.arch.layers[1].neurons[idx].c_f = temp_f + lvq.env.beta * (lvq.arch.layers[1].neurons[idx].w_s - temp_f);
            lvq.arch.layers[1].neurons[idx].b_v = lvq.env.gama * (1.0/lvq.env.no_clusters - lvq.arch.layers[1].neurons[idx].c_f );
        }
    }
    lvq_cur_state = LVQ_GET_PATTERN;
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
static void lvq_training_done (void)
{
    lvq_weight_normalization();
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
static void lvq_recall_done (void)
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
   res = sum;      // No Guassian in LVQ
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
   res = sum;     // No sigmoid in LVQ
	 return (res);
}

