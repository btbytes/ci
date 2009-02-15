#include "headfile.h"

// Listing 8.15 Implementation of main()
void
main(int argc,char *argv[])
{
    extern void fl(char *);

    if (argc !=2)
    {
        fprintf(stderr,"usuage: fl run_file_name\n");
        exit(1);
    }
    fl(argv[1]);
 }
