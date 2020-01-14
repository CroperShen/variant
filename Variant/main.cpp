#define NO_DATA_TEST

#include "variant.h"
#include <iostream>
using namespace std;
using namespace croper;
int func(vector<int> a, vector<string> b) {
	for (int i = 0; i < a.size(); ++i) {
		for (int j = 0; j < a[i]; ++j) {
			cout << b[i] << endl;
		}
	}
	return 0;
}
int main() {
	R"([1,2,3,4,5][mama,baba,popo,yeye,nainai])"_V.call(func);
	system("pause");
}