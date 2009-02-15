#include "headfile.h"
#include "ruleset.h"

//constructors
FuzzyRuleSet::FuzzyRuleSet(int a):
	ruleSetSize(a)
{
    rules = new FuzzyRule[ruleSetSize];
    assert(rules !=0);
}

FuzzyRuleSet::FuzzyRuleSet(int a,FuzzyRule *b):
ruleSetSize(a)
{
    rules=new FuzzyRule[ruleSetSize];
    assert(rules !=0);
    for (int i=0;i<ruleSetSize;i++)
        rules[i]=b[i];
}


FuzzyRuleSet::FuzzyRuleSet(const FuzzyRuleSet& a):
ruleSetSize(a.ruleSetSize)
{
    rules =new FuzzyRule[ruleSetSize];
    assert(rules !=0);
    for (int i=0;i<ruleSetSize;i++)
        rules[i]=a.rules[i];
}


//member functions

FuzzyRuleSet&
FuzzyRuleSet::addRuleB(const FuzzyRule& a,const int& b)
{
    FuzzyRuleSet newSet(ruleSetSize,rules);
    ruleSetSize++;

    delete []rules;
    rules =new FuzzyRule[ruleSetSize];
    assert(rules !=0);

    for (int i=0;i<b;i++)
        rules[i]=newSet[i];

    rules[b]=a;

    for (i=b;i<(ruleSetSize-1);i++)
        rules[i+1]=newSet[i];

    return *this;
}

FuzzyRuleSet&
FuzzyRuleSet::addRule(const FuzzyRule& a)
{
    FuzzyRuleSet newSet(ruleSetSize,rules);

    delete []rules;
    rules =new FuzzyRule[ruleSetSize+1];
    assert(rules !=0);

    for (int i=0;i<ruleSetSize;i++)
        rules[i]=newSet[i];

    rules[ruleSetSize]=a;

    ruleSetSize++;
    return *this;
}

FuzzyRuleSet&
FuzzyRuleSet::deleteRule(const int& a)
{
    assert((a>=0)&&(a<ruleSetSize));
    FuzzyRuleSet newSet(ruleSetSize,rules);

    delete []rules;
    ruleSetSize--;
    rules =new FuzzyRule[ruleSetSize];
    assert(rules !=0);
    for (int i=0;i<a;i++)
        rules[i]=newSet[i];

    for (i=a;i<ruleSetSize;i++)
        rules[i]=newSet[i+1];

    return *this;
}

//a:input vector,
//b: mode for antecedent--0:min 1:aver, c: mode for defuzzyfy--0:max 1:centroid without overlap
//2: with overlap;  output_add: add the contribution from each rule

// Listing 8.11 Implementation of method fuzzyOutputValue_max() in class FuzzyRuleSet
vector< vector<float> >
FuzzyRuleSet::fuzzyOutputValue_max(const vector<float>& a, const int& b) const
{
    if (a.len() !=rules[0].get_variableSize())
    {
        fprintf(stderr,"input dim doesn't match the inputVariable no. of the rule");
        exit(1);
    }

    int outVarDim=rules[0].get_outputSize();   //outputVariable no. in rules

    vector< vector<float> > result(outVarDim);

    vector<int> varDim(outVarDim);

    for (int i=0;i<outVarDim;i++)
    {
        varDim[i]=rules[0].get_outputVariable(i).get_setSize();
        result[i].changeSize(varDim[i]);
    }

    //initialization of result
    for (i=0;i<outVarDim;i++)
        for (int j=0;j<varDim[i];j++)
	 result[i][j]=0;

    vector<float> tmpres(outVarDim);
    for (i=0;i<ruleSetSize;i++)
    {
        int ter=rules[i].checkRuleActive(a);
        if (ter==1)
        {
	 vector<int> tmpvec=rules[i].get_outputSetFlag();

	 if (b==1)
	     tmpres=rules[i].FuzzyOutput_average(a);
	 else
	     tmpres=rules[i].FuzzyOutput(a);

	 for (int j=0;j<outVarDim;j++)
	 {
	     if (tmpvec[j] !=0)
		result[j][abs(tmpvec[j])-1]=max(result[j][abs(tmpvec[j])-1],tmpres[j]);
	 }
        }
    }
    return result;
}

