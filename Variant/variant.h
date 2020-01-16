//=======================================================================================
//--------------------------------------------------------------------------------------|
//variant��																				|	
//Created by Croper	                                                                    |
//last modified at Jan.15 2020                                                          |
//��python�����ı����ࡣ																|
//��Ҫ��Ϊ������ Variant��Data															|
//variant������һ������ָ��İ�װ����Ϊ������ݣ�ָ��Data								|
//Data��һ��ʵ�����͵İ�װ��������ģ�壬��ͬӵ��һ���ӿ�IData							|
//--------------------------------------------------------------------------------------| 
//========================================================================================

#ifndef __VARIANT_H
#define __VARIANT_H

#ifndef VARIANT_REGISTER_TYPE
#define VARIANT_REGISTER_TYPE \
	VARIANT_REGISTER(int)\
	VARIANT_REGISTER(double)\
	VARIANT_REGISTER(bool)\
	VARIANT_REGISTER(string)\
	VARIANT_REGISTER(list)
#endif

#include "variant_meta.h"
#include "variant_struct.h"
#include "variant_data.h"

template <>
struct std::equal_to<croper::variant> {
	bool operator()(const croper::variant& v1, const croper::variant& v2) const{
		return v1 == v2;
	}
};
#endif