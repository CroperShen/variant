//=======================================================================================
//--------------------------------------------------------------------------------------|
//                          variant��Ŀ�ܣ�����Data����								|	
//--------------------------------------------------------------------------------------| 
//=======================================================================================
#ifndef __VARIANT_STRUCT_H
#define __VARIANT_STRUCT_H

#include <share.h>
#include <iostream>
#include <initializer_list>
#include <string>
#include <vector>
#include <assert.h>
#include <unordered_map>
#include "variant_marco.h"
#include "variant_meta.h"
//=========================================================================



namespace croper {
	class variant;
	using list = std::vector<variant>;
	using dict = std::unordered_map<variant, variant>;
	using string = std::string;
//================================================================================
//���忪ʼ
//===============================================================================
	class variant {
	public:
		static const variant None;
//****************************************************************************

		struct IData;
		template <int,typename, typename, typename> struct Judge;
		template <typename> struct _IData_templ;
//==================================================================================
//IData�࣬����Data�Ľӿ�
//==================================================================================
		struct IData {
		public:
			//���������
			virtual std::string mytype() const = 0;
			//ת��Ϊ�ַ�����������__str__����
			virtual std::string to_string() const = 0;
			//��������Ŀ���
			virtual std::shared_ptr<IData> copy() const = 0;
			//������T��ȡ���ݣ��ᷢ�����͵�Ĭ��ת���������Ͳ�ƥ��ʱ�����Ĭ��ֵ,��������˺�VARIANT_STRICT_TYPE_CHECK�Ļ��ᱨ��
			template <typename T> T get_data();
			//������T��ȡ���ݣ��ᷢ�����͵�Ĭ��ת��
			template <typename T> void trans_type(variant*);
			//ԭʼ���ݣ�����ָ����������
			template <typename T> T& original_data();


			//��������������أ���ʹ��operator�Ա������
			virtual std::shared_ptr<IData> next() const = 0;
			virtual std::shared_ptr<IData> prev() const = 0;


			//*******************************������***************************************
#define OPERATOR_TEMPLATE(OP_NAME,OP_SYMBOL,RET_TYPE,...)											\
			virtual RET_TYPE OP_NAME##(const IData&) const=0;										\
			template <typename T> RET_TYPE OP_NAME##2(const _IData_templ<T>&) const;				

		OPERATOR_TEMPLATE_ALL()
#undef OPERATOR_TEMPLATE


#define OPERATOR_TEMPLATE(OP_NAME,OP_SYMBOL,RET_TYPE,T,...)											\
			virtual RET_TYPE OP_NAME##_##T##(const _IData_templ<T>&) const = 0;						\
			template <>  RET_TYPE OP_NAME##2(const _IData_templ<T>& t) const {						\
				return OP_NAME##_##T##(t);															\
			}

#define VARIANT_REGISTER(T)																			\
			virtual T _get_data_##T##() = 0;														\
			virtual void _trans_type_##T##(variant*) = 0;											\
			template<> T get_data<T>(){																\
				return _get_data_##T##();															\
			}																						\
			template<> void trans_type<T>(variant* base) {											\
				_trans_type_##T##(base);															\
			}																						\
			OPERATOR_TEMPLATE_ALL(T)

			
			VARIANT_REGISTER_TYPE;
#undef VARIANT_REGISTER
#undef OPERATOR_TEMPLATE
			//****************************************************************************
		};
		//����Ϊ��Ԫ
		friend struct IData;
		//����Data���������
		template <typename T>
		static std::shared_ptr<IData> CreateData();
		template <typename T>
		static std::shared_ptr<IData> CreateData(const T& t);

//========================================================================================
//Judge�࣬�����࣬���ݲ�ͬ��ģ�����������ͬ�ĺ���									
//========================================================================================
		template <int N, typename T, typename my_type, typename templ_arg>
		struct Judge;
		//Judge<0>
		template <typename T, typename my_type, typename templ_arg>
		struct Judge<0, T, my_type, templ_arg> {
			static T get_data(my_type*);
			static void transtype(my_type*, variant*);
		};
		//Judge<1>
		template <typename T, typename my_type, typename templ_arg>
		struct Judge<1, T, my_type, templ_arg> {
			static T get_data(my_type* self);
			static void transtype(my_type*, variant*);
		};

		//Judge<2>
		template <typename T, typename my_type, typename templ_arg>
		struct Judge<2, T, my_type, templ_arg> {
			static void transtype(my_type*, variant*);
		};

