#include <iostream>

struct base {
	virtual ~base() { // деструктор

	}

	virtual void print() = 0; // то же самое, что abstract в жабе
};

struct derived : base {
	derived(std::string const& msg)
		: msg(msg); // вызов конструктора стринга (this.msg = msg) 
	{}

	void print() {
		std::cout << msg << endl;
	}

private:
	std::string msg;
};

struct derived2 : derived {
	~derived\() {
		
	}
};

int main() {
	base* b = new derived("hello");
	b -> print();
	delete b; // вызывает деструктор не того, чего надо
	// но работает, когда короткие строки, потому что сделано так, что стринг внутри себя хранит не только 
	// указатель, но и не большой буфер, поэтому при вызове деструктора при короткой строке объект удаляется нормально
	// а если нет, то будет утечка памяти, потому что у того стрига память, занятая под ссылку, не освободится

	// если есть базовый класс, который на самом деле наследник, то для корректного удаления
	// надо, чтобы у базового класса был виртуальный деструктор
}