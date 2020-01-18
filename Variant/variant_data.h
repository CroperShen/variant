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
#define  VARIANT_REGISTER(T2) IsSame<T,T2>::result ||
		static_assert(
			VARIANT_REGISTER_TYPE false,
			"complier error,unsupported type for template Data!"
			);
#undef VARIANT_REGISTER
	public:
		T data;
		Data():data(T()){};
		Data(const T& t) :data(t) {};
		T& get_data() override;
		const T& get_data() const override;
		std::string mytype() const override;
		virtual std::shared_ptr<variant::IData> prev() const override;
		virtual std::shared_ptr<variant::IData> next() const override;
		std::shared_ptr<variant::IData> copy() const override;
		std::string to_string() const override;
	};


	template<typename T>
	inline T& Data<T>::get_data() {
		return data;
	}

	template<typename T>
	inline const T& Data<T>::get_data() const{
		return data;
	}

	template<typename T>
	inline std::string Data<T>::mytype() const {
		return typeid(data).name();
	}

	template <typename T,typename T2=decltype(++std::declval<T&>())>
	std::shared_ptr<variant::IData> __Data_next(const Data<T>& d) {
		T another = d.data;
		return std::make_shared<Data<T>>(++another);
	}
	
	template <typename>
	std::shared_ptr<variant::IData> __Data_next(...) {
		return nullptr;
	}

	template<typename T>
	inline std::shared_ptr<variant::IData> Data<T>::next() const
	{
		return __Data_next<T>(*this);
	}

	template <typename T, typename T2 = decltype(--std::declval<T&>())>
	std::shared_ptr<variant::IData> __Data_prev(const Data<T>& d) {
		T another = d.data;
		return std::make_shared<Data<T>>(--another);
	}

	template <typename>
	std::shared_ptr<variant::IData> __Data_prev(...) {
		return nullptr;
	}

	template<typename T>
	inline std::shared_ptr<variant::IData> Data<T>::prev() const
	{
		return __Data_prev<T>(*this);
	}


	template<>
	inline std::string Data<std::string>::mytype() const {
		return "string";
	}

	template<>
	inline std::string Data<list>::mytype() const {
		return "list";
	}

	template<typename T>
	inline std::shared_ptr<variant::IData> Data<T>::copy() const {
		return std::make_shared<Data<T>>(*this);
	}

	template<typename T>
	std::string Data<T>::to_string() const {
		return std::to_string(data);
	}

	template <>
	inline std::string Data<std::string>::to_string() const {
		return "\""+data+"\"";
	}

	template <>
	inline std::string Data<bool>::to_string() const {
		return data ? "(true)" : "(false)";
	}


	//使用图的dps算法来构造当data的type为list时的字符串算法
	//不能使用树的dps算法，可能会有环路。
	template <>
	inline std::string Data<std::vector<variant>>::to_string() const {	
		struct ele {
			const list* p;
			size_t index;
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
			size_t &i = stk.top().index;
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