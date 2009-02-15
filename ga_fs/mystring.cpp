#include "headfile.h"
#include "mystring.h"

MyString::MyString(int a):
	stringSize(a),currentPosition(0)
{
    stringPtr=new char[stringSize+1];
    assert(stringPtr !=0);
}

MyString::MyString(char *str):
	currentPosition(0)
{
    int length=strlen(str);
    stringSize=length;
    stringPtr= new char[stringSize+1];
    assert(stringPtr !=0);
    strncpy(stringPtr,str,length+1);
}

MyString::MyString(const MyString& a)
{
    stringSize=a.stringSize;
    currentPosition=a.currentPosition;
    stringPtr=new char[stringSize+1];
    assert(stringPtr !=0);
    strncpy(stringPtr,a.stringPtr,stringSize+1);
}

//member functions
MyString&
MyString::change_stringSize(const int& a)
{
    assert(a>=0);
    stringSize=a;
    return *this;
}

MyString&
MyString::change_currentPosition(const int& a)
{
    assert(a>=0&&a<stringSize);
    currentPosition=a;
    return *this;
}

MyString&
MyString::change_stringContent(char *str)
{
    delete []stringPtr;
    int length=strlen(str);
    stringSize=length;
    currentPosition=0;
    stringPtr=new char[stringSize+1];
    assert(stringPtr !=0);
    strncpy(stringPtr,str,length+1);

    return *this;
}

int
MyString::findNextF(char ch) const
{
    int tmp=-1;
    for (int i=currentPosition;i<stringSize;i++)
    {
        if (stringPtr[i]==ch)
        {
	 tmp=i;
	 break;
        }
    }
    return tmp;
}

int
MyString::findNextB(char ch) const
{
    int tmp=-1;
    for (int i=currentPosition;i>=0;i--)
    {
        if (stringPtr[i]==ch)
        {
	 tmp=i;
	 break;
        }
    }
    return tmp;
}

int
MyString::totalNumberF(char ch) const
{
    int sum=0;
    for (int i=currentPosition;i<stringSize;i++)
    {
        if (stringPtr[i]==ch)
	 sum++;
    }
    return sum;
}

int
MyString::totalNumberB(char ch) const
{
    int sum=0;
    for (int i=currentPosition;i>=0;i--)
    {
        if (stringPtr[i]==ch)
	 sum++;
    }
    return sum;
}

MyString
MyString::get_subString(const int& a)
{
    assert((currentPosition+a)<=stringSize);
    MyString substr(a);
    for (int i=currentPosition;i<(currentPosition+a);i++)
        substr.stringPtr[i-currentPosition]=stringPtr[i];

    substr.stringPtr[a]='\0';
    return substr;
}

//operators

char&
MyString::operator [] (int i) const
{
    assert(i>=0&&i<stringSize);
    return stringPtr[i];
}

MyString&
MyString::operator =(const MyString& a)
{
    if ((&a)==this) return *this;
    delete []stringPtr;
    stringSize=a.stringSize;
    currentPosition=a.currentPosition;
    stringPtr=new char[stringSize+1];
    assert(stringPtr !=0);
    strncpy(stringPtr,a.stringPtr,stringSize+1);

    return *this;
}

int
MyString::operator ==(const MyString& a) const
{
    if ((&a)==this) return 1;

    if (stringSize !=a.stringSize) return 0;

    int tmp=1;
    for (int i=0;i<stringSize;i++)
    {
        if (stringPtr[i] !=a.stringPtr[i])
        {
	 tmp=0;
	 break;
        }
    }

    return tmp;
}


// friend I/O operators

ostream& operator <<(ostream& os,const MyString& a)
{
    os<<a.stringPtr<<endl;
    return os;
}

istream& operator >>(istream& is,MyString& a)
{
    char tmp[256];
    is>>tmp;
    int length=strlen(tmp);
    a.stringSize=length;
    a.currentPosition=0;
    delete []a.stringPtr;
    a.stringPtr=new char[length+1];
    assert(a.stringPtr !=0);
    strncpy(a.stringPtr,tmp,length+1);

    return is;
}








