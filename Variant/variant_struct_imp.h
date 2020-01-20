#ifndef _VARIANT_STRUCT_IMP_H
#define _VARIANT_STRUCT_IMP_H

#include "variant_struct.h"

namespace croper{
	//=============================================================================
	//variant_struct��ģ�庯����ʵ�ִ���
	//=============================================================================


	//*******************************������************************************************************
	//���������-�������㲿��-���� == , > , < , >= , <= 
	#pragma warning(push)
	#pragma warning(disable:4804)    //��ʱ�������Ͳ�ƥ��ľ���
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
	//���� + , - , * , / , % 
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
	//���������ز�ͬ���͵�����
	//===============================================================================================================
	inline std::shared_ptr<variant::IData> __data_mul(const variant::_IData_templ<string>& t1, const variant::_IData_templ<int>& t2);  //�ַ�����������==>�ַ����ظ�����

	//=========================================================================================
		//_IData_templ�������ػ�������ʵ��

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
	//IData��ĺ���
	//-------------------------------------------------------------------------
	//ԭʼ���ݣ�����ָ����������
	template<typename T>
	inline T& variant::IData::original_data()
	{
		return dynamic_cast<_IData_templ<T>*>(this)->get_data();
	}


	//----------------------------------------------------------------------------
	//Judge��ĺ���
	//----------------------------------------------------------------------------

	//��ԭ������Ŀ��������ͬʱ
	template<typename T, typename my_type, typename templ_arg>
	inline void variant::Judge<0, T, my_type, templ_arg>::transtype(my_type* self, variant* base)
	{
		//ʲôҲ����
	}

	//��Ŀ������Ϊlist��Դ���Ͳ�Ϊlistʱ��ת��Ϊ����Ϊ1��list����һ��Ԫ�ؼ�Ϊ����
	template<typename T, typename my_type, typename templ_arg>
	inline void variant::Judge<1, T, my_type, templ_arg>::transtype(my_type* self, variant* base)
	{
		base->_data = CreateData<list>(list({ *base }));
	}

	//��Ŀ�����Ͳ�Ϊlist��Դ����Ϊlistʱ������һ��Ԫ��ת��ΪĿ��Ԫ��
	template<typename T, typename my_type, typename templ_arg>
	inline void variant::Judge<2, T, my_type, templ_arg>::transtype(my_type* self, variant* base)
	{
		*base = base[0];
		base->set_type<T>();
	}

	//��Դ������ת��ΪĿ������ʱ������Ĭ��ת��
	template<typename T, typename my_type, typename templ_arg>
	inline void variant::Judge<3, T, my_type, templ_arg>::transtype(my_type* self, variant* base)
	{
		base->_data = CreateData<T>(static_cast<const T&>(self->get_data()));
	}

	//��Դ���Ͳ���ת��ΪĿ������ʱ����ΪĬ��ֵ
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
		std::string s = std::string(typeid(T).name()) + "��" + typeid(templ_arg).name() + "�������޷�ƥ��";
		ErrorMsg(s);
	#endif
		return T();
	}

	//------------------------------------------------------------------------------------
	//variant����ĺ���
	//----------------------------------------------------------------------------------------

	//����equal_to




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


	//��ֵ����
	//����ֵΪstd::vectorʱ������ͳһΪlist(std::vector<variant>)
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
			std::cerr << "��ת���Ĳ������Ͳ�ƥ��,Ŀ����" << typeid(T).name() << ",��Դ����������" << type() << std::endl;
			assert(0);
		}
		return _data->original_data<T>();
	}

	template<typename T>
	inline const T& variant::__My_base() const
	{
		if (is_type<T>() == false) {
			std::cerr << "��ת���Ĳ������Ͳ�ƥ��,Ŀ����" << typeid(T).name() << ",��Դ����������" << type() << std::endl;
			assert(0);
		}
		return _data->original_data<T>();
	}


	template <typename T>
	inline variant::operator std::vector<T>() const
	{
		if (!is_type<list>()) {
			ErrorMsg("variant����list���ͣ��޷�ת��Ϊstd::vector");
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
