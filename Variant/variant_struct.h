//=======================================================================================
//--------------------------------------------------------------------------------------|
//                          variant类的框架，不含Data部分								|	
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

#include "variant_meta.h"
//=========================================================================



namespace croper {
	class variant;
	using list = std::vector<variant>;
	using string = std::string;
//================================================================================
//定义开始
//===============================================================================
	class variant {
	public:
		static const variant None;
#ifndef VARIANT_REGISTER_TYPE
#define VARIANT_REGISTER_TYPE
#endif
//****************************************************************************


		struct IData;
		template <int,typename, typename, typename> struct Judge;
		template <typename> struct _IData_templ;
//==================================================================================
//IData类，定义Data的接口
//==================================================================================
		struct IData {
		public:
			//自身的类型
			virtual std::string mytype() const = 0;
			//转化为字符串，对于于__str__方法
			virtual std::string to_string() const = 0;
			//返回自身的拷贝
			virtual std::shared_ptr<IData> copy() const = 0;
			//以类型T获取数据，会发生类型的默认转化，当类型不匹配时会给出默认值,如果定义了宏VARIANT_STRICT_TYPE_CHECK的话会报错
			template <typename T> T get_data();


			
			virtual std::shared_ptr<IData> next() const = 0;
			virtual std::shared_ptr<IData> prev() const = 0;

			virtual std::shared_ptr<IData> add(const IData&) const = 0;
			virtual std::shared_ptr<IData> sub(const IData&) const = 0;
			virtual std::shared_ptr<IData> mul(const IData&) const = 0;
			virtual std::shared_ptr<IData> div(const IData&) const = 0;
			virtual std::shared_ptr<IData> rem(const IData&) const = 0;

			virtual bool equal(const IData&) const = 0;
			virtual bool greater(const IData&) const = 0;
			virtual bool less(const IData&)const = 0;
			virtual bool ge(const IData&) const = 0;
			virtual bool le(const IData&) const = 0;

			template <typename T>  std::shared_ptr<IData> add2(const _IData_templ<T>&) const;
			template <typename T>  std::shared_ptr<IData> sub2(const _IData_templ<T>&) const;
			template <typename T>  std::shared_ptr<IData> mul2(const _IData_templ<T>&) const;
			template <typename T>  std::shared_ptr<IData> div2(const _IData_templ<T>&) const;
			template <typename T>  std::shared_ptr<IData> rem2(const _IData_templ<T>&) const;

			template <typename T>  bool equal2(const _IData_templ<T>&) const;
			template <typename T>  bool greater2(const _IData_templ<T>&) const;
			template <typename T>  bool less2(const _IData_templ<T>&)const;
			template <typename T>  bool ge2(const _IData_templ<T>&) const;
			template <typename T>  bool le2(const _IData_templ<T>&) const;

			//以类型T获取数据，会发生类型的默认转化
			template <typename T> void trans_type(variant*);
			//原始数据：类型指定错误会错误
			template <typename T> T& original_data();
		

			//*******************************宏代码块***************************************
					//get_data与astype是模板，本身无法继承，这里让get_data调用_get_data_int,_get_data_float等以实现继承
#define VARIANT_REGISTER(T)																			\
			virtual T _get_data_##T##() = 0;														\
			virtual void _trans_type_##T##(variant*) = 0;											\
			virtual std::shared_ptr<IData> add_##T##(const _IData_templ<T>&) const = 0;				\
			virtual std::shared_ptr<IData> sub_##T##(const _IData_templ<T>&) const = 0;				\
			virtual std::shared_ptr<IData> mul_##T##(const _IData_templ<T>&) const = 0;				\
			virtual std::shared_ptr<IData> div_##T##(const _IData_templ<T>&) const = 0;				\
			virtual std::shared_ptr<IData> rem_##T##(const _IData_templ<T>&) const = 0;				\
																									\
			virtual bool equal_##T##(const _IData_templ<T>&) const = 0;								\
			virtual bool greater_##T##(const _IData_templ<T>&) const = 0;							\
			virtual bool less_##T##(const _IData_templ<T>&)const = 0;								\
			virtual bool ge_##T##(const _IData_templ<T>&) const = 0;								\
			virtual bool le_##T##(const _IData_templ<T>&) const = 0;								\
																									\
			template <>  std::shared_ptr<IData> add2(const _IData_templ<T>& t) const {				\
				return add_##T##(t);																\
			}																						\
			template <>  std::shared_ptr<IData> sub2(const _IData_templ<T>& t) const {				\
				return sub_##T##(t);																\
			}																						\
			template <>  std::shared_ptr<IData> mul2(const _IData_templ<T>& t) const {				\
				return mul_##T##(t);																\
			}																						\
			template <>  std::shared_ptr<IData> div2(const _IData_templ<T>& t) const {				\
				return div_##T##(t);																\
			}																						\
			template <>  std::shared_ptr<IData> rem2(const _IData_templ<T>& t) const {				\
				return rem_##T##(t);																\
			}																						\
																									\
			template <>  bool equal2(const _IData_templ<T>& t) const {								\
				return equal_##T##(t);																\
			}																						\
			template <>  bool greater2(const _IData_templ<T>& t) const {							\
				return greater_##T##(t);															\
			}																						\
			template <>  bool less2(const _IData_templ<T>& t)const {								\
				return less_##T##(t);																\
			}																						\
			template <>  bool ge2(const _IData_templ<T>& t) const {									\
				return ge_##T##(t);																	\
			}																						\
			template <>  bool le2(const _IData_templ<T>& t) const {									\
				return le_##T##(t);																	\
			}																						\
																									\
			template<> T get_data<T>(){																\
				return _get_data_##T##();															\
			}																						\
			template<> void trans_type<T>(variant* base) {											\
				_trans_type_##T##(base);															\
			}																						\

			
			VARIANT_REGISTER_TYPE;
#undef VARIANT_REGISTER
			//****************************************************************************
		};
		//声明为友元
		friend struct IData;
		//创建Data，解耦合用
		template <typename T>
		static std::shared_ptr<IData> CreateData();
		template <typename T>
		static std::shared_ptr<IData> CreateData(const T& t);

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
		//_IData_templ类，技术类
		//模板分离后的IData，使分离的_get_data_int,_get_data_double等都指向同一个get_data,并使用Judge判断其类型是否匹配，
		//不匹配时，其将指向一个报错的函数
		//如果使用if语句会导致虽然这一块永远不会执行但是无法通过编译。
		//========================================================================================

