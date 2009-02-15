#include "headfile.h"
#include "psostate.h"
#include "bp.h"
#include "mem_loc.h"

/**************************************/
/* Constants and Macros               */
/**************************************/
#define NUM_RUN 50

static void main_start_up(char *psoDataFile, char *bpDataFile);
static void main_clean_up(void);

// Listing 6.14 The main() routine of evolutioanry BP net
void main (int argc, char *argv[])
{
    // check command line
    if (argc != 3)
	  {
		  printf("Usage: exe_file pso_run_file bp_run_file\n");
		  exit(1);
	  }

    // initialize
    main_start_up(argv[1],argv[2]);

    PSO_Main_Loop();

    // clean up memory space
    main_clean_up();
}

static void main_start_up (char *psoDataFile,char *bpDataFile)
{
    BP_Start_Up(bpDataFile);
    PSO_Start_Up(psoDataFile);
}

static void main_clean_up (void)
{
    PSO_Clean_Up();
    BP_Clean_Up();
}
