#ifndef __MEMBERF_H__
#define __MEMBERF_H__

#include "vector.h"
#include "mystring.h"

// Listing 8.3 Definition of class FuzzyMember
class FuzzyMember
{
private:
    float	 startPoint;
    float   endPoint;
    char	*functionType;

public:
    //constructor
    FuzzyMember():startPoint(0),endPoint(0),functionType(0) {}
    FuzzyMember(float a,float b, char *str);
    FuzzyMember(const FuzzyMember& a);

    //destructor
    ~FuzzyMember(){delete []functionType;}

    //member fuction
    float memberFunction(const float& a) const;
    float not(const float& a) const;
    vector<float> membership2input(const float& a) const ;
    float get_startPoint() const {return startPoint;}
    float get_endPoint() const {return endPoint;}
    char* get_functionType() const {return functionType;}
    int member_flag(const float& a) const; //a belong to this member
    int setTypeFlag() const;	//0:unknown 1:leftT 2:rightT 3:T

    FuzzyMember& change_member(const float& a,const float& b,char *str);

    vector<float> centroid(const float& a,const float& b) const;

    //operators
    FuzzyMember& operator =(const FuzzyMember& a);
    int operator ==(const FuzzyMember& a) const;
    int operator < (const FuzzyMember& a) const;
        //the FuzzyMember is left to a);
    int operator > (const FuzzyMember& a) const;
        //the FuzzyMember is right to a);

    //friend operator I/O
    friend istream& operator >> (istream& is,FuzzyMember& a);
    friend ostream& operator << (ostream& os,const FuzzyMember& a);
};

inline FuzzyMember::FuzzyMember(float a,float b,char* str):
	startPoint(a),endPoint(b)
{
    assert(startPoint<=endPoint);
    int length=strlen(str)+2;

    functionType= new char[length];
    assert(functionType !=0);
    strncpy(functionType,str,length);
}

inline FuzzyMember::FuzzyMember(const FuzzyMember& a):
	startPoint(a.startPoint),endPoint(a.endPoint)
{
    assert(startPoint<=endPoint);
    int length=strlen(a.functionType)+2;

    functionType= new char[length];
    assert(functionType !=0);
    strncpy(functionType,a.functionType,length);
}
#endif
