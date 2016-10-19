#pragma once

//этот класс работает как обертка (к примеру для fopen) для класса, 
//который надо вызывать с new и который не удалится при выходе из scope

#include <cstdio>

struct file_descriptor {
	file_descriptor(char const* filename, char const* flags);
	~file_descriptor();

	void read(char* buff, int BUF_SIZE);

private:
	file_descriptor(file_descriptor const&) = delete;	//не позволяет делать file_des a = b;
	file_descriptor operator= (file_descriptor const&) = delete;

private:
	FILE* file;
}