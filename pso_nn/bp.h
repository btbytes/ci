#ifndef BP_NN_H
#define BP_NN_H

#include "nnet.h"

/**************************************************************/
/* Constants and Macros                                       */
/**************************************************************/

/**************************************************************/
/* Enumerations                                               */
/**************************************************************/
typedef enum BP_Training_Mode_Tag
{
  NN_BATCH_MODE,
  NN_SEQUENTIAL_MODE,
  NUM_NN_TRAINING_MODES
} BP_Training_Mode_Type;

typedef enum BP_State_Tag
{
  BP_GET_PATTERN,
  BP_FEEDFORWARD_INPUT,
  BP_FEEDFORWARD_HIDDEN,
  BP_FEEDFORWARD_OUTPUT,
  BP_BACK_PROPAGATION_OUTPUT,
  BP_BACK_PROPAGATION_HIDDENS,
  BP_BATCH_TEMP_WEIGHT_STEP_CHANGE,
  BP_NEXT_PATTERN,
  BP_WEIGHT_STEP_CHANGE,
  BP_WEIGHT_CHANGE,
  BP_NEXT_GENERATION,
  BP_UPDATE_LEARNING_RATE,
  BP_UPDATE_MOMENTUM_RATE,
  BP_TRAINING_DONE,
  BP_RECALL_DONE,
  NUM_BP_STATES
} BP_State_Type;


/**************************************************************/
/* Structures                                                 */
/**************************************************************/
typedef struct Neuron_Type_Tag
{
  NN_Function_Type       neuron_function;     // neuron function 
  float                  in;                  // neuron input
  float                  out;                 // neuron output
  double                 error;               // error of neuron's output
  FVECTOR                delta_w;             // step change of weights
  FVECTOR                temp_delta_w;        // temp. step change of weights
  FVECTOR                w;                   // connection weights from the previous  layers
} Neuron_Type;

typedef struct NN_Layer_Arch_Type_Tag
{
  int                    size;                // number of neurons in the layer
  Neuron_Type            *neurons;            // pointer to the array of the neurons
  NN_Layer_Type          layer_type;
} NN_Layer_Arch_Type;

typedef struct BP_Arch_Type_Tag
{
  int                    size;                // number of layers
  NN_Layer_Arch_Type     *layers;             // pointer to the layers
  int                    *hidden_number;      // pointer to the neuron numbers of hidden layers
} BP_Arch_Type;

typedef struct BP_Env_Type_Tag
{
  NN_Operation_Mode_Type operation_mode;      // training or recall
  BP_Training_Mode_Type  train_mode;          // training mode if in training operation mode
  float                  alpha;               // learning rate  0.075
  float                  gama;                // momentum rate  0.15
  float                  criterion;           // training error criterion for termination
  int                    max_gen;             // maximum number of generations
  int                    cur_gen;             // current generation index
  int                    max_tra_pat;         // total number of training patterns
  int                    cur_pat;             // current training pattern index

} BP_Env_Type;

typedef struct BP_Type_Tag
{ 
  BP_Arch_Type           arch;
  BP_Env_Type            env;  
  double                 mse;                 // mean squared error
} BP_Type;

typedef struct BP_Pattern_Set_Type_Tag
{
  int                    size;                // number of patterns
  int                    dim_in;              // input dimension
  int                    dim_out;             // output dimension
  FMATRIX                patterns;            // pointer to the array of in/out patterns
} BP_Pattern_Set_Type;

/**************************************************************/
/* Function Prototypes for functions with file level scope    */
/**************************************************************/


/**************************************************************/
/* Const Variable with global level scope                     */
/**************************************************************/


/**************************************************************/
/* Function Prototypes for functions with global level scope  */
/**************************************************************/

extern float BP_Main_Loop(void);
extern void  BP_Start_Up(char *dataFile);
extern void  BP_Clean_Up(void);

extern void  BP_Weights_From_PSO(float *vec);
extern int   BP_Get_PSO_Dimension(void);
#endif