		template <typename templ_arg>
		struct _IData_templ :public IData {
			virtual templ_arg& get_data() = 0;
			virtual const templ_arg& get_data() const = 0;
			virtual std::shared_ptr<IData> add(const IData&) const override;
			virtual std::shared_ptr<IData> sub(const IData&) const override;
			virtual std::shared_ptr<IData> mul(const IData&) const override;
			virtual std::shared_ptr<IData> div(const IData&) const override;
			virtual std::shared_ptr<IData> rem(const IData&) const override;

			virtual bool equal(const IData&) const override;
			virtual bool greater(const IData&) const override;
			virtual bool less(const IData&)const override;
			virtual bool ge(const IData&) const override;
			virtual bool le(const IData&) const override;
			//*******************************宏代码块***************************************
#define VARIANT_REGISTER(T) \
			virtual T _get_data_##T##() override;													\
			virtual void _trans_type_##T##(variant* base) override;									\
																									\
			virtual std::shared_ptr<IData> add_##T##(const _IData_templ<T>&) const override;		\
			virtual std::shared_ptr<IData> sub_##T##(const _IData_templ<T>&) const override;		\
			virtual std::shared_ptr<IData> mul_##T##(const _IData_templ<T>&) const override;		\
			virtual std::shared_ptr<IData> div_##T##(const _IData_templ<T>&) const override;		\
			virtual std::shared_ptr<IData> rem_##T##(const _IData_templ<T>&) const override;		\
																									\
			virtual bool equal_##T##(const _IData_templ<T>&) const override;						\
			virtual bool greater_##T##(const _IData_templ<T>&) const override;						\
			virtual bool less_##T##(const _IData_templ<T>&)const override;							\
			virtual bool ge_##T##(const _IData_templ<T>&) const override;							\
			virtual bool le_##T##(const _IData_templ<T>&) const override;							\

			VARIANT_REGISTER_TYPE;
#undef VARIANT_REGISTER
			//****************************************************************************
		};
		//=====================================================================
		//variant类自身的函数
		//=====================================================================
	private:
		//使用智能指针指向_data,这样不再需要对_data进行内存管理，且能进行浅拷贝。
		//python变量也是相似的机制
		std::shared_ptr<IData> _data;
		//====================================================================

	public:
		//构造函数
		variant();
		//根据原始数据构造
		template <typename T> variant(const T& t);
		//根据多个原始数据构造出列表
		variant(const std::initializer_list<variant>&);
		template <typename ...T> variant(const T&... t);
		//复制构造函数,不需要，直接浅复制即可
		variant(const variant&) = default;
		//析构函数,不需要
		~variant() = default;

		//输出自身以及data的地址信息
		std::string __My_Address() const;
		//以字符串形式输出Data的类型
		std::string type() const;
		//Data是否是某种特定类型
		template <typename T>bool is_type() const;
		//将Data转化为某种特定类型，如果不兼容会转化为空值
		template <typename T>variant& set_type();
		//将自身重置为None
		void clear();
		//转化为字符串：
		std::string to_string() const;
		//将自身解包为参数传入function
		template <typename retT, typename ...TArgs> retT call(retT(*fp)(TArgs...));
		//将自身解包为参数传入function,返回值为void的重载版本
		template <typename ...TArgs> void call(void(*fp)(TArgs...));