		//Judge<3>
		template <typename T, typename my_type, typename templ_arg>
		struct Judge<3, T, my_type, templ_arg> {
			static void transtype(my_type*, variant*);
		};

		//Judge<4>
		template <typename T, typename my_type, typename templ_arg>
		struct Judge<4, T, my_type, templ_arg> {
			static void transtype(my_type*, variant*);
		};
		//========================================================================================
		//DistinguishType�࣬�����࣬����type��ͬʱ����0����һ����listʱ����1���ڶ�����listʱ����2����ת��ʱ����3������ת��ʱ����4								
		//========================================================================================
	public:
		template <typename Dest, typename Orig>
		struct DistinguishType {
		public:
			enum { result = 4 - CanMatch<Dest, Orig>::result };
		};
		template <typename T>
		struct DistinguishType<T, T> {
			enum { result = 0 };
		};
		template <typename T>
		struct DistinguishType<list, T> {
			enum { result = 1 };
		};
		template <typename T>
		struct DistinguishType<T, list> {
			enum { result = 2 };
		};
		template <>
		struct DistinguishType<list, list> {
			enum { result = 0 };
		};
		//========================================================================================
		//_IData_templ�࣬������
		//ģ�������IData��ʹ�����_get_data_int,_get_data_double�ȶ�ָ��ͬһ��get_data,��ʹ��Judge�ж��������Ƿ�ƥ�䣬
		//��ƥ��ʱ���佫ָ��һ������ĺ���
		//���ʹ��if���ᵼ����Ȼ��һ����Զ����ִ�е����޷�ͨ�����롣
		//========================================================================================

		template <typename templ_arg>
		struct _IData_templ :public IData {
			virtual templ_arg& get_data() = 0;
			virtual const templ_arg& get_data() const = 0;

			//*******************************������***************************************
#define OPERATOR_TEMPLATE(OP_NAME, OP_SYMBOL, RET_TYPE,...)	\
			virtual RET_TYPE OP_NAME(const IData&) const override;		
		OPERATOR_TEMPLATE_ALL()
#undef	OPERATOR_TEMPLATE

#define OPERATOR_TEMPLATE(OP_NAME, OP_SYMBOL, RET_TYPE,T)	\
			virtual  RET_TYPE OP_NAME##_##T##(const _IData_templ<T>&) const override;				

#define VARIANT_REGISTER(T) \
			virtual T _get_data_##T##() override;													\
			virtual void _trans_type_##T##(variant* base) override;									\
			OPERATOR_TEMPLATE_ALL(T)

			VARIANT_REGISTER_TYPE;
#undef VARIANT_REGISTER
#undef	OPERATOR_TEMPLATE
			//****************************************************************************
		};
		//=====================================================================
		//variant������ĺ���
		//=====================================================================
	private:
		//ʹ������ָ��ָ��_data,����������Ҫ��_data�����ڴ�������ܽ���ǳ������
		//python����Ҳ�����ƵĻ���
		std::shared_ptr<IData> _data;
		//====================================================================

	public:
		//���캯��
		variant();
		//����ԭʼ���ݹ���
		template <typename T> variant(const T& t);
		//���ݶ��ԭʼ���ݹ�����б�
		variant(const std::initializer_list<variant>&);
		template <typename ...T> variant(const T&... t);
		//���ƹ��캯��,����Ҫ��ֱ��ǳ���Ƽ���
		variant(const variant&) = default;
		//��������,����Ҫ
		~variant() = default;

		//��������Լ�data�ĵ�ַ��Ϣ
		std::string __My_Address() const;
		//���ַ�����ʽ���Data������
		std::string type() const;
		//Data�Ƿ���ĳ���ض�����
		template <typename T>bool is_type() const;
		//��Dataת��Ϊĳ���ض����ͣ���������ݻ�ת��Ϊ��ֵ
		template <typename T>variant& set_type();
		//����������ΪNone
		void clear();
		//ת��Ϊ�ַ�����
		std::string to_string() const;
		//��������Ϊ��������function
		template <typename retT, typename ...TArgs> retT call(retT(*fp)(TArgs...));
		//��������Ϊ��������function,����ֵΪvoid�����ذ汾
		template <typename ...TArgs> void call(void(*fp)(TArgs...));

		//������Ϊlistʱ��β������һ��Ԫ�ء�
		void append(const variant&);
		//������Ϊlistʱ���list�Ĵ�С
		size_t size() const;
		//������Ϊlistʱ����β��Ԫ��
		variant& back();

