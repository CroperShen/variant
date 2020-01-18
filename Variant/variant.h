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


#include "variant_marco.h"
#include "variant_meta.h"
#include "variant_struct.h"
#include "variant_data.h"

template <>
struct std::equal_to<croper::variant> {
	bool operator()(const croper::variant& v1, const croper::variant& v2) const{
		return v1 == v2;
	}
};
template <>
struct std::less<croper::variant> {
	bool operator()(const croper::variant& v1, const croper::variant& v2) const {
		return v1 < v2;
	}
};

#define VARIANT_HEADER_END
#include "variant_marco.h"
#endif