		//当自身为list时于尾部插入一个元素。
		void append(const variant&);
		//当自身为list时获得list的大小
		size_t size() const;
		//当自身为list时返回尾部元素
		variant& back();

		//赋值函数
		template <typename T> variant& operator=(const T&);
		//赋值函数，std::vector重载版本
		template <typename T> variant& operator=(const std::vector<T>&);
		//赋值函数，const char*重载版本，以接受字面常量
		variant& operator=(const char* sz);
		//复制构造函数，不需要重写
		variant& operator=(const variant& v) = default;


		//转化为特定类型
		template <typename T> operator T() const;
		//转化为特定类型，std::vector重载版本
		template <typename T> operator std::vector<T>() const;
		//获取原始数据，慎用
		template <typename T> T& __My_base();
		template <typename T> const T& __My_base() const;

		//================================以下是各种运算符的重载==============================

		//已完成部分
		//重载<<实现输出
		friend std::ostream& operator<<(std::ostream& os, const variant&);
		//重载[]

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
	//以下是周边函数的声明
	//=============================================================================
	//错误信息，debug模式下输出错误信息，release模式下直接抛出异常
	void ErrorMsg(std::string s);
	//由字符串转化为variant，特色功能
	variant Variant_Read(const std::string&);
	//可以在字符串后加_V以简化实现上述功能
	variant operator ""_V(const char* p, size_t s);

	 

	//=============================================================================
	//声明结束，以下是模板函数的实现代码
	//=============================================================================

		
	//*******************************宏代码块************************************************************
#define OPERATOR_TEMPLATE(OP_NAME,FLAG)																	\
	template <typename T1,typename T2, typename = decltype(std::declval<T1>() FLAG std::declval<T2>())>				\
	bool __data_##OP_NAME##(const variant::_IData_templ<T1>& t1, const variant::_IData_templ<T2>& t2){		\
		return t1.get_data() FLAG t2.get_data();																		\
	}																												\
	constexpr bool __data_##OP_NAME##(...) {																	\
		return false;																								\
	}																												\

	OPERATOR_TEMPLATE(equal  ,  == );
	OPERATOR_TEMPLATE(greater, >  );
	OPERATOR_TEMPLATE(less   ,<  );
	OPERATOR_TEMPLATE(ge     , >= );
	OPERATOR_TEMPLATE(le     , <= );
#undef OPERATOR_TEMPLATE

#define OPERATOR_TEMPLATE(OP_NAME,FLAG)																				\
	template <typename T1,typename T2, typename retT= decltype(std::declval<T1>() FLAG std::declval<T2>())>				\
	std::shared_ptr<variant::IData> __data_##OP_NAME##(const variant::_IData_templ<T1>& t1, const variant::_IData_templ<T2>& t2){		\
		typename disable_if<retT,variant,T1,T2>::t v=variant::None;													\
		return variant::CreateData<retT>(t1.get_data() FLAG t2.get_data());																		\
	}																												\
	inline std::shared_ptr<variant::IData> __data_##OP_NAME##(...) {																	\
		return nullptr;																								\
	}																												\


	OPERATOR_TEMPLATE(add,+);
	OPERATOR_TEMPLATE(sub,-);
	OPERATOR_TEMPLATE(mul,*);
	OPERATOR_TEMPLATE(div,/);
	OPERATOR_TEMPLATE(rem,%);
