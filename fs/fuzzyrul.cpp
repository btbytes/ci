#include "headfile.h"
#include "fuzzyrul.h"

//constructors
FuzzyRule::FuzzyRule(int a,int b,vector<int> c,vector<int> d):
variableSize(a),outputSize(b)
{
    if ((c.len()!=variableSize)||(d.len()!=outputSize))
    {
        fprintf(stderr,"input(or output)Vector dimension doesn't equal rule's input(output) dimension");
        exit(1);
    }
    inputSetFlag=c;
    outputSetFlag=d;
    inputVariable= new FuzzyVariable[variableSize];
    assert(inputVariable !=0);
    outputVariable= new FuzzyVariable[outputSize];
    assert(outputVariable !=0);
    ruleContent=new char[256];
    assert(ruleContent !=0);
    strcpy(ruleContent,"rule_not_formed_yet");
}

FuzzyRule::FuzzyRule(int a,int b,vector<int> c,vector<int> d,char* str):
variableSize(a),outputSize(b)
{
    if ((c.len()!=variableSize)||(d.len()!=outputSize))
    {
        fprintf(stderr,"input(or output)Vector dimension doesn't equal rule's input(output) dimension");
        exit(1);
    }
    inputSetFlag=c;
    outputSetFlag=d;
    inputVariable= new FuzzyVariable[variableSize];
    assert(inputVariable !=0);
    outputVariable= new FuzzyVariable[outputSize];
    assert(outputVariable !=0);
    int length=strlen(str)+2;
    ruleContent=new char[length];
    assert(ruleContent !=0);
    strncpy(ruleContent,str,length);
}

FuzzyRule::FuzzyRule(const FuzzyRule& a):
	variableSize(a.variableSize),outputSize(a.outputSize)
{
    inputSetFlag=a.inputSetFlag;
    outputSetFlag=a.outputSetFlag;
    inputVariable= new FuzzyVariable[variableSize];
    assert(inputVariable !=0);
    for (int i=0;i<variableSize;i++)
        inputVariable[i]=a.inputVariable[i];
    outputVariable= new FuzzyVariable[outputSize];
    assert(outputVariable !=0);
    for (i=0;i<outputSize;i++)
        outputVariable[i]=a.outputVariable[i];
    delete []ruleContent;
    int length=strlen(a.ruleContent)+2;
    ruleContent= new char[length];
    assert(ruleContent !=0);
    strncpy(ruleContent,a.ruleContent,length);
}

//member functions
FuzzyRule&
FuzzyRule::change_inputSetFlag(const vector<int>& a)
{
    //check whether have the correct dimension
    if (variableSize !=a.len())
    {
        fprintf(stderr,"assign inputSetFlag with a different dimension");
        exit(1);
    }

    inputSetFlag=a;
    return *this;
}

FuzzyRule&
FuzzyRule::change_outputSetFlag(const vector<int>& a)
{
    //check whether have the correct dimension
    if (outputSize !=a.len())
    {
        fprintf(stderr,"assign outputSetFlag with a different dimension");
        exit(1);
    }

    outputSetFlag=a;
    return *this;
}

FuzzyRule&
FuzzyRule::change_variableSize(const int& a)
{
    variableSize=a;
    return *this;
}

FuzzyRule&
FuzzyRule::change_outputSize(const int& a)
{
    outputSize=a;
    return *this;
}

FuzzyRule&
FuzzyRule::change_ruleContent(char *str)
{
    delete []ruleContent;
    int length=strlen(str)+2;
    ruleContent= new char[length];
    assert(ruleContent !=0);
    strncpy(ruleContent,str,length);
    return *this;
}

