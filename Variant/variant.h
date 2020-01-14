//=======================================================================================
//--------------------------------------------------------------------------------------|
//variant��																				|	
//��python�����ı����ࡣ																|
//��Ҫ��Ϊ������ Variant��Data															|
//variant������һ������ָ��İ�װ����Ϊ������ݣ�ָ��Data								|
//Data��һ��ʵ�����͵İ�װ��������ģ�壬��ͬӵ��һ���ӿ�IData							|
//--------------------------------------------------------------------------------------| 
//========================================================================================

#ifndef __VARIANT_H
#define __VARIANT_H

#include <string>
#include <vector>
#include <share.h>
#include <iostream>
#include <functional>
#include <initializer_list>
#include "meta.h"

using namespace std;

namespace croper{

	//������Ϣ��debugģʽ�����������Ϣ��releaseģʽ��ֱ���׳��쳣
	void ErrorMsg(string s);

	//================================================================================
	//���Ͷ��忪ʼ
	//===============================================================================
	class variant {
	public:
		using list = vector<variant>;
	//*******************************������***************************************
	//�˴�ΪĬ�ϵ�֧�����͡����ڵ���ǰ����VARIANT_REGISTER_TYPE���Զ�����֧�ֵ����͡�
	#ifndef VARIANT_REGISTER_TYPE
	#define VARIANT_REGISTER_TYPE \
		VARIANT_REGISTER(int)\
		VARIANT_REGISTER(double)\
		VARIANT_REGISTER(bool)\
		VARIANT_REGISTER(string)\
		VARIANT_REGISTER(list)
	#endif
	//****************************************************************************


	//==================================================================================
	//IData�࣬����Data�Ľӿ�
	//==================================================================================
		struct IData {
			public:
			//���������
			virtual string mytype() const = 0;
			//ת��Ϊ�ַ�����������__str__����
			virtual string to_string() const = 0;
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
		friend struct IData;
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
			enum { result = 4-CanMatch<Dest,Orig>::result};
		};
		template <typename T>
		struct DistinguishType<T, T> {
			enum { result =0 };
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

		template <template <typename> class templ, typename templ_arg>
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


	//==================================================================================
	//Data�࣬��ʵ�ʼ�¼���ݵ���
	//==================================================================================
	#ifndef NO_DATA_TEST
		template <typename T>
		class Data :public _IData_templ<Data, T> {
		public:
			T data;
			Data() {};
			Data(const T& t) :data(t) {};
			T& get_data() override;
			string mytype() const override;
			Data* copy() const override;
			string to_string() const override;
		};
	#endif

	//==================================================================================
	//Data�Ĺ�����
	//==================================================================================

	//=====================================================================
	//variant������ĺ���
	//=====================================================================
	private:

		shared_ptr<IData> _data;
		template <typename T,typename ...Rest> static void push_element(list& l, const T& t, Rest... rest);
	//====================================================================

	public: 
		//���캯��
		variant();
		//����ԭʼ���ݹ���
		template <typename T> variant(const T& t);
		//���ݶ��ԭʼ���ݹ�����б�
		variant(const initializer_list<variant>&);
		template <typename ...T> variant(const T&... t);
		//���ƹ��캯��,����Ҫ��ֱ��ǳ���Ƽ���
		variant(const variant&) = default;
		//��������,����Ҫ
		~variant() = default;

		//���ַ�����ʽ���Data������
		string type() const;
		//Data�Ƿ���ĳ���ض�����
		template <typename T>bool is_type() const;
		//��Dataת��Ϊĳ���ض����ͣ���������ݻ�ת��Ϊ��ֵ
		template <typename T>variant& set_type();
		//��ɫ���ܣ���ͨ���ض��ַ�������variant
		variant& read(const string& sz);
		//����������ΪNone
		void clear();
		//ת��Ϊ�ַ�����
		string to_string() const;
		//��������Ϊ��������function
		template <typename retT,typename ...TArgs>
		retT call(retT(*fp)(TArgs...));
		template <typename ...TArgs>
		void call(void(*fp)(TArgs...));

		void append(const variant&);
		size_t size() const;
		variant& back();
		//��ֵ����
		template <typename T> variant& operator=(const T&);
		template <typename T> variant& operator=(const vector<T>&);
		//�����ػ�Ϊchar*
		variant& operator=(const char* sz);
		variant& operator=(const variant& v) = default;
	

		//ת��Ϊ�ض�����
		template <typename T> operator T() const;
		template <typename T> operator vector<T>() const;
		//================================�����Ǹ��������������==============================
		//����ɲ���
		//����<<ʵ�����
		friend ostream& operator<<(ostream& os, const variant&);
		variant& operator[](int i);

		//δ��ɲ���
		//�������е����ʱ������д
		bool operator==(const variant&);
		template <typename T> bool operator==(const T&);
		template <typename T> bool friend operator== (const T&, const variant&);

		bool operator!=(const variant&);
		template <typename T> bool operator!=(const T&);
		template <typename T> bool friend operator!=(const T&, const variant&);

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

	variant Variant_Read(const string&);
	variant operator ""V(const char* p, size_t s);
	//=============================================================================
	//������������������ģ�庯���Ĵ���
	//=============================================================================


	template<typename T, typename ...Rest>
	inline void variant::push_element(list & l, const T & t, Rest ...rest) {
		l.push_back(t);
		if (sizeof...(Rest) != 0) {
			push_element(l, rest...);
		}
	}

