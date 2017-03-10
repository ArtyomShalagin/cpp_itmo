Варианты реализации строки

struct string {
	T* data;
	size_t size;
	size_t capacity;
}

struct string {
	size_t size;
	size_t capacity;
	union {
		T* data;
		T static_data[16];
	}
}

В реализации Apple распределение либо 8-8-8, либо 1-23 + один бит (что-то
	хранится сдвинутым на 1, т.к. полезной информации нет в нем) -- 
короткая или длинная строка хранится

Можно хранить так

struct large_string_buf {
	size_t capacity;
	size_t size;
	char* data;
}

struct small_string_buf {
	uint8_t size;
	char data[23];
}

union string {
	large_string_buf large_buf;
	small_string_buf small_buf;
}

Пример copy on write
Копируешь только когда ссылаешься, до этого хранится ссылка на один и тот же 
объект


struct storage {
	size_t size;
	size_t capacity;
	size_t ref_counter;
	char data[];
}

_____________________

struct header {
	size_t size, capacity, refs;
}

struct string {
	char* data;
	
	header* get_header() {
		return (header*) data - 1;
	}

	char* alloc_data(size_t size) {
		header* ptr = (header*) operator new(sizeof(header) + size);
		return (char*) (ptr + 1);
	}
} 






































