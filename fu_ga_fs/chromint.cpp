#include "headfile.h"
#include "chromint.h"

//constructors
IndividualInt::IndividualInt(int a,float b):
length(a),m_rate(b)
{
    ptr=new int[length];
    assert(ptr !=0);
}

IndividualInt::IndividualInt(int a,int* b,float c):
length(a),m_rate(c)
{
    ptr=new int[length];
    assert(ptr !=0);
    for (int i=0;i<length;i++)
        ptr[i]=b[i];
}

IndividualInt::IndividualInt(vector<int> a, float b)
{
    length=a.len();
    m_rate=b;
    ptr =new int[length];
    assert(ptr !=0);
    for (int i=0;i<length;i++)
        ptr[i]=a[i];
}

IndividualInt::IndividualInt(const IndividualInt& a):
length(a.length),m_rate(a.m_rate)
{
    ptr=new int[length];
    assert(ptr !=0);
    for (int i=0;i<length;i++)
        ptr[i]=a.ptr[i];
}

//member functions
IndividualInt&
IndividualInt::change_length(const int& a)
{
    length=a;
    return *this;
}

IndividualInt&
IndividualInt::change_mrate(const float& a)
{
    m_rate=a;
    return *this;
}

IndividualInt&
IndividualInt::initialize(const int& a,const int& b)
{  
    for (int i=0;i<length;i++)
        ptr[i] =random(a+1) + b;
    return *this;
}

IndividualInt&
IndividualInt::initialize_range(const IndividualInt& a)
{
    if (((length-1)%(a.length-1)) !=0)
    {
        cerr<<"in initialize, length of individual is not exact times of length of range individual"<<endl;
        cout<<" length="<<length<<"  a.length="<<a.length<<endl;
        exit(1);
    }
    int mui=(length-1)/(a.length-1);
    ptr[0]=random(a.ptr[0])+1;
    for (int i=0;i<mui;i++)
        for (int j=1;j<a.length;j++)
	 ptr[i*(a.length-1)+j]=random(2*a.ptr[j]+1)-a.ptr[j];
    return *this;
}

IndividualInt&
IndividualInt::initialize_range_RM(const IndividualInt& a)
{
    ptr[0]=random(a.ptr[0])+1;     //rule number
    int tmplen=1;
    for (int i=2;i<a.length;i++)
        tmplen +=2*a[i];
    if (((length-tmplen)%(a.length-2)) !=0)
    {
        cerr<<"in initialize, length of individual is not exact times of length of range individual"<<endl;
        cout<<" length="<<length<<"  a.length="<<a.length<<endl;
        exit(1);
    }
    //initialize  membership part
    for (i=1;i<tmplen;i++)
        ptr[i]=random(a.ptr[1]);
    //initialize  rule part
    int mui=(length-tmplen)/(a.length-2);
    for (i=0;i<mui;i++)
        for (int j=2;j<a.length;j++)
	 ptr[tmplen+i*(a.length-2)+j-2]=random(2*a.ptr[j]+1)-a.ptr[j];
    return *this;
}

IndividualInt&
IndividualInt::initialize_range_RMT(const IndividualInt& a)
{
    ptr[0]=random(a.ptr[0])+1;     //rule number
    int tmplen=1;
    for (int i=3;i<a.length;i++)
        tmplen +=3*a[i];    //each funtion having two points and one type
    if (((length-tmplen)%(a.length-3)) !=0)
    {
        cerr<<"in initialize, length of individual is not exact times of length of range individual"<<endl;
        cout<<" length="<<length<<"  a.length="<<a.length<<endl;
        exit(1);
    }
    //initialize  membership part
    int inde=3;
    int ch=0;
    for (i=1;i<tmplen;i++)
    {
        if ((i%3)==0)
        {
	 ch++;
	 ptr[i]=random(a.ptr[2])+1;	//1 to a.ptr[2]
	 if (ch==1)
	 {
	     if ((ptr[i]==2)||(ptr[i]==4))
		ptr[i]++;
	 }
	 else if ((ch>1)&&(ch<a[inde]))
	 {
	     if (ptr[i]<=3)
		ptr[i]=3;
	     else
		ptr[i]=6;
	 }
	 else
	 {
	     inde++;
	     ch=0;
	     if ((ptr[i]==1)||(ptr[i]==5))
	     	ptr[i]++;
	 }
        }
        else
	 ptr[i]=random(a.ptr[1]);
    }
    //initialize  rule part
    int mui=(length-tmplen)/(a.length-3);
    for (i=0;i<mui;i++)
        for (int j=3;j<a.length;j++)
	 ptr[tmplen+i*(a.length-3)+j-3]=random(2*a.ptr[j]+1)-a.ptr[j];
    return *this;
}


