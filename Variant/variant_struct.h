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
#include "variant_meta.h"

namespace croper {

//================================================================================
//���忪ʼ
//===============================================================================
	class variant {
	private:
		using string = std::string;
	public:
		using list = std::vector<variant>;
		static const variant None;
#ifndef VARIANT_REGISTER_TYPE
#define VARIANT_REGISTER_TYPE
#endif
//****************************************************************************


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
			virtual IData* copy() const = 0;
			//������T��ȡ���ݣ��ᷢ�����͵�Ĭ��ת���������Ͳ�ƥ��ʱ�����Ĭ��ֵ,��������˺�VARIANT_STRICT_TYPE_CHECK�Ļ��ᱨ��
			template <typename T> T get_data();
			//������T��ȡ���ݣ��ᷢ�����͵�Ĭ��ת��
			template <typename T> void trans_type(variant*);
			//ԭʼ���ݣ�����ָ����������
			template <typename T> T& original_data();


			//*******************************������***************************************
					//get_data��astype��ģ�壬�����޷��̳У�������get_data����_get_data_int,_get_data_float����ʵ�ּ̳�
#define VARIANT_REGISTER(T) \
			virtual T _get_data_##T##() = 0;\
			virtual void _trans_type_##T##(variant*) = 0;\
			template<> T get_data<T>(){\
				return _get_data_##T##();\
			}\
			template<> void trans_type<T>(variant* base) {\
				_trans_type_##T##(base);\
			}
			VARIANT_REGISTER_TYPE;
#undef VARIANT_REGISTER
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
			//*******************************������***************************************
#define VARIANT_REGISTER(T) \
			virtual T _get_data_##T##() override{\
				return Judge<CanMatch<T,templ_arg>::result,T,_IData_templ,templ_arg>::get_data(this);\
			}\
			virtual void _trans_type_##T##(variant* base) override{\
				constexpr int d=DistinguishType<T,templ_arg>::result;\
				Judge<d, T, _IData_templ, templ_arg>::transtype(this,base);\
			}
			VARIANT_REGISTER_TYPE;
#undef VARIANT_REGISTER
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
		//const�汾
		const variant& operator[](int i) const;


		//δ��ɲ���
		//�������е����ʱ������д
		bool operator==(const variant&);
		template <typename T> bool operator==(const T&);
		//template <typename T> bool friend operator== (const T&, const variant&);

		bool operator!=(const variant&);
		template <typename T> bool operator!=(const T&);
		//template <typename T> bool friend operator!=(const T&, const variant&);

		variant& operator++();
		variant& operator--();
		variant operator++(int);
		variant operator--(int);

		variant& operator+ (const variant&);
		template <typename T> variant& operator+(const T&);
		template <typename T> friend variant& operator+(const T&, const variant&);
		variant& operator+=(const variant&);
		template <typename T> variant& operator+= (const T&);

		variant& operator- (const variant&);
		template <typename T> variant& operator-(const T&);
		template <typename T> friend variant& operator-(const T&, const variant&);
		variant& operator-=(const variant&);
		template <typename T> variant& operator-=(const T&);

		variant& operator* (const variant&);
		template <typename T> variant& operator*(const T&);
		template <typename T> friend variant& operator*(const T&, const variant&);
		variant& operator*=(const variant&);
		template <typename T> variant& operator*=(const T&);

		variant& operator/ (const variant&);
		template <typename T> variant& operator/(const T&);
		template <typename T> friend variant& operator/(const T&, const variant&);
		variant& operator/=(const variant&);
		template <typename T> variant& operator/=(const T&);

		variant& operator% (const variant&);
		template <typename T> variant& operator%(const T&);
		template <typename T> friend variant& operator%(const T&, const variant&);
		variant& operator%=(const variant&);
		template <typename T> variant& operator%=(const T&);


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
		base->_data = CreateData<T>(self->get_data());
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
			return SendArgs<TArgs..., T>::call(fp, lst_v, i + 1, args..., lst_v[i].__My_base<T>());
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
			SendArgs<TArgs..., T>::call(fp, lst_v, i + 1, args..., lst_v[i].__My_base<T>());
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
		return _data->original_data<T>();
	}

	template<typename T>
	inline const T & variant::__My_base() const
	{
		return _data->original_data<T>();
	}

	template<typename T>
	inline variant::operator std::vector<T>() const
	{
		if (!is_type<list>()) {
			ErrorMsg("variant����list���ͣ��޷�ת��Ϊstd::vector");
			return std::vector<T>();
		}
		std::vector<T> ret;
		for (int i = 0; i < this->size(); ++i) {
			ret.push_back(this->operator[](i));
		}
		//ret.assign(_data->original_data<list>().begin(), _data->original_data<list>().end());
		return ret;
	}
};

#endif