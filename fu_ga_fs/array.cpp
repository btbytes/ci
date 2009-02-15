#include "headfile.h"
#include "array.h"

//constructors
array::array(int a,int b):
row(a),col(b) {
    arr=new float[row*col];
    assert(arr!=0);
    for (int i=0;i<row;i++)
        for (int j=0;j<col;j++)
	 *(arr+i*col+j)=0;
}

array::array(const array& a):
row(a.row),col(a.col) {
	 arr = new float[row*col];
	 assert(arr !=0);
    for (int i=0;i<row;i++)
        for (int j=0;j<col;j++)
	 *(arr+i*col+j)=*(a.arr+i*(a.col)+j);
}

//arithmetic operation
float*   array::operator [] (int i) const
{assert(i>=0 && i<row);  return &arr[i*col];}

array&
array::changeSize(const int& r, const int& c)
{
	 delete []arr;
	 row = r;
	 col = c;
	 arr = new float[row*col];
	 assert(arr !=0);
    for (int i=0;i<row;i++)
        for (int j=0;j<col;j++)
	         *(arr+i*col+j)=0;
    return *this;
}

vector<int>
array::max_index() const
{
    vector<int> tmp(2);
    float max=*(arr)-2;
    for (int i=0;i<row;i++)
        for (int j=0;j<col;j++)
        {
	 if (max<(*(arr+i*col+j)))
	 {
	     max=*(arr+i*col+j);
	     tmp[0]=i;
	     tmp[1]=j;
	 }
        }

    return tmp;
}

float
array::sum() const
{
    float tmp=0;
    for (int i=0;i<row;i++)
        for (int j=0;j<col;j++)
	 tmp +=*(arr+i*col+j);
    return tmp;
}

array&
array::operator +=(const array& a)
{
    if ((row !=a.row)||(col !=a.col))
    {
        cerr<<"adding two arrays with different dimention"<<endl;
        exit(0);
    }
    for (int i=0;i<row;i++)
        for (int j=0;j<col;j++)
	 *(arr+i*col+j) +=a[i][j];
    return *this;
}

array&
array::operator -=(const array& a)
{
    if ((row !=a.row)||(col !=a.col))
    {
        cerr<<"subtracting two arrays with different dimention"<<endl;
        exit(0);
    }
    for (int i=0;i<row;i++)
        for (int j=0;j<col;j++)
	 *(arr+i*col+j) -=a[i][j];
    return *this;
}

array&
array::operator *=(const array& a)
{
    if (col !=a.row)
    {
        cerr<<"multify two nonconformant arrays"<<endl;
        exit(0);
    }
    array newArr=*this;
    for (int i=0;i<row;i++)
        for (int j=0;j<a.col;j++)
        {
	 float sum=0.0;
	 for (int k=0;k<col;k++)
	 sum +=newArr[i][k]*a[k][j];
	 *(arr+i*a.col+j) =sum;
        }
    col=a.col;
    return *this;
}

array&
array::operator *=(const float& a)
{
    for (int i=0;i<row;i++)
        for (int j=0;j<col;j++)
	 *(arr+i*col+j) *=a;
    return *this;
}

array&
array::operator =(const array& a)
{
    if ((&a)==this) return *this;
	 delete []arr;
    row=a.row;
    col=a.col;
    arr = new float [row*col];
    assert(arr !=0);
    for (int i=0;i<row;i++)
        for (int j=0;j<col;j++)
	 *(arr+i*col+j)=a[i][j];
    return *this;
}

array&
array::operator =(const float& a)
{
    for (int i=0;i<row;i++)
        for (int j=0;j<col;j++)
	 *(arr+i*col+j)=a;
    return *this;
}

array&
array::operator =(const vector<float>& a)
{
    if ((col !=1)||(row !=a.len()))
    {
        cerr<<"array in-compabaility with vector"<<endl;
        exit(0);
    }
    for (int i=0;i<row;i++)
        *(arr+i)=a[i];
    return *this;
}

