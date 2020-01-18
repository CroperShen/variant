#include<vector>
#include <iostream>
#include "variant.h"  //include头文件

using namespace std;
using namespace croper;  //variant定义在croper命名空间下
int Test1(int, vector<string>);
void Swap(int& a, int& b);


int main2() {
	cout << "================================================================================================" << endl;
	cout <<    "1、能接受并输出多种类型的变量，现在支持int，double，bool，string，以及vector 5种类型。"        << endl;
	cout << "================================================================================================" << endl;
	variant a = { 5 };                      //int
	variant b = 4.99;                   //double
	variant c = "hello world";			//string
	variant d = false;                  //bool
	variant o;
	cout << a << endl;
	cout << b << endl;
	cout << c << endl;
	cout << d << endl;
	cout << o << endl;
	cout << "================================================================================================" << endl;
	cout << "    2、其中vector特化为python里的list类型，能容纳各种数据类型（其实就是vector<variant>)"          << endl;
	cout << "================================================================================================" << endl;
	variant e = { 5,4.99,false,"hello world" };            //同时容纳多种数据类型
	variant f = { {"there","is","no"},12315,"bigbang" }; //支持嵌套的初始化
	cout << e << endl;
	cout << f << endl;
	f.append(e);                  //可以继续添加元素
	e.append(f);                   //出现环路也可以正确识别
	cout << f << endl;

	cout << "================================================================================================" << endl;
	cout << "       3、可以使用字符串初始化（这其实是最初的目的)										     " << endl;
	cout << "================================================================================================" << endl;
	variant s1 = Variant_Read("5,Bob,false,[48,22,4.99],[4,5],[125,54]");   //使用字符串初始化
	variant s2 = "5,3,9,[48,2 2,4.99],[4,5],[125,54]"_V;               //更加简单的写法
	cout << s1 << endl;
	cout << s2 << endl;

	variant s3 = R"(\,\\\[552\])"_V;
	cout << s3 << endl;
	
	cout << "================================================================================================" << endl;
	cout << "       4、可以将自身展开并传递给特定函数：												         " << endl;
	cout << "================================================================================================" << endl;
	variant r1 = R"(5,["Allice","Bob","Carol","Dave"])"_V;    
	cout << r1 << endl;
	int x = r1.call(Test1);          //将r1展开传递给test1
	cout << "返回值为" << x << endl;
	cout << endl;

	variant r2 = { 5,2 };
	cout << "调用前r2为" << r2 << endl;
	r2.call(Swap);                   //将r2展开传递给Swap,Swap将交换两个参数的值，这将正确反映到r2上
	cout << "调用后r2为" << r2 << endl;

	cout << "================================================================================================" << endl;
	cout << "       5、大部分运算符重载工作未完工，dict类型尚未完成，敬请期待   					         " << endl;
	cout << "================================================================================================" << endl;
	system("pause");
	return 0;
}

int Test1(int n, vector<string> v) {
	int ret = 0;
	for (auto s : v) {
		for (int i = 0; i < n; ++i) {
			cout << s << endl;
		}
		cout << endl;
		ret += s.size();
	}
	return ret;
}
void Swap(int& a, int& b) {
	int c = a;
	a = b;
	b = c;
}


int main() {
}
