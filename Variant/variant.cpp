
#include <sstream>
#include <stack>
#include <string>
#include "variant.h"
using namespace std;
namespace croper {
	const variant variant::None = variant();
	using list = variant::list;
	void ErrorMsg(string s)
	{
#ifdef _DEBUG
		cerr << s << endl;
#else
		throw "s";
#endif
	}

	ostream & operator<<(ostream & os, const variant &t)
	{
		if (t._data == nullptr) {
			os << "(None)";
			return os;
		}
		os << t._data->to_string();
		return os;
	}

	//========================================================================================
	//以下是variant的函数



	//构造函数
	variant::variant() {};
	variant::variant(const initializer_list<variant>& l)
	{
		set_type<list>();
		for (auto v : l) {
			append(v);
		}
	}
	variant Variant_Read_Simple(string& sz, int digit_type) {
		variant v0;
		if (sz.empty()) {
			return v0;
		}
		else if (digit_type == 0) {
			stringstream ss(sz);
			int i;
			ss >> i;
			v0 = i;
		}
		else if (digit_type == 1) {
			stringstream ss(sz);
			double d;
			ss >> d;
			v0 = d;
		}
		else if (digit_type == 2) {
			if (sz == "true" || sz == "True") {
				v0 = true;
			}
			else if (sz == "false" || sz == "False") {
				v0 = false;
			}
			else {
				if (sz.size() >= 2 && sz[0] == sz.back() && (sz[0] == '\'' || sz[0] == '\"')) {
					sz = sz.substr(1, sz.size() - 2);
				}
				v0 = sz;
			}
		}
		return v0;
	}
	variant Variant_Read(const string& sz)
	{
		variant ret;
		stack<variant*> stk;
		stk.push(&ret);
		bool Recording = false;		//是否正在记录
		int digit_type = 0;			//0为整数，1为浮点数，2为字符串
		bool transfered = false;	//转义标签，上一个字符串是否为转义符。
		string temp;

		ret.set_type<list>();
		for (const char* p = sz.c_str();; p++) {
			const char& c = *p;
			variant& v = *stk.top();
			if (Recording && c == '\0' || !transfered && c == ',' || Recording && !transfered && (c == '[' || c == ']')) {  //结算当前字符串
				variant v0= Variant_Read_Simple(temp,digit_type);
				v.append(v0);
				Recording = false;
				if (*p == ',') {
					continue;
				}
			}
			if (!transfered && *p == '[') {
				v.append(variant());
				v.back().set_type<list>();
				stk.push(&v.back());
				continue;
			}
			else if (!transfered && *p == ']') {
				stk.pop();
				if (stk.empty()) {
					break;
				}
				continue;
			}
			else if (!transfered && !Recording && (*p == ' ' || *p == '\n' || *p == '\t')) {
				continue;
			}
			else if (*p == '\0') {
				break;
			}
			else {
				if (!transfered && *p == '//') {
					transfered = true;
					continue;
				}
				if (!Recording) {
					Recording = true;
					digit_type = 0;
					temp.clear();
				}
				while (digit_type < 2) {
					if (!isdigit(*p) && *p != '-' && *p != '.') {
						digit_type = 2;
						break;
					}
					if (*p == '-' && !temp.empty()) {
						digit_type = 2;
						break;
					}
					if (*p == '.') {
						digit_type += 1;
						break;
					}
					break;
				}
				temp.push_back(*p);
			}
		}
		if (ret.size() == 0) {
			return variant();
		}
		else if (ret.size() == 1) {
			return ret[0];
		}
		return ret;
	}

	void variant::clear()
	{
		_data = nullptr;
	}
	string variant::to_string() const
	{
		if (_data == nullptr) {
			return "(None)";
		}
		return _data->to_string();
	}
	void variant::append(const variant & v)
	{
		if (!is_type<list>()) {
			ErrorMsg("错误:variant的类型不是list，无法使用append方法");
			return;
		}
		_data->original_data<list>().push_back(v);
	}

	size_t variant::size() const
	{
		if (!is_type<list>()) {
			ErrorMsg("错误:variant的类型不是list，无法使用size方法");
			return -1;
		}
		return _data->original_data<list>().size();
	}

	variant& variant::back()
	{
		if (!is_type<list>()) {
			ErrorMsg("错误:variant的类型不是list，无法使用back方法");
			return *this;
		}
		return _data->original_data<list>().back();
	}

	variant & variant::operator=(const char * sz)
	{
		_data = CreateData<string>(sz);
		return *this;
	}


	variant & variant::operator[](int i)
	{
		if (is_type<list>()) {
			return _data->original_data<list>()[i];
		}
#ifdef VARIANT_REGISTER_TYPE
		ErrorMsg("variant的内部类型不是list");
#endif
		return *this;
	}

	const variant & variant::operator[](int i) const
	{
		if (is_type<list>()) {
			return _data->original_data<list>()[i];
		}
#ifdef VARIANT_REGISTER_TYPE
		ErrorMsg("variant的内部类型不是list");
#endif
		return *this;
	}

	bool variant::operator==(const variant & v2)
	{
		if (_data == v2._data) return true;
		return false;
	}

	string variant::type() const
	{
		if (_data == nullptr) {
			return "void";
		}
		return _data->mytype();
	}

	variant operator ""_V(const char* p, size_t s) {
		return Variant_Read(p);
	}
}