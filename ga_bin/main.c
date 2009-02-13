#include <stdio.h>
#include <stdlib.h>
#include "ga_bin.h"

static void main_initialize(char *);
static void main_clean_up(void);

// Listing 4.8 The main() routine of the binary GA implementation
int main(int argc,char *argv[])
{
	if (argc != 2)
	{
		printf("usage: ga  [datFile]\n");
		exit(1);
	}

  // initialize
  main_initialize(argv[1]);

  // run GA
  GA_Main_Loop();

  // clean up memory space
  main_clean_up();
}

static void main_initialize (char *dataFile)
{
    GA_Start_Up(dataFile);
}

static void main_clean_up (void)
{
    GA_Clean_Up();
}
