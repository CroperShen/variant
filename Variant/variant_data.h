#ifndef __VARIANT_DATA_H
#define __VARIANT_DATA_H
#include <string>
#include <vector>
#include <unordered_map>
#include <stack>
#include "variant.h"

namespace croper {
	template <typename T>
	class Data :public variant::_IData_templ<T> {
	public:
		T data;
		Data() {};
		Data(const T& t) :data(t) {};
		T& get_data() override;
		std::string mytype() const override;
		Data* copy() const override;
		std::string to_string() const override;
	};


	template<typename T>
	inline T& Data<T>::get_data() {
		return data;
	}

	template<typename T>
	inline std::string Data<T>::mytype() const {
		return typeid(*this).name();
	}

	template<typename T>
	inline Data<T> * Data<T>::copy() const {
		return new Data(*this);
	}

	template<typename T>
	inline std::string Data<T>::to_string() const {
		return std::to_string(data);
	}

	template <>
	inline std::string Data<std::string>::to_string() const {
		return data;
	}

	template <>
	inline std::string Data<bool>::to_string() const {
		return data ? "(true)" : "(false)";
	}


	//使用图的dps算法来构造当data的type为list时的字符串算法
	//不能使用树的dps算法，可能会有环路。
	template <>
	inline std::string Data<std::vector<variant>>::to_string() const {	
		using list = variant::list;
		struct ele {
			const list* p;
			int index;
		};
		struct map_ele {
			int state;
			std::string sz;
			map_ele() :state(0), sz("") {};
		};
		std::unordered_map<const list*,map_ele> checked;
		std::stack<ele> stk;
		const list* p = &data;
		stk.push({ p,0 });
		checked[p].sz = "[";
		while (!stk.empty()) {
			checked[p].state = 1;
			int &i = stk.top().index;
			do {
				std::string& sz = checked[p].sz;
				const variant& v = (*p)[i];
				if (!v.is_type<list>()) {
					sz += v.to_string();
					sz += ",";
				}
				else {
					map_ele& e = checked[&v.__My_base<list>()];
					if (e.state == 2) {
						sz += e.sz;
						sz += ',';
					}
					else if (e.state == 1) {
						sz += "[...]";
						sz += ',';
					}
					else
					{
						p = &v.__My_base<list>();
						stk.push({ p,0 });
						checked[p].sz = "[";
						break;
					}
				}
			} while (++i < p->size());
	

			while (stk.top().index == p->size()) {
				if (p->empty()) {
					checked[p].sz.push_back(']');
				}
				else {
					checked[p].sz.back() = ']';
				}
				checked[p].state = 2;
				std::string& sz = checked[p].sz;
				stk.pop();
				if (stk.empty()) break;
				p = stk.top().p;
				checked[p].sz += sz;
				checked[p].sz +=',';
				stk.top().index++;
			}
		}
	
		return checked[&data].sz;
	}

	template <typename T>
	inline std::shared_ptr<variant::IData> variant::CreateData() {
		return std::make_shared<Data<T>>();
	}

	template <typename T>
	inline std::shared_ptr<variant::IData> variant::CreateData(const T& t) {
		return std::make_shared<Data<T>>(t);
	}
};

#endif