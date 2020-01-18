#include<vector>
#include <iostream>
#include "variant.h"  //includeͷ�ļ�

using namespace std;
using namespace croper;  //variant������croper�����ռ���
int Test1(int, vector<string>);
void Swap(int& a, int& b);


int main2() {
	cout << "================================================================================================" << endl;
	cout <<    "1���ܽ��ܲ�����������͵ı���������֧��int��double��bool��string���Լ�vector 5�����͡�"        << endl;
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
	cout << "    2������vector�ػ�Ϊpython���list���ͣ������ɸ����������ͣ���ʵ����vector<variant>)"          << endl;
	cout << "================================================================================================" << endl;
	variant e = { 5,4.99,false,"hello world" };            //ͬʱ���ɶ�����������
	variant f = { {"there","is","no"},12315,"bigbang" }; //֧��Ƕ�׵ĳ�ʼ��
	cout << e << endl;
	cout << f << endl;
	f.append(e);                  //���Լ������Ԫ��
	e.append(f);                   //���ֻ�·Ҳ������ȷʶ��
	cout << f << endl;

	cout << "================================================================================================" << endl;
	cout << "       3������ʹ���ַ�����ʼ��������ʵ�������Ŀ��)										     " << endl;
	cout << "================================================================================================" << endl;
	variant s1 = Variant_Read("5,Bob,false,[48,22,4.99],[4,5],[125,54]");   //ʹ���ַ�����ʼ��
	variant s2 = "5,3,9,[48,2 2,4.99],[4,5],[125,54]"_V;               //���Ӽ򵥵�д��
	cout << s1 << endl;
	cout << s2 << endl;

	variant s3 = R"(\,\\\[552\])"_V;
	cout << s3 << endl;
	
	cout << "================================================================================================" << endl;
	cout << "       4�����Խ�����չ�������ݸ��ض�������												         " << endl;
	cout << "================================================================================================" << endl;
	variant r1 = R"(5,["Allice","Bob","Carol","Dave"])"_V;    
	cout << r1 << endl;
	int x = r1.call(Test1);          //��r1չ�����ݸ�test1
	cout << "����ֵΪ" << x << endl;
	cout << endl;

	variant r2 = { 5,2 };
	cout << "����ǰr2Ϊ" << r2 << endl;
	r2.call(Swap);                   //��r2չ�����ݸ�Swap,Swap����������������ֵ���⽫��ȷ��ӳ��r2��
	cout << "���ú�r2Ϊ" << r2 << endl;

	cout << "================================================================================================" << endl;
	cout << "       5���󲿷���������ع���δ�깤��dict������δ��ɣ������ڴ�   					         " << endl;
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
