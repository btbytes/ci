#include <fstream.h>
#include <stdlib.h>
#include <stdio.h>

#include "memberf.h"
#include "variablf.h"
#include "vector.h"
#include "fuzzyrul.h"
#include "ruleset.h"

#include "extern.h"

#define max(a,b) ((a-b)>0)?a:b

#define NAME_MAX 80

char ruleName[NAME_MAX];
char ruleInName[NAME_MAX];

FuzzyRuleSet ruleSet;

static void read_fl_runfile(char *dataFile); // read fuzzy system run file
static void read_fl_rulefile(void);          // read fuzzy system rule file
static void write_fl_rules(void);            // output fuzzy rules in verbal to a file
static void view_fl_result(void);

void fl (char *dataFile)
{
    read_fl_runfile(dataFile);
    read_fl_rulefile();
    write_fl_rules();
        
    ifstream dFile;
    dFile.open(dataFileName,ios::in);
    if (!dFile)
    {
        cerr<<"cann't open file "<<dataFileName<<"  for input"<<endl;
        exit(1);
    }

    int indim,outdim; //input dim and output dim
    dFile>>indim>>outdim;
    vector<float> invec(indim);
    vector<int>   outvec(outdim);
    vector<int>   classN(outdim);  //store class no. for each output
    dFile>>classN;

    int outVarDim=ruleSet[0].get_outputSize();
    if (outdim !=outVarDim)
    {
        cout<<"dim of data outputs isn't equal to dim of output variables in rules"<<endl;
        exit(1);
    }

    ofstream rFile;
    rFile.open(resultFileName,ios::out);
    if (!rFile)
    {
        cerr<<"cann't open file " <<resultFileName<< " for output\n"<<endl;
        exit(1);
    }

    rFile<<"index\t"<<"Wrong?\t"<<"Target\t"<<"Obtained"<<endl;

    int in_order=0;
    int misclassify=0;
    vector<int> cla(outVarDim);
    vector<float> tmp(outVarDim);

    while (dFile>>invec)
    {
        dFile>>outvec;
        in_order++;
        rFile<<in_order<<"\t";
        if (ruleSet.checkRuleSetFired(invec)==1)
        {
	 tmp=ruleSet.output(invec,ruleEffectFlag,fuzzyFlag,defuzzyFlag);

	 //get output class
	 for (int idx=0;idx<outVarDim;idx++)
         	      cla[idx]=(int)(tmp[idx]*classN[idx]);

	 //output data dim equal to outputVariable dim
	 if (cla !=outvec)
	 {
	     rFile<<"wrong\t";
	     misclassify++;
	 }
	 else
	     rFile<<"\t";
                
            rFile<<(outvec|cla);
        }
        else
        {
            rFile<<"rule set not fired"<<endl;
	 misclassify++;
        }
    }
    dFile.close();

    rFile<<"total misclassification is  :"<<misclassify<<endl;
    rFile.close();

    view_fl_result();
}

static void read_fl_runfile (char *dataFile)
{
    int true;
    char Msg[NAME_MAX];
    strcpy(Msg,"edit ");
    strcat(Msg,dataFile);

    ifstream runFile;
    do 
    {
        runFile.open(dataFile,ios::in);
        if (!runFile)
        {
             cerr<<"cann't open file "<<dataFile<<"  for input"<<endl;
             exit(1);
        }
        runFile>>ruleInName>>dataFileName>>ruleName>>resultFileName;
        runFile>>fuzzyFlag>>defuzzyFlag>>ruleEffectFlag;
        runFile.close();
        cout<<ruleInName<<endl;
        cout<<dataFileName<<endl;
        cout<<ruleName<<endl;
        cout<<resultFileName<<endl;
        cout<<fuzzyFlag<<"	0:minimum	1:average"<<endl;
        cout<<defuzzyFlag<<"	0:maximum	1:without overlap	2:with overlap"<<endl;
        cout<<ruleEffectFlag<<"	1: maximum of output values from each rule 0:add"<<endl;
        cout<<"(C)ontinue, (Q)uit, (M)odify runfile ";
        char condition;
        cin>>condition;
        switch(condition)
        {
        	 case 'c': true=0;
	      break;
	 case 'C': true=0;
	      break;
	 case 'q': exit(1);
	 case 'Q': exit(1);
	 case 'm': true=1;
	      system(Msg);
	      break;
	 case 'M': true=1;
	      system(Msg);
	      break;
	 default:
                 true=1;
                 break;
        }
    } while (true==1);
}

static void read_fl_rulefile (void)
{
    // FuzzyRule
    ifstream iFile;
    iFile.open(ruleInName,ios::in);
    if (!iFile)
    {
        cerr<<"cann't open file "<<ruleInName<<"  for input"<<endl;
        exit(1);
    }
    
    iFile>>ruleSet;
    iFile.close();
}

static void write_fl_rules (void)
{
    //output formed rules
    ofstream oFile;
    oFile.open(ruleName,ios::out);
    if (!oFile)
    {
        cerr<<"cann't open file "<<ruleName<<" for output"<<endl;
        exit(1);
    }
    for (int i=0;i<ruleSet.get_ruleSetSize();i++)
        oFile<<i<<"th rule: "<<ruleSet[i].get_ruleContent()<<endl;
    oFile.close();
}

static void view_fl_result (void)
{
    cout<<"view classification result? Y/N ";
    char condition;
    int true;
    char Msg[NAME_MAX];
    do
    {
        cin >>condition;
        if ((condition=='Y')||(condition=='y'))
        {
	 strcpy(Msg,"edit ");
	 strcat(Msg,resultFileName);
	 system(Msg);
	 true=1;
	 cout<<"view classification result again? Y/N ";
        }
        else
	 true=0;
    }while (true==1);
}