FuzzyRule&
FuzzyRule::form_ruleContent()
{
    delete []ruleContent;

    char str[256];
    strcpy(str,"if_");

    int inflag=0,outflag=0;
    int intmp=0,outtmp=0;
    for (int i=0;i<variableSize;i++)
    {
        if (inputSetFlag[i]==0)
	 inflag++;
        else
        {
	 if (intmp>0)
	     strcat(str,"_and_");
	 intmp++;

	 strcat(str,this->inputVariable[i].get_variableName());
	 strcat(str,"_is_");
	 if (inputSetFlag[i]<0)
	     strcat(str,"Not");
	 strcat(str,this->inputVariable[i].setMeaning(this->inputVariable[i].get_setSize(),abs(inputSetFlag[i])-1));


        }
    }

    strcat(str,"_then_");
    for (i=0;i<outputSize;i++)
    {
        if (outputSetFlag[i]==0)
	 outflag++;
        else
        {
	 if (outtmp>0)
	     strcat(str,"_and_");
	 outtmp++;

	 strcat(str,this->outputVariable[i].get_variableName());
	 strcat(str,"_is_");
	 if (outputSetFlag[i]<0)
	     strcat(str,"Not");
	 strcat(str,this->outputVariable[i].setMeaning(this->outputVariable[i].get_setSize(),abs(outputSetFlag[i])-1));

        }
    }

    if ((inflag==variableSize)||(outflag==outputSize))
    {
        ruleContent=new char[60];
        assert(ruleContent !=0);
        strcpy(ruleContent,"This_rule_doesn't_exist");
    }
    else
    {
        int length=strlen(str)+2;
        ruleContent=new char[length];
        assert(ruleContent !=0);
        strncpy(ruleContent,str,length);
    }
    return *this;
}

FuzzyRule&
FuzzyRule::change_outputVariable(const FuzzyVariable& a,const int& b)
{
    assert(b>=0&&b<outputSize);
    outputVariable[b]=a;
    return *this;
}

FuzzyVariable&	//need & ?
FuzzyRule::get_outputVariable(const int&a) const
{
    assert(a>=0&&a<outputSize);
    return outputVariable[a];
}

// Listing 8.8 Implementation of method checkRuleActive() in class FuzzyRule
int
FuzzyRule::checkRuleActive(const vector<float>& a) const
{//check whether this has been activated

    assert(a.len()==variableSize);

    vector<int>* vec;
    vec= new vector<int>[variableSize];

    int sum=0;
    for (int i=0;i<variableSize;i++)
    {
        if (inputSetFlag[i]==0)
	 sum++;
        else
        {
	 vec[i]=inputVariable[i].setFireFlag(a[i]);
	 int ind=abs(inputSetFlag[i])-1;
	 if (vec[i][ind]==1)
	     sum++;
        }
    }

    delete []vec;
    if (sum==variableSize)
        return 1;
    else
        return 0;
}

// Listing 8.9 Implementation of method FuzzyOutput() in class FuzzyRule
vector<float>
FuzzyRule::FuzzyOutput(const vector<float>& a) const
{
    //check the input dimension
    assert(a.len()==variableSize);

    //check whethe the rule is activated
    if (checkRuleActive(a) !=1)
    {
        fprintf(stderr,"try to use unactivated rule\n");
        exit(1);
    }

    float min=1.0,tmp;

    for (int i=0;i<variableSize;i++)
    {
        if (inputSetFlag[i]!=0)
        {
	 tmp=inputVariable[i].output(a[i],inputSetFlag[i]);
            if (min>tmp)
	     min=tmp;	//get the minimum value
        }
    }

    vector<float> tmpout(outputSize);
    for (i=0;i<outputSize;i++)
    {
        if (outputSetFlag[i] ==0)
	 tmpout[i]=0.0;
        else
        {
	 if (outputSetFlag[i]>0)
	     tmpout[i]=min;
	 else
	 {
	     if (min>=0.9999)
		tmpout[i]=0.0001;
	     else
		tmpout[i]=1-min;
	 }
        }
    }

    return tmpout;
}

