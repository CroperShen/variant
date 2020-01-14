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

#endif