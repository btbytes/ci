#ifndef __POPUINT_H__
#define __POPUINT_H__
#include "headfile.h"

#include "chromint.h"

template <class Type>
class vector;

// Listing 8.24 Defition of class PopulationInt
class PopulationInt
{
private:
    int length;	//population size
    int width;	//individual length
    IndividualInt* ptr;	//pointer to the individual
    float c_rate;	//crossover rate
    float m_rate;	//mutation rate

public:
    PopulationInt():length(0),width(0),ptr(0),c_rate(0),m_rate(0) {}
    PopulationInt(int a,int b);
    PopulationInt(int a,int b,float c,float d);
    PopulationInt(const PopulationInt& a);

    //member function
    int get_length() const {return length;}
    int get_width() const {return width;}
    float get_crate() const {return c_rate;}
    float get_mrate() const {return m_rate;}

	 PopulationInt& change_length(const int& a);
	 PopulationInt& change_width(const int& a)
			{assert(a>=0); width=a; return *this; }
    PopulationInt& change_crate(const float& a)
      	{assert(a>=0&&a<=1); c_rate=a; return *this; }
    PopulationInt& change_mrate(const float& a);
    PopulationInt& initialize_range(const IndividualInt& a);
        //a: rule range, for evolving rule set only
    PopulationInt& initialize_range_RM(const IndividualInt& a);
        //a: rule range, for evolving rule set and tuning membership functions
    PopulationInt& initialize_range_RMT(const IndividualInt& a);
        //a: rule range, for evolving rule set and tuning membership functions
    PopulationInt& mutate_one(const IndividualInt& a,const int& b);
        //a: rule range  b: best fitness index
        //for rule set only
    PopulationInt& mutate_one_RM(const IndividualInt& a,const int& b);
        //a: rule range  b: best fitness index
        //for rule set and membership functions
    PopulationInt& mutate_one_RMT(const IndividualInt& a,const int& b);
        //a: rule range  b: best fitness index
        //for rule set and membership functions and type
    PopulationInt& crossover(const int& a, const int& b); //a: crossover flag
        //0:uniform  1:one point  2: two point  b: best individual index
    PopulationInt& selection(const vector<float>& a,const int& b,const int& c);
        //a: fitness vector  b: best indi index  c: shift flag (1: yes, 0: no)
    vector<float> fitness(const FuzzyRule& a,const array& b,const vector<int>& cn,const int& c,const int& d,const int&e) const;
        //a:base rule, b:input array  c:ruleEffectFlag
        //d:fuzzyFlag  e:defuzzyFlag  cn:class no. for output
        //for evolving rule set only
    vector<float> fitness_RM(const FuzzyRule& a,const array& b,const vector<int>& cn,const int& c,const int& d,const int& e,const IndividualInt& f) const;
        //a:base rule, b:input array  c:ruleEffectFlag
        //d:fuzzyFlag  e:defuzzyFlag  cn:class no. for output
        //f: range individual
        //for evolving rule set and membership functions
    vector<float> fitness_RMT(const FuzzyRule& a,const array& b,const vector<int>& cn,const int& c,const int& d,const int& e,const IndividualInt& f) const;
        //a:base rule, b:input array  c:ruleEffectFlag
        //d:fuzzyFlag  e:defuzzyFlag  cn:class no. for output
        //f: range individual
        //for evolving rule set and membership functions

    //operators
    IndividualInt& operator [] (int i) const
        {assert(i>=0&&i<length); return ptr[i];}

    //I/O operators
    friend ostream& operator<<(ostream& os,const PopulationInt& a);
    friend istream& operator>>(istream& is,PopulationInt& a);
};
#endif