IndividualInt&
IndividualInt::mutate_one(const IndividualInt& a)
{
    //mutate rule number  part
    float prob=(1.0*random(1000))/1000;
    if (prob<m_rate)
    {
        prob=(1.0*random(1000))/1000;
        if (prob>0.5)
        {
	 //add one
	 if (ptr[0]<a[0])
	     ptr[0]++;
	 else
	     ptr[0]=1;
        }
        else
        {
	 //subtract one
	 if (ptr[0]>1)
	     ptr[0]--;
	 else
	     ptr[0]=a[0];
        }
    }
    // mutate rule part
    for (int i=1;i<length;i++)
    {
        prob=(1.0*random(1000))/1000;
        if (prob<m_rate)
        {
	 int tmp=(i-1)%(a.length-1);
	 prob=(1.0*random(1000))/1000;
	 if (prob>0.5)
	 {
	     //add one
	     if (ptr[i]<a[tmp+1])
		ptr[i]++;
	     else
		ptr[i]=-1*a[tmp+1];
	 }
	 else
	 {
	     //subtract one
	     if (ptr[i]>(-1*a[tmp+1]))
		ptr[i]--;
	     else
		ptr[i]=a[tmp+1];
            }
        }
    }
    return *this;
}

IndividualInt&
IndividualInt::mutate_one_RM(const IndividualInt& a)
{
    //mutate rule number  part
    float prob=(1.0*random(1000))/1000;
    if (prob<m_rate)
    {
        prob=(1.0*random(1000))/1000;
        if (prob>0.5)
        {
	 //add one
	 if (ptr[0]<a[0])
	     ptr[0]++;
	 else
	     ptr[0]=1;
        }
        else
        {
	 //subtract one
	 if (ptr[0]>1)
	     ptr[0]--;
	 else
	     ptr[0]=a[0];
        }
    }
    //mutate membership functions part
    int tmplen=1;
    for (int i=2;i<a.length;i++)
        tmplen +=2*a[i];
    for (i=1;i<tmplen;i++)
    {
        prob=(1.0*random(1000))/1000;
        if (prob<m_rate)
        {
	 prob=(1.0*random(1000))/1000;
	 if (prob>0.5)
	 {
	     //add one
	     ptr[i]++;
	     if (ptr[i]>=a[1])
		ptr[i]=0;
	 }
        }
        else
        {
	 //subtract one
	 if (ptr[i]>0)
	     ptr[i]--;
	 else
	     ptr[i]=a[1]-1;
        }
    }
    // mutate rule part
    for (i=tmplen;i<length;i++)
    {
        prob=(1.0*random(1000))/1000;
        if (prob<m_rate)
        {
	 int tmp=(i-tmplen)%(a.length-2);
	 prob=(1.0*random(1000))/1000;
	 if (prob>0.5)
	 {
	     //add one
	     if (ptr[i]<a[tmp+2])
		ptr[i]++;
	     else
		ptr[i]=-1*a[tmp+2];
	 }
	 else
	 {
	     //subtract one
	     if (ptr[i]>(-1*a[tmp+2]))
		ptr[i]--;
	     else
		ptr[i]=a[tmp+2];
            }
        }
    }
    return *this;
}

