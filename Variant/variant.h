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


#include <vector>
/*
template <>
struct std::equal_to<croper::variant> {
	bool operator()(const croper::variant& v1, const croper::variant& v2) const;
};
*/
#include <string>
#include <set>
#include <unordered_map>



namespace croper {
	class variant;

	using string = std::string;
	using list = std::vector<variant>;



	struct hash_t {
		std::size_t operator()(const croper::list& l) const;
		std::size_t operator()(const croper::variant& v) const;
	};


	struct less_t {
		bool operator()(const croper::variant& v1, const croper::variant& v2) const;
	};

	using set = std::set<variant, less_t>;
	using dict = std::unordered_map<variant, variant,hash_t, std::equal_to<variant>>;
}

#include "variant_marco.h"
#include "variant_meta.h"
#include "variant_struct.h"
#include "variant_data.h"
#include "variant_struct_imp.h"
#include "variant_data_imp.h"


/*

bool std::equal_to<croper::variant>::operator()(const croper::variant& v1, const croper::variant& v2) const {
	return v1 == v2;
}

*/


#define VARIANT_HEADER_END
#include "variant_marco.h"
#endif