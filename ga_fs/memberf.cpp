#include "headfile.h"

#include "memberf.h"

float LeftTriangle(float,float,float);
float RightTriangle(float,float,float);
float Triangle(float,float,float);
float Sigmoid(float,float,float);
float reverseSigmoid(float,float,float);
float Gaussian(float,float,float);

vector<float> antiLeftTriangle(float,float,float);
vector<float> antiRightTriangle(float,float,float);
vector<float> antiTriangle(float,float,float);
vector<float> antiSigmoid(float,float,float);
vector<float> antiReverseSigmoid(float,float,float);
vector<float> antiGaussian(float,float,float);

//operators 
FuzzyMember&
FuzzyMember::operator =(const FuzzyMember& a)
{
    if ((&a)==this) return *this;
    delete []functionType;
    startPoint=a.startPoint;
    endPoint=a.endPoint;
    int length=strlen(a.functionType)+2;
    functionType= new char[length];
    assert(functionType !=0);
    strncpy(functionType,a.functionType,length);
    return *this;
}

int
FuzzyMember::operator ==(const FuzzyMember& a) const
{
    int tmp=1;
    if ((&a)==this) return 1;
    MyString str1(functionType);
    MyString str2(a.functionType);
    if (str1==str2)
    {
        if (startPoint !=a.startPoint)
	 tmp=0;
        if (endPoint !=a.endPoint)
	 tmp=0;
    }
    else
        return 0;
    return tmp;
}

int
FuzzyMember::operator <(const FuzzyMember& a) const
{
    int sum=0;
    if (startPoint<a.startPoint) sum++;
    if (endPoint<a.endPoint) sum++;
    if ((startPoint<=a.startPoint)&&(endPoint<=a.endPoint)&&(sum>=1)) return 1;
    else	return 0;
}

int
FuzzyMember::operator >(const FuzzyMember& a) const
{
    int sum=0;
    if (endPoint>a.endPoint) sum++;
    if (startPoint>a.startPoint) sum++;
    if ((endPoint>=a.endPoint)&&(startPoint>=a.startPoint)&&sum>=1) return 1;
    else	return 0;
}


//member functions
FuzzyMember&
FuzzyMember::change_startPoint(const float& a)
{
    startPoint=a;
    return *this;
}

FuzzyMember&
FuzzyMember::change_endPoint(const float& a)
{
    endPoint=a;
    return *this;
}

FuzzyMember&
FuzzyMember::change_member(const float& a,const float& b, char *str)
{
    startPoint=a;
    endPoint=b;
    delete []functionType;
    int length=strlen(str)+2;
    functionType = new char[length];
    assert(functionType !=0);
    strncpy(functionType,str,length);
    return *this;
}

int
FuzzyMember::member_flag(const float& a) const
{
    int tmp=0;
    if (strncmpi(functionType,"leftTriangle",strlen(functionType)-1)==0)
    {
        if (a<endPoint)
	 tmp=1;
    }
    else if  (strncmpi(functionType,"rightTriangle",strlen(functionType)-1)==0)
    {
        if (a>startPoint)
	 tmp=1;
    }
    else if  (strncmpi(functionType,"Triangle",strlen(functionType)-1)==0)
    {  //Triangle
        if ((a>startPoint)&&(a<endPoint))
	 tmp=1;
    }
    else
    { //Sigmoid and reverSigmoid and Gaussian
        tmp=1;
    }
    return tmp;
}

FuzzyMember&
FuzzyMember::change_functionType(const int& a)
{
    int length;
    delete []functionType;
    switch(a)
    {
        case 1:
	 length=strlen("leftTriangle")+2;
	 functionType=new char[length];
	 assert(functionType !=0);
	 strncpy(functionType,"leftTriangle",length);
	 break;
        case 2:
	 length=strlen("rightTriangle")+2;
	 functionType=new char[length];
	 assert(functionType !=0);
	 strncpy(functionType,"rightTriangle",length);
	 break;
        case 3:
	 length=strlen("Triangle")+2;
	 functionType=new char[length];
	 assert(functionType !=0);
	 strncpy(functionType,"Triangle",length);
	 break;
        case 4:
	 length=strlen("Sigmoid")+2;
	 functionType=new char[length];
	 assert(functionType !=0);
	 strncpy(functionType,"Sigmoid",length);
	 break;
        case 5:
	 length=strlen("reverseSigmoid")+2;
	 functionType=new char[length];
	 assert(functionType !=0);
	 strncpy(functionType,"reverseSigmoid",length);
	 break;
        case 6:
	 length=strlen("Gaussian")+2;
	 functionType=new char[length];
	 assert(functionType !=0);
	 strncpy(functionType,"Gaussian",length);
	 break;
        default:
	 cout<<"unknown function type in memberf.cpp, line 187"<<endl;
	 exit(1);
    }
    return *this;
}
 