array
array::noise(const float& a,const float& b) const
{
    time_t  t;
    srand((unsigned) time(&t));

    array newArr(row,col);
    for (int i=0;i<row;i++)
        for (int j=0;j<col;j++)
        {
	 float prob=(1.0*random(1000))/1000;
	 newArr[i][j] +=a*prob + b;
        }
    return newArr;
}

//transpose
array
array::t() const
{
    array newArr(col,row);
    for (int i=0;i<newArr.row;i++)
        for (int j=0;j<newArr.col;j++)
	 newArr[i][j]=*(arr+j*col+i);
    return newArr;
}

//element-wise square
array
array::square() const
{
    array newArr(row,col);
    for (int i=0;i<row;i++)
        for (int j=0;j<col;j++)
	 newArr[i][j]=(*(arr+i*col+j))*(*(arr+i*col+j));
    return newArr;
}


//X.map(f) returns element by element mapping f(X)
array
array::map(float (*f) (float)) const
{
    array newArr(row,col);
    for (int i=0;i<row;i++)
        for (int j=0;j<col;j++)
	 newArr[i][j]=f((*(arr+i*col+j)));	
    return newArr;
}

array operator % (const array& a,const array& b)
{
    if ((a.row !=b.row)||(a.col !=b.col))
    {
        cerr<<"inner product of non-compabability arrays"<<endl;
        exit(-1);
    }
    array newArr(a.row,a.col);
    for (int i=0;i<a.row;i++)
        for (int j=0;j<a.col;j++)
	 newArr[i][j]=a[i][j]*b[i][j];
    return newArr;
}

array operator * (const float& a, const array& b)
{
    array newArr(b.row,b.col);
    for (int i=0;i<b.row;i++)
        for (int j=0;j<b.col;j++)
	 newArr[i][j]=a*b[i][j];
    return newArr;
}

array operator * (const array& a, const float& b)
{
    array newArr(a.row,a.col);
    for (int i=0;i<a.row;i++)
        for (int j=0;j<a.col;j++)
	 newArr[i][j]=a[i][j]*b;
    return newArr;
}

array operator * (const array& a, const array& b)
{
    if (a.col !=b.row)
    {
        cerr<<"multify two arrays nonconformant"<<endl;
        exit(0);
    }
    array newArr(a.row,b.col);
    for (int i=0;i<a.row;i++)
        for (int j=0;j<b.col;j++)
	 for (int k=0;k<a.col;k++)
	     newArr[i][j] +=a[i][k]*b[k][j];
    return newArr;
}
			
array  operator + (const array& a, const array& b)
{
    if ((a.row !=b.row)||(a.col !=b.col))
    {
        cerr<<"adding two arrays with different dimensions"<<endl;
        exit(0);
    }
    array newArr(a.row,a.col);
    for (int i=0;i<a.row;i++)
        for (int j=0;j<a.col;j++)
	 newArr[i][j] = a[i][j]+b[i][j];
    return newArr;
}

array operator - (const array& a, const array& b)
{
    if ((a.row !=b.row)||(a.col !=b.col))
    {
        cerr<<"adding two arrays with different dimensions"<<endl;
        exit(0);
    }
    array newArr(a.row,a.col);
    for (int i=0;i<a.row;i++)
        for (int j=0;j<a.col;j++)
	 newArr[i][j] = a[i][j]-b[i][j];
    return newArr;
}

array operator - (const float& a,const array& b)
{
    array newArr(b.row,b.col);
    for (int i=0;i<b.row;i++)
        for (int j=0;j<b.col;j++)
	 newArr[i][j]=a-b[i][j];
    return newArr;
}

array operator - (const array& a,const float& b)
{
    array newArr(a.row,a.col);
    for (int i=0;i<a.row;i++)
        for (int j=0;j<a.col;j++)
	 newArr[i][j]=a[i][j]-b;
    return newArr;
}

istream& operator >> (istream& is,array& a)
{
    for (int i=0;i<a.row;i++)
        for (int j=0;j<a.col;j++)
	 is >> a[i][j];
    return is;
}

ostream& operator << (ostream& os,const array& a)
{
    for (int i=0;i<a.row;i++)
    {
        for (int j=0;j<a.col;j++)
	 os << a[i][j]<<"\t";
        os<<endl;
    }
    os << endl;
    return os;
}

