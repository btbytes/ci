#include "headfile.h"
#include "popuint.h"
#include "chromint.h"
#include "misc.h"
#include "extern.h"

#define NAME_MAX 80

char  baseRuleName[NAME_MAX];
int   shift;	//1:shift  0: no shift for selection
int   flag_c;	//0:uniform   1:one point  2:two point crossover
float c_rate;	//crossover rate
float m_rate;	//mutation rate
int   evolve_type;  //0:rule set  1:rule set & membership function tuning
		  //2: rule set & membership function tuning and type selection
int   generation;	//generation number
int   p_size; //population length
int   ruleNo;	//maximum rule number
int   divisionNo;	//the number of division for each variable
int   patNo;	//training pattern number
float criterion;	//fitness criterion

FuzzyRule     baseRule;
vector<int>   rangeint;
PopulationInt popu;
vector<float> fitvec;
int           inLen,outLen;  //pattern input, output length
array         arrayPat;
vector<int>   classN;

static void read_ga_runfile(char *dataFile);
static void read_fuzzy_base_rule(void);
static void read_ga_training_patterns(void);
static void form_range_vector(void);
static int  get_population_length(void);
static void update_popu(int size, int wide, float c_rate, float m_rate);
static void popu_initialize(IndividualInt& range);
static void calculate_fitness(IndividualInt& range);
static void write_ga_fuzzy_rules(int idx, IndividualInt& range, int best_fit);
static void popu_mutate(IndividualInt& range, int best_fit);
static void ga_post_processing(void);

// Listing 8.26 Definition of the ga() routine
void ga(char *dataFile)
{
    time_t  t;
	 srand((unsigned) time(&t));

	 read_ga_runfile(dataFile);
    read_fuzzy_base_rule();

	 //input training data pairs
    read_ga_training_patterns();

	 //form rule range
	 form_range_vector();
    IndividualInt range(rangeint,0);

	 //get population length
	 int tmplen = get_population_length();

	 //update population parameters
	 update_popu(p_size,tmplen,c_rate,m_rate);

	 //population initialize
	 popu_initialize(range);

	 //update fitness vector size
	 fitvec.changeSize(p_size);

	 cout.precision(4);

	 cout<<"\t   Fitness vs. Generation"<<endl;
	 cout<<"generation\t best\t average\t variance\t "<<endl;
	 int bestfit;
	 for (int idx = 0;idx < generation;idx++)
	 {
		  //calculate the fitness
		  calculate_fitness(range);
		  bestfit=fitvec.maximum_index();
		  float aver=average(fitvec);        //average of fitnesses
		  float var=variance(fitvec,aver);   //variance of fitnesses

		  if ((idx%5)==0)
				 cout<<idx<<"\t\t"<<fitvec[bestfit]<<"\t"<<aver<<"\t\t"<<var<<endl;

		  if (fitvec[bestfit]>criterion)
				break;

		  if (idx != (generation -1))
		  {   //not the last generation
				//selection: form new population
				popu.selection(fitvec,bestfit,shift); //1:sfite
				popu.crossover(flag_c,bestfit);
				popu_mutate(range,bestfit);
		  }
    }
    write_ga_fuzzy_rules(idx, range, bestfit);
	 ga_post_processing();
}

