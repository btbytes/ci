#include "headfile.h"
#include "definiti.h"
#include "bp.h"
#include "mem_loc.h"
#include "sigmoid.h"


#define MAX_NUM_CHARS 100

/**************************************************************/
/* Static Variable and Const Variable with File level scope   */
/**************************************************************/
 static BP_Type                bp;
 static BP_Pattern_Set_Type    patset;
 static BP_State_Type          bp_cur_state;

 static char                   pat_file_name[MAX_NUM_CHARS];
 static float                  *target_out;
 static int                    pso_dimension;
/**************************************************************/
/* Function Prototypes for functions with file level scope    */
/**************************************************************/
static void read_bp_parameters(char *dataFile);       // read BP parameters from I/O file
static void allocate_bp_memory(void);                 // allocate BP memory spaces
static void free_bp_memory(void);                     // free BP memory spaces
static void store_bp_results(void);                   // write BP results to I/O file

static void bp_initialize(void);
static void bp_initialize_weights(void);
static void bp_state_handler(int);                    // BP state handle routine

static void bp_get_pattern(void);
static void bp_feedforward_input(void);
static void bp_feedforward_hidden(void);
static void bp_feedforward_output(void);
static void bp_back_propagation_output(void);
static void bp_back_propagation_hiddens(void);
static void bp_batch_temp_weight_step_change(void);
static void bp_next_pattern(void);
static void bp_weight_step_change(void);
static void bp_weight_change(void);
static void bp_next_generation(void);
static void bp_update_learning_rate(void);
static void bp_update_momentum_rate(void);
static void bp_training_done(void);
static void bp_recall_done(void);

static float activate_function(float, int);
static float nn_linear_function(float);
static float nn_gausian_function(float);
static float nn_sigmoid_function(float);

static void print_net_parameters(void);
static void print_mse(void);
static void update_recall_result(void);

/**************************************************************/
/*      Function Definitions                                  */
/**************************************************************/


/**************************************************************/
/*   BP Start and clean routines and interfaces               */
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

