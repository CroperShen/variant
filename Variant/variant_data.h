#ifndef __VARIANT_DATA_H
#define __VARIANT_DATA_H

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

	template <>
	inline std::string Data<std::vector<variant>>::to_string() const {
		if (data.empty()) return "[]";
		std::string ret = "[";
		for (auto &v : data) {
			ret += v.to_string();
			ret.push_back(',');
		}
		ret.back() = ']';
		return ret;
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