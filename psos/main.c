#include "headfile.h"
#include "psostate.h"
#include "mem_loc.h"

/**************************************/
/* Constants and Macros               */
/**************************************/
#define NUM_RUN 1

static void main_start_up(char *dataFile);
static void main_clean_up(void);

// Listing 4.17 PSO main() routine
int main (int argc, char *argv[])
{
    int idx_i;
    // global variable definitions


    // check command line
    if (argc != 2)
	  {
		  printf("Usage: exe_file run_file\n");
		  exit(1);
	  }

    // initialize
    main_start_up(argv[1]);

    // run PSO
    for (idx_i  = 0; idx_i < NUM_RUN ; idx_i++)
    {
        PSO_Main_Loop();
    }
    // clean up memory space
    main_clean_up();
    return 0;
}

static void main_start_up (char *dataFile)
{
    PSO_Start_Up(dataFile);

}

static void main_clean_up (void)
{
    PSO_Clean_Up();
}