static void read_ga_runfile (char *dataFile)
{
    int true;
    char Msg[NAME_MAX];
    strcpy(Msg,"edit ");
	 strcat(Msg,dataFile);
	 ifstream runFile;
	 char condition;
	 do {
		  clrscr();
		  runFile.open(dataFile,ios::in);

		  if (!runFile)
		  {
				 cerr<<"cann't open file "<<dataFile<<"  for input"<<endl;
	          exit(1);
		  }
		  runFile>>dataFileName>>baseRuleName>>resultFileName;
		  //runFile>>mutateName; //new added
		  runFile>>shift>>flag_c>>c_rate>>m_rate; //>>m_flag;
		  runFile>>evolve_type>>generation;
		  runFile>>p_size>>ruleNo>>divisionNo>>patNo>>criterion;
		  runFile>>fuzzyFlag>>defuzzyFlag>>ruleEffectFlag;
		  runFile.close();

		  cout<<dataFileName<<endl;
		  cout<<baseRuleName<<endl;
		  cout<<resultFileName<<endl;
		  cout<<shift<<"  0:no shift  1:shift  during selection"<<endl;
		  cout<<flag_c<<"  0:uniform  1:one point  2:two point  crossover"<<endl;
		  cout<<c_rate<<"  crossover probability"<<endl;
		  cout<<m_rate<<"  mutation probability"<<endl;
		  cout<<evolve_type<<"  0:rule set 1:rule & function 2:rule & function & type"<<endl;
		  cout<<generation<<"  generation number"<<endl;
		  cout<<p_size<<"  population size"<<endl;
		  cout<<ruleNo<<"  maximum rule number"<<endl;
		  cout<<divisionNo<<"  Number of division for each variable"<<endl;
		  cout<<patNo<<"  training pattern number"<<endl;
		  cout<<criterion<<" fitness criterion "<<endl;

		  cout<<fuzzyFlag<<"	0:minimum	1:average"<<endl;
		  cout<<defuzzyFlag<<"	0:maximum	1:without overlap	2:with overlap"<<endl;
		  cout<<ruleEffectFlag<<"	1: maximum of output values from each rule 0:add"<<endl;
		  cout<<"(C)ontinue, (Q)uit, (M)odify runfile ";

		  cin>>condition;
		  switch(condition)
		  {
				case 'c':
				case 'C':
					 true=0;
					 break;
				case 'q':
				case 'Q': exit(1);
				case 'm':
				case 'M': true=1;
					 system(Msg);
					 break;
				default:
					 true=1;
		  }
	 } while (true==1);
}

static void read_fuzzy_base_rule (void)
{
    //input base rule
	 ifstream rFile;
	 rFile.open(baseRuleName,ios::in);
	 if (!rFile)
	 {
		  cerr<<"cann't open file base.rul  for input"<<endl;
		  exit(1);
	 }
	 rFile>>baseRule;
	 rFile.close();
}

static void read_ga_training_patterns (void)
{
    ifstream dFile;
    dFile.open(dataFileName,ios::in);
    if (!dFile)
    {
        cerr<<"cann't open file iris.dat  for input"<<endl;
        exit(1);
    }
	 dFile>>inLen>>outLen;
	 classN.changeSize(outLen);
	 dFile>>classN;
    arrayPat.changeSize(patNo,inLen+outLen);
	 dFile>>arrayPat;
    dFile.close();
}

static void form_range_vector (void)
{
    switch(evolve_type)
    {
        case 0:
				rangeint=baseRule.formRange(ruleNo);
				break;
        case 1:
				rangeint=baseRule.formRange_RM(ruleNo,divisionNo);
				break;
        default:
	         rangeint=baseRule.formRange_RMT(ruleNo,divisionNo,6); //6: no of types
    }
}

static int get_population_length (void)
{
	 int tmplen=ruleNo*(inLen+outLen) +1; //evolve_type=0

    if (evolve_type==1)
    {
        for (int i=2;i<rangeint.len();i++)
				tmplen +=2*rangeint[i];	//each function has two points
	 }
	 else if (evolve_type==2)
	 {
        for (int i=3;i<rangeint.len();i++)
	         tmplen +=3*rangeint[i];	//each function has two points and one type
	 }
    return tmplen;
}

static void popu_initialize (IndividualInt& range)
{
    switch(evolve_type)
    {
        case 0:
				popu.initialize_range(range);
				break;
		  case 1:
				popu.initialize_range_RM(range);
				break;
		  default:
				popu.initialize_range_RMT(range);
	 }
}

