#ifndef __VECTOR_H__
#define __VECTOR_H__

#include "headfile.h"

// Listing 8.1 Definition of template class vector
template <class Type>
class vector
{
private:
    int   row;        //length of array (column);
    Type *arr;      //pointer to the array;
public:
    //constructors
    vector():row(0),arr(0) {}
    vector(int a);
    vector(const vector<Type>& a);
    vector(int a,Type* b);
    ~vector(){delete []arr;}

    //operators
    vector<Type>& operator =(const vector<Type>& a);
    vector<Type>& operator +=(const vector<Type>& a);
    int operator !=(const vector<Type>& a) const;
    int operator <(const vector<Type>& a) const {return (row<a.row);}

    Type& operator [] (int i) const {assert(i>=0&&i<row); return arr[i];}

    //member functions
    int len() const {return row;}
    Type sum() const;
    int maximum_index() const;
    vector<Type>& changeSize(const int& a);
    Type minimum() const;

    friend vector<Type> operator |(const vector<Type>& a,const vector<Type>& b);
    friend istream& operator >> (istream& is,vector<Type>& a);
    friend ostream& operator << (ostream& os, const vector<Type>& a);
};


template <class Type>
vector<Type>::vector(int a):
        row(a)
{
    arr=new Type [row];
    assert(arr!=0);
}

template <class Type>
vector<Type>::vector(int a,Type* b):
        row(a)
{
    arr=new Type[row];
    assert(arr !=0);
    for (int i=0;i<row;i++)
        arr[i]=b[i];
}


template <class Type>
vector<Type>::vector(const vector<Type>& a):
         row(a.row)
{
    arr = new Type [row];
    assert(arr!=0);
    for (int i=0;i<row;i++)
        arr[i]=a[i];
}

template <class Type>
vector<Type>&
vector<Type>::changeSize(const int& a)
{
    delete []arr;
    row=a;
    arr=new Type[row];
    assert(arr!=0);
    return *this;
}


template <class Type>
vector<Type>&
vector<Type>::operator =(const vector<Type>& a)
{
    if ((&a)==this) return *this;

    delete []arr;
    row=a.row;
    arr = new Type [row];
    assert(arr!=0);
    for (int i=0;i<row;i++)
        arr[i]=a[i];
    return *this;
}

template <class Type>
vector<Type>&
vector<Type>::operator +=(const vector<Type>& a)
{
    assert(row==a.row);
    for (int i=0;i<row;i++)
        arr[i] +=a[i];

    return *this;
}

template <class Type>
int
vector<Type>::operator !=(const vector<Type>& a) const
{
    if ((&a)==this) return 0;

    if (row !=a.row) return 1;

    for (int i=0;i<row;i++)
    {
        if (arr[i] !=a[i]) return 1;
    }
    return 0;
}

//member functions

template <class Type>
Type
vector<Type>::sum() const
{
    Type tmp=arr[0];

    for (int i=1;i<row;i++)
        tmp +=arr[i];

    return tmp;
}

template <class Type>
int
vector<Type>::maximum_index() const
{
    Type max=arr[0];
    int ind=0;

    for (int i=1;i<row;i++)
    {
        if (max<arr[i])
        {
	 max=arr[i];
	 ind=i;
        }
    }

    return ind;
}

template <class Type>
Type
vector<Type>:: minimum() const
{
    Type mini=arr[0];
    for (int i=1;i<row;i++)
    {
        if (arr[i]<mini)
        	 mini=arr[i];
    }

    return mini;
}


//friend operators
template <class Type>
vector<Type> operator | (const vector<Type>& a,const vector<Type>& b)
{
    vector<Type> newVec(a.row+b.row);
    for (int i=0;i<a.row;i++)
        newVec[i]=a[i];
    for (i=0;i<b.row;i++)
        newVec[a.row+i]=b[i];
    return newVec;
}

template <class Type>
istream& operator >> (istream& is,vector<Type>& a)
{
    for (int i=0;i<a.row;i++)
        is >> a[i];
    return is;
}

template <class Type>
ostream& operator << (ostream& os,const vector<Type>& a)
{
    int sum=0;
    for (int j=0;j<a.row;j++)
    {
        os << a[j]<<"\t";
        sum++;
        if ((sum%8)==0)
        {
	  os <<endl;
	  sum=0;
        }
    }
    os<<endl;
    return os;
}

#endif // _vector_h_


