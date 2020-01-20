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
	//================================================================================
	//���忪ʼ
	//===============================================================================
	class variant {
	public:
		static const variant None;
		//****************************************************************************

		struct IData;
		template <int, typename, typename, typename> struct Judge;
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
			//hashcode
			virtual size_t hashcode() const = 0;


			//��������������أ���ʹ��operator�Ա������
			virtual std::shared_ptr<IData> next() const = 0;
			virtual std::shared_ptr<IData> prev() const = 0;


			//*******************************������***************************************
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

		//���ƹ��캯��,����Ҫ��ֱ��ǳ���Ƽ���
		variant(const variant&) = default;
		variant(variant&&) = default;
		//���ƹ��캯��������Ҫ��д
		variant& operator=(const variant & v) = default;
		variant& operator=(variant && v) = default;

		//����ԭʼ���ݹ���
		template <typename T,EXCLUDE_TYPE(T,variant)> variant(T&& t);
		//���ݶ��ԭʼ���ݹ�����б�
		variant(const std::initializer_list<variant>&);
		//��ֵ����
		template <typename T, EXCLUDE_TYPE(T, variant)> variant& operator=(T&&);
		//��ֵ������std::vector���ذ汾
		template <typename T> variant& operator=(const std::vector<T>&);
		//��ֵ������const char*���ذ汾���Խ������泣��
		variant& operator=(const char* sz);




		//��������ʹ��
		//template <typename ...T> variant(const T&... t);


		//��������,����Ҫ
		~variant() = default;

		//hashcode
		size_t hashcode() const;

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




		//ת��Ϊ�ض�����
		template <typename T> operator T() const;
		operator unsigned int() const;
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
	//�������ܱߺ���������
	//=============================================================================
	//������Ϣ��debugģʽ�����������Ϣ��releaseģʽ��ֱ���׳��쳣
	void ErrorMsg(std::string s);
	//���ַ���ת��Ϊvariant����ɫ����
	variant Variant_Read(const std::string&);
	//�������ַ������_V�Լ�ʵ����������
	variant operator ""_V(const char* p, size_t s);


}
#endif