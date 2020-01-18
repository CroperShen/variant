#ifndef __VARIANT_META_H
#define __VARIANT_META_H
//variant��Ҫ�õ���ĳЩԪ���ģ��


//������������Ƿ���ͬ
template<typename T1, typename T2>
struct IsSame {
	enum { result = 0};
};
template<typename T>
struct IsSame<T,T> {
	enum { result = 1 };
};

//������������Ƿ��ܻ���ת��
template<typename Dest, typename Orig>
class CanMatch{
private:
	static char Test(Dest);
	static int Test(...);
	static Orig MakeOrig();
public:
#pragma warning(disable:4244)
	enum { result = sizeof(Test(MakeOrig())) == sizeof(char) };
#pragma warning(default:4244)
};


//���ĳ�������Ƿ�������
template<typename T>
struct isReference{
	enum { result = 0};
};

template<typename T>
struct isReference<T&> {
	enum { result = 1 };
};

template<typename T>
struct isReference<T&&> {
	enum { result = 1 };
};

//������������Ƿ��ܽ���"=="���㡣
template <typename T1, typename T2>
struct CanEqual {
private:
	template <typename = decltype(declval<T1>() == declval<T2>())>
	constexpr static auto answer(int) { return true; };
	constexpr static auto answer(...) { return false; };
public:
	constexpr operator bool() {
		return answer(0);
	}
};


template <typename T1, typename T2, typename T3, typename T4>
struct disable_if {
	typedef T2 t;
};

template <typename T, typename T3, typename T4>
struct disable_if<T, T,T3,T4> {
};

template <typename T1, typename T2, typename T3, typename T4>
struct disable_if<T1&,T2,T3, T4> {
	typedef typename disable_if<T1,T2,T3,T4>::t t;
};

template <typename T1, typename T2, typename T3,typename T4>
struct disable_if<T1, T2&, T3, T4> {
	typedef typename disable_if<T1, T2,T3,T4>::t t;
};

template <typename T1, typename T2, typename T3, typename T4>
struct disable_if<T1&, T2&, T3, T4> {
	typedef typename disable_if<T1, T2,T3,T4>::t t;
};

#define IF_0(Code)
#define IF_1(Code) Code
#define IF(Bool,Code) IF_##Bool##(Code)


#endif