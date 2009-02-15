#ifndef __VARIABLF_H__
#define __VARIABLF_H__

#include "memberf.h"
#include "vector.h"

// Listing 8.6 Definiton of class FuzzyVariable
class FuzzyVariable
{
private:
    int          setSize;
    float	      startPoint;
    float        endPoint;
    char        *variableName;
    FuzzyMember *fuzzySet;

public:
    //constructors
    FuzzyVariable():setSize(0),startPoint(0),endPoint(0),variableName(0),fuzzySet(0){}
    FuzzyVariable(int a,float b,float c);
    FuzzyVariable(int a,float b,float c,char *str);
    FuzzyVariable(const FuzzyVariable& a);

    //destructor
    ~FuzzyVariable(){delete []fuzzySet;delete []variableName;}

    //member functions
    FuzzyVariable& standardVariable();
    char* get_variableName() const {return variableName;}
    int get_setSize() const {return setSize;}
    float get_startPoint() const {return startPoint;}
    float get_endPoint() const {return endPoint;}

    FuzzyVariable& change_setSize(const int& a);
    FuzzyVariable& change_startPoint(const float& a);
    FuzzyVariable& change_endPoint(const float& a);
    FuzzyVariable& change_variableName(char *str);

    char* setMeaning(const int& a,const int& b) const;	//a:setSize b:which set
    vector<int> setFireFlag(const float& a) const ;
    float output(const float& a,const int& b) const;	//b:set being chosen, output of set b
    float defuzzifyMax(const int& a,const vector<float>& b) const;
    	//return the value
    int defuzzyMax_index(const int& a,const vector<float>& b) const;
    	//return the set index
    float defuzzyCentroid_add(const int& a,const vector<float>& b) const;
    float defuzzyCentroid(const int& a,const vector<float>& b) const;

    //operators
    FuzzyMember& operator [] (int i) const;
    FuzzyVariable& operator =(const FuzzyVariable& a);

    //friend operator I/O
    friend istream& operator >> (istream& is,FuzzyVariable& a);
    friend ostream& operator << (ostream& os,const FuzzyVariable& a);
};

#endif