vector<float>
FuzzyRule::FuzzyOutput_average(const vector<float>& a) const
{
    //check the input dimension
    assert(a.len()==variableSize);

    //check whethe the rule is activated
    if (checkRuleActive(a) !=1)
    {
        fprintf(stderr,"try to use unactivated rule\n");
        exit(1);
    }

    float ave=0.0;
    int tmp=0;
    for (int i=0;i<variableSize;i++)
    {
        if (inputSetFlag[i]!=0)
        {
	 ave +=inputVariable[i].output(a[i],inputSetFlag[i]);
	 tmp++;
        }
    }
    ave =ave/tmp;

    vector<float> tmpout(outputSize);
    for (i=0;i<outputSize;i++)
    {
        if (outputSetFlag[i] ==0)
	 tmpout[i]=0.0;
        else
        {
	 if (outputSetFlag[i]>0)
	     tmpout[i]=ave;
	 else
	 {
	     if (ave>=0.9999)
		tmpout[i]=0.0001;
	     else
		tmpout[i]=1-ave;
	 }
        }
    }

    return tmpout;
}

vector<int>
FuzzyRule::formRange(const int& a) const
{
    int tmp=variableSize+outputSize+1;
    vector<int> range(tmp);
    range[0]=a;	//how many rules
    for (int i=0;i<variableSize;i++)
        range[i+1]=inputVariable[i].get_setSize();
    for (i=0;i<outputSize;i++)
        range[variableSize+i+1]=outputVariable[i].get_setSize();

    return range;
}


//operators
FuzzyVariable&
FuzzyRule::operator [] (int i) const
{
    assert(i>=0&&i<variableSize);
    return inputVariable[i];
}

FuzzyRule&
FuzzyRule::operator =(const FuzzyRule& a)
{
    if ((&a)==this) return *this;

    delete []inputVariable;
    delete []outputVariable;
    delete []ruleContent;
    variableSize=a.variableSize;
    outputSize=a.outputSize;
    inputSetFlag=a.inputSetFlag;
    outputSetFlag=a.outputSetFlag;
    inputVariable=new FuzzyVariable[variableSize];
    assert(inputVariable !=0);
    for (int i=0;i<variableSize;i++)
        inputVariable[i]=a.inputVariable[i];
    outputVariable=new FuzzyVariable[outputSize];
    assert(outputVariable !=0);
    for (i=0;i<outputSize;i++)
        outputVariable[i]=a.outputVariable[i];
    int length=strlen(a.ruleContent)+2;
    ruleContent=new char[length];
    assert(ruleContent !=0);
    strncpy(ruleContent,a.ruleContent,length);

    return *this;
}



//I/O operators
ostream& operator <<(ostream& os,const FuzzyRule& a)
{
    os<<a.variableSize<<"\t";
    os<<a.outputSize<<endl;

    for (int i=0;i<a.variableSize;i++)
        os<<a[i];

    for (i=0;i<a.outputSize;i++)
        os<<a.get_outputVariable(i);

    vector<int> intvec=a.inputSetFlag|a.outputSetFlag;
    os<<intvec;

    return os;
}

istream& operator >>(istream& is, FuzzyRule& a)
{
    int tmpVariableSize, tmpOutputSize;
    is>>tmpVariableSize>>tmpOutputSize;
    vector<int> tmpInputSetFlag(tmpVariableSize);
    vector<int> tmpOutputSetFlag(tmpOutputSize);

    FuzzyRule tmpRule(tmpVariableSize,tmpOutputSize,tmpInputSetFlag,tmpOutputSetFlag);
    for (int i=0;i<tmpVariableSize;i++)
        is>>tmpRule.inputVariable[i];
    for (i=0;i<tmpOutputSize;i++)
        is>>tmpRule.outputVariable[i];

    is>>tmpInputSetFlag>>tmpOutputSetFlag;

    tmpRule.change_inputSetFlag(tmpInputSetFlag);
    tmpRule.change_outputSetFlag(tmpOutputSetFlag);
    tmpRule.form_ruleContent();
    a=tmpRule;

    return is;
}


