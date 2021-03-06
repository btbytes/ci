#include "headfile.h"
#include "bp.h"
#include "mem_loc.h"

/**************************************/
/* Constants and Macros               */
/**************************************/
#define NUM_RUN 50

static void main_start_up(char *dataFile);
static void main_clean_up(void);

// Listing 6.4 Back-propagation main() routine
int main (int argc, char *argv[])
{
    int idx_i;
    // global variable definitions


    // check command line
    if (argc != 2)
	  {
		  printf("Usage: exe_file run_file");
		  exit(1);
	  }

    // initialize
    main_start_up(argv[1]);

    // run
    for (idx_i  = 0; idx_i < NUM_RUN ; idx_i++)
    {
        BP_Main_Loop();
    }
    // clean up memory space
    main_clean_up();
    return 0;
}

static void main_start_up (char *dataFile)
{
    BP_Start_Up(dataFile);

}

static void main_clean_up (void)
{
    BP_Clean_Up();
}
