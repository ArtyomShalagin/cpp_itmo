#include <stdio.h>
#include <stdexcept>
#include <cstdlib>

#include "file_descriptor.h"

// если бросается exception, вызывается деструктор

template <typename T> 
struct a 
{
	scoped_ptr():ptr(nullptr)
	{}

	scoped_ptr(T* ptr):ptr(ptr)
	{}

	~scoped_ptr()
	{
		delete ptr;
	}

	scoped_ptr(scoped_ptr const&) = delete;
	scoped_ptr& operator=(scoped_ptr const&) = delete;

	void reset (T& nptr = nullptr)
	{
		delete ptr;
		ptr = nptr;
	}

	T* get() const {
		return ptr;
	}

	T* release()
	{
		T* tmp = ptr;
		ptr = nullptr;
		return tmp;
	}

	T& operator*() const
	{
		return *ptr;
	}

	T* operator->() const
	{
		return ptr;
	}
};

struct scoped_ptr
{
	
	private:
		T* ptr;
};

struct string
{
	string() : string("")
	{}

	string(char const* chars)
	:data(my_strdup(chars))
	, size(strlen(chars))//инициализация мемеров
	{}

	string (string  const& other)
	:string(other.c_str()) //перевызов конструктора
	{}
}

int main() {
	try 
	{
		a aa;
		throw 2;
	}
	catch (int)
	{
		printf("a"); //aa - удалена, все ок
	}

	try
	{
		a aa = *new a();
		throw 2;
	}
	catch (int) 
	{
		printf("b"); //aa не удалена. потерялась память
	}
	file_descriptor a, b, c; //лучше делать в класс (хз почему)
	scoped_ptr<int> d(new int(5)); //деструктор вызовется при выходе из scope
}

// 1. nothrow(delete, free, swap) никогда не бросает исключений
// 2. strong(strict) во время возникновения исключения структура остается без изменений
// 3. weak не ломает программу но и сам хер знает каким останется

// nullptr -- появился в c++11
// в C если заменить NULL на nullptr то ничего не поменяется, в С++ это не так: 
// в темплейтах (не проходили) и перегрузке функций (void f(int); void f(void *)) 
// непонятно что мы хотели передать

// NULL --  какая-то хуйня с нулем. в c++98 define NULL 0
// есть тип nullptr_t 

//swap_trick:
string& operator=(string const& rhs) 
{
	string(rhs).swap(*this);
	return *this;
}
//распространено чтобы сделать strong:
void f_basic()
{

}

void f_strong()
{
	string copy(*this);
	copy.f_basic();
	swap(copy, *this);
}

// бросание исключение из деструктора если снаружи еще одно исключение приведет к вылету программы.
