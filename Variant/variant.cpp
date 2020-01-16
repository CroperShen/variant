
#include <sstream>
#include <stack>
#include <string>
#include "variant.h"
using namespace std;
namespace croper {
	const variant variant::None = variant();
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
	//������variant�ĺ���




	//���캯��
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
		bool Recording = false;		//�Ƿ����ڼ�¼
		int digit_type = 0;			//0Ϊ������1Ϊ��������2Ϊ�ַ���
		bool transfered = false;	//ת���ǩ����һ���ַ����Ƿ�Ϊת�����
		bool quotes_start = false;  //�Ƿ������ſ�ʼ�������ſ�ʼҲӦ�������Ž�����
		char prev_end = 0;          //��һ��������������ķ���
		string temp;

		ret.set_type<list>();
		for (const char* p = sz.c_str();; p++) {
			const char& c = *p;
			variant& v = *stk.top();

			bool paragraph_end_flag = false;            //�Ƿ�Ӧ�ý�������¼��ǰ�ַ���
			do {
				if (Recording && c == '\0') {
					paragraph_end_flag = true;
				}
				if (c == '\0') break;
				if (transfered) break;

				if (!Recording && prev_end==',' && c == ',') {
					paragraph_end_flag = true;
				}
				if (!Recording) break;

				if (quotes_start && (c == '\"' || c == '\0')) {
					paragraph_end_flag = true;
				}
				if (quotes_start) break;

				if (c == '\0' || c==','||c == '\"' || c == '[' || c == ']' || c == '\n' || c == '\t' || c == ' ') {
					paragraph_end_flag = true;
				}
			} while (false);
	
			

			if (paragraph_end_flag) {  //���㵱ǰ�ַ���
				variant v0= Variant_Read_Simple(temp,digit_type);
				v.append(v0);
				prev_end = c;
				Recording = false;
				quotes_start = false;
				if (prev_end == '\"') {
					continue;
				}
			}
			if (c == '\0') {
				break;
			}

			if (!transfered && c== '[') {
				v.append(variant());
				v.back().set_type<list>();
				stk.push(&v.back());
				continue;
			}
			if (!transfered && c == ']') {
				stk.pop();
				if (stk.empty()) {
					break;
				}
				continue;
			}
			if (!transfered && !Recording && (c == ' ' || c == '\n' || c == '\t' || c == ',')) {
				continue;
			}
			if (!transfered && c == '\\') {
				transfered = true;
				continue;
			}

			transfered = false;
			if (!Recording) {
				Recording = true;
				digit_type = 0;
				temp.clear();
				if (c == '\"') {
					quotes_start = true;
					digit_type = 2;
				}
				else {
					temp.push_back(c);
				}
			}
			else {
				temp.push_back(c);
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
			ErrorMsg("����:variant�����Ͳ���list���޷�ʹ��append����");
			return;
		}
		_data->original_data<list>().push_back(v);
	}

	size_t variant::size() const
	{
		if (!is_type<list>()) {
			ErrorMsg("����:variant�����Ͳ���list���޷�ʹ��size����");
			return -1;
		}
		return _data->original_data<list>().size();
	}

	variant& variant::back()
	{
		if (!is_type<list>()) {
			ErrorMsg("����:variant�����Ͳ���list���޷�ʹ��back����");
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
		ErrorMsg("variant���ڲ����Ͳ���list");
#endif
		return *this;
	}

	const variant & variant::operator[](int i) const
	{
		if (is_type<list>()) {
			return _data->original_data<list>()[i];
		}
#ifdef VARIANT_REGISTER_TYPE
		ErrorMsg("variant���ڲ����Ͳ���list");
#endif
		return *this;
	}

	bool variant::operator==(const variant & v1) const
	{
		if (v1._data == this->_data) {
			return true;
		}
		if (v1._data == nullptr || this->_data == nullptr) {
			return false;
		}
		return *this->_data==*v1._data;
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