#include "headfile.h"

#define NAME_MAX 80
char resultFileName[NAME_MAX], dataFileName[NAME_MAX];
int fuzzyFlag; //1: average  0:minimum
int defuzzyFlag; //0: maximum, 1: centroid without overlap, 2: with overlap
int ruleEffectFlag; //1:maximum 0:add output values from each rule

void
main(int argc,char *argv[])
{
    extern void ga(char *);
    extern void fl(char *);

    if (argc!=2)
    {
        printf("usage: flga  runfile\n");
        exit(1);
    }
    char gaName[80],flName[80];
    char Msg[80];
    strcpy(Msg,"edit ");
    strcat(Msg,argv[1]);
    char condition;
    int true=1;
    do
    {
        clrscr();
        ifstream runFile;
        runFile.open(argv[1],ios::in);
        if (!runFile)
        {
	 cerr<<"cann't open file "<<argv[1]<<"  for input"<<endl;
	 exit(1);
        }
        runFile>>gaName>>flName;
        cout<<gaName<<"	run file for generating rules"<<endl;
        cout<<flName<<"	run file for classification using fuzzy rule system"<<endl;
        runFile.close();
        cout<<"C: continue"<<endl;
        cout<<"M: modify"<<endl;
        cout<<"Q: quit"<<endl;
        cout<<"your choice? ";
        cin>>condition;
        switch(condition)
        {
	 case 'c':
	 case 'C':
	     true=0;
	     break;
	 case  'm':
	 case  'M':
	     system(Msg);
	     break;
	 case 'q':
	 case 'Q':
	     exit(1);
	 default:
	     break;
        }
    }while (true==1);

    true=1;
    do
    {
        clrscr();
        cout<<"G: generating rules"<<endl;
        cout<<"C: classification"<<endl;
        cout<<"Q: quit"<<endl;
        cout<<"your choice?  ";
        cin>>condition;
        switch(condition)
        {
	 case  'g':
	 case  'G':
	     ga(gaName);
	     break;
	 case  'c':
	 case  'C':
	     fl(flName);
	     break;
            case  'q':
	 case  'Q': 
	     true=0;
	     break;
	 default:
	     true=0;
        }
    } while (true==1);
}