int
FuzzyMember::setTypeFlag() const
{
    int tmp;
    if (strncmpi(functionType,"leftTriangle",strlen(functionType))==0)
        tmp=1;
    else if  (strncmpi(functionType,"rightTriangle",strlen(functionType))==0)
        tmp=2;
    else if 	(strncmpi(functionType,"Triangle",strlen(functionType))==0)
        tmp=3;
    else if 	(strncmpi(functionType,"Sigmoid",strlen(functionType))==0)
        tmp=4;
    else if 	(strncmpi(functionType,"reverseSigmoid",strlen(functionType))==0)
        tmp=5;
    else if 	(strncmpi(functionType,"Gaussian",strlen(functionType))==0)
        tmp=6;
    else
        tmp=0;
    return tmp;
}

float
FuzzyMember::memberFunction(const float& a) const
{
    float tmp;
    switch(this->setTypeFlag())
    {
        case 1:
	 tmp=LeftTriangle(a,startPoint,endPoint);
	 break;
        case 2:
	 tmp=RightTriangle(a,startPoint,endPoint);
	 break;
        case 3:
	 tmp=Triangle(a,startPoint,endPoint);
	 break;
        case 4:
	 tmp=Sigmoid(a,startPoint,endPoint);
	 break;
        case 5:
	 tmp=reverseSigmoid(a,startPoint,endPoint);
	 break;
        case 6:
	 tmp=Gaussian(a,startPoint,endPoint);
	 break;
        default:
	 cout<<"unknown fuzzySet type"<<endl;
	 exit(1);
    }
    return tmp;
}

float
FuzzyMember::not(const float& a) const
{
    float tmp;
    tmp=this->memberFunction(a);
    if (tmp>=0.9999)
        tmp=0.0001;   
    else
        tmp=1-tmp;
    return tmp;
}


vector<float>
FuzzyMember::membership2input(const float& a) const
{
    vector<float> tmp(2);
    switch(this->setTypeFlag())
    {
        case 1:
	 tmp=antiLeftTriangle(a,startPoint,endPoint);
	 break;
        case 2:
	 tmp=antiRightTriangle(a,startPoint,endPoint);
	 break;
        case 3:
	 tmp=antiTriangle(a,startPoint,endPoint);
	 break;
        case 4:
	 tmp=antiSigmoid(a,startPoint,endPoint);
	 break;
        case 5:
	 tmp=antiReverseSigmoid(a,startPoint,endPoint);
	 break;
        case 6:
	 tmp=antiGaussian(a,startPoint,endPoint);
	 break;
        default:
	 cout<<"unknown fuzzySet type"<<endl;
	 exit(1);
    }
    return tmp;
}

vector<float>
FuzzyMember::centroid(const float& a,const float& b) const
{
    assert(b !=0);
    vector<float> res(2);
    res[0]=0;
    res[1]=0;
    int total=(int)((endPoint-startPoint)/b);
    for (int i=0;i<total;i++)
    {
        float tmp1=startPoint+i*b;
        float tmp2=memberFunction(tmp1);
        if (tmp2>a)
	 tmp2=a;
        res[0] +=tmp2*tmp1;
        res[1] +=tmp2;
    }
    return res;
}

//friend I/O operators
istream& operator >> (istream& is, FuzzyMember& a)
{
    char tmp[256];
    float b1,b2;
    is>>tmp>>b1>>b2;
    a.change_member(b1,b2,tmp);
    return is;
}

ostream& operator << (ostream& os, const FuzzyMember& a)
{
    os<<"\t"<<a.functionType<<"\t"<<a.startPoint<<"\t"<<a.endPoint;
    os<<endl;
    return os;
}

//local auxiliary function 
float
LeftTriangle(float a, float firstPoint,float secondPoint)
{
    float tmp;
    if (a>=secondPoint)
    {
        fprintf(stderr," %f is outside the LeftTraingle rangle(%f,%f)",a,firstPoint,secondPoint);
        exit(1);
    }
    if (a<=firstPoint)
        tmp=1.0;
    else
        tmp=(secondPoint-a)/(secondPoint-firstPoint);
    return tmp;
}

float
RightTriangle(float a, float firstPoint,float secondPoint)
{
    float tmp;
    if (a<=firstPoint)
    {
        fprintf(stderr," %f is outside the rightTraingle rangle(%f,%f)",a,firstPoint,secondPoint);
        exit(1);
    }
    if (a>=secondPoint)
        tmp=1.0;
    else
        tmp=(a-firstPoint)/(secondPoint-firstPoint);
	return tmp;
}

float
Triangle(float a, float firstPoint,float secondPoint)
{
    float tmp,med;
    if ((a<=firstPoint)||(a>=secondPoint))
    {
        fprintf(stderr," %f is outside the Traingle rangle(%f,%f)",a,firstPoint,secondPoint);
        exit(1);
    }
    med=(secondPoint-firstPoint)/2;
    if (med==0)
    {
        fprintf(stderr,"Triangle is a line, range (%f,%f)",firstPoint,secondPoint);
        exit(1);
    }
    if (a<=(med+firstPoint))
        tmp=(a-firstPoint)/med;
    else
        tmp=(secondPoint-a)/med;
    return tmp;
}