IndividualInt&
IndividualInt::mutate_one_RMT(const IndividualInt& a)
{
    //mutate rule number  part
    float prob=(1.0*random(1000))/1000;
    if (prob<m_rate)
    {
        prob=(1.0*random(1000))/1000;
        if (prob>0.5)
        {
	 //add one
	 if (ptr[0]<a[0])
	     ptr[0]++;
	 else
	     ptr[0]=1;
        }
        else
        {
	 //subtract one
	 if (ptr[0]>1)
	     ptr[0]--;
	 else
	     ptr[0]=a[0];
        }
    }
    //mutate membership functions part
    int tmplen=1;
    for (int i=3;i<a.length;i++)
        tmplen +=3*a[i];
    int ch=0;
    int inde=3;
    for (i=1;i<tmplen;i++)
    {
        prob=(1.0*random(1000))/1000;
        if (prob<m_rate)
        {
	 prob=(1.0*random(1000))/1000;
	 if (prob>0.5)
	 {
	     //add one
	     ptr[i]++;
                if ((i%3)==0)  //mutate type
	     {
		ch++;
		if (ch==1)
		{
		    if (ptr[i]>a[2])
		        ptr[i]=1;
		    if ((ptr[i]==2)||(ptr[i]==4))
		        ptr[i]++;
		}
		else if ((ch>1)&&(ch<a[inde]))
		{
		    if (ptr[i]>a[2])
		        ptr[i]=3;
		    if ((ptr[i]==1)||(ptr[i]==2))
		        ptr[i]=3;
		    if ((ptr[i]==4)||(ptr[i]==5))
		        ptr[i]=6;
		}
		else if (ch==a[inde])
		{
		    inde++;
		    ch=0;
		    if (ptr[i]>a[2])
		        ptr[i]=2;
		    if ((ptr[i]==1)||(ptr[i]==5))
		        ptr[i]++;
		}
                }
	     else      //tune membership function
	     {
		if (ptr[i]>=a[1])
		    ptr[i]=0;
	     }
	 }
	 else
	 {
	     //subtract one
	     ptr[i]--;
                if ((i%3)==0)  //mutate type
	     {
		ch++;
		if (ch==1)
		{
		    if (ptr[i]<1)
		        ptr[i]=a[2];
		    if ((ptr[i]==2)||(ptr[i]==4))
		        ptr[i]--;
		}
		else if ((ch>1)&&(ch<a[inde]))
		{
		    if (ptr[i]<1)
		        ptr[i]=6;
		    if ((ptr[i]==1)||(ptr[i]==2))
		        ptr[i]=6;
		    if ((ptr[i]==4)||(ptr[i]==5))
		        ptr[i]=3;
		}
		else if (ch==a[inde])
		{
		    inde++;
		    ch=0;
                        if ((ptr[i]==1)||(ptr[i]==5))
		        ptr[i]--;
		    if (ptr[i]<1)
		        ptr[i]=6;
		}
                }
	     else           //tune membership function
	     {
		if (ptr[i]<0)
		    ptr[i]=a[1]-1;
	     }
	 }
        }
        else
        {
	 if ((i%3)==0)
	 {
	     ch++;
	     if (ch==a[inde])
	     {
		ch=0;
		inde++;
	     }
	 }
        }
    }
    // mutate rule part
    for (i=tmplen;i<length;i++)
    {
        prob=(1.0*random(1000))/1000;
        if (prob<m_rate)
        {
	 int tmp=(i-tmplen)%(a.length-3);
	 prob=(1.0*random(1000))/1000;
	 if (prob>0.5)
	 {
	     //add one
	     if (ptr[i]<a[tmp+3])
		ptr[i]++;
	     else
		ptr[i]=-1*a[tmp+3];
	 }
	 else
	 {
	     //subtract one
	     if (ptr[i]>(-1*a[tmp+3]))
		ptr[i]--;
	     else
		ptr[i]=a[tmp+3];
            }
        }
    }
    return *this;
}

