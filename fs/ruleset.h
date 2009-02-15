#ifndef __RULESET_H__
#define __RULESET_H__

#include "fuzzyrul.h"
#include "variablf.h"
#include "memberf.h"
#include "vector.h"

// Listing 8.10 Definition of class FuzzyRuleSet
class FuzzyRuleSet
{
private:
    int        ruleSetSize;	//how many rules in the set
    FuzzyRule *rules;	//pointers to the fuzzy rule set

public:
    FuzzyRuleSet():ruleSetSize(0),rules(0) {}
    FuzzyRuleSet(int a);
    FuzzyRuleSet(int a, FuzzyRule *b);
    FuzzyRuleSet(const FuzzyRuleSet& a);

    ~FuzzyRuleSet() {delete []rules;}

    //member functions
    int get_ruleSetSize() const {return ruleSetSize;}
    FuzzyRuleSet& addRuleB(const FuzzyRule& a,const int& b); //add rule a at position b
    FuzzyRuleSet& addRule(const FuzzyRule& a); //add rule a at the end of set
    FuzzyRuleSet& deleteRule(const int& a); //delete the 'a'th rule

    vector< vector<float> > fuzzyOutputValue_max(const vector<float>& a, const int& b) const;
    vector< vector<float> > fuzzyOutputValue_add(const vector<float>& a, const int& b) const;
    	//a:input vector,
    	//b: mode for antecedent--0:min 1:aver,
    vector<float> defuzzify(const vector< vector<float> >& a,const int& b) const;
    	//b: mode for defuzzyfy--0:max 1:centroid without overlap
    	//2: with overlap;
    	// a: fuzzy output values (for each fuzzy set of each output variable
    vector<float> output(const vector<float>& a, const int& b,const int& c,const int& d) const;
    	//a: input  b:add/max c:min/aver d:max/without/with overlap
    	// return the value after defuzzify

    vector<float> output_new(const vector<float>& a, const int& b,const int& c,const int& d) const;

    FuzzyVariable& get_outputVariable(const int& a) const;

    int checkRuleSetFired(const vector<float>& a) const;
    	//check this rule set is fired or not due to a

    //operators
    FuzzyRule& operator [](int i) const;
    FuzzyRuleSet& operator =(const FuzzyRuleSet& a);

    //I/O operators
    friend istream& operator>>(istream& is, FuzzyRuleSet& a);
    friend ostream& operator<<(ostream& os, const FuzzyRuleSet& a);
};

#endif
