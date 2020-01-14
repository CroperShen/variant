//=======================================================================================
//--------------------------------------------------------------------------------------|
//variant类																				|	
//仿python变量的变体类。																|
//主要分为两部分 Variant和Data															|
//variant本身是一个智能指针的包装，作为表层数据，指向Data								|
//Data是一个实际类型的包装，本身是模板，共同拥有一个接口IData							|
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

	//错误信息，debug模式下输出错误信息，release模式下直接抛出异常
	void ErrorMsg(string s);

	//================================================================================
	//类型定义开始
	//===============================================================================
	class variant {
	public:
		using list = vector<variant>;
	//*******************************宏代码块***************************************
	//此处为默认的支持类型。可于导入前定义VARIANT_REGISTER_TYPE宏以定义其支持的类型。
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
	//IData类，定义Data的接口
	//==================================================================================
		struct IData {
			public:
			//自身的类型
			virtual string mytype() const = 0;
			//转化为字符串，对于于__str__方法
			virtual string to_string() const = 0;
			//返回自身的拷贝
			virtual IData* copy() const = 0;
			//以类型T获取数据，会发生类型的默认转化，当类型不匹配时会给出默认值,如果定义了宏VARIANT_STRICT_TYPE_CHECK的话会报错
			template <typename T> T get_data();
			//以类型T获取数据，会发生类型的默认转化
			template <typename T> void trans_type(variant*);
			//原始数据：类型指定错误会错误
			template <typename T> T& original_data();


	//*******************************宏代码块***************************************
			//get_data与astype是模板，本身无法继承，这里让get_data调用_get_data_int,_get_data_float等以实现继承
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
	//Judge类，技术类，根据不同的模板参数给出不同的函数									
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
	//DistinguishType类，技术类，当两type相同时返回0，第一个是list时返回1，第二个是list时返回2，能转化时返回3，不能转化时返回4								
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
	//_IData_templ类，技术类
	//模板分离后的IData，使分离的_get_data_int,_get_data_double等都指向同一个get_data,并使用Judge判断其类型是否匹配，
	//不匹配时，其将指向一个报错的函数
	//如果使用if语句会导致虽然这一块永远不会执行但是无法通过编译。
	//========================================================================================

		template <template <typename> class templ, typename templ_arg>
		struct _IData_templ :public IData {
			virtual templ_arg& get_data() = 0;
	//*******************************宏代码块***************************************
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
	//Data类，即实际记录数据的类
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
	//Data的工厂类
	//==================================================================================

	//=====================================================================
	//variant类自身的函数
	//=====================================================================
	private:

		shared_ptr<IData> _data;
		template <typename T,typename ...Rest> static void push_element(list& l, const T& t, Rest... rest);
	//====================================================================

	public: 
		//构造函数
		variant();
		//根据原始数据构造
		template <typename T> variant(const T& t);
		//根据多个原始数据构造出列表
		variant(const initializer_list<variant>&);
		template <typename ...T> variant(const T&... t);
		//复制构造函数,不需要，直接浅复制即可
		variant(const variant&) = default;
		//析构函数,不需要
		~variant() = default;

		//以字符串形式输出Data的类型
		string type() const;
		//Data是否是某种特定类型
		template <typename T>bool is_type() const;
		//将Data转化为某种特定类型，如果不兼容会转化为空值
		template <typename T>variant& set_type();
		//特色功能！！通过特定字符串创建variant
		variant& read(const string& sz);
		//将自身重置为None
		void clear();
		//转化为字符串：
		string to_string() const;
		//将自身解包为参数传入function
		template <typename retT,typename ...TArgs>
		retT call(retT(*fp)(TArgs...));
		template <typename ...TArgs>
		void call(void(*fp)(TArgs...));

		void append(const variant&);
		size_t size() const;
		variant& back();
		//赋值函数
		template <typename T> variant& operator=(const T&);
		template <typename T> variant& operator=(const vector<T>&);
		//不让特化为char*
		variant& operator=(const char* sz);
		variant& operator=(const variant& v) = default;
	

		//转化为特定类型
		template <typename T> operator T() const;
		template <typename T> operator vector<T>() const;
		//================================以下是各种运算符的重载==============================
		//已完成部分
		//重载<<实现输出
		friend ostream& operator<<(ostream& os, const variant&);
		variant& operator[](int i);

		//未完成部分
		//工作量有点大，有时间慢慢写
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
	//类声明结束，以下是模板函数的代码
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


	//赋值函数
	//当右值为vector时，类型统一为list(vector<variant>)
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
			ErrorMsg("variant不是list类型，无法转化为vector");
			return vector<T>();
		}
		vector<T> ret;
		ret.assign(_data->original_data<list>().begin(), _data->original_data<list>().end());
		return ret;
	}




	template<typename T, typename my_type, typename templ_arg>
	inline void variant::Judge<0, T, my_type, templ_arg>::transtype(my_type * self, variant * base)
	{
		//什么也不做
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
		string s = string(typeid(T).name()) + "与" + typeid(templ_arg).name() + "的类型无法匹配";
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