void crossoverOP(IndividualInt& a,IndividualInt& b)
{
    if (a.length != b.length)
    {
        cerr<<"try to crossover two different size of individuals with OP"<<endl;
        exit(1);
    }
    time_t  t;
    srand((unsigned) time(&t));

    int Fir=random(a.length);
    /* crossover through exchanging information */
    for (int i=Fir; i<a.length;i++)
    {
        int Tmp=a[i];
        a.ptr[i]=b.ptr[i];
        b.ptr[i]=Tmp;
    }/* end for */
}

void crossoverTP(IndividualInt& a,IndividualInt& b)
{
    if (a.length != b.length)
    {
        cerr<<"try to crossover two different size of individuals with OP"<<endl;
        exit(1);
    }
    time_t  t;
    srand((unsigned) time(&t));
    int Fir=random(a.length);	/* choose the first crossover point */

    int Sec=random(a.length);	/* choose the second crossover point */
    Sec=(2*Sec)%a.length;
    	  /* sort crossover points */
    int tmp;
    if (Fir>=Sec)
    {
        tmp=Fir;
        Fir=Sec;
        Sec=tmp;
    }
    /* crossover through exchanging information */
    for (int i=Fir; i<Sec;i++)
    {
        int Tmp=a.ptr[i];
        a.ptr[i]=b.ptr[i];
        b.ptr[i]=Tmp;
    }/* end for */
}

void crossoverUniform(IndividualInt& a,IndividualInt& b)
{
    if (a.length != b.length)
    {
        cerr<<"try to crossover two different size of individuals with OP"<<endl;
        exit(1);
    }
    time_t  t;
    srand((unsigned) time(&t));
    for (int i=0; i<a.length;i++)
    {
        float prob=(1.0*random(1000))/1000;
        if (prob>0.5)
        {
	 int Tmp=a.ptr[i];
	 a.ptr[i]=b.ptr[i];
	 b.ptr[i]=Tmp;
        }
    }/* end for */
}

FuzzyRuleSet
IndividualInt::formRuleSet(const FuzzyRule& a) const
{
    int tmp=a.get_variableSize()+a.get_outputSize();
    if (((length-1)%tmp)!=0)
    {
        cerr<<"in chromint.cpp length of individual is not exact times of length";
        cout<<"  of range individual in formRuleSet"<<endl;
        cerr<<"length="<<length<<"   tmp="<<tmp<<endl;
        exit(1);
    }
    FuzzyRuleSet ruleSet;
    vector<int> invec(a.get_variableSize());
    vector<int> outvec(a.get_outputSize());
    //get zero input and output vector
    vector<int> inzero(a.get_variableSize());
    vector<int> outzero(a.get_outputSize());
    for (int i=0;i<a.get_variableSize();i++)
        inzero[i]=0;
    for (i=0;i<a.get_outputSize();i++)
        outzero[i]=0;
    FuzzyRule tmprule;
    //add feasible rules
    for (i=0;i<ptr[0];i++) //ptr[0] store the maximum rule no.
    {
        for (int j=0;j<a.get_variableSize();j++)
	 invec[j]=ptr[i*tmp+j+1];
        for (j=0;j<a.get_outputSize();j++)
	 outvec[j]=ptr[i*tmp+a.get_variableSize()+j+1];
        if ((invec !=inzero)&&(outvec !=outzero))
        {
	 tmprule=a;
	 tmprule.change_inputSetFlag(invec);
	 tmprule.change_outputSetFlag(outvec);
	 tmprule.form_ruleContent();
	 ruleSet.addRule(tmprule);
        }
    }
    return ruleSet;
}

