#ifndef _VARIANT_STRUCT_IMP_H
#define _VARIANT_STRUCT_IMP_H

#include "variant_struct.h"

namespace croper{
	//=============================================================================
	//variant_struct中模板函数的实现代码
	//=============================================================================


	//*******************************宏代码块************************************************************
	//运算符重载-最终运算部分-解析 == , > , < , >= , <= 
	#pragma warning(push)
	#pragma warning(disable:4804)    //暂时忽视类型不匹配的警告
	#pragma warning(disable:4805)   
	#define OPERATOR_TEMPLATE(OP_NAME, OP_SYMBOL, RET_TYPE,...) 															\
		template <typename T1,typename T2, typename = decltype(std::declval<T1>() OP_SYMBOL std::declval<T2>() )>			\
		RET_TYPE __data_##OP_NAME(const variant::_IData_templ<T1>& t1, const variant::_IData_templ<T2>& t2){				\
			return t1.get_data() OP_SYMBOL t2.get_data();																	\
		}																													\
		constexpr RET_TYPE __data_##OP_NAME(...) {																		\
			return false;																									\
		}																													\

	OPERATOR_TEMPLATE_PART_A()
	#undef OPERATOR_TEMPLATE
	//解析 + , - , * , / , % 
	#define OPERATOR_TEMPLATE(OP_NAME, OP_SYMBOL, RET_TYPE,...)																				\
		template <typename T1,typename T2, typename retT= decltype(std::declval<T1>() OP_SYMBOL std::declval<T2>())>						\
		RET_TYPE __data_##OP_NAME(const variant::_IData_templ<T1>& t1, const variant::_IData_templ<T2>& t2){								\
			return variant::CreateData<retT>(t1.get_data() OP_SYMBOL t2.get_data());														\
		}																																	\
		inline RET_TYPE __data_##OP_NAME(...) {																							\
			return nullptr;																													\
		}																																	\

	OPERATOR_TEMPLATE_PART_B()
	#undef OPERATOR_TEMPLATE
	#pragma warning(pop)
	//==========================================================================================================================
	//在这里重载不同类型的运算
	//===============================================================================================================
	inline std::shared_ptr<variant::IData> __data_mul(const variant::_IData_templ<string>& t1, const variant::_IData_templ<int>& t2);  //字符串乘以整数==>字符串重复数次

	//=========================================================================================
		//_IData_templ的类型特化函数的实现

	#define OPERATOR_TEMPLATE(OP_NAME, OP_SYMBOL, RET_TYPE,T)		                                                        \
				template <typename T2>																						\
				RET_TYPE variant::_IData_templ<T2>::OP_NAME##_##T(const variant::_IData_templ<T>& d2) const {			\
					return __data_##OP_NAME(d2,*this);																	\
				}																												


	#define VARIANT_REGISTER(T)																								\
				template <typename T2>																						\
				T variant::_IData_templ<T2>::_get_data_##T(){																\
					return Judge<CanMatch<T,T2>::result,T,_IData_templ,T2>::get_data(this);									\
				}																											\
				template <typename T2>																						\
				void variant::_IData_templ<T2>::_trans_type_##T(variant* base){											\
					constexpr int d=DistinguishType<T,T2>::result;															\
					Judge<d, T, _IData_templ,T2>::transtype(this,base);														\
				}																											\
				OPERATOR_TEMPLATE_ALL(T)

	VARIANT_REGISTER_TYPE
	#undef OPERATOR_TEMPLATE
	#undef VARIANT_REGISTER

	//***************************************************************************************************************************************


	//------------------------------------------------------------------------
	//IData类的函数
	//-------------------------------------------------------------------------
	//原始数据：类型指定错误会错误
	template<typename T>
	inline T& variant::IData::original_data()
	{
		return dynamic_cast<_IData_templ<T>*>(this)->get_data();
	}


	//----------------------------------------------------------------------------
	//Judge类的函数
	//----------------------------------------------------------------------------

	//当原类型与目标类型相同时
	template<typename T, typename my_type, typename templ_arg>
	inline void variant::Judge<0, T, my_type, templ_arg>::transtype(my_type* self, variant* base)
	{
		//什么也不做
	}

	//当目标类型为list但源类型不为list时，转化为长度为1的list，第一个元素即为自身
	template<typename T, typename my_type, typename templ_arg>
	inline void variant::Judge<1, T, my_type, templ_arg>::transtype(my_type* self, variant* base)
	{
		base->_data = CreateData<list>(list({ *base }));
	}

	//当目标类型不为list但源类型为list时，将第一个元素转化为目标元素
	template<typename T, typename my_type, typename templ_arg>
	inline void variant::Judge<2, T, my_type, templ_arg>::transtype(my_type* self, variant* base)
	{
		*base = base[0];
		base->set_type<T>();
	}

	//当源类型能转化为目标类型时，进行默认转化
	template<typename T, typename my_type, typename templ_arg>
	inline void variant::Judge<3, T, my_type, templ_arg>::transtype(my_type* self, variant* base)
	{
		base->_data = CreateData<T>(static_cast<const T&>(self->get_data()));
	}

	//当源类型不能转化为目标类型时，设为默认值
	template<typename T, typename my_type, typename templ_arg>
	inline void variant::Judge<4, T, my_type, templ_arg>::transtype(my_type* self, variant* base)
	{
		base->_data = CreateData<T>(T());
	}

	template<typename T, typename my_type, typename templ_arg>
	inline T variant::Judge<1, T, my_type, templ_arg>::get_data(my_type* self) {
		return static_cast<T>(self->get_data());
	}

	template<typename T, typename my_type, typename templ_arg>
	inline T variant::Judge<0, T, my_type, templ_arg>::get_data(my_type* self) {
	#ifdef VARIANT_STRICT_TYPE_CHECK
		std::string s = std::string(typeid(T).name()) + "与" + typeid(templ_arg).name() + "的类型无法匹配";
		ErrorMsg(s);
	#endif
		return T();
	}

	//------------------------------------------------------------------------------------
	//variant自身的函数
	//----------------------------------------------------------------------------------------

	//重载equal_to




	template<typename T,typename>
	inline variant::variant(T&& t) {
		operator=(std::forward<T>(t));
	}


	template<typename T>
	inline bool variant::is_type() const
	{
		IData* p = &*_data;
		return dynamic_cast<_IData_templ <T>*>(p) != nullptr;
	}

	template<>
	inline bool variant::is_type<void>() const {
		return !_data;
	}


	template<typename T>
	inline variant& variant::set_type()
	{
		if (!_data) {
			_data = CreateData<T>();
		}
		_data->trans_type<T>(this);
		return *this;
	}


	template<typename ...TArgs>
	struct SendArgs {
		template <typename retT, typename T>
		static retT call(retT(*fp)(TArgs..., T), variant lst_v, int i, TArgs...args) {
			return fp(args..., lst_v[i]);
		}

		template <typename retT, typename T>
		static retT call(retT(*fp)(TArgs..., T&), variant lst_v, int i, TArgs...args) {
			return fp(args..., lst_v[i].__My_base<T>());
		}

		template <typename retT, typename T, typename ...TArgs2>
		static retT call(retT(*fp)(TArgs..., T, TArgs2...), variant lst_v, int i, TArgs...args) {
			return SendArgs<TArgs..., T>::call(fp, lst_v, i + 1, args..., lst_v[i]);
		}

		template <typename retT, typename T, typename ...TArgs2>
		static retT call(retT(*fp)(TArgs..., T&, TArgs2...), variant lst_v, int i, TArgs...args) {
			return SendArgs<TArgs..., T&>::call(fp, lst_v, i + 1, args..., lst_v[i].__My_base<T>());
		}

		template <typename T>
		static void call(void(*fp)(TArgs..., T), variant lst_v, int i, TArgs...args) {
			fp(args..., lst_v[i]);
		}

		template <typename T>
		static void call(void(*fp)(TArgs..., T&), variant lst_v, int i, TArgs...args) {
			fp(args..., lst_v[i].__My_base<T>());
		}

		template < typename T, typename ...TArgs2>
		static void call(void(*fp)(TArgs..., T, TArgs2...), variant lst_v, int i, TArgs...args) {
			SendArgs<TArgs..., T>::call(fp, lst_v, i + 1, args..., lst_v[i]);
		}

		template < typename T, typename ...TArgs2>
		static void call(void(*fp)(TArgs..., T&, TArgs2...), variant lst_v, int i, TArgs...args) {
			SendArgs<TArgs..., T&>::call(fp, lst_v, i + 1, args..., lst_v[i].__My_base<T>());
		}

	};
	template<typename retT, typename ...TArgs>
	inline retT variant::call(retT(*fp)(TArgs...))
	{
		variant temp = *this;
		temp.set_type<list>();
		list l = temp._data->original_data<list>();
		return SendArgs<>::call(fp, temp, 0);
	}

	template<typename ...TArgs>
	inline void variant::call(void(*fp)(TArgs...))
	{
		variant temp = *this;
		temp.set_type<list>();
		list l = temp._data->original_data<list>();
		SendArgs<>::call(fp, temp, 0);
	}


	template<typename T,typename>
	variant& variant::operator=(T&& t)
	{
		
		_data = CreateData<BaseType<std::decay<T>::type>::type>(std::forward<T>(t));
		return *this;
	}


	//赋值函数
	//当右值为std::vector时，类型统一为list(std::vector<variant>)
	template<typename T>
	inline variant& variant::operator=(const std::vector<T>& v)
	{
		_data = CreateData<list>(v);
		return *this;
	}

	template<typename T>
	inline variant::operator T() const
	{
		static_assert(!IsSame<T, unsigned int>::result, "???????????????????????????????");
		if (_data == nullptr) {
			return T();
		}
		return _data->get_data<T>();
	}

	template<typename T>
	inline T& variant::__My_base()
	{
		if (is_type<T>() == false) {
			std::cerr << "待转换的参数类型不匹配,目标是" << typeid(T).name() << ",但源参数类型是" << type() << std::endl;
			assert(0);
		}
		return _data->original_data<T>();
	}

	template<typename T>
	inline const T& variant::__My_base() const
	{
		if (is_type<T>() == false) {
			std::cerr << "待转换的参数类型不匹配,目标是" << typeid(T).name() << ",但源参数类型是" << type() << std::endl;
			assert(0);
		}
		return _data->original_data<T>();
	}


	template <typename T>
	inline variant::operator std::vector<T>() const
	{
		if (!is_type<list>()) {
			ErrorMsg("variant不是list类型，无法转化为std::vector");
			return std::vector<T>();
		}
		std::vector<T> ret;
		for (size_t i = 0; i < this->size(); ++i) {
			ret.push_back(this->operator[](i));
		}
		return ret;
	}

	#define OPERATOR_TEMPLATE(OP_NAME, OP_SYMBOL, RET_TYPE,...)								\
		template<typename T>																\
		inline RET_TYPE variant::_IData_templ<T>::OP_NAME(const IData & d) const		\
		{																					\
			return d.OP_NAME##2(*this);													\
		}																					\

	OPERATOR_TEMPLATE_ALL()
	#undef OPERATOR_TEMPLATE
};







#endif
