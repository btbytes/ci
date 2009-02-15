#ifndef __MYSTRING_H__
#define __MYSTRING_H__

class MyString
{
private:
    int   stringSize;
    char *stringPtr;
    int   currentPosition;

public:
    //constructors
    MyString():stringSize(0),stringPtr(0),currentPosition(0) {}
    MyString(int a);
    MyString(char * str);
    MyString(const MyString& a);

    //destructor
    ~MyString() {delete []stringPtr;}

    //member functions
    int get_stringSize() const {return stringSize;}
    int get_currentPosition() const {return currentPosition;}
    char* get_stringPtr() const {return stringPtr;}

    MyString& change_stringSize(const int& a);
    MyString& change_currentPosition(const int& a);
    MyString& change_stringContent(char *str);

    int findNextF(char ch) const;
    int findNextB(char ch) const;
    int totalNumberF(char ch) const;
    int totalNumberB(char ch) const;
    MyString get_subString(const int& a);   //a: size of subString
    //from current position

    // operators
    char& operator [] (int i) const;
    MyString& operator =(const MyString& a);
    int operator ==(const MyString& a) const;

    //friend I/O operators
    friend ostream& operator <<(ostream& os, const MyString& a);
    friend istream& operator >>(istream& is, MyString& a);
};
#endif