FuzzyRuleSet
IndividualInt::formRuleSet_RM(const FuzzyRule& a,const IndividualInt& b) const
{
    int tmpin=a.get_variableSize();
    int tmpout=a.get_outputSize();
    int tmp=tmpin+tmpout;
    int tmplen=1;
    for (int i=2;i<b.length;i++)
        tmplen +=2*b[i];       //length for tuning membership functions
    if ((((length-tmplen)%(b.length-2)) !=0)||(tmp!=(b.length-2)))
    {
        cerr<<"in formRuleSet_RM, chromint.cpp, length of individual is not exact times of length of range individual"<<endl;
        cerr<<"length="<<length<<"   tmp="<<tmp<<endl;
        exit(1);
    }
    FuzzyRuleSet ruleSet;
    FuzzyRule tmpruleA;
    //tuning membership function of rule a
    int iter=0;
    tmpruleA=a;
    //input variables
    for (i=0;i<tmpin;i++)
    {
        float var_startPoint=a[i].get_startPoint();
        float var_endPoint=a[i].get_endPoint();
        float var_length=var_endPoint-var_startPoint;
        float stepSize=var_length/(b[i+2]+1);
        float set_startPoint;
        float set_endPoint;
        for (int j=0;j<b[i+2];j++) //variable i has b[i+2] fuzzy sets
        {
	 //get the start point
	 iter++;
	 if (j==0)
	     set_startPoint=(j+1)*stepSize - (((b[1]+ptr[iter])*stepSize)/(2.0*b[1]))+var_startPoint;
	 else
	     set_startPoint=(j+1)*stepSize - (((ptr[iter]+b[1]/2)*stepSize)/b[1])+var_startPoint;
            //get the end point
	 iter++;
	 if (j==(b[i+2]-1))
	     set_endPoint=(j+1)*stepSize + (((b[1]+ptr[iter])*stepSize)/(2.0*b[1])) +var_startPoint;
	 else
	     set_endPoint=(j+1)*stepSize + (((ptr[iter]+b[1]/2)*stepSize)/b[1]) +var_startPoint;
            //change startPoint and endPoint
	 assert((set_endPoint-set_startPoint)>0);
	 tmpruleA[i][j].change_startPoint(set_startPoint);
	 tmpruleA[i][j].change_endPoint(set_endPoint);
        }
    }
    //output variable
    for (i=0;i<tmpout;i++)
    {
        float var_startPoint=a.get_outputVariable(i).get_startPoint();
        float var_endPoint=a.get_outputVariable(i).get_endPoint();
        float var_length=var_endPoint-var_startPoint;
        float stepSize=var_length/(b[i+2+tmpin]+1);
        for (int j=0;j<b[i+2+tmpin];j++) //variable i has b[i+2+tmpin] fuzzy sets
        {
	 //get the start point
	 iter++;
	 float set_startPoint=(j+1)*stepSize - (((1+ptr[iter])*stepSize)/b[1])+var_startPoint;
	 //get the end point
	 iter++;
	 float set_endPoint=(j+1)*stepSize + (((1+ptr[iter])*stepSize)/b[1])+var_startPoint;
	 	//change startPoint and endPoint
            assert((set_endPoint-set_startPoint)>0);
	 tmpruleA.get_outputVariable(i)[j].change_startPoint(set_startPoint);
	 tmpruleA.get_outputVariable(i)[j].change_endPoint(set_endPoint);
        }
    }
    //get rule set
    vector<int> invec(tmpin);
    vector<int> outvec(tmpout);
    //get zero input and output vector
    vector<int> inzero(tmpin);
    vector<int> outzero(tmpout);
    for (i=0;i<tmpin;i++)
        inzero[i]=0;
    for (i=0;i<tmpout;i++)
        outzero[i]=0;
    FuzzyRule tmprule;
    //add feasible rules
    for (i=0;i<ptr[0];i++) //ptr[0] store the maximum rule no.
    {
        for (int j=0;j<tmpin;j++)
	 invec[j]=ptr[i*tmp+j+iter+1];
        for (j=0;j<tmpout;j++)
	 outvec[j]=ptr[i*tmp+tmpin+j+iter+1];
        if ((invec !=inzero)&&(outvec !=outzero))
        {
	 tmprule=tmpruleA;
	 tmprule.change_inputSetFlag(invec);
	 tmprule.change_outputSetFlag(outvec);
	 tmprule.form_ruleContent();
	 ruleSet.addRule(tmprule);
        }
    }
    return ruleSet;
}

