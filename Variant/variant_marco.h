//========================================================================================================
//variant需要用到的宏，主要是简化不同类型之间的相识代码以及不同运算符重载之间的重复代码
//========================================================================================================


#ifndef VARIANT_HEADER_END
	//define VARIANT_REGISTERE(T) ....code....
	#ifndef VARIANT_REGISTER_TYPE
	#define VARIANT_REGISTER_TYPE	\
		VARIANT_REGISTER(int)		\
		VARIANT_REGISTER(double)	\
		VARIANT_REGISTER(bool)		\
		VARIANT_REGISTER(string)	\
		VARIANT_REGISTER(list)
	#endif

	//define OPERATOR_TEMPLATE(OP_NAME,OP_SYMBOL,RET_TYPE,...) ....code....
	#define OPERATOR_PARTA_RET_T bool
	#define OPERATOR_PARTB_RET_T std::shared_ptr<croper::variant::IData>

	#ifndef OPERATOR_TEMPLATE_PART_A
	#define OPERATOR_TEMPLATE_PART_A(...)						\
		OPERATOR_TEMPLATE(equal, == ,  OPERATOR_PARTA_RET_T,__VA_ARGS__)\
		OPERATOR_TEMPLATE(greater, > , OPERATOR_PARTA_RET_T,__VA_ARGS__)\
		OPERATOR_TEMPLATE(less, < ,    OPERATOR_PARTA_RET_T,__VA_ARGS__)\
		OPERATOR_TEMPLATE(ge, >= ,     OPERATOR_PARTA_RET_T,__VA_ARGS__)\
		OPERATOR_TEMPLATE(le, <= ,     OPERATOR_PARTA_RET_T,__VA_ARGS__)
	#endif

	#ifndef OPERATOR_TEMPLATE_PART_B
	#define OPERATOR_TEMPLATE_PART_B(...)						\
		OPERATOR_TEMPLATE(add, + ,  OPERATOR_PARTB_RET_T,__VA_ARGS__)	\
		OPERATOR_TEMPLATE(sub, - ,  OPERATOR_PARTB_RET_T,__VA_ARGS__)	\
		OPERATOR_TEMPLATE(mul, * ,  OPERATOR_PARTB_RET_T,__VA_ARGS__)	\
		OPERATOR_TEMPLATE(div, / ,  OPERATOR_PARTB_RET_T,__VA_ARGS__)	\
		OPERATOR_TEMPLATE(rem, % ,  OPERATOR_PARTB_RET_T,__VA_ARGS__)	
	#endif

	#ifndef OPERATOR_TEMPLATE_ALL
	#define OPERATOR_TEMPLATE_ALL(...)		\
			OPERATOR_TEMPLATE_PART_A(__VA_ARGS__)		\
			OPERATOR_TEMPLATE_PART_B(__VA_ARGS__)
	#endif
#else
	#undef VARIANT_REGISTER_TYPE
	#undef OPERATOR_PARTA_RET_T
	#undef OPERATOR_PARTB_RET_T
	#undef OPERATOR_TEMPLATE_PART_A
	#undef OPERATOR_TEMPLATE_PART_B
	#undef OPERATOR_TEMPLATE_ALL
#endif