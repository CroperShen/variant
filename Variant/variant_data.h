#ifndef __VARIANT_DATA_H
#define __VARIANT_DATA_H
#include <string>
#include <vector>
#include <unordered_map>
#include <stack>
#include "variant_struct.h"

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
		Data() :data(T()) {};
		Data(const T& t) :data(t) {};
		T& get_data() override;
		const T& get_data() const override;
		std::string mytype() const override;
		virtual std::shared_ptr<variant::IData> prev() const override;
		virtual std::shared_ptr<variant::IData> next() const override;
		inline size_t hashcode() const override;
		std::shared_ptr<variant::IData> copy() const override;
		std::string to_string() const override;
	};
}
#endif