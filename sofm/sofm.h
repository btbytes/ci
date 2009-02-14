#ifndef SOFM_NN_H
#define SOFM_NN_H

#include "nnet.h"

/**************************************************************/
/* Constants and Macros                                       */
/**************************************************************/

// Listing 6.12 Data type definiton for SOFM
/**************************************************************/
/* Enumerations                                               */
/**************************************************************/
typedef enum SOFM_Training_Mode_Tag
{
  SOFM_RANDOM_MODE,
  SOFM_SEQUENTIAL_MODE,
  NUM_SOFM_TRAINING_MODES
} SOFM_Training_Mode_Type;


typedef enum SOFM_State_Tag
{
  SOFM_GET_PATTERN,
  SOFM_WEIGHT_NORMALIZATION,
  SOFM_FEEDFORWARD_INPUT,
  SOFM_FEEDFORWARD_OUTPUT,
  SOFM_WINNING_NEURON,
  SOFM_UPDATE_NEIGHBORHOOD,
  //SOFM_WEIGHT_STEP_CHANGE,
  SOFM_WEIGHT_CHANGE,
  SOFM_NEXT_PATTERN,
  SOFM_NEXT_ITERATION,
  SOFM_UPDATE_LEARNING_RATE,
  SOFM_UPDATE_CONSCIENCE_FACTOR,
  SOFM_TRAINING_DONE,
  SOFM_RECALL_DONE,
  NUM_SOFM_STATES
} SOFM_State_Type;

typedef enum SOFM_Conscience_Type_Tag
{
  SOFM_NO_CONSCIENCE,
  SOFM_CONSCIENCE,
  NUM_SOFM_CONSCIENCE
} SOFM_Conscience_Type;

typedef enum Neighbor_Function_Type_Tag
{
  CHEF_HAT,
  MEXICAN_HAT,
  STOVEPIPE_HAT,
  NUM_NEIGHBOR_FUNC
} Neighbor_Function_Type;

/**************************************************************/
/* Structures                                                 */
/**************************************************************/
typedef struct SOFM_2D_Size_Type_Tag
{       // rectangular
  int   width;                               // horizontal length of the neighborhood
  int   height;                              // vertical length of the neighborhood
} SOFM_2D_Size_Type;

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
  SOFM_2D_Size_Type      size;                // size of the slab
  Neuron_Type            **neurons;           // pointer to the array of the neurons
  NN_Layer_Type          slab_type;
} NN_Slab_Arch_Type;

typedef struct SOFM_Arch_Type_Tag
{
  int                    size;                // number of layers
  NN_Slab_Arch_Type      *slabs;              // pointer to the layers
} SOFM_Arch_Type;

typedef struct SOFM_Env_Type_Tag
{
  NN_Operation_Mode_Type  operation_mode;      // training or recall
  SOFM_Training_Mode_Type train_mode;          // training mode if in training operation mode
  float                   eta;                 // learning rate
  float                   gama;                // bias factor
  float                   beta;                //
  float                   shrink;              // learning rate (eta) shrinking coefficient
  float                   criterion;           // training  criterion for termination
  int                     max_ite;             // maximum number of iterations
  int                     cur_ite;             // current iteration index
  int                     max_tra_pat;         // total number of training patterns
  int                     cur_pat;             // current training pattern index
  int                     pat_counter;         // pattern index increase counter within a iteration
  SOFM_Conscience_Type    conscience;          // 0: no conscience, 1: conscience
  SOFM_2D_Size_Type       winner;              // index of winning neuron
  SOFM_2D_Size_Type       neighbor;            // neighborhood size
  SOFM_2D_Size_Type       cur_neighbor;        // current neighborhood size
  Neighbor_Function_Type  neighbor_function;   // neighborhood function type
} SOFM_Env_Type;

typedef struct SOFM_Type_Tag
{
  SOFM_Arch_Type          arch;
  SOFM_Env_Type           env;
} SOFM_Type;

typedef struct SOFM_Pattern_Set_Type_Tag
{
  int                    size;                // number of patterns
  int                    dim_in;              // input dimension
  int                    dim_out;             // output dimension
  FMATRIX                patterns;            // pointer to the array of in/out patterns
} SOFM_Pattern_Set_Type;

/**************************************************************/
/* Function Prototypes for functions with file level scope    */
/**************************************************************/


/**************************************************************/
/* Const Variable with global level scope                     */
/**************************************************************/


/**************************************************************/
/* Function Prototypes for functions with global level scope  */
/**************************************************************/

extern void SOFM_Main_Loop(void);
extern void SOFM_Start_Up(char *dataFile);
extern void SOFM_Clean_Up(void);

#endif