float
Sigmoid(float a, float firstPoint,float secondPoint)
{
    float tmp=((a-firstPoint)*12)/(secondPoint-firstPoint);
    float result=1.0/(1.0+exp(-tmp+6));
    if (result<0.00001)
        result=0.00001;
    return result;
}

float
reverseSigmoid(float a, float firstPoint,float secondPoint)
{
    float result=1-Sigmoid(a,firstPoint,secondPoint);
    if (result<0.00001)
        result=0.00001;
    return result;
}

float
Gaussian(float a,float firstPoint,float secondPoint)
{
    float tmp=((a-firstPoint)*8.0)/(secondPoint-firstPoint) -4;
    float result=exp(-0.5*tmp*tmp);
    if (result<0.00001)
        result=0.00001;
    return result;
}

vector<float>
antiLeftTriangle(float a, float firstPoint,float secondPoint)
{
    vector<float> tmp(2);
    if (a>1||a<0)
    {
        fprintf(stderr," %f is outside the LeftTraingle value[0,1]",a);
        exit(1);
    }
    if (a==0)  {tmp[0]=tmp[1]=secondPoint; return tmp;}
    if (a==1)  {tmp[0]=tmp[1]=firstPoint; return tmp;}
    tmp[0]=secondPoint-(a*(secondPoint-firstPoint));
    tmp[1]=tmp[0];
    return tmp;
}

vector<float>
antiRightTriangle(float a, float firstPoint,float secondPoint)
{
    vector<float> tmp(2);
    if (a>1||a<0)
    {
        fprintf(stderr," %f is outside the LeftTraingle value[0,1]",a);
        exit(1);
    }
    if (a==0)  {tmp[0]=tmp[1]=firstPoint; return tmp;}
    if (a==1)  {tmp[0]=tmp[1]=secondPoint; return tmp;}
    tmp[0]=a*(secondPoint-firstPoint) + firstPoint;
    tmp[1]=tmp[0];
    return tmp;
}

vector<float>
antiTriangle(float a, float firstPoint,float secondPoint)
{
    vector<float> tmp(2);
    if (a>1||a<0)
    {
        fprintf(stderr," %f is outside the antiTraingle value[0,1]",a);
        exit(1);
    }
    if (a==0)  {tmp[0]=firstPoint;tmp[1]=secondPoint; return tmp;}
    if (a==1)  {tmp[0]=tmp[1]=0.5*(secondPoint-firstPoint)+firstPoint; return tmp;}
    float med=(secondPoint-firstPoint)/2;
    tmp[0]=a*med+firstPoint;
    tmp[1]=secondPoint-a*med;
    return tmp;
}

vector<float>
antiSigmoid(float a, float firstPoint,float secondPoint)
{
    vector<float> tmp(2);
    if (a>1||a<0)
    {
        fprintf(stderr," %f is outside the antiSigmoid value[0,1]",a);
        exit(1);
    }
    float tmpfloat;
    if (a>0.1)
    {
        tmpfloat=(1-a)/a;
        tmpfloat=log(tmpfloat);
        tmpfloat=6-tmpfloat;
    }
    else
    {
        tmpfloat=a/(1-a);
        tmpfloat=log(tmpfloat);
        tmpfloat=tmpfloat+6;
    }
    tmpfloat=(tmpfloat*(secondPoint-firstPoint))/12;
    tmp[0]=tmpfloat+firstPoint;
    tmp[1]=tmp[0];
    return tmp;
}

vector<float>
antiReverseSigmoid(float a, float firstPoint,float secondPoint)
{
    vector<float> tmp(2);
    if (a>1||a<0)
    {
        fprintf(stderr," %f is outside the antiReverseSigmoid value[0,1]",a);
        exit(1);
    }
    float tmpfloat=1-a;
    tmp=antiSigmoid(tmpfloat,firstPoint,secondPoint);
    return tmp;
}

vector<float>
antiGaussian(float a,float firstPoint,float secondPoint)
{
    vector<float> tmp(2);
    if (a>1||a<0)
    {
        fprintf(stderr," %f is outside the antiGaussian value[0,1]",a);
        exit(1);
    }
    assert(a>0);
    float tmpfloat=-2.0*log(a);
    assert(tmpfloat>=0);
    tmp[1]=sqrt(tmpfloat);
    tmp[0]=-tmp[1];
    tmp[0]=tmp[0]+4;
    tmp[1]=tmp[1]+4;
    tmp[0]=(tmp[0]*(secondPoint-firstPoint))/8+firstPoint;
    tmp[1]=(tmp[1]*(secondPoint-firstPoint))/8+firstPoint;
    return tmp;
}