static void update_popu (int size, int wide, float c_rate, float m_rate)
{
	 popu.change_length(size);
	 popu.change_width(wide);
	 popu.change_crate(c_rate);
    popu.change_mrate(m_rate);
}

static void calculate_fitness (IndividualInt& range)
{
    switch(evolve_type)
	 {
		  case 0:
				fitvec=popu.fitness(baseRule,arrayPat,classN,ruleEffectFlag,fuzzyFlag,defuzzyFlag); //check selection choice
				break;
		  case 1:
				fitvec=popu.fitness_RM(baseRule,arrayPat,classN,ruleEffectFlag,fuzzyFlag,defuzzyFlag,range); //check selection choice
				break;
		  default:
				fitvec=popu.fitness_RMT(baseRule,arrayPat,classN,ruleEffectFlag,fuzzyFlag,defuzzyFlag,range); //check selection choice
	 }
}

static void popu_mutate (IndividualInt& range, int bestfit)
{
    switch(evolve_type)
	 {
		  case 0:
				popu.mutate_one(range,bestfit);
				break;
		  case 1:
				popu.mutate_one_RM(range,bestfit);
				break;
		  default:
		      popu.mutate_one_RMT(range,bestfit);
	 }
}

static void write_ga_fuzzy_rules (int idx, IndividualInt& range, int bestfit)
{
    //open file for save result
    ofstream reFile;
    reFile.open(resultFileName,ios::out);
    if (!reFile)
    {
        cerr<<"cann't open file result.rul  for output"<<endl;
        exit(1);
	 }
	 switch(evolve_type)
	 {
		  case 0:
				reFile<<popu[bestfit].formRuleSet(baseRule);
				break;
		  case 1:
				reFile<<popu[bestfit].formRuleSet_RM(baseRule,range);
				break;
		  default:
				reFile<<popu[bestfit].formRuleSet_RMT(baseRule,range);
	 }

	 reFile<<"generation  = "<<idx<<endl;
	 reFile<<"fitness = "<<fitvec[bestfit]<<endl<<endl;
    reFile<<dataFileName<<endl;
	 reFile<<baseRuleName<<endl;
	 reFile<<resultFileName<<endl;
	 reFile<<shift<<"  0:no shift  1:shit  during selection"<<endl;
	 reFile<<flag_c<<"  0:uniform  1:one point  2:two point  crossover"<<endl;
	 reFile<<c_rate<<"  crossover probability"<<endl;
	 reFile<<m_rate<<"  mutation probability"<<endl;
	 reFile<<evolve_type<<"  0:r 1:rm 2:rmt"<<endl;
	 reFile<<generation<<"  generation number"<<endl;
	 reFile<<p_size<<"  population size"<<endl;
	 reFile<<ruleNo<<"  maximum rule number"<<endl;
	 reFile<<divisionNo<<"  Number of division for each variable"<<endl;
    reFile<<patNo<<"  training pattern number"<<endl;
	 reFile<<criterion<<" fitness criterion "<<endl;
	 reFile<<fuzzyFlag<<"	0:minimum	1:average"<<endl;
	 reFile<<defuzzyFlag<<"	0:maximum	1:without overlap	2:with overlap"<<endl;
	 reFile<<ruleEffectFlag<<"	1: maximum of output values from each rule 0:add"<<endl;

	 reFile.close();
}

static void ga_post_processing (void)
{
	 char Msg[NAME_MAX];
    clrscr();
	 strcpy(Msg,"move ");
	 strcat(Msg,resultFileName);
	 cout<<"rename  "<<resultFileName<<"   Y/N?  ";
	 char condition;
	 cin>>condition;
	 switch(condition)
	 {
		  case 'Y':
		  case 'y':
				 cout<<"input destination file name: ";
				 char Mmsg[80];
				 cin>>Mmsg;
				 strcat(Msg,"  ");
				 strcat(Msg,Mmsg);
				 system(Msg);
				 break;
		  default:
				cout<<"don't change file name"<<endl;
    }
}
