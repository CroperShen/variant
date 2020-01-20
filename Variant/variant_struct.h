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
#include <unordered_map>
#include "variant_marco.h"
#include "variant_meta.h"
//=========================================================================



namespace croper {
	//================================================================================
	//定义开始
	//===============================================================================
	class variant {
	public:
		static const variant None;
		//****************************************************************************

		struct IData;
		template <int, typename, typename, typename> struct Judge;
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
			//以类型T获取数据，会发生类型的默认转化
			template <typename T> void trans_type(variant*);
			//原始数据：类型指定错误会错误
			template <typename T> T& original_data();
			//hashcode
			virtual size_t hashcode() const = 0;


			//各种运算符的重载，不使用operator以避免混淆
			virtual std::shared_ptr<IData> next() const = 0;
			virtual std::shared_ptr<IData> prev() const = 0;


			//*******************************宏代码块***************************************
#define OPERATOR_TEMPLATE(OP_NAME,OP_SYMBOL,RET_TYPE,...)											\
			virtual RET_TYPE OP_NAME(const IData&) const=0;										\
			template <typename T> RET_TYPE OP_NAME##2(const _IData_templ<T>&) const;				

			OPERATOR_TEMPLATE_ALL()
#undef OPERATOR_TEMPLATE


#define OPERATOR_TEMPLATE(OP_NAME,OP_SYMBOL,RET_TYPE,T,...)											\
			virtual RET_TYPE OP_NAME##_##T(const _IData_templ<T>&) const = 0;						\
			template <>  RET_TYPE OP_NAME##2(const _IData_templ<T>& t) const {						\
				return OP_NAME##_##T(t);															\
			}

#define VARIANT_REGISTER(T)																			\
			virtual T _get_data_##T() = 0;														\
			virtual void _trans_type_##T(variant*) = 0;											\
			template<> T get_data<T>(){																\
				return _get_data_##T();															\
			}																						\
			template<> void trans_type<T>(variant* base) {											\
				_trans_type_##T(base);															\
			}																						\
			OPERATOR_TEMPLATE_ALL(T)


				VARIANT_REGISTER_TYPE;
#undef VARIANT_REGISTER
#undef OPERATOR_TEMPLATE
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

			//*******************************宏代码块***************************************
#define OPERATOR_TEMPLATE(OP_NAME, OP_SYMBOL, RET_TYPE,...)	\
			virtual RET_TYPE OP_NAME(const IData&) const override;		
			OPERATOR_TEMPLATE_ALL()
#undef	OPERATOR_TEMPLATE

#define OPERATOR_TEMPLATE(OP_NAME, OP_SYMBOL, RET_TYPE,T)	\
			virtual  RET_TYPE OP_NAME##_##T(const _IData_templ<T>&) const override;				

#define VARIANT_REGISTER(T) \
			virtual T _get_data_##T() override;													\
			virtual void _trans_type_##T(variant* base) override;									\
			OPERATOR_TEMPLATE_ALL(T)

				VARIANT_REGISTER_TYPE;
#undef VARIANT_REGISTER
#undef	OPERATOR_TEMPLATE
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

		//复制构造函数,不需要，直接浅复制即可
		variant(const variant&) = default;
		variant(variant&&) = default;
		//复制构造函数，不需要重写
		variant& operator=(const variant & v) = default;
		variant& operator=(variant && v) = default;

		//根据原始数据构造
		template <typename T,EXCLUDE_TYPE(T,variant)> variant(T&& t);
		//根据多个原始数据构造出列表
		variant(const std::initializer_list<variant>&);
		//赋值函数
		template <typename T, EXCLUDE_TYPE(T, variant)> variant& operator=(T&&);
		//赋值函数，std::vector重载版本
		template <typename T> variant& operator=(const std::vector<T>&);
		//赋值函数，const char*重载版本，以接受字面常量
		variant& operator=(const char* sz);




		//废弃，不使用
		//template <typename ...T> variant(const T&... t);


		//析构函数,不需要
		~variant() = default;

		//hashcode
		size_t hashcode() const;

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




		//转化为特定类型
		template <typename T> operator T() const;
		operator unsigned int() const;
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

		variant& operator[](const variant&);
		const variant& operator[](const variant&) const;

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


}
#endif