#undef OPERATOR_TEMPLATE
	
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
			template <typename T2>																						\
			bool variant::_IData_templ<T2>::equal_##T##(const variant::_IData_templ<T>& d2) const {						\
				return __data_equal(*this,d2);																			\
			}																											\
			template <typename T2>																						\
			bool variant::_IData_templ<T2>::greater_##T##(const variant::_IData_templ<T>& d2) const {					\
				return __data_greater(*this,d2);																		\
			}																											\
			template <typename T2>																						\
			bool variant::_IData_templ<T2>::less_##T##(const variant::_IData_templ<T>& d2) const {						\
				return __data_less(*this,d2);																			\
			}																											\
			template <typename T2>																						\
			bool variant::_IData_templ<T2>::ge_##T##(const variant::_IData_templ<T>& d2) const {						\
				return __data_ge(*this,d2);																				\
			}																											\
			template <typename T2>																						\
			bool variant::_IData_templ<T2>::le_##T##(const variant::_IData_templ<T>& d2) const {						\
				return __data_le(*this,d2);																							\
			}																														\
			template <typename T2>																									\
			std::shared_ptr<variant::IData> variant::_IData_templ<T2>::add_##T##(const variant::_IData_templ<T>& d2) const {		\
				return __data_add(*this,d2);																						\
			}																														\
			template <typename T2>																									\
			std::shared_ptr<variant::IData> variant::_IData_templ<T2>::sub_##T##(const variant::_IData_templ<T>& d2) const {		\
				return __data_sub(*this,d2);																						\
			}																														\
			template <typename T2>																									\
			std::shared_ptr<variant::IData> variant::_IData_templ<T2>::mul_##T##(const variant::_IData_templ<T>& d2) const {		\
				return __data_mul(*this,d2);																						\
			}																														\
			template <typename T2>																									\
			std::shared_ptr<variant::IData> variant::_IData_templ<T2>::div_##T##(const variant::_IData_templ<T>& d2) const {		\
				return __data_div(*this,d2);																						\
			}																														\
			template <typename T2>																									\
			std::shared_ptr<variant::IData> variant::_IData_templ<T2>::rem_##T##(const variant::_IData_templ<T>& d2) const {		\
				return __data_rem(*this,d2);																						\
			}																														\


	   VARIANT_REGISTER_TYPE

	//***************************************************************************************************


	//------------------------------------------------------------------------
	//IData类的函数
	//-------------------------------------------------------------------------
	//原始数据：类型指定错误会错误
	template<typename T>
	inline T & variant::IData::original_data()
	{
		return dynamic_cast<_IData_templ<T>*>(this)->get_data();
	}
	

	//----------------------------------------------------------------------------
	//Judge类的函数
	//----------------------------------------------------------------------------

	//当原类型与目标类型相同时
	template<typename T, typename my_type, typename templ_arg>
	inline void variant::Judge<0, T, my_type, templ_arg>::transtype(my_type * self, variant * base)
	{
		//什么也不做
	}

	//当目标类型为list但源类型不为list时，转化为长度为1的list，第一个元素即为自身
	template<typename T, typename my_type, typename templ_arg>
	inline void variant::Judge<1, T, my_type, templ_arg>::transtype(my_type * self, variant * base)
	{
		base->_data = CreateData<list>(list({ *base }));
	}

	//当目标类型不为list但源类型为list时，将第一个元素转化为目标元素
	template<typename T, typename my_type, typename templ_arg>
	inline void variant::Judge<2, T, my_type, templ_arg>::transtype(my_type * self, variant * base)
	{
		*base = base[0];
		base->set_type<T>();
	}

	//当源类型能转化为目标类型时，进行默认转化
	template<typename T, typename my_type, typename templ_arg>
	inline void variant::Judge<3, T, my_type, templ_arg>::transtype(my_type * self, variant * base)
	{
		base->_data = CreateData<T>(self->get_data());
	}

	//当源类型不能转化为目标类型时，设为默认值
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
		std::string s = std::string(typeid(T).name()) + "与" + typeid(templ_arg).name() + "的类型无法匹配";
		ErrorMsg(s);
#endif
		return T();
	}

	//------------------------------------------------------------------------------------
	//variant自身的函数
	//----------------------------------------------------------------------------------------

	//重载equal_to




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


	//赋值函数
	//当右值为std::vector时，类型统一为list(std::vector<variant>)
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
			std::cerr << "待转换的参数类型不匹配,目标是" << typeid(T).name() << ",但源参数类型是" << type() << std::endl;
			assert(0);
		}
		return _data->original_data<T>();
	}

	template<typename T>
	inline const T & variant::__My_base() const
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
		for (int i = 0; i < this->size(); ++i) {
			ret.push_back(this->operator[](i));
		}
		return ret;
	}

#define OPERATOR_TEMPLATE(OP_NAME,RETURN_TYPE)										    \
	template<typename T>																\
	inline RETURN_TYPE variant::_IData_templ<T>::##OP_NAME##(const IData & d) const		\
	{																					\
		return d.##OP_NAME##2(*this);													\
	}																					\


OPERATOR_TEMPLATE(add, std::shared_ptr<variant::IData>);				
OPERATOR_TEMPLATE(sub, std::shared_ptr<variant::IData>);				
OPERATOR_TEMPLATE(mul, std::shared_ptr<variant::IData>);				
OPERATOR_TEMPLATE(div, std::shared_ptr<variant::IData>);				
OPERATOR_TEMPLATE(rem, std::shared_ptr<variant::IData>);				
OPERATOR_TEMPLATE(equal, bool);										
OPERATOR_TEMPLATE(greater, bool);										
OPERATOR_TEMPLATE(less, bool);										
OPERATOR_TEMPLATE(ge, bool);											
OPERATOR_TEMPLATE(le, bool);											
#undef OPERATOR_TEMPLATE

};

#endif