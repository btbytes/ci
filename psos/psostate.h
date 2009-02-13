#ifndef PSO_STATE_H
#define PSO_STATE_H

#include "definiti.h"

/**************************************/
/* Constants and Macros               */
/**************************************/

/**************************************/
/* Enumerations                       */
/**************************************/

// Listing 4.15 & 4.21 Definition of some new data types in the PSO implementation
typedef enum PSO_State_Tag
{
    //PSO_INITIALIZE,                 // Initialize the population
    PSO_UPDATE_INERTIA_WEIGHT,        // Update inertia weight
    PSO_EVALUATE,                     // Evaluate partiles
    PSO_UPDATE_GLOBAL_BEST,           // Update global best
    PSO_UPDATE_LOCAL_BEST,            // Update local best
    PSO_UPDTAE_VELOCITY,              // Update particle's velocity
    PSO_UPDATE_POSITION,              // Update particle's position
    PSO_GOAL_REACH_JUDGE,             // Judge whether reach the goal
    PSO_NEXT_GENERATION,              // Move to the next generation
    PSO_UPDATE_PBEST_EACH_CYCLE,      // Update pbest each cycle for co-pso due to the environment changed
    PSO_NEXT_PSO,                     // Move to the next PSO in the same cycle or the first pso in the next cycle
    PSOS_DONE,                        // Finish one cycle of PSOs
    NUM_PSO_STATES                    // Total number of PSO states
} PSO_State_Type;

typedef enum PSO_Initialize_Tag
{
    PSO_RANDOM_SYMMETRY_INITIALIZE,   // 0  :Symmetry Initialization
    PSO_RANDOM_ASYMMETRY_INITIALIZE,  // 1  :Symmetry Initialization
    NUM_PSO_INITIALIZE                // Number of initialization methods
} PSO_Initialize_Type;

typedef enum MINMAX_Tag
{
    MINIMIZATION,                     // 0  :Minimization problem
    MAXIMIZATION                      // 1  :Maximization problem
} MINMAX_Type;


// Listing 4.23 Expanded Evaluate_Function_Type
typedef enum Evaluate_Function_Tag
{
    G1_MIN,                           // 0  :G1 from Tahk and Sun's IEEE Trans EC paper, min part
    G1_MAX,                           // 1  :G2 from Tahk and Sun's IEEE Trans EC paper, max part
    G7_MIN,                           // 2  :G7 from Tahk and Sun's IEEE Trans EC paper, min part
    G7_MAX,                           // 3  :G7 from Tahk and Sun's IEEE Trans EC paper, max part
    G9_MIN,                           // 4  :G9 from Tahk and Sun's IEEE Trans EC paper, min part
    G9_MAX,                           // 5 :G9 from Tahk and Sun's IEEE Trans EC paper, max part
    F6,                               // 6 :F6: min
    SPHERE,                           // 7 :Sphere: min
    ROSENBROCK,                       // 8 :Rosenbrock: min
    RASTRIGRIN,                       // 9 :Rastrigrin: min
    GRIEWANK,                         // 10 :Griewank: min
    NUM_EVALUATE_FUNCTIONS            // Total number of evaluation functions
} Evaluate_Function_Type;


typedef enum Inertia_Weight_Update_Method_Tag
{
  CONSTANT_IW,                        // 0  :constant inertia weight
  LINEAR_IW,                          // 1  :Linearly decreasing inertia weight
  NOISE_ADDITION_IW,                  // 2  :Adding nosie to the constant inertia weight
  NUM_IW_UPDATE_METHODS               // Number of inertia weight update methods
} IW_Update_Type;


// Listing 4.16 Structure data type definition for PSO
/**************************************/
/* Structures                         */
/**************************************/
typedef struct PSO_Initizlize_Range_Type_Tag
{
  float  left;
  float  right;
} PSO_Initizlize_Range_Type;

typedef struct PSO_Environment_Type_Tag      // PSO working condition
{
  MINMAX_Type                         opti_type;
  Evaluate_Function_Type              function_type;
  IW_Update_Type                      iw_method;
  PSO_Initialize_Type                 init_type;
  PSO_Initizlize_Range_Type           init_range;
  float                               max_velocity;
  float                               max_position;
  int                                 max_generation;
  int                                 boundary_flag;         // 1: boundary; 0: no boundary
  FVECTOR                             low_boundaries;
  FVECTOR                             up_boundaries;
} PSO_Environment_Type;

typedef struct PSO_Type_Tag           // PSO parameters
{
  PSO_Environment_Type                env_data;
  int                                 popu_size;
  int                                 dimension;
  float                               inertia_weight;
  float                               init_inertia_weight;
  int                                 global_best_index;
  FVECTOR                             pbest_values;
  FMATRIX                             velocity_values;
  FMATRIX                             position_values;
  FMATRIX                             pbest_position_values;
  float                               eva_fun_value;         // value obtained from evaluatation for current individual
  int                                 popu_index;
  int                                 gene_index;
} PSO_Type;


/**************************************/
/* Global and Const Variable          */
/**************************************/

/**************************************/
/* Function Prototypes                */
/**************************************/
extern void PSO_Main_Loop(void);
extern void PSO_Start_Up(char *dataFile);
extern void PSO_Clean_Up(void);

#define min(x,y) ((x) < (y) ? (x) : (y))
#define max(x,y) ((x) > (y) ? (x) : (y))

#endif  
