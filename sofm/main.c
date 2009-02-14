#include "headfile.h"
#include "sofm.h"
#include "mem_loc.h"

/**************************************/
/* Constants and Macros               */
/**************************************/

static void main_start_up(char *dataFile);
static void main_clean_up(void);

void main (int argc, char *argv[])
{
    // check command line
    if (argc != 2)
	  {
		  printf("Usage: exe_file run_file\n");
		  exit(1);
	  } 

    // initialize
    main_start_up(argv[1]);

    // run 
    SOFM_Main_Loop();        

    // clean up memory space
    main_clean_up();

}

static void main_start_up (char *dataFile)
{
    SOFM_Start_Up(dataFile);      

}

static void main_clean_up (void)
{
    SOFM_Clean_Up();
}
