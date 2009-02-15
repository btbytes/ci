#include "headfile.h"

#include "memberf.h"
#include "variablf.h"
#include "vector.h"

//constructors 
FuzzyVariable::FuzzyVariable(int a,float b,float c):
setSize(a),startPoint(b),endPoint(c)
{
    fuzzySet= new FuzzyMember[setSize];
    assert(fuzzySet !=0);
    char *string="default";
    int length=strlen(string)+2;
    variableName= new char[length];
    assert(variableName !=0);
    strncpy(variableName,string,length);
}

FuzzyVariable::FuzzyVariable(int a,float b,float c,char *str):
setSize(a),startPoint(b),endPoint(c)
{
    int length=strlen(str)+2;
    variableName= new char[length];
    assert(variableName !=0);
    strncpy(variableName,str,length);
    fuzzySet= new FuzzyMember[setSize];
    assert(fuzzySet !=0);
}

FuzzyVariable::FuzzyVariable(const FuzzyVariable& a):
setSize(a.setSize),startPoint(a.startPoint),endPoint(a.endPoint)
{
    fuzzySet= new FuzzyMember[setSize];
    assert(fuzzySet !=0);
    for (int i=0;i<setSize;i++)
        fuzzySet[i]=a.fuzzySet[i];
    int length=strlen(a.variableName)+2;
    variableName= new char[length];
    assert(variableName !=0);
    strncpy(variableName,a.variableName,length);
}

//member functions
FuzzyVariable&
FuzzyVariable::standardVariable()
{
    float stepSize=(endPoint-startPoint)/(2*setSize);
    delete []fuzzySet;
    fuzzySet=new FuzzyMember[setSize];
    assert(fuzzySet !=0);
    FuzzyMember first(startPoint,startPoint+2*stepSize,"leftTriangle");
    //most left rule
    fuzzySet[0]=first;
    //most right rule
    first.change_member(endPoint-2*stepSize,endPoint,"rightTriangle");
    fuzzySet[setSize-1]=first;
    //rules inside
    for (int i=1;i<(setSize-1);i++)
    {
        	first.change_member(startPoint+(2*i-1)*stepSize,startPoint+(2*i+3)*stepSize,"Triangle");
        	fuzzySet[i]=first;
    }
    return *this;
}

FuzzyVariable&
FuzzyVariable::change_setSize(const int& a)
{
    setSize=a;
    return *this;
}

FuzzyVariable&
FuzzyVariable::change_startPoint(const float& a)
{
    startPoint=a;
    return *this;
}

FuzzyVariable&
FuzzyVariable::change_endPoint(const float& a)
{
    endPoint=a;
    return *this;
}

FuzzyVariable&
FuzzyVariable::change_variableName(char *str)
{
    delete []variableName;
    int length=strlen(str)+2;
    variableName=new char[length];
    strncpy(variableName,str,length);
    return *this;
}

char*
FuzzyVariable::setMeaning(const int& a,const int& b) const
{
    if (a!=setSize)
    {
        fprintf(stderr,"wrong setSize in member setMenaing");
        exit(1);
    }

    char str[32];
    if (a==3) //three sets for the variable
    {
        switch(b)
        {
	 case 0:
	     strcpy(str,"Low");
	     break;
	 case 1:
	     strcpy(str,"Medium");
	     break;
	 default:
	     strcpy(str,"High");
        }
    }
    else if (a==5)
    {
        switch(b)
        {
	 case 0:
	     strcpy(str,"veryLow");
	     break;
	 case 1:
	     strcpy(str,"Low");
	     break;
	 case 2:
	     strcpy(str,"Medium");
	     break;
	 case 3:
	     strcpy(str,"High");
	     break;
	 default:
	     strcpy(str,"veryHigh");
        }
    }
    else if (a==7)
    {
        switch(b)
        {
	 case 0:
	     strcpy(str,"veryLow");
	     break;
	 case 1:
	     strcpy(str,"Low");
	     break;
	 case 2:
	     strcpy(str,"littleLow");
	     break;
	 case 3:
	     strcpy(str,"Medium");
	     break;
	 case 4:
	     strcpy(str,"littleHigh");
	     break;
	 case 5:
	     strcpy(str,"High");
	     break;
	 default:
	     strcpy(str,"veryHigh");
        }
    }
    else if (a==9)
    {
        switch(b)
        {
	 case 0:
	     strcpy(str,"extremeLow");
	     break;
	 case 1:
	     strcpy(str,"veryLow");
	     break;
	 case 2:
	     strcpy(str,"Low");
	     break;
	 case 3:
	     strcpy(str,"littleLow");
	     break;
	 case 4:
	     strcpy(str,"Medium");
	     break;
	 case 5:
	     strcpy(str,"littleHigh");
	     break;
	 case    6:
	     strcpy(str,"High");
	     break;
	 case    7:
	     strcpy(str,"veryHigh");
	     break;
	 default:
	     strcpy(str,"extremeHigh");
        }
    }
    else
    {
        if (b>=10)
        {
	 int temdec=b/10;
	 temdec=temdec+48;
	 strcpy(str,(char*)&temdec);
	 int tmp=b%10 +48;
	 strcat(str,(char*)&tmp);
        }
        else
        {
	 int tmp=b+48;
	 strcpy(str,(char*)&tmp);
        }
        strcat(str,"th");
        strcat(str,"Part");
    }
    char* tmp=str;
    return tmp;
}

