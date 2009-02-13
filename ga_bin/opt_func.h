#ifndef OPT_FUNC_H
#define OPT_FUNC_H

/************************************************************/
/* Enumerations and Structures and Typedefs                 */
/************************************************************/
typedef enum Evaluate_Function_Tag
{
    F6,                               // 0 :F6: min
    SPHERE,                           // 1 :Sphere: min
    ROSENBROCK,                       // 2 :Rosenbrock: min
    RASTRIGRIN,                       // 3 :Rastrigrin: min
    GRIEWANK,                         // 4 :Griewank: min
    NUM_EVALUATE_FUNCTIONS            // Total number of evaluation functions
} Evaluate_Function_Type;

/************************************************************/
/* Function Prototypes                                      */
/************************************************************/
extern double OPT_Function_Routines(int, int, double *);
 
#endif
