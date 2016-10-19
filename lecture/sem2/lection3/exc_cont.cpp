#include <iostream>

struct base
{
	virtual char const* msg() const //без virtual выведется base
	{
		return "base";
	}
};

struct derived : base
{
	char const* msg() const
	{
		return "derived";
	}
};

int main() 
{
	try{
		try
		{
			throw new derived();
		}
		catch (base b)
		// исключение не поймается так как мы бросаем *derived. надо либо 
		// бросать указатель, либо объект. 
		// чтобы выводилось derived надо ловить ссылку, иначе будет вызван 
		// конструктор base от derived но таблица 
		// виртуальных функций будет от base
		{
			std::cout << b.msg();
			throw; //бросится ровно то, что здесь. если написать throw b то 
			//бросим базу
		}
	}	
	catch (base* b)
	{
		std::cout << b->msg() << std::endl;
	}
}