vector<int>
FuzzyVariable::setFireFlag(const float& a) const
 {
    vector<int> newVec(setSize);
    for (int i=0;i<setSize;i++)
        newVec[i]=fuzzySet[i].member_flag(a);
    return newVec;
}

float
FuzzyVariable::output(const float& a, const int& b) const
{
    assert(b !=0);
    vector<int> vec=this->setFireFlag(a);
    if (vec[abs(b)-1]==0)
    {
        fprintf(stderr,"try to fire a unfired set");
        exit(1);
    }
    float tmp;
    if (b>0)
        tmp=this->fuzzySet[b-1].memberFunction(a);
    else
        tmp=this->fuzzySet[abs(b)-1].not(a);
    return tmp;
}

float
FuzzyVariable::output_max(const float& a) const  //return maximum membership value when input a
{
    vector<int> vecint(setSize);  //store setFireFlag
    vector<float> vecfloat(setSize); //store membership value
    vecint=this->setFireFlag(a);   //get setFireFlag
    for (int j=0;j<setSize;j++)
    {
        if (vecint[j]==1)   //this set fired?
        {
	 vecfloat[j]=fuzzySet[j].memberFunction(a);
	 if (vecfloat[j]==0.0)
	     cout<<"output =0 in variablf.cpp, 281"<<endl; //test only
        }
        else
	 vecfloat[j]=0.0;  //membership value
    }
    int ind=vecfloat.maximum_index();   //maximum index
    return vecfloat[ind];
}

float
FuzzyVariable::defuzzifyMax(const int& a,const vector<float>& b) const
{
    assert((a==setSize)&&(a==b.len()));
    //check whther b is zero vector
    int sum=0;
    for (int i=0;i<b.len();i++)
    {
        if (b[i]>0)
	 sum++;
    }
    if (sum==0)
    {
        cerr<<"try to defuzzy while no rule fired in defuzzifyMax in variablf.cpp"<<endl;
        exit(1);
    }
    int ind=b.maximum_index();
    float first=fuzzySet[ind].get_startPoint();
    float second=fuzzySet[ind].get_endPoint();
    return (first+(second-first)/2);
}

int
FuzzyVariable::defuzzyMax_index(const int&a, const vector<float>& b) const
{
    assert((a==setSize)&&(a==b.len()));
    int sum=0;
    for (int i=0;i<b.len();i++)
    {
        if (b[i]>0)
	 sum++;
    }
    if (sum==0)
    {
        cerr<<"try to defuzzy while no rule fired in defuzzyMax_index in variablf.cpp"<<endl;
        exit(1);
    }
    return b.maximum_index();
}

