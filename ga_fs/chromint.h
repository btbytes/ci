#ifndef __CHROMINT_H__
#define __CHROMINT_H__


#include "ruleset.h"
#include "fuzzyrul.h"
#include "variablf.h"
#include "memberf.h"
#include "vector.h"
#include "array.h"

// Listing 8.23 Definition of class IndividualInt
class IndividualInt
{
private:
    int length;	//length of the individual
    int* ptr;	//pointer to the Individual
    float m_rate;	//mutation rate
public:
    IndividualInt():length(0),ptr(0),m_rate(0) {}
    IndividualInt(int a,float b);
    IndividualInt(int a,int* b,float c);
    IndividualInt(vector<int> a, float b);
    IndividualInt(const IndividualInt& a);

    ~IndividualInt() {delete []ptr;}

    //member function
    int get_length() const {return length;}
    float get_mrate() const {return m_rate;}
    IndividualInt& change_mrate(const float& a);
    IndividualInt& change_length(const int& a);
    IndividualInt& initialize(const int& a,const int& b);
    IndividualInt& initialize_range(const IndividualInt& a);
    //for evolving rule set only
    IndividualInt& initialize_range_RM(const IndividualInt& a);
    //for evolving rule set and tuning membersgip function
    IndividualInt& initialize_range_RMT(const IndividualInt& a);
    //for evolving rule set and tuning membersgip function and type
    FuzzyRuleSet formRuleSet(const FuzzyRule& a) const;
    	//a:base rule, for rule set only
    FuzzyRuleSet formRuleSet_RM(const FuzzyRule& a,const IndividualInt& b) const;
    	//a:base rule, b:range, for rule set & membership functions
    FuzzyRuleSet formRuleSet_RMT(const FuzzyRule& a,const IndividualInt& b) const;
    	//a:base rule, b:range, for rule set & membership functions and type
    float fitness(const FuzzyRule& a, const array& b,const vector<int>& cn,const int&c,const int& d,const int&e) const;
    	//a: base rule, b: input array  c:ruleEffectFlag
    	//d:fuzzyFlag  e:defuzzyFlag  cn:class no. for output
    //for evolving rule set only
    float fitness_RM(const FuzzyRule& a, const array& b,const vector<int>& cn,const int&c,const int& d,const int& e,const IndividualInt& f) const;
    	//a: base rule, b: input array  c:ruleEffectFlag
    	//d:fuzzyFlag  e:defuzzyFlag  cn:class no. for output
    	//f: range individual
    //for evolving rule set and tuning membership functions
    float fitness_RMT(const FuzzyRule& a, const array& b,const vector<int>& cn,const int&c,const int& d,const int& e,const IndividualInt& f) const;
    	//a: base rule, b: input array  c:ruleEffectFlag
    	//d:fuzzyFlag  e:defuzzyFlag  cn:class no. for output
    	//f: range individual
    //for evolving rule set and tuning membership functions and type
    IndividualInt& mutate_one(const IndividualInt& a);
    	//a: rule range, for evolving rule set only
    IndividualInt& mutate_one_RM(const IndividualInt& a);
    	//a: rule range, for evolving rule set & tuning membership functions
    IndividualInt& mutate_one_RMT(const IndividualInt& a);
    	//a: rule range, for evolving rule set & tuning membership functions & type

    friend void crossoverOP(IndividualInt& a,IndividualInt& b);
    friend void crossoverTP(IndividualInt& a,IndividualInt& b);
    friend void crossoverUniform(IndividualInt& a,IndividualInt& b);

    //operators
    int& operator [] (int i) const;
    IndividualInt& operator =(const IndividualInt& a);

    //I/O operators
    friend istream& operator >>(istream& is,IndividualInt& a);
    friend ostream& operator <<(ostream& os,const IndividualInt& a);
};
#endif

