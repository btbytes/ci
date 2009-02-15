#ifndef __FUZZYRUL_H__
#define __FUZZYRUL_H__

#include "variablf.h"
#include "memberf.h"
#include "vector.h"

// Listing 8.7 Definition of class FuzzyRule
class FuzzyRule
{
private:
    int            variableSize;    //number of variables in a rule
    int            outputSize;      //number of output in a rule
    vector<int>    inputSetFlag;    //vector tell which set is activated for each variable
    vector<int>    outputSetFlag;   //vector tell which set is activated for each variable;

    FuzzyVariable *inputVariable;   //pointers to the input variables
    FuzzyVariable *outputVariable;  //pointers to the output variables
    char          *ruleContent;

public:
    FuzzyRule():variableSize(0),outputSize(0),ruleContent(0),inputVariable(0),outputVariable(0)
    {
        vector<int> vec;
        inputSetFlag=vec;
        outputSetFlag=vec;
    }
    FuzzyRule(int a,int b,vector<int> c,vector<int> d);
    FuzzyRule(int a,int b,vector<int> c,vector<int> d,char* str);

    FuzzyRule(const FuzzyRule& a);

    ~FuzzyRule(){delete []ruleContent; delete []inputVariable;delete []outputVariable;}

    //member functions
    int get_variableSize() const {return variableSize;}
    int get_outputSize() const {return outputSize;}
    vector<int> get_inputSetFlag()const {return inputSetFlag;}
    vector<int> get_outputSetFlag() const {return outputSetFlag;}
    char* get_ruleContent() const {return ruleContent;}

    FuzzyRule& change_inputSetFlag(const vector<int>& a);
    FuzzyRule& change_outputSetFlag(const vector<int>& a);
    FuzzyRule& change_variableSize(const int& a);
    FuzzyRule& change_outputSize(const int& a);
    FuzzyRule& change_ruleContent(char* str);
    FuzzyRule& form_ruleContent();

    FuzzyRule& change_outputVariable(const FuzzyVariable& a,const int& b);
        //bth outputVariable change to a

    int checkRuleActive(const vector<float>& a) const;
        //check this rule is activated via input a or not

    vector<float> FuzzyOutput(const vector<float>& a) const;
        //calculate the fuzzy output vector
    vector<float> FuzzyOutput_average(const vector<float>& a) const;

    FuzzyVariable& get_outputVariable(const int& a) const;
        //need & ?
    vector<int> formRange(const int& a) const;
        //a: maximum rules; get possible maximum fuzzy set no. for each variable

    //operator
    FuzzyVariable& operator [] (int i) const;
    FuzzyRule& operator =(const FuzzyRule& a);

    //I/O operators
    friend istream& operator >>(istream& is, FuzzyRule& a);
    friend ostream& operator <<(ostream& os,const FuzzyRule& a);

};
#endif