FuzzyRuleSet
IndividualInt::formRuleSet_RMT(const FuzzyRule& a,const IndividualInt& b) const
{
    int tmpin=a.get_variableSize();
    int tmpout=a.get_outputSize();
    int tmp=tmpin+tmpout;
    int tmplen=1;
    for (int i=3;i<b.length;i++)
        tmplen +=3*b[i];       //length for tuning membership functions ans type
    if ((((length-tmplen)%(b.length-3)) !=0)||(tmp!=(b.length-3)))
    {
        cerr<<"in formRuleSet_RMT, chromint.cpp, length of individual is not exact times of length of range individual"<<endl;
        cerr<<"length="<<length<<"   tmp="<<tmp<<endl;
        exit(1);
    }
    FuzzyRuleSet ruleSet;
    FuzzyRule tmpruleA;
    //tuning membership function of rule a
    int iter=0;
    tmpruleA=a;
    	//input variables
    for (i=0;i<tmpin;i++)
    {
        float var_startPoint=a[i].get_startPoint();
        float var_endPoint=a[i].get_endPoint();
        float var_length=var_endPoint-var_startPoint;
        float stepSize=var_length/(b[i+3]+1);
        float set_startPoint;
        float set_endPoint;
        for (int j=0;j<b[i+3];j++) //variable i has b[i+3] fuzzy sets
        {
	 //get the start point
	 iter++;
	 if (j==0)
	     set_startPoint=(j+1)*stepSize - (((b[1]+ptr[iter])*stepSize)/(2.0*b[1]))+var_startPoint;
	 else
	     set_startPoint=(j+1)*stepSize - (((ptr[iter]+b[1]/2)*stepSize)/b[1])+var_startPoint;
            //get the end point
	 iter++;
	 if (j==(b[i+3]-1))
	     set_endPoint=(j+1)*stepSize + (((b[1]+ptr[iter])*stepSize)/(2.0*b[1])) +var_startPoint;
	 else
	     set_endPoint=(j+1)*stepSize + (((ptr[iter]+b[1]/2)*stepSize)/b[1]) +var_startPoint;
            //get function type
	 iter++;
	 int typeF=ptr[iter];
	 //change startPoint and endPoint
	 assert((set_endPoint-set_startPoint)>0);
	 tmpruleA[i][j].change_startPoint(set_startPoint);
	 tmpruleA[i][j].change_endPoint(set_endPoint);
	 tmpruleA[i][j].change_functionType(typeF);
        }
    }
    //output variable
    for (i=0;i<tmpout;i++)
    {
        float var_startPoint=a.get_outputVariable(i).get_startPoint();
        float var_endPoint=a.get_outputVariable(i).get_endPoint();
        float var_length=var_endPoint-var_startPoint;
        float stepSize=var_length/(b[i+3+tmpin]+1);
        for (int j=0;j<b[i+3+tmpin];j++) //variable i has b[i+3+tmpin] fuzzy sets
        {
	 //get the start point
	 iter++;
	 float set_startPoint=(j+1)*stepSize - (((1+ptr[iter])*stepSize)/b[1])+var_startPoint;
	 //get the end point
	 iter++;
	 float set_endPoint=(j+1)*stepSize + (((1+ptr[iter])*stepSize)/b[1])+var_startPoint;
            //get function type
	 iter++;
	 int typeF=ptr[iter];
	 //change startPoint and endPoint
	 assert((set_endPoint-set_startPoint)>0);
	 tmpruleA.get_outputVariable(i)[j].change_startPoint(set_startPoint);
	 tmpruleA.get_outputVariable(i)[j].change_endPoint(set_endPoint);
	 tmpruleA.get_outputVariable(i)[j].change_functionType(typeF);
        }
    }
    //get rule set
    vector<int> invec(tmpin);
    vector<int> outvec(tmpout);
    //get zero input and output vector
    vector<int> inzero(tmpin);
    vector<int> outzero(tmpout);
    for (i=0;i<tmpin;i++)
        inzero[i]=0;
    for (i=0;i<tmpout;i++)
        outzero[i]=0;
    FuzzyRule tmprule;
    //add feasible rules
    for (i=0;i<ptr[0];i++) //ptr[0] store the maximum rule no.
    {
        for (int j=0;j<tmpin;j++)
	 invec[j]=ptr[i*tmp+j+iter+1];
        for (j=0;j<tmpout;j++)
	 outvec[j]=ptr[i*tmp+tmpin+j+iter+1];
        if ((invec !=inzero)&&(outvec !=outzero))
        {
	 tmprule=tmpruleA;
	 tmprule.change_inputSetFlag(invec);
	 tmprule.change_outputSetFlag(outvec);
	 tmprule.form_ruleContent();
	 ruleSet.addRule(tmprule);
        }
    }
    return ruleSet;
}

