#ifndef NEURAL_NET_H
#define NEURAL_NET_H

#include "definiti.h"

/**************************************************************/
/* Enumerations                                               */
/**************************************************************/

// Listing 6.1 Enumeration data type definitions for neural networks
typedef enum NN_Operation_Mode_Type_Tag
{
  NN_TRAINING,
  NN_RECALL,
  NUM_BP_OPERATION_MODES
} NN_Operation_Mode_Type;

typedef enum NN_Function_Type_Tag
{
  NN_LINEAR_FUNCTION,
  NN_GAUSIAN_FUNCTION,
  NN_SIGMOID_FUNCTION,
  NUM_NN_FUNCTION_TYPES
} NN_Function_Type;

typedef enum NN_Layer_Type_Tag
{
  NN_INPUT_LAYER,
  NN_HIDDEN_LAYER,
  NN_OUTPUT_LAYER,
  NUM_NN_LAYERS
} NN_Layer_Type;

#endif 