		//��ֵ����
		template <typename T> variant& operator=(const T&);
		//��ֵ������std::vector���ذ汾
		template <typename T> variant& operator=(const std::vector<T>&);
		//��ֵ������const char*���ذ汾���Խ������泣��
		variant& operator=(const char* sz);
		//���ƹ��캯��������Ҫ��д
		variant& operator=(const variant& v) = default;


		//ת��Ϊ�ض�����
		template <typename T> operator T() const;
		//ת��Ϊ�ض����ͣ�std::vector���ذ汾
		template <typename T> operator std::vector<T>() const;
		//��ȡԭʼ���ݣ�����
		template <typename T> T& __My_base();
		template <typename T> const T& __My_base() const;

		//================================�����Ǹ��������������==============================

		//����ɲ���
		//����<<ʵ�����
		friend std::ostream& operator<<(std::ostream& os, const variant&);
		//����[]

		variant& operator[](int i);
		const variant& operator[](int i) const;


		bool operator==(const variant&) const;
		bool operator!=(const variant&) const;
		bool operator<(const variant&) const;
		bool operator>(const variant&) const;
		bool operator>=(const variant&) const;
		bool operator<=(const variant&) const;

		variant& operator++();
		variant& operator--();
		variant operator++(int);
		variant operator--(int);

		variant operator+ (const variant&) const;
		variant& operator+=(const variant&);

		variant operator- (const variant&) const;
		variant& operator-=(const variant&);

		variant operator* (const variant&) const;
		variant& operator*=(const variant&);

		variant operator/ (const variant&) const;
		variant& operator/=(const variant&);

		variant operator% (const variant&) const;
		variant& operator%=(const variant&);

	};
	//=============================================================================
	//�������ܱߺ���������
	//=============================================================================
	//������Ϣ��debugģʽ�����������Ϣ��releaseģʽ��ֱ���׳��쳣
	void ErrorMsg(std::string s);
	//���ַ���ת��Ϊvariant����ɫ����
	variant Variant_Read(const std::string&);
	//�������ַ������_V�Լ�ʵ����������
	variant operator ""_V(const char* p, size_t s);

	 

	//=============================================================================
	//����������������ģ�庯����ʵ�ִ���
	//=============================================================================

		
	//*******************************������************************************************************
	//���������-�������㲿��-���� == , > , < , >= , <= 
#pragma warning(push)
#pragma warning(disable:4804)    //��ʱ�������Ͳ�ƥ��ľ���
#pragma warning(disable:4805)   
#define OPERATOR_TEMPLATE(OP_NAME, OP_SYMBOL, RET_TYPE,...) 															\
	template <typename T1,typename T2, typename = decltype(std::declval<T2>() OP_SYMBOL std::declval<T1>() )>			\
	RET_TYPE __data_##OP_NAME##(const variant::_IData_templ<T1>& t1, const variant::_IData_templ<T2>& t2){				\
		return t2.get_data() OP_SYMBOL t1.get_data();																	\
	}																													\
	constexpr RET_TYPE __data_##OP_NAME##(...) {																		\
		return false;																									\
	}																													\

	OPERATOR_TEMPLATE_PART_A()
#undef OPERATOR_TEMPLATE
	//���� + , - , * , / , % 
#define OPERATOR_TEMPLATE(OP_NAME, OP_SYMBOL, RET_TYPE,...)																				\
	template <typename T1,typename T2, typename retT= decltype(std::declval<T2>() OP_SYMBOL std::declval<T1>())>						\
	RET_TYPE __data_##OP_NAME##(const variant::_IData_templ<T1>& t1, const variant::_IData_templ<T2>& t2){								\
		return variant::CreateData<retT>(t2.get_data() OP_SYMBOL t1.get_data());														\
	}																																	\
	inline RET_TYPE __data_##OP_NAME##(...) {																							\
		return nullptr;																													\
	}																																	\

	OPERATOR_TEMPLATE_PART_B()
#undef OPERATOR_TEMPLATE
#pragma warning(pop)

	//_IData_templ�������ػ�������ʵ��

#define OPERATOR_TEMPLATE(OP_NAME, OP_SYMBOL, RET_TYPE,T)		                                                        \
			template <typename T2>																						\
			RET_TYPE variant::_IData_templ<T2>::##OP_NAME##_##T##(const variant::_IData_templ<T>& d2) const {			\
				return __data_##OP_NAME##(*this,d2);																	\
			}																												