void BP_Start_Up (char *dataFile)
{
    read_bp_parameters(dataFile);
    allocate_bp_memory();            // allocate memory for BP
    bp_initialize();
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
void BP_Clean_Up (void)
{
    store_bp_results();
    free_bp_memory();                // free memory space of BP
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
void BP_Weights_From_PSO (float *vec)
{   int idx_layer, idx_cn, idx_pn;
    int counter = 0;

    for (idx_layer = 1; idx_layer < (bp.arch.size) ; idx_layer++ )
    {   // loop through the layers
        for (idx_cn = 0; idx_cn < (bp.arch.layers[idx_layer].size) ; idx_cn++)
        {   // loop through the neurons in the current layer
            for (idx_pn = 0; idx_pn <= (bp.arch.layers[idx_layer - 1].size) ; idx_pn++)
            {   // loop through the connect weights of the current neurons
                bp.arch.layers[idx_layer].neurons[idx_cn].w[idx_pn] = vec[counter++];
                //fscanf(fout,"%f",&(bp.arch.layers[idx_layer].neurons[idx_cn].w[idx_pn]));
            }
        }
    }
    if (counter != pso_dimension)
    {
        printf("not match in BP_Weights_From_PSO routine 1 \n");
        exit(1);
    }
}

// Listing 6.15 The BP_Get_PSO_Dimension() routine
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
int BP_Get_PSO_Dimension (void)
{
    int idx_l;
    pso_dimension = 0;

    for (idx_l = 0; idx_l < (bp.arch.size - 1); idx_l++)
    {
        pso_dimension += ((bp.arch.layers[idx_l].size + 1) * bp.arch.layers[idx_l + 1].size);
    }
    return(pso_dimension);
}

/************************************************************/
/*      BP  functons                                        */
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
float BP_Main_Loop (void)
{
    BOOLEAN running;

    bp.mse = 0.0;    // clean mean squared error
    // bp randomly initialize weights

    // bp initial setting
    bp_cur_state = BP_GET_PATTERN;
    bp.env.cur_gen = 0;
    bp.env.cur_pat = 0;

    running = TRUE;
    while (running)
    {
        if ((bp_cur_state == BP_TRAINING_DONE) || (bp_cur_state == BP_RECALL_DONE) )
        {
            running = FALSE;
        }
        bp_state_handler(bp_cur_state);
    }
    return(bp.mse);    // bp.mse should be replaced with recognition error here

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
static void read_bp_parameters (char *dataFile)
{
    int idx_i;
    FILE  *frun;

    // open the runfile to input parameters
	  if ((frun=fopen(dataFile,"r"))==NULL)
	  {
		  printf("Cant read run file");
		  exit(1);
	  }

    // read BP's parameters from run file
       // read BP's environment data
    //fscanf(frun, "%d",&(bp.env.operation_mode));       // training or recall
    //fscanf(frun, "%d",&(bp.env.train_mode));           // training mode if in training operation mode
    //fscanf(frun, "%f",&(bp.env.alpha));                // learning rate
    //fscanf(frun, "%f",&(bp.env.gama));                 // momentum rate
    //fscanf(frun, "%f",&(bp.env.criterion));            // training error criterion for termination
    //fscanf(frun, "%d",&(bp.env.max_gen));              // maximum number of generations
    //fscanf(frun, "%d",&(bp.env.max_tra_pat));          // total number of training patterns

       // read BP's Arch
    fscanf(frun, "%d",&(bp.arch.size));                // number of layers
       // allocate memory for numbers of neurons in hidden layers
    bp.arch.hidden_number = calloc((bp.arch.size - 2),sizeof(int));
    if (!(bp.arch.hidden_number))
    {
       printf("Allocation error in read_bp_parameters() - aborting.");
       exit(1);
    }
       // read in numbers of neurons in hidden layers
    for (idx_i = 0 ; (idx_i < (bp.arch.size - 2)) ; idx_i++)
    {
       fscanf(frun, "%d",&(bp.arch.hidden_number[idx_i]));
    }

    // read in/out pattern parameters
    fscanf(frun, "%d",&(patset.size));                 // number of pattern
       // read pattern input dimension
    fscanf(frun, "%d",&(patset.dim_in));               // input dimension
       // read pattern output dimension
    fscanf(frun, "%d",&(patset.dim_out));              // output dimension
       // read pattern file name
    fscanf(frun, "%s", pat_file_name);

    fclose(frun);

    bp.env.operation_mode = 1;
    bp.env.train_mode = 0;
    bp.env.alpha = 0.075;
    bp.env.gama = 0.15;
    bp.env.criterion = 0.01;
    bp.env.max_gen = 1;
    bp.env.max_tra_pat = 1;

    print_net_parameters();

}

static void print_net_parameters (void)
{
    int idx_i;
    // test
    //printf( "%d\n",(bp.env.operation_mode));       // training or recall
    //printf( "%d\n",(bp.env.train_mode));           // training mode if in training operation mode
    //printf( "%f\n",(bp.env.alpha));                // learning rate
    //printf( "%f\n",(bp.env.gama));                 // momentum rate
    //printf( "%f\n",(bp.env.criterion));            // training error criterion for termination
    //printf( "%d\n",(bp.env.max_gen));              // maximum number of generations
    //printf( "%d\n\n",(bp.env.max_tra_pat));          // total number of training patterns

    printf( "number of layers:   %d\n",(bp.arch.size));                // number of layers
    for (idx_i = 0 ; (idx_i < (bp.arch.size - 2)) ; idx_i++)
    {
       printf( "number of hidden layer %d's neurons   %d\n",idx_i, bp.arch.hidden_number[idx_i]);
    }
    printf( "number of patterns:    %d\n",(patset.size));                 // number of pattern
     // read pattern input dimension
    printf( "pattern input dimension:    %d\n",(patset.dim_in));               // input dimension
     // read pattern output dimension
    printf( "pattern output dimnesion:    %d\n",(patset.dim_out));              // output dimension
     // read pattern file name
    printf( "pattern file name:    %s\n", pat_file_name);
}

static void print_mse (void)
{
    printf("%f\n",bp.mse);        // output mean squared error
}

static void update_recall_result(void)
{
	  int   idx_out, res_nn, res_ta;
    float max = 0.0;

	  for (idx_out = 0; idx_out < (bp.arch.layers[bp.arch.size - 1].size ); idx_out++)
	  {   // loop throught the neurons of the output layer
        if (bp.arch.layers[bp.arch.size - 1].neurons[idx_out].out > max )
        {
            max  = bp.arch.layers[bp.arch.size - 1].neurons[idx_out].out;
            res_nn = idx_out;
        }
	  }

    max = 0.0;
    for (idx_out = 0; idx_out < (bp.arch.layers[bp.arch.size - 1].size ); idx_out++)
	  {   // loop throught the neurons of the output layer
        if (target_out[idx_out] > max)
        {
            max = target_out[idx_out];
            res_ta = idx_out;
        }
	  }
    if (res_nn == res_ta)
    {
        bp.mse += 1;
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
static void allocate_bp_memory (void)
{
    int idx, idx_i;

    // allocate memory for BP net
    bp.arch.layers = calloc(bp.arch.size,sizeof(NN_Layer_Arch_Type));

    //allocate memory for input layer
    bp.arch.layers[0].size       = patset.dim_in;
    bp.arch.layers[0].neurons    = calloc(patset.dim_in,sizeof(Neuron_Type));
    bp.arch.layers[0].layer_type =  NN_INPUT_LAYER;
       // specify and allocate memory for neurons of input layer
    for (idx =0 ; idx < bp.arch.layers[0].size ; idx++)
    {
        bp.arch.layers[0].neurons[idx].neuron_function = NN_LINEAR_FUNCTION;
		  FVectorAllocate(&(bp.arch.layers[0].neurons[idx].delta_w),1);
        FVectorAllocate(&(bp.arch.layers[0].neurons[idx].temp_delta_w),1);
		  FVectorAllocate(&(bp.arch.layers[0].neurons[idx].w),1);
    }

    // allocate memory for hidden layers
    for (idx =0 ; idx < (bp.arch.size - 2);idx++ )
    {
       bp.arch.layers[idx + 1].size       = bp.arch.hidden_number[idx];
       bp.arch.layers[idx + 1].neurons    = calloc(bp.arch.layers[idx + 1].size,sizeof(Neuron_Type));
       bp.arch.layers[idx + 1].layer_type =  NN_HIDDEN_LAYER;
       // specify and allocate memory for neurons of hiddem layer
       for (idx_i =0 ; idx_i < bp.arch.layers[idx + 1].size ; idx_i++)
       {
           bp.arch.layers[idx + 1].neurons[idx_i].neuron_function = NN_SIGMOID_FUNCTION;
			  FVectorAllocate(&(bp.arch.layers[idx + 1].neurons[idx_i].delta_w),bp.arch.layers[idx].size + 1);   // add one bias
            FVectorAllocate(&(bp.arch.layers[idx + 1].neurons[idx_i].temp_delta_w),bp.arch.layers[idx].size + 1);
			  FVectorAllocate(&(bp.arch.layers[idx + 1].neurons[idx_i].w),bp.arch.layers[idx].size + 1);
       }
    }

    // allocate memory for output layer
    bp.arch.layers[bp.arch.size - 1].size       = patset.dim_out;
    bp.arch.layers[bp.arch.size - 1].neurons    = calloc(patset.dim_out,sizeof(Neuron_Type));
    bp.arch.layers[bp.arch.size - 1].layer_type =  NN_OUTPUT_LAYER;
       // specify and allocate memory for neurons of output layer
    for (idx =0 ; idx < bp.arch.layers[bp.arch.size - 1].size ; idx++)
    {
		  bp.arch.layers[bp.arch.size - 1].neurons[idx].neuron_function = NN_SIGMOID_FUNCTION;
		  FVectorAllocate(&(bp.arch.layers[bp.arch.size - 1].neurons[idx].delta_w),bp.arch.layers[bp.arch.size - 2].size +1);
        FVectorAllocate(&(bp.arch.layers[bp.arch.size - 1].neurons[idx].temp_delta_w),bp.arch.layers[bp.arch.size - 2].size +1);
        FVectorAllocate(&(bp.arch.layers[bp.arch.size - 1].neurons[idx].w),bp.arch.layers[bp.arch.size - 2].size + 1);
    }

    // allocate memory for pattern set
    FMatrixAllocate(&(patset.patterns),patset.size,(patset.dim_in + patset.dim_out));

    // allocate memory for target output
    target_out = calloc(patset.dim_out,sizeof(float));
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
static void free_bp_memory (void)
{
    int idx, idx_i;
    // free memory for pattern set
    FMatrixFree(patset.patterns,patset.size);

    // free memory for BP net
       // free memory for output layer
    for (idx =0 ; idx < bp.arch.layers[bp.arch.size - 1].size ; idx++)
    {
       free(bp.arch.layers[bp.arch.size - 1].neurons[idx].w);
       free(bp.arch.layers[bp.arch.size - 1].neurons[idx].delta_w);
       free(bp.arch.layers[bp.arch.size - 1].neurons[idx].temp_delta_w);
    }
    free(bp.arch.layers[bp.arch.size - 1].neurons);

       // free memory for hidden layers
    for (idx =0 ; idx < (bp.arch.size - 2);idx++ )
    {
       for (idx_i =0 ; idx_i < bp.arch.layers[idx + 1].size ; idx_i++)
       {
           free(bp.arch.layers[idx + 1].neurons[idx_i].w);
           free(bp.arch.layers[idx + 1].neurons[idx_i].delta_w);
           free(bp.arch.layers[idx + 1].neurons[idx_i].temp_delta_w);
       }
       free(bp.arch.layers[idx + 1].neurons);
    }
       // free memory for input layer
    for (idx =0 ; idx < bp.arch.layers[0].size ; idx++)
    {
        free(bp.arch.layers[0].neurons[idx].w);
        free(bp.arch.layers[0].neurons[idx].delta_w);
        free(bp.arch.layers[0].neurons[idx].temp_delta_w);
    }
    free(bp.arch.layers[0].neurons);

    free(bp.arch.layers);
    free(bp.arch.hidden_number);

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
static void bp_initialize (void)
{
    int idx_i,idx_j;
    int stime;
    long ltime;
    FILE  *frun;

    // get the current calendar time
    ltime = time(NULL);
    stime = (unsigned) ltime/2;
    srand(stime);

    // bp read patterns from file
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
				//printf("%f\t",patset.patterns[idx_i][idx_j]);
		  }
        //printf("\n");
    }
	  fclose(frun);

    bp.mse = 0.0;    // clean mean squared error
    // bp randomly initialize weights
    //bp_initialize_weights();

    // bp initial setting
    bp_cur_state = BP_GET_PATTERN;
    bp.env.cur_gen = 0;
    bp.env.cur_pat = 0;
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
static void bp_initialize_weights (void)
{
    int idx_layer,idx_neuron,idx_weight;
    float prob;
    int idx_cn, idx_pn;
    FILE  *fout;

    if (bp.env.operation_mode == NN_TRAINING)
    {
        // initialize weight for the neurons in the hidden layers
        for (idx_layer = 1; idx_layer < ((bp.arch.size - 1)) ; idx_layer++)
        {
            for (idx_neuron = 0; idx_neuron < (bp.arch.layers[idx_layer].size); idx_neuron++)
            {
                for (idx_weight = 0; idx_weight < (bp.arch.layers[idx_layer - 1].size + 1); idx_weight++ )
                {
                    prob = (rand()%1000)/1000.0 - 0.5;
                    bp.arch.layers[idx_layer].neurons[idx_neuron].w[idx_weight]       =  prob;       // (-1,1)
                    bp.arch.layers[idx_layer].neurons[idx_neuron].delta_w[idx_weight] = 0.0;
                    bp.arch.layers[idx_layer].neurons[idx_neuron].temp_delta_w[idx_weight] = 0.0;
                }
            }
        }

        // initialize weight for the neurons in the output layer
        for (idx_neuron = 0; idx_neuron < (bp.arch.layers[bp.arch.size - 1].size); idx_neuron++)
        {
            for (idx_weight = 0; idx_weight < (bp.arch.layers[bp.arch.size - 2].size + 1); idx_weight++ )
            {
                prob = (rand()%1000)/1000.0 - 0.5;
                bp.arch.layers[bp.arch.size - 1].neurons[idx_neuron].w[idx_weight]       =  prob;       // (-1,1)
                bp.arch.layers[bp.arch.size - 1].neurons[idx_neuron].delta_w[idx_weight] = 0.0;
                bp.arch.layers[bp.arch.size - 1].neurons[idx_neuron].temp_delta_w[idx_weight] = 0.0;
            }
		  }
        //store_bp_results();
    }
    else
    {   // RECALL operation mode
        // read in weights from file
        // open file for write
        if ((fout=fopen("BP_res.txt","r"))==NULL)
	      {
		      printf("Cant open file for write BP training  results");
		      exit(1);
	      }
        for (idx_layer = 1; idx_layer < (bp.arch.size) ; idx_layer++ )
        {   // loop through the layers
            for (idx_cn = 0; idx_cn < (bp.arch.layers[idx_layer].size) ; idx_cn++)
            {   // loop through the neurons in the current layer
                for (idx_pn = 0; idx_pn <= (bp.arch.layers[idx_layer - 1].size) ; idx_pn++)
                {   // loop through the connect weights of the current neurons
                    fscanf(fout,"%f",&(bp.arch.layers[idx_layer].neurons[idx_cn].w[idx_pn]));
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
static void  store_bp_results (void)
{
    // store weights
    int idx_layer, idx_cn, idx_pn;
    FILE  *fout;

    // open file for write

    if (bp.env.operation_mode == NN_TRAINING)
	 {
        if ((fout=fopen("BP_res.txt","w"))==NULL)
		  {
				printf("Cant open file for reading BP weights");
				exit(1);
		  }
        for (idx_layer = 1; idx_layer < (bp.arch.size) ; idx_layer++ )
        {   // loop through the layers
            for (idx_cn = 0; idx_cn < (bp.arch.layers[idx_layer].size) ; idx_cn++)
            {   // loop through the neurons in the current layer
                for (idx_pn = 0; idx_pn <= (bp.arch.layers[idx_layer - 1].size) ; idx_pn++)
                {   // loop through the connect weights of the current neurons
                    fprintf(fout,"%f\n",bp.arch.layers[idx_layer].neurons[idx_cn].w[idx_pn]);
                }
            }
        }
		  fprintf(fout,"\n\nmse = %f\n",bp.mse);
        fclose(fout);
    }
    else // RECALL
    {

    }
}

/************************************************************/
/*      BP  State Handlers                                  */
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
static void bp_state_handler (int state_index)
{
   switch (state_index)
   {
       case BP_GET_PATTERN:
          bp_get_pattern();
			 break;
		 case BP_FEEDFORWARD_INPUT:
			 bp_feedforward_input();
          break;
       case BP_FEEDFORWARD_HIDDEN:
          bp_feedforward_hidden();
          break;
       case BP_FEEDFORWARD_OUTPUT:
          bp_feedforward_output();
          break;
       case BP_BACK_PROPAGATION_OUTPUT:
          bp_back_propagation_output();
          break;
       case BP_BACK_PROPAGATION_HIDDENS:
          bp_back_propagation_hiddens();
          break;
       case BP_BATCH_TEMP_WEIGHT_STEP_CHANGE:
          bp_batch_temp_weight_step_change();
          break;
       case BP_NEXT_PATTERN:
          bp_next_pattern();
          break;
       case BP_WEIGHT_STEP_CHANGE:
          bp_weight_step_change();
          break;
       case BP_WEIGHT_CHANGE:
          bp_weight_change();
          break;
       case BP_NEXT_GENERATION:
          bp_next_generation();
          break;
       case BP_UPDATE_LEARNING_RATE:
          bp_update_learning_rate();
          break;
       case BP_UPDATE_MOMENTUM_RATE:
          bp_update_momentum_rate();
          break;
       case BP_TRAINING_DONE:
          bp_training_done();
          break;
       case BP_RECALL_DONE:
          bp_recall_done();
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
static void bp_get_pattern (void)
{
    int idx;

	 for (idx = 0; idx < (bp.arch.layers[0].size); idx++)
	 {
		  bp.arch.layers[0].neurons[idx].in = patset.patterns[bp.env.cur_pat][idx];
	 }
   for (idx = 0; idx < patset.dim_out; idx++ )
   {
		  target_out[idx] = patset.patterns[bp.env.cur_pat][patset.dim_in + idx];
		  //printf("%d: %f\n",bp.env.cur_pat, target_out[idx]);
   }

	 bp_cur_state = BP_FEEDFORWARD_INPUT;
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
static void bp_feedforward_input(void)
{
	 int idx;

   for (idx = 0; idx < (bp.arch.layers[0].size); idx++)
	 {
		  bp.arch.layers[0].neurons[idx].out = bp.arch.layers[0].neurons[idx].in;
	 }

	 bp_cur_state = BP_FEEDFORWARD_HIDDEN;
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
static void bp_feedforward_hidden (void)
{
	 int idx, idx_prev,idx_cur;
	 float sum;

	 for (idx = 1; idx < ( bp.arch.size - 1); idx++)
	 {   // loop through the hidden layers
		  for (idx_cur = 0; idx_cur < (bp.arch.layers[idx].size ); idx_cur++)
		  {   // loop throught the neurons of the current hidden layer
				sum = 0.0;
				for ( idx_prev = 0; idx_prev < (bp.arch.layers[idx - 1].size ); idx_prev++)
				{   // loop through the outputs of the previous layer
					 sum += (bp.arch.layers[idx -1].neurons[idx_prev].out ) * (bp.arch.layers[idx].neurons[idx_cur].w[idx_prev] );
				}
				sum += (bp.arch.layers[idx].neurons[idx_cur].w[bp.arch.layers[idx - 1].size] );
            bp.arch.layers[idx].neurons[idx_cur].in = sum;
			   bp.arch.layers[idx].neurons[idx_cur].out =  activate_function(sum,bp.arch.layers[idx].neurons[idx_cur].neuron_function);
		  }
	 }
	 bp_cur_state = BP_FEEDFORWARD_OUTPUT;
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
static void bp_feedforward_output (void)
{
	  int idx_out, idx_prev;
	  float sum;
	  for (idx_out = 0; idx_out < (bp.arch.layers[bp.arch.size - 1].size ); idx_out++)
	  {   // loop throught the neurons of the output layer
	      sum = 0.0;
		   for ( idx_prev = 0; idx_prev < (bp.arch.layers[bp.arch.size - 2].size ); idx_prev++)
		   {   // loop through the outputs of the previous layer
			    sum += (bp.arch.layers[bp.arch.size - 2].neurons[idx_prev].out ) * (bp.arch.layers[bp.arch.size - 1].neurons[idx_out].w[idx_prev] );
		   }
		   sum += (bp.arch.layers[bp.arch.size - 1].neurons[idx_out].w[bp.arch.layers[bp.arch.size - 2].size] );
		   bp.arch.layers[bp.arch.size - 1].neurons[idx_out].in = sum;
		   bp.arch.layers[bp.arch.size - 1].neurons[idx_out].out =  activate_function(sum,bp.arch.layers[bp.arch.size - 1].neurons[idx_out].neuron_function);
			//if (bp.env.operation_mode == NN_RECALL)
			//{
			//	printf("patten index: %d\t%f\t%f\t%f\n",bp.env.cur_pat,bp.arch.layers[0].neurons[0].in,bp.arch.layers[0].neurons[1].in,bp.arch.layers[bp.arch.size - 1].neurons[idx_out].out);
         //}
	  }

    if (bp.env.operation_mode == NN_TRAINING)
    {
        bp_cur_state = BP_BACK_PROPAGATION_OUTPUT;
    }
    else
    {   // recall
        update_recall_result();
        bp_cur_state = BP_NEXT_PATTERN;
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
static void bp_back_propagation_output (void)
{
	 int idx;
	 double tempA,tempB;

	 for (idx = 0; idx < (bp.arch.layers[bp.arch.size - 1].size); idx++)
	 {
        tempA = (target_out[idx] - bp.arch.layers[bp.arch.size - 1].neurons[idx].out);
        switch (bp.arch.layers[bp.arch.size - 1].neurons[idx].neuron_function)
        {
            case NN_LINEAR_FUNCTION:
              bp.arch.layers[bp.arch.size - 1].neurons[idx].error =  tempA;
              break;
            case NN_GAUSIAN_FUNCTION:
              printf("BP net can't have Gaussian Neurons, exit\n");
              exit(1);
              break;
            default:   // NN_SIGMOID_FUNCTION
              tempB = (bp.arch.layers[bp.arch.size - 1].neurons[idx].out) * ( 1.0 - (bp.arch.layers[bp.arch.size - 1].neurons[idx].out));
              bp.arch.layers[bp.arch.size - 1].neurons[idx].error = tempA * tempB;
              break;
        }
        bp.mse += (tempA * tempA);
	 }

    bp_cur_state = BP_BACK_PROPAGATION_HIDDENS;
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
static void bp_back_propagation_hiddens (void)
{
	 int idx_l, idx_cn, idx_nn;
	 double tempA,sum;

   for (idx_l = bp.arch.size - 2; idx_l > 0; idx_l--)
   {   // loop through all the hidden layers
       for (idx_cn = 0; idx_cn < (bp.arch.layers[idx_l].size) ; idx_cn++ )
       {   // loop through all the neurons in the current hidden layer
           sum = 0.0;
           for (idx_nn = 0; idx_nn < (bp.arch.layers[idx_l + 1].size); idx_nn++ )
           {   // loop through the next layer's neurons
               sum += (bp.arch.layers[idx_l + 1].neurons[idx_nn].error) * (bp.arch.layers[idx_l + 1].neurons[idx_nn].w[idx_cn]);
           }
           tempA = bp.arch.layers[idx_l].neurons[idx_cn].out * ( 1.0 - (bp.arch.layers[idx_l].neurons[idx_cn].out));
           bp.arch.layers[idx_l].neurons[idx_cn].error = sum * tempA;
       }

   }

   bp_cur_state = BP_BATCH_TEMP_WEIGHT_STEP_CHANGE;
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
static void bp_batch_temp_weight_step_change (void)
{
    int idx_layer,idx_cn,idx_pn;
    double tempA;

    for (idx_layer = bp.arch.size - 1; idx_layer > 0; idx_layer--)
    {   // loop through layers
        for (idx_cn = 0; idx_cn < (bp.arch.layers[idx_layer].size) ; idx_cn++)
        {   // loop neurons in the current layer
            for (idx_pn = 0; idx_pn < (bp.arch.layers[idx_layer - 1].size) ; idx_pn++)
            {   // loop through neurons in the previous layer
                tempA = bp.arch.layers[idx_layer].neurons[idx_cn].error * bp.arch.layers[idx_layer - 1].neurons[idx_pn].out;
                tempA *= bp.env.alpha;
					 bp.arch.layers[idx_layer].neurons[idx_cn].temp_delta_w[idx_pn] += tempA;
            }
            bp.arch.layers[idx_layer].neurons[idx_cn].temp_delta_w[bp.arch.layers[idx_layer - 1].size] += bp.env.alpha * bp.arch.layers[idx_layer].neurons[idx_cn].error;
        }
    }

    if (bp.env.train_mode == NN_BATCH_MODE)
    {
        bp_cur_state = BP_NEXT_PATTERN;
    }
    else
    {
        bp_cur_state = BP_WEIGHT_STEP_CHANGE;
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
static void bp_next_pattern (void)
{
	 bp.env.cur_pat++;
    if (bp.env.operation_mode == NN_TRAINING)
    {
        if (bp.env.train_mode == NN_BATCH_MODE)
        {
            if (bp.env.cur_pat < bp.env.max_tra_pat)
            {
                bp_cur_state = BP_GET_PATTERN;
            }
            else
            {
                bp_cur_state = BP_WEIGHT_STEP_CHANGE;
            }
        }
        else   // sequential learning
        {
            if (bp.env.cur_pat < bp.env.max_tra_pat)
            {
                bp_cur_state = BP_GET_PATTERN;
            }
            else
            {
                bp_cur_state = BP_NEXT_GENERATION;
            }
        }
    }
    else  // recall
    {
        if (bp.env.cur_pat < patset.size)
        {
            bp_cur_state = BP_GET_PATTERN;
        }
        else
        {
            bp_cur_state = BP_RECALL_DONE;
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
static void bp_weight_step_change (void)
{
    int idx_layer, idx_cn, idx_pn;

    for (idx_layer = 1; idx_layer < (bp.arch.size) ; idx_layer++ )
    {   // loop through the layers
		  for (idx_cn = 0; idx_cn < (bp.arch.layers[idx_layer].size) ; idx_cn++)
        {   // loop through the neurons in the current layer
            for (idx_pn = 0; idx_pn <= (bp.arch.layers[idx_layer - 1].size) ; idx_pn++)
            {   // loop through the connect weights of the current neurons
                bp.arch.layers[idx_layer].neurons[idx_cn].delta_w[idx_pn] *= bp.env.gama;
					 bp.arch.layers[idx_layer].neurons[idx_cn].delta_w[idx_pn] += (bp.arch.layers[idx_layer].neurons[idx_cn].temp_delta_w[idx_pn]);  // /(bp.env.max_tra_pat);
					 bp.arch.layers[idx_layer].neurons[idx_cn].temp_delta_w[idx_pn] = 0.0;
            }
        }
    }
    bp_cur_state = BP_WEIGHT_CHANGE;
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
static void bp_weight_change (void)
{
    int idx_layer, idx_cn, idx_pn;

    for (idx_layer = 1; idx_layer < (bp.arch.size) ; idx_layer++ )
    {   // loop through the layers
        for (idx_cn = 0; idx_cn < (bp.arch.layers[idx_layer].size) ; idx_cn++)
        {   // loop through the neurons in the current layer
            for (idx_pn = 0; idx_pn <= (bp.arch.layers[idx_layer - 1].size) ; idx_pn++)
            {   // loop through the connect weights of the current neurons
                bp.arch.layers[idx_layer].neurons[idx_cn].w[idx_pn] += bp.arch.layers[idx_layer].neurons[idx_cn].delta_w[idx_pn];
            }
        }
    }

    if (bp.env.train_mode == NN_BATCH_MODE)
    {
        bp_cur_state =  BP_NEXT_GENERATION;
    }
    else
    {
        bp_cur_state =  BP_NEXT_PATTERN;
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
static void bp_next_generation (void)
{
    int idx_layer, idx_cn;  // idx_pn;

    for (idx_layer = 0; idx_layer < (bp.arch.size) ; idx_layer++ )
    {   // loop through the layers
        for (idx_cn = 0; idx_cn < (bp.arch.layers[idx_layer].size) ; idx_cn++)
        {   // loop through the neurons in the current layer
            // clean the error
            bp.arch.layers[idx_layer].neurons[idx_cn].error = 0.0;
				//if (idx_layer >0)
				//{  // clean temp. step weight weights
				//   for (idx_pn = 0; idx_pn <= (bp.arch.layers[idx_layer - 1].size) ; idx_pn++)
				//   {   // loop through the connect weights of the current neurons
				//       bp.arch.layers[idx_layer].neurons[idx_cn].temp_delta_w[idx_pn] = 0.0;
				//   }
				//}
		  }
    }

    bp.mse /= bp.env.max_tra_pat;
print_mse();

    //bp.env.cur_gen++;

    if ((++bp.env.cur_gen) < bp.env.max_gen) // add error criterion later
    {
        bp.mse = 0.0; //clear mean squared error
        bp_cur_state = BP_UPDATE_LEARNING_RATE;
    }
    else
    {
        bp_cur_state = BP_TRAINING_DONE;
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
static void bp_update_learning_rate (void)
{
    bp_cur_state = BP_UPDATE_MOMENTUM_RATE;
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
static void bp_update_momentum_rate (void)
{
    bp.env.cur_pat = 0;
    bp_cur_state = BP_GET_PATTERN;
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
static void bp_training_done (void)
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
static void bp_recall_done (void)
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
   printf("no Gaussian function implemented\n");
   exit(1);
	 return (sum);
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
    res = (float)sigmoid(sum);
	 return (res);
}
