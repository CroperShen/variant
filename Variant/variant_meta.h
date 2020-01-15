#ifndef __VARIANT_META_H
#define __VARIANT_META_H
//variant��Ҫ�õ���ĳЩԪ���ģ��


//������������Ƿ���ͬ
template<typename T1, typename T2>
class IsSame {
private:
	enum { result = 0};
};
template<typename T>
class IsSame<T,T> {
private:

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

#define IF_0(Code)
#define IF_1(Code) Code
#define IF(Bool,Code) IF_##Bool##(Code)

#endif