	template<typename T>
	inline variant::variant(const T & t) {
		operator=(t);
	}

	template<typename T>
	inline bool variant::is_type() const
	{
		IData *p = &*_data;
		return dynamic_cast<Data<T>*>(p) != nullptr;
	}


	template<typename T>
	inline variant & variant::set_type()
	{
		if (!_data) {
			_data = make_shared<Data<T>>();
		}
		_data->trans_type<T>(this);
		return *this;
	}

	template<>
	inline bool variant::is_type<void>() const {
		return !_data;
	}




	template<typename ...TArgs>
	struct SendArgs {
		template <typename retT, typename T>
		static retT call(retT(*fp)(TArgs..., T), variant lst_v, int i, TArgs...args) {
			return fp(args..., lst_v[i]);
		}
		template <typename retT, typename T, typename ...TArgs2>
		static retT call(retT(*fp)(TArgs..., T, TArgs2...), variant lst_v, int i, TArgs...args) {
			return SendArgs<TArgs..., T>::call(fp, lst_v, i + 1, args..., lst_v[i]);
		}
		template <typename T>
		static void call(void(*fp)(TArgs..., T), variant lst_v, int i, TArgs...args) {
			fp(args..., lst_v[i]);
		}
		template < typename T, typename ...TArgs2>
		static void call(void(*fp)(TArgs..., T, TArgs2...), variant lst_v, int i, TArgs...args) {
			SendArgs<TArgs..., T>::call(fp, lst_v, i + 1, args..., lst_v[i]);
		}

	};
	template<typename retT, typename ...TArgs>
	inline retT variant::call(retT(*fp)(TArgs...))
	{
		variant temp = *this;
		temp.set_type<list>();
		list l = temp._data->original_data<list>();
		return SendArgs<>::call(fp,temp,0);
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
		_data = make_shared<Data<T>>(t);
		return *this;
	}


	//��ֵ����
	//����ֵΪvectorʱ������ͳһΪlist(vector<variant>)
	template<typename T>
	inline variant & variant::operator=(const vector<T>& v)
	{
		_data = make_shared<Data<list>>(v.begin(), v.end());
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
	inline variant::operator vector<T>() const
	{
		if (!is_type<list>()) {
			ErrorMsg("variant����list���ͣ��޷�ת��Ϊvector");
			return vector<T>();
		}
		vector<T> ret;
		ret.assign(_data->original_data<list>().begin(), _data->original_data<list>().end());
		return ret;
	}




	template<typename T, typename my_type, typename templ_arg>
	inline void variant::Judge<0, T, my_type, templ_arg>::transtype(my_type * self, variant * base)
	{
		//ʲôҲ����
	}

	template<typename T, typename my_type, typename templ_arg>
	inline void variant::Judge<1, T, my_type, templ_arg>::transtype(my_type * self, variant * base)
	{
		base->_data = make_shared<Data<list>>(list({*base}));
	}

	template<typename T, typename my_type, typename templ_arg>
	inline void variant::Judge<2, T, my_type, templ_arg>::transtype(my_type * self, variant * base)
	{
		*base = base[0];
		base->set_type<T>();
	}

	template<typename T, typename my_type, typename templ_arg>
	inline void variant::Judge<3, T, my_type, templ_arg>::transtype(my_type * self, variant * base)
	{
		base->_data = make_shared<Data<T>>(self->get_data());
	}

	template<typename T, typename my_type, typename templ_arg>
	inline void variant::Judge<4, T, my_type, templ_arg>::transtype(my_type * self, variant * base)
	{
		base->_data = make_shared<Data<T>>(T());
	}

	template<typename T, typename my_type, typename templ_arg>
	inline T variant::Judge<1, T, my_type, templ_arg>::get_data(my_type * self) {
		return static_cast<T>(self->get_data());
	}

	template<typename T, typename my_type, typename templ_arg>
	inline T variant::Judge<0, T, my_type, templ_arg>::get_data(my_type * self) {
	#ifdef VARIANT_STRICT_TYPE_CHECK
		string s = string(typeid(T).name()) + "��" + typeid(templ_arg).name() + "�������޷�ƥ��";
		ErrorMsg(s);
	#endif
		return T();
	}

	template<typename T>
	inline T& variant::Data<T>::get_data() {
		return data;
	}

	template<typename T>
	inline string variant::Data<T>::mytype() const {
		return typeid(*this).name();
	}

	template<typename T>
	inline variant::Data<T> * variant::Data<T>::copy() const {
		return new Data(*this);
	}



	template<typename T>
	inline string variant::Data<T>::to_string() const {
		return std::to_string(data);
	}

	template <>
	inline string variant::Data<string>::to_string() const {
		return data;
	}

	template <>
	inline string variant::Data<bool>::to_string() const {
		return data ? "(true)" : "(false)";
	}

	template <>
	inline string variant::Data<vector<variant>>::to_string() const {
		if (data.empty()) return "[]";
		string ret = "[";
		for (auto &v : data) {
			ret += v.to_string();
			ret.push_back(',');
		}
		ret.back() = ']';
		return ret;
	}

	template<typename T>
	inline T & variant::IData::original_data()
	{
		return dynamic_cast<Data<T>*>(this)->get_data();
	}


};

#endif