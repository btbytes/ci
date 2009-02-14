#ifndef LVQ_NN_H
#define LVQ_NN_H

#include "nnet.h"

/**************************************************************/
/* Constants and Macros                                       */
/**************************************************************/

// Listing 6.6 Enumeration data types for the LVQ network
/**************************************************************/
/* Enumerations                                               */
/**************************************************************/
typedef enum LVQ_Training_Mode_Tag
{
  LVQ_RANDOM_MODE,
  LVQ_SEQUENTIAL_MODE,
  NUM_LVQ_TRAINING_MODES
} LVQ_Training_Mode_Type;


typedef enum LVQ_State_Tag
{
  LVQ_GET_PATTERN,
  LVQ_WEIGHT_NORMALIZATION,
  LVQ_FEEDFORWARD_INPUT,
  LVQ_FEEDFORWARD_OUTPUT,
  LVQ_WINNING_NEURON,
  LVQ_WEIGHT_STEP_CHANGE,
  LVQ_WEIGHT_CHANGE,
  LVQ_NEXT_PATTERN,
  LVQ_NEXT_ITERATION,
  LVQ_UPDATE_LEARNING_RATE,
  LVQ_UPDATE_CONSCIENCE_FACTOR,
  LVQ_TRAINING_DONE,
  LVQ_RECALL_DONE,
  NUM_LVQ_STATES
} LVQ_State_Type;

typedef enum LVQ_Conscience_Type_Tag
{
  LVQ_NO_CONSCIENCE,
  LVQ_CONSCIENCE,
  NUM_LVQ_CONSCIENCE
} LVQ_Conscience_Type;

// Listing 6.7 Structure data types for the LVQ netowrk
/**************************************************************/
/* Structures                                                 */
/**************************************************************/
typedef struct Neuron_Type_Tag
{
  NN_Function_Type       neuron_function;     // neuron function
  float                  in;                  // neuron input
  float                  out;                 // neuron output, euclidean distance for output neuron
  FVECTOR                w;                   // connection weights from the previous  layers
  FVECTOR                delta_w;             // step change of weights
  float                  c_f;                 // conscience factor
  float                  b_v;                 // bias value
  int                    w_s;                 // winner status, y in equation
} Neuron_Type;

typedef struct NN_Layer_Arch_Type_Tag
{
  int                    size;                // number of neurons in the layer
  Neuron_Type            *neurons;            // pointer to the array of the neurons
  NN_Layer_Type          layer_type;
} NN_Layer_Arch_Type;

typedef struct LVQ_Arch_Type_Tag
{
  int                    size;                // number of layers
  NN_Layer_Arch_Type     *layers;             // pointer to the layers
} LVQ_Arch_Type;

typedef struct LVQ_Env_Type_Tag
{
  NN_Operation_Mode_Type operation_mode;      // training or recall
  LVQ_Training_Mode_Type train_mode;          // training mode if in training operation mode
  float                  eta;                 // learning rate
  float                  gama;                // bias factor
  float                  beta;                //
  float                  shrink;              // learning rate (eta) shrinking coefficient
  float                  criterion;           // training  criterion for termination
  int                    max_ite;             // maximum number of iterations
  int                    cur_ite;             // current iteration index
  int                    max_tra_pat;         // total number of training patterns
  int                    cur_pat;             // current training pattern index
  int                    pat_counter;         // pattern index increase counter within a iteration
  LVQ_Conscience_Type    conscience;          // 0: no conscience, 1: conscience
  int                    winner;              // index of winning neuron
  int                    no_clusters;         // number of clusters
} LVQ_Env_Type;

typedef struct LVQ_Type_Tag
{
  LVQ_Arch_Type          arch;
  LVQ_Env_Type           env;
} LVQ_Type;

typedef struct LVQ_Pattern_Set_Type_Tag
{
  int                    size;                // number of patterns
  int                    dim_in;              // input dimension
  int                    dim_out;             // output dimension
  FMATRIX                patterns;            // pointer to the array of in/out patterns
} LVQ_Pattern_Set_Type;

/**************************************************************/
/* Function Prototypes for functions with file level scope    */
/**************************************************************/


/**************************************************************/
/* Const Variable with global level scope                     */
/**************************************************************/


/**************************************************************/
/* Function Prototypes for functions with global level scope  */
/**************************************************************/

extern void LVQ_Main_Loop(void);
extern void LVQ_Start_Up(char *dataFile);
extern void LVQ_Clean_Up(void);

#endif
