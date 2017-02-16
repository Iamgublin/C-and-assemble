// ConsoleApplication1.cpp : 定义控制台应用程序的入口点。
//

#include "stdafx.h"
#include<Windows.h>
#include<iostream>
#include<string>
using namespace std;
class asd
{
public:
	asd(){}
	asd(char* temp) { this->a = temp; }
	char* operator[](size_t num);
	const char* operator[](size_t num) const;
	asd& operator=(char[]);
public:
	char *a;
};
char* asd::operator[](size_t num)
{
	cout << "调用了下标重载" << endl;
	return &a[num+1];
}
const char* asd::operator[](size_t num) const
{
	cout << "调用了下标重载" << endl;
	return &a[num + 1];
}
asd& asd::operator=(char temp[])
{
	this->a = temp;
	return *this;
}
ostream& operator<<(ostream &o, asd &cls)
{
	cout << "调用了<<重载"<<endl;
	o << cls.a << endl;
	return o;
}
int main()
{
	asd a[3] = { "asdad","ada","asdfdfd" };
	cout << a[2][0] << endl;   //a[2][0]类型为char*
	cout << a[1];              //a[1]类型为asd
    return 0;
}