float
IndividualInt::fitness(const FuzzyRule& a, const array& b,const vector<int>& cn,const int&c, const int& d,const int& e) const
{
    int outVarDim=a.get_outputSize();
    int inVarDim=a.get_variableSize();
    int tmpint=inVarDim+outVarDim;
    int arrayLen=b.len();
    if (tmpint !=b.wid())
    {
        cerr<<"dim of input is not exact number of variables in fitness"<<endl;
        exit(1);
    }
    //get rule set
    FuzzyRuleSet ruleSet=this->formRuleSet(a);
    if (ruleSet.get_ruleSetSize()==0) return 0;
    vector<float> invec(inVarDim);
    vector<int> outvec(outVarDim);
    //begin classify
    int misclassify=0;
    vector<int> cla(outVarDim);
    vector<float> tmp(outVarDim);
    for (int i=0;i<arrayLen;i++)
    {
        //get input vector for the ith row
        for (int j=0;j<inVarDim;j++)
	 invec[j]=b[i][j];
        for (j=0;j<outVarDim;j++)
	 outvec[j]=(int)(b[i][inVarDim+j]+0.001);
        if (ruleSet.checkRuleSetFired(invec)==1)
        {
	 tmp=ruleSet.output(invec,c,d,e); //ruleEffectFlag, fuzzyFlag, defuzzyFlag
            //get output class
	 for (j=0;j<outVarDim;j++)
	     cla[j]=(int)(tmp[j]*cn[j]);
	 //output data dim equal to outputVariable dim
	 if (cla !=outvec)
	     misclassify++;
        }
        else
	 misclassify++;
    }
    float result=(1.0*(arrayLen-misclassify))/b.len();
    return result ;
}

