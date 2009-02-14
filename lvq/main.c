#include "headfile.h"
#include "lvq.h"
#include "mem_loc.h"

/**************************************/
/* Constants and Macros               */
/**************************************/

static void main_start_up(char *dataFile);
static void main_clean_up(void);

// Listing 6.9 LVQ main() routine
void main (int argc, char *argv[])
{
    // check command line
    if (argc != 2)
	  {
		  printf("Usage: exe_file run_file");
		  exit(1);
	  }

    // initialize
    main_start_up(argv[1]);

    // run
    LVQ_Main_Loop();

    // clean up memory space
    main_clean_up();

}

static void main_start_up (char *dataFile)
{
    LVQ_Start_Up(dataFile);

}

static void main_clean_up (void)
{
    LVQ_Clean_Up();
}