vector< vector<float> >
FuzzyRuleSet::fuzzyOutputValue_add(const vector<float>& a, const int& b) const
{
    if (a.len() !=rules[0].get_variableSize())
    {
        fprintf(stderr,"input dim doesn't match the inputVariable no. of the rule");
        exit(1);
    }

    int outVarDim=rules[0].get_outputSize();   //outputVariable no. in rules

    vector< vector<float> > result(outVarDim);

    vector<int> varDim(outVarDim);

    for (int i=0;i<outVarDim;i++)
    {
        varDim[i]=rules[0].get_outputVariable(i).get_setSize();
        result[i].changeSize(varDim[i]);
    }

    //initialization of result
    for (i=0;i<outVarDim;i++)
        for (int j=0;j<varDim[i];j++)
	 result[i][j]=0;

    vector<float> tmpres(outVarDim);
    for (i=0;i<ruleSetSize;i++)
    {
        int ter=rules[i].checkRuleActive(a);
        if (ter==1)
        {
	 vector<int> tmpvec=rules[i].get_outputSetFlag();
            if (b==1)
	     tmpres=rules[i].FuzzyOutput_average(a);
	 else
	     tmpres=rules[i].FuzzyOutput(a);

	 for (int j=0;j<outVarDim;j++)
	 {
	     if (tmpvec[j] !=0)
	     {
		result[j][abs(tmpvec[j])-1] +=tmpres[j];
		if ((result[j][abs(tmpvec[j])-1])>1)
		    result[j][abs(tmpvec[j])-1]=1;
	     }
	 }
        }
    }

    return result;
}

// Listing 8.12 Implementation of method defuzzify(0 in class FuzzyRuleSet
vector<float>
FuzzyRuleSet::defuzzify(const vector< vector<float> >& a,const int& b) const
{
    //get output variables in a rule
    int outVarDim=rules[0].get_outputSize();
    vector<float> tmp(outVarDim);
    vector<int> varDim(outVarDim);

    for (int i=0;i<outVarDim;i++)
    {
        //fuzzy set no.`in output variable i
        varDim[i]=this->get_outputVariable(i).get_setSize();
        //defuzzify for output variable i

        if (b==0)
	 tmp[i]=this->get_outputVariable(i).defuzzifyMax(varDim[i],a[i]);
        else if (b==1)
	 tmp[i]=this->get_outputVariable(i).defuzzyCentroid(varDim[i],a[i]);
        else
	 tmp[i]=this->get_outputVariable(i).defuzzyCentroid_add(varDim[i],a[i]);

    }
    return tmp;
}

FuzzyVariable&
FuzzyRuleSet::get_outputVariable(const int& a) const
{
    assert (a>=0&&a<rules[0].get_outputSize());
    return rules[0].get_outputVariable(a);
}

int
FuzzyRuleSet::checkRuleSetFired(const vector<float>& a) const
{
    assert(a.len()==rules[0].get_variableSize());
    int sum=0;
    for (int i=0;i<ruleSetSize;i++)
        	sum +=rules[i].checkRuleActive(a);
    return (sum>0);
}

// Listing 8.13 Implementation of method output() in class FuzzyruleSet
vector<float>
FuzzyRuleSet::output(const vector<float>& a, const int& b,const int& c,const int& d) const
{		//a: input  b:add/max c:min/aver d:max/without/with overlap
		// return the value after defuzzify
    if (a.len() !=rules[0].get_variableSize())
    {
        fprintf(stderr,"input dim doesn't match the inputVariable no. of the rule");
        exit(1);
    }

    int outVarDim=rules[0].get_outputSize();   //outputVariable no. in rules
    vector< vector<float> > result(outVarDim);

    vector<int> varDim(outVarDim);

    for (int i=0;i<outVarDim;i++)
    {
        varDim[i]=rules[0].get_outputVariable(i).get_setSize();
        result[i].changeSize(varDim[i]);
    } //allocate memory for result

    if (b==1)
        result=this->fuzzyOutputValue_max(a,c);
    else
        result= this->fuzzyOutputValue_add(a,c);

    vector<float> tmp(outVarDim);
    tmp=this->defuzzify(result,d);

    return tmp;
}


