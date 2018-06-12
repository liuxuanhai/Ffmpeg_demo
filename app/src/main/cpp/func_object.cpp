#include <iostream>
#include <vector>
#include <algorithm>
#include <functional>
#include <string>
using namespace std;

//函数对象
/*
template <typename T>
class Func
{
public:
	//重载“()”
	void operator()(T &t){
		cout << t << endl;
	}
};

void main()
{

	int i = 9;
	//普通写法
	//Func<int> func;
	//func(i); //func实际是一个对象名称，但是这样写看起来像函数名

	//函数对象，匿名对象
	Func<int>(a);

	system("pause");
}
*/

//函数对象的好处
//函数没有状态，对象才有状态，可以记录函数执行的状态
/*
template <typename T>
class MyPrint
{
public:
	void operator()(T &t)
	{
		cout << t << endl;
	}
};

void main()
{
	vector<int> v1;
	v1.push_back(7);
	v1.push_back(20);
	v1.push_back(5);
	v1.push_back(3);
	v1.push_back(40);

	//遍历v1集合
	//函数对象
	//for_each函数执行过程中，自动将容器中的元素传入MyPrint的operator()函数中
	for_each(v1.begin(), v1.end(), MyPrint<int>());
	system("pause");
}
*/

//谓词（根据参数个数还可分为：一元谓词、二元谓词）
//需求：查找集合中能够被5整除的第一个元素
//用于STL算法函数进行特定的运算
/*
template <typename T>
class MyDivision
{
public:
	bool operator()(T &t)
	{
		return (t % 5 == 0);
	}
};

void main()
{
	vector<int> v1;
	v1.push_back(7);
	v1.push_back(20);
	v1.push_back(5);
	v1.push_back(3);
	v1.push_back(40);

	vector<int>::iterator it = find_if(v1.begin(), v1.end(), MyDivision<int>());
	cout << "第一个能被5整除的元素："<< *it << endl;

	system("pause");
}
*/

/*
//二元谓词
//需求：两个Vector（v1, v2 ），将v1和v2中所有元素相加，然后重新装入一个新的容器v3中
template <typename T>
class MyAdd
{
public:
	T operator()(T &t1, T &t2)
	{
		return t1 + t2;
	}
};

template <typename T>
class MyPrint
{
public:
	MyPrint()
	{
		this->num = 0;
	}
	void operator()(T &t)
	{
		cout << t << endl;
		//统计
		if (t > 50)
		{
			num++;
		}
	}

public:
	int num;
};


void main()
{
	vector<int> v1;
	v1.push_back(7);
	v1.push_back(20);
	v1.push_back(5);
	v1.push_back(3);
	v1.push_back(40);

	vector<int> v2;
	v2.push_back(17);
	v2.push_back(24);
	v2.push_back(12);
	v2.push_back(67);
	v2.push_back(28);

	//v3分配空间跟v2的大小一致
	vector<int> v3(v2.size());

	transform(v1.begin(), v1.end(), v2.begin(), v3.begin(), MyAdd<int>());

	//遍历v3
	//统计v3中大于50的数字的个数
	//MyPrint<int> pobj;
	//pobj = for_each(v3.begin(), v3.end(), pobj);

	//for_each的返回值是你传入的类型
	MyPrint<int> pobj = for_each(v3.begin(), v3.end(), MyPrint<int>());

	cout << "大于50的数字个数：" << pobj.num << endl;

	system("pause");
}
*/

//使用二元谓词进行排序Set
//需求：字符串容器，忽略大小写查找一个字符串
/*
#include <set>
struct CompareNoCase
{
	bool operator()(const string &str1, const string &str2)
	{
		//全部变成小写之后再来比较
		string str_1;
		str_1.resize(str1.size()); //转换之后的长度不变
		transform(str1.begin(), str1.end(), str_1.begin(), tolower); //预定义函数对象

		string str_2;
		str_2.resize(str2.size()); //转换之后的长度不变
		transform(str2.begin(), str2.end(), str_2.begin(), tolower); //预定义函数对象

		cout << "------" << endl;
		return (str_1 == str_2);
	}
};

void main()
{
	set<string, CompareNoCase> s1;
	s1.insert("jason");
	s1.insert("Jack");
	s1.insert("rose");
	s1.insert("evan");

	set<string, CompareNoCase>::iterator it = s1.find("Rose");
	if (it != s1.end())
	{
		cout << "找到了" << endl;
	}
	else
	{
		cout << "没有找到" << endl;
	}

	system("pause");
}
*/

//预定函数
//plus<int> greater<int> equal_to<string>
/*
void main()
{
	//plus<int> myPlus;
	//不是主要用途
	//int x = 9;
	//int y = 10;
	//myPlus(x,y);

	//plus<string> myPlus;
	//string str1 = "alan ";
	//string str2 = "jackson";
	//str1 = myPlus(str1, str2);

	//transform(v1.begin(), v1.end(), v2.begin(), v3.begin(), plus<int>());

	//统计一个字符串容器中出现“rose”的次数
	vector<string> v1;
	v1.push_back("jack");
	v1.push_back("rose");
	v1.push_back("jason");
	v1.push_back("rose");
	v1.push_back("alan");

	string key = "rose";
	int num = count_if(v1.begin(), v1.end(), bind2nd(equal_to<string>(), key));

	cout << "出现的次数：" << num << endl;

	system("pause");
}
*/

//函数适配器
//bind2nd(equal_to<string>(), key)
//把预定义函数对象和第二个参数进行绑定
//需求：查找一个容器中元素大于20的个数
//比较器
template <typename T>
class IsGreat
{
public:
	IsGreat(T num)
	{
		this->num = num;
	}
	bool operator()(T &n)
	{
		return (n > this->num);
	}

private:
	T num;
};

void main()
{
	vector<int> v1;
	v1.push_back(7);
	v1.push_back(20);
	v1.push_back(5);
	v1.push_back(3);
	v1.push_back(40);

	//v1容器中的元素会和10这个属性进行比较
	//int num = count_if(v1.begin(), v1.end(), IsGreat<int>(10));
	//前面二元谓词的两个参数，是由transform算法函数自动解决的
	//IsGreat是我们写的，我们可以给构造函数传参，而预定义的函数不行
	//可以通过函数绑定解决
	//Collections.sort Java api  算法函数
	//bind to second
	int num = count_if(v1.begin(), v1.end(), bind2nd(greater<int>(), 10));

	cout << "大于20的元素的个数:" << num << endl;

	system("pause");
}