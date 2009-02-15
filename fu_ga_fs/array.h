#ifndef __ARRAY_H__
#define __ARRAY_H__

#include "vector.h"

class array
{
private:
	 int row;	         //length of array (column);
    int col;	         //wide of array (row);
    float* arr;	         //pointer to the array;

public:
    array():row(0),col(0),arr(0) {}
    array(int a,int b);
    array(const array& a);
    ~array(){delete []arr;}

    array& operator =(const array& a);
    array& operator =(const float& a);
    array& operator =(const vector<float>& a);
    float* operator [] (int i) const;

	 array& changeSize(const int& r,const int& c);
	 int len() const {return row;}
    int wid() const {return col;}
    float* poi() const {return arr;}
    //find the index of maximum element
    vector<int> max_index() const;  //vec[0]:row, vec[1]:col
    //sum of  all elements
    float sum() const;
    //add noise
    array noise(const float& a,const float& b) const;
    //element-wise square
    array square() const;
    //transpose
    array t() const;

    //arithmetic operation
    array& operator +=(const array& a);
    array& operator -=(const array& a);
    array& operator *=(const array& a);
    array& operator *=(const float& a);

    //X.map(f) returns  element by element mapping f(X)
    array map(float (*f)(float)) const;

    friend array  operator * (const float& a, const array& b);
    friend array  operator * (const array& a, const float& b);
    friend array  operator * (const array& a, const array& b);
    friend array  operator % (const array& a,const array& b);
    friend array  operator + (const array& a, const array& b);
    friend array  operator - (const array& a, const array& b);
    friend array  operator - (const float& a,const array& b);
    friend array  operator - (const array& a, const float& b);
    friend istream& operator >> (istream& is,array& a);
    friend ostream& operator << (ostream& os,const array& a);
};

#endif // __ARRAY_H__