vector<float>
FuzzyRuleSet::output_new(const vector<float>& a, const int& b,const int& c,const int& d) const
{   //a: input  b:add/max c:min/aver d:max/without/with overlap
    // return the value after defuzzify
    if (a.len() !=rules[0].get_variableSize())
    {
        fprintf(stderr,"input dim doesn't match the inputVariable no. of the rule");
        exit(1);
    }

    int outVarDim=rules[0].get_outputSize();   //outputVariable no. in rules
    vector< vector<float> > result(outVarDim);
    vector<int> varDim(outVarDim);

    for (int i=0;i<outVarDim;i++)
    {
        varDim[i]=this->get_outputVariable(i).get_setSize();
        result[i].changeSize(varDim[i]);
    } //allocate memory for result

    //initialization of result
    for (i=0;i<outVarDim;i++)
        for (int j=0;j<varDim[i];j++)
	 result[i][j]=0;

    //get fuzzy output
    vector<float> tmpres(outVarDim);
    for (i=0;i<ruleSetSize;i++)
    {
        int ter=rules[i].checkRuleActive(a);
        if (ter==1)
        {
	 vector<int> tmpvec=rules[i].get_outputSetFlag();

	 if (c==1)
	     tmpres=rules[i].FuzzyOutput_average(a);
	 else
	     tmpres=rules[i].FuzzyOutput(a);

	 for (int j=0;j<outVarDim;j++)
	 {
	     if (tmpvec[j] !=0)
	     {
		if (b==1)
		    result[j][abs(tmpvec[j])-1]=max(result[j][abs(tmpvec[j])-1],tmpres[j]);
		else
		{
		    result[j][abs(tmpvec[j])-1] +=tmpres[j];
		    if ((result[j][abs(tmpvec[j])-1])>1)
		        result[j][abs(tmpvec[j])-1]=1;
		}
	     }
	 }
        }
    }

    //defuzzy
    vector<float> tmp(outVarDim);
    for (i=0;i<outVarDim;i++)
    {
        //defuzzify for output variable i
        if (d==0)
	 tmp[i]=this->get_outputVariable(i).defuzzifyMax(varDim[i],result[i]);
        else if (d==1)
	 tmp[i]=this->get_outputVariable(i).defuzzyCentroid(varDim[i],result[i]);
        else
	 tmp[i]=this->get_outputVariable(i).defuzzyCentroid_add(varDim[i],result[i]);
    }

    return tmp;
}


//operators

FuzzyRule&
FuzzyRuleSet::operator [] (int i) const
{
    assert((i>=0)&&(i<ruleSetSize));
    return rules[i];
}

FuzzyRuleSet&
FuzzyRuleSet::operator =(const FuzzyRuleSet& a)
{
    if ((&a)==this) return *this;

    delete []rules;
    ruleSetSize=a.ruleSetSize;
    rules =new FuzzyRule[ruleSetSize];
    assert(rules !=0);

    for (int i=0;i<ruleSetSize;i++)
        rules[i]=a[i];

    return *this;
}

//I/O operators

// Listing 8.14 Definiton of operators << and >> in FuzzyRuleSet class
istream& operator>>(istream& is, FuzzyRuleSet& a)
{
    is>>a.ruleSetSize;
    if (a.rules !=0)
        delete []a.rules;
    a.rules =new FuzzyRule[a.ruleSetSize];
    is>>a.rules[0];
    vector<int> vecin(a.rules[0].get_variableSize());
    vector<int> vecout(a.rules[0].get_outputSize());
    for (int i=1;i<a.ruleSetSize;i++)
    {
        a.rules[i]=a.rules[0];
        is>>vecin;
        a.rules[i].change_inputSetFlag(vecin);
        is>>vecout;
        a.rules[i].change_outputSetFlag(vecout);
        a.rules[i].form_ruleContent();
    }

    return is;
}

ostream& operator<<(ostream& os, const FuzzyRuleSet& a)
{
    assert(a.ruleSetSize !=0);
    os<<a.ruleSetSize<<endl;
    os<<a[0];

    for (int i=1;i<a.ruleSetSize;i++)
    {
        os<<(a[i].get_inputSetFlag()|a[i].get_outputSetFlag());
    }

    return os;
}



