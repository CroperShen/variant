//=======================================================================================
//--------------------------------------------------------------------------------------|
//variant类																				|	
//Created by Croper	                                                                    |
//last modified at Jan.15 2020                                                          |
//仿python变量的变体类。																|
//主要分为两部分 Variant和Data															|
//variant本身是一个智能指针的包装，作为表层数据，指向Data								|
//Data是一个实际类型的包装，本身是模板，共同拥有一个接口IData							|
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