float
FuzzyVariable::defuzzyCentroid_add(const int& a,const vector<float>& b) const
{
    assert((a==setSize)&&(a==b.len()));
    //check whether b is zero vector
    int sum=0;
    for (int i=0;i<b.len();i++)
    {
        if (b[i]>0)
	 sum++;
    }
    if (sum==0)
    {
        cerr<<"try to defuzzy while no rule fired in defuzzyCentroid_add in variablf.cpp"<<endl;
        exit(1);
    }
    float stepsize=(endPoint-startPoint)/100;  //divide into 100 intervals
    vector<float> res(2);   //first:nom  second: dem
    res[0]=0;
    res[1]=0;
    for (i=0;i<setSize;i++)
    {
        if (b[i] !=0.0)
	 res +=fuzzySet[i].centroid(b[i],stepsize);
    }
    //maybe should add the left and right shoulder's affect
    int totalstep;
    if (b[0] !=0)
    {
        if ((strncmp(fuzzySet[0].get_functionType(),"leftTriangle",strlen("leftTriangle")-1)==1)||(strncmp(fuzzySet[0].get_functionType(),"reverseSigmoid",strlen("reverseSigmoid")-1)==1))
        {
	 totalstep=(fuzzySet[0].get_startPoint()-startPoint)/stepsize;
	 assert(totalstep>=0);
	 for (i=0;i<totalstep;i++)
	 {
	     res[0] +=(startPoint+i*stepsize)*b[0];
	     res[1] +=b[0];
	 }
        }
    }
    if (b[setSize-1] !=0)
    {
        if ((strncmp(fuzzySet[setSize-1].get_functionType(),"rightTriangle",strlen("rightTriangle")-1)==1)||(strncmp(fuzzySet[setSize-1].get_functionType(),"Sigmoid",strlen("Sigmoid")-1)==1))
        {
	 float tmp=fuzzySet[setSize-1].get_endPoint();
	 totalstep=(endPoint-tmp)/stepsize;
	 assert(totalstep>=0);
	 for (i=0;i<totalstep;i++)
	 {
	     res[0] +=(tmp+i*stepsize)*b[setSize-1];
	     res[1] +=b[setSize-1];
	 }
        }
    }
    return res[0]/res[1];
}

float
FuzzyVariable::defuzzyCentroid(const int& a,const vector<float>& b) const
{
    assert((a==setSize)&&(a==b.len()));
    //check whether b is zero vector
    int sum=0;
    for (int i=0;i<b.len();i++)
    {
        if (b[i]>0)
	 sum++;
    }
    if (sum==0)
    {
        cerr<<"413:try to defuzzy while no rule fired in defuzzyCentroid in variablf.cpp"<<endl;
        exit(1);
    }
    float stepsize=(endPoint-startPoint)/100;  //divide into 100 intervals
    vector<int> vecint(setSize);  //store setFireFlag
    vector<float> vecfloat(setSize); //store membership value
    float tmp,num,dem;  //tmp: set value, num &dem for centroid
    num=0.0;
    dem=0.0;
    for (i=0;i<100;i++)
    {
        tmp=startPoint+i*stepsize;
        vecint=this->setFireFlag(tmp);   //get setFireFlag
        for (int j=0;j<setSize;j++)
        {
	 vecfloat[j]=0;
	 if (vecint[j]==1)   //this set fired?
	 {
	     vecfloat[j]=fuzzySet[j].memberFunction(tmp);  //membership value
	     if (vecfloat[j]>b[j]) //compare two membership values
		vecfloat[j]=b[j];  //minimum
	 }
        }
        int ind=vecfloat.maximum_index();   //maximum index
        num +=tmp*(vecfloat[ind]);
        dem +=1.0*(vecfloat[ind]);
    }
    return num/dem;   //centroid
}


//operators 
FuzzyMember&
FuzzyVariable::operator [] (int i) const
{
    assert(i>=0&&i<setSize);
    return fuzzySet[i];
}

FuzzyVariable&
FuzzyVariable::operator =(const FuzzyVariable& a)
{
    if ((&a)==this) return *this;
    delete []fuzzySet;
    delete []variableName;
    setSize=a.setSize;
    startPoint=a.startPoint;
    endPoint=a.endPoint;
    fuzzySet=new FuzzyMember[setSize];
    assert(fuzzySet !=0);
    for (int i=0;i<setSize;i++)
        fuzzySet[i]=a.fuzzySet[i];
    int length=strlen(a.variableName)+2;
    variableName =new char[length];
    assert(variableName !=0);
    strncpy(variableName,a.variableName,length);
    return *this;
}

//friend I/O
ostream& operator << (ostream& os, const FuzzyVariable& a)
{
    os<<a.variableName<<"\t";
    os<<a.setSize<<"\t";
    os<<a.startPoint<<"\t";
    os<<a.endPoint<<endl;
    for (int i=0;i<a.setSize;i++)
        os<<a.fuzzySet[i];
    return os;
}

istream& operator >>(istream& is,  FuzzyVariable& a)
{
    char str[256];
    int  tmpsetSize;
    float tmpstart,tmpend;
    is>>str>>tmpsetSize>>tmpstart>>tmpend;
    FuzzyVariable tmpVariable(tmpsetSize,tmpstart,tmpend,str);
    for (int i=0;i<tmpsetSize;i++)
        is>>tmpVariable.fuzzySet[i];
    a=tmpVariable;
    return is;
}




