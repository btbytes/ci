#ifndef GA_BINARY_H
#define GA_BINARY_H


#include "opt_func.h"

/************************************************************/
/* Constants and Macros                                     */
/************************************************************/
#define NAME_MAX 80

/************************************************************/
/* Enumerations and Structures and Typedefs                 */
/************************************************************/

// Listing 4.6 Enumeration data type in the GA implementaion
typedef enum selection_type_tag
{
    ROULETTE_WHEEL_SCALING,
    BINARY_TOURNAMENT,
    RANKING,
    BOLTZMANN,
    NUM_SELECTION
} selection_type;

// Listing 4.1 Enumeration data type for crossover operators
typedef enum  crossover_type_tag
{
    ONE_POINT_CROSSOVER,
    UNIFORM_CROSSOVER,
    TWO_POINT_CROSSOVER,
    NUM_CROSSOVER
} crossover_type;

// Listing 4.7 Structure data type in the GA implementation
typedef struct ga_binary_data_type_tag
{
    unsigned char      **population;               // double pointer to the population of binary GA
    double             *fit;                       // pointer to the fitness vector
    int                popu_size;                  // population szie:   popsize
    int                indi_length;                // length of chromosome: length
    int                iter_max;                   // iter:  maximum number of iterations
    double             crossover_rate;             // crossover rate
    double             mutation_rate;              // mutation rate
    double             termination_criterion;      // criterion
    int                best_index;                 // index of the best individual of the current population
    unsigned char      bits_per_para;              // each weight represented by bits_per_para bits
    unsigned char      mutation_flag;              // flag for mutatation, 1, variable, 0 constant
    crossover_type     c_type;                     // crossover type: 0: one, 1: uniform, 2: two
    selection_type     s_type;                     // selection method
    double             *gau;                       // store gaussian function value for each bit
    int                gene_index;                 // index of current generation
    double             fit_variance;               // variance of fitness of the current generation
    double             fit_mean;                   // average of fitness of the current generation
} ga_binary_data_type;

typedef struct ga_env_data_type_tag
{
    char                     resultFile[NAME_MAX]; // result file name
    int                      dimension;            // N:
    Evaluate_Function_Type   function;             // function to be solved

} ga_env_data_type;

/*******************************************************************/
/* Global Variable extern Declarations                             */
/*******************************************************************/
extern ga_binary_data_type ga_data;
extern ga_env_data_type ga_env_data;

/*******************************************************************/
/* Function Prototypes                                             */
/*******************************************************************/
extern void GA_Main_Loop(void);
extern void GA_Start_Up(char*);
extern void GA_Clean_Up(void);

extern void debug_print_result(void);
extern void debug_selection(int* flag);
#endif 