#define VARIANT_REGISTER(T)																								\
			template <typename T2>																						\
			T variant::_IData_templ<T2>::_get_data_##T##(){																\
				return Judge<CanMatch<T,T2>::result,T,_IData_templ,T2>::get_data(this);									\
			}																											\
			template <typename T2>																						\
			void variant::_IData_templ<T2>::_trans_type_##T##(variant* base){											\
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
	inline T & variant::IData::original_data()
	{
		return dynamic_cast<_IData_templ<T>*>(this)->get_data();
	}
	

	//----------------------------------------------------------------------------
	//Judge��ĺ���
	//----------------------------------------------------------------------------

	//��ԭ������Ŀ��������ͬʱ
	template<typename T, typename my_type, typename templ_arg>
	inline void variant::Judge<0, T, my_type, templ_arg>::transtype(my_type * self, variant * base)
	{
		//ʲôҲ����
	}

	//��Ŀ������Ϊlist��Դ���Ͳ�Ϊlistʱ��ת��Ϊ����Ϊ1��list����һ��Ԫ�ؼ�Ϊ����
	template<typename T, typename my_type, typename templ_arg>
	inline void variant::Judge<1, T, my_type, templ_arg>::transtype(my_type * self, variant * base)
	{
		base->_data = CreateData<list>(list({ *base }));
	}

	//��Ŀ�����Ͳ�Ϊlist��Դ����Ϊlistʱ������һ��Ԫ��ת��ΪĿ��Ԫ��
	template<typename T, typename my_type, typename templ_arg>
	inline void variant::Judge<2, T, my_type, templ_arg>::transtype(my_type * self, variant * base)
	{
		*base = base[0];
		base->set_type<T>();
	}

	//��Դ������ת��ΪĿ������ʱ������Ĭ��ת��
	template<typename T, typename my_type, typename templ_arg>
	inline void variant::Judge<3, T, my_type, templ_arg>::transtype(my_type * self, variant * base)
	{
		base->_data = CreateData<T>(static_cast<const T&>(self->get_data()));
	}

	//��Դ���Ͳ���ת��ΪĿ������ʱ����ΪĬ��ֵ
	template<typename T, typename my_type, typename templ_arg>
	inline void variant::Judge<4, T, my_type, templ_arg>::transtype(my_type * self, variant * base)
	{
		base->_data = CreateData<T>(T());
	}

	template<typename T, typename my_type, typename templ_arg>
	inline T variant::Judge<1, T, my_type, templ_arg>::get_data(my_type * self) {
		return static_cast<T>(self->get_data());
	}

	template<typename T, typename my_type, typename templ_arg>
	inline T variant::Judge<0, T, my_type, templ_arg>::get_data(my_type * self) {
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




	template<typename T>
	inline variant::variant(const T & t) {
		operator=(t);
	}


	template<typename T>
	inline bool variant::is_type() const
	{
		IData *p = &*_data;
		return dynamic_cast<_IData_templ <T>*>(p) != nullptr;
	}
	
	template<>
	inline bool variant::is_type<void>() const {
		return !_data;
	}


	template<typename T>
	inline variant & variant::set_type()
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


	template<typename T>
	variant & variant::operator=(const T & t)
	{
		_data = CreateData<T>(t);
		return *this;
	}


	//��ֵ����
	//����ֵΪstd::vectorʱ������ͳһΪlist(std::vector<variant>)
	template<typename T>
	inline variant & variant::operator=(const std::vector<T>& v)
	{
		_data = CreateData<list>(v);
		return *this;
	}

	template<typename T>
	inline variant::operator T() const
	{
		if (_data == nullptr) {
			return T();
		}
		return _data->get_data<T>();
	}



	template<typename T>
	inline T & variant::__My_base()
	{
		if (is_type<T>() == false) {
			std::cerr << "��ת���Ĳ������Ͳ�ƥ��,Ŀ����" << typeid(T).name() << ",��Դ����������" << type() << std::endl;
			assert(0);
		}
		return _data->original_data<T>();
	}

	template<typename T>
	inline const T & variant::__My_base() const
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
	inline RET_TYPE variant::_IData_templ<T>::##OP_NAME##(const IData & d) const		\
	{																					\
		return d.##OP_NAME##2(*this);													\
	}																					\

OPERATOR_TEMPLATE_ALL()											
#undef OPERATOR_TEMPLATE

};

#endif