float
IndividualInt::fitness_RM(const FuzzyRule& a, const array& b,const vector<int>& cn,const int&c, const int& d,const int& e,const IndividualInt& f) const
{
    int outVarDim=a.get_outputSize();
    int inVarDim=a.get_variableSize();
    int tmpint=inVarDim+outVarDim;
    int arrayLen=b.len();
    if (tmpint !=b.wid())
    {
        cerr<<"dimension of iuput is not exact number of variables in fitness_RM"<<endl;
        exit(1);
    }
    //get rule set
    FuzzyRuleSet ruleSet=this->formRuleSet_RM(a,f);
    if (ruleSet.get_ruleSetSize()==0)
        return 0;
    vector<float> invec(inVarDim);
    vector<int> outvec(outVarDim);
    //begin classify
    int misclassify=0;
    vector<int> cla(outVarDim);
    vector<float> tmp(outVarDim);
    for (int i=0;i<arrayLen;i++)
    {
        //get input vector for the ith row
        for (int j=0;j<inVarDim;j++)
	 invec[j]=b[i][j];
        for (j=0;j<outVarDim;j++)
	 outvec[j]=(int)(b[i][inVarDim+j]+0.001);
        if (ruleSet.checkRuleSetFired(invec)==1)
        {
	 tmp=ruleSet.output(invec,c,d,e);
	 //get output class
	 for (j=0;j<outVarDim;j++)
	      cla[j]=(int)(tmp[j]*cn[j]);
	 //output data dim equal to outputVariable dim
	 if (cla !=outvec)
	     misclassify++;
        }
        else
	 misclassify++;
    }
    float result=(1.0*(arrayLen-misclassify))/b.len();
    return result ;
}

float
IndividualInt::fitness_RMT(const FuzzyRule& a, const array& b,const vector<int>& cn,const int&c, const int& d,const int& e,const IndividualInt& f) const
{
    int outVarDim=a.get_outputSize();
    int inVarDim=a.get_variableSize();
    int tmpint=inVarDim+outVarDim;
    int arrayLen=b.len();
    if (tmpint !=b.wid())
    {
        cerr<<"input dimension is not exact number of variables in fitness_RMT"<<endl;
        exit(1);
    }
    //get rule set
    FuzzyRuleSet ruleSet=this->formRuleSet_RMT(a,f);
    if (ruleSet.get_ruleSetSize()==0)
        return 0;
    vector<float> invec(inVarDim);
    vector<int> outvec(outVarDim);
    //begin classify
    int misclassify=0;
    vector<int> cla(outVarDim);
    vector<float> tmp(outVarDim);
    for (int i=0;i<arrayLen;i++)
    {
        //get input vector for the ith row
        for (int j=0;j<inVarDim;j++)
	 invec[j]=b[i][j];
        for (j=0;j<outVarDim;j++)
	 outvec[j]=(int)(b[i][inVarDim+j]+0.001);
        if (ruleSet.checkRuleSetFired(invec)==1)
        {
	 tmp=ruleSet.output(invec,c,d,e);
            //get output class
	 for (j=0;j<outVarDim;j++)
	     cla[j]=(int)(tmp[j]*cn[j]);
	 //output data dim equal to outputVariable dim
	 if (cla !=outvec)
	     misclassify++;
        }
        else
	 misclassify++;
    }
    float result=(1.0*(arrayLen-misclassify))/b.len();
    return result ;
}

//operators

int&
IndividualInt::operator [] (int i) const
{
    assert(i>=0&&i<length);
    return ptr[i];
}

IndividualInt&
IndividualInt::operator =(const IndividualInt& a)
{
    if ((&a)==this) return *this;
    delete []ptr;
    length=a.length;
    m_rate=a.m_rate;
    ptr=new int[length];
    assert(ptr !=0);
    for (int i=0;i<length;i++)
        ptr[i]=a.ptr[i];
    return *this;
}

//I/O operators
ostream& operator <<(ostream& os, const IndividualInt& a)
{
    os<<a.length<<"\t"<<a.m_rate<<endl;
    os<<"\t";
    int sum=0;
    for (int i=0;i<a.length;i++)
    {
        os<<a[i]<<"\t";
        sum++;
        if ((sum%8)==0)
        {
	 os<<endl;
	 os<<"\t";
	 sum=0;
        }
    }
    os<<endl;
    return os;
}

istream& operator >>(istream& is,IndividualInt& a)
{
    is>>a.length>>a.m_rate;
    if (a.ptr !=0)
        delete []a.ptr;
    a.ptr=new int[a.length];
    assert(a.ptr !=0);
    for (int i=0;i<a.length;i++)
        is>>a.ptr[i];
    return is;
}



