#include <unordered_map>
#include <vector>
#include <typeinfo>
#include "variant.h"
using namespace std;
using namespace croper;



int main() {
	variant a = variant::list();
	variant b = { a,a };
	a.append(b);
	a.append(b);
	variant c = { a ,a};
	cout << c << endl;
	system("pause");
}