#pragma once
#include <cocos2d.h>

class MyFileFind
{
public:
	/*
	判断文件是否存在
	*/
	static bool isExist(std::string path);

	/*
	该函数有两个参数，第一个为路径字符串(string类型，最好为绝对路径)；
	第二个参数为文件夹与文件名称存储变量(vector类型, 引用传递)。
	*/
	static int getFiles(std::string path, std::vector<std::string>& files);

	/*
	递归(即包含当前目录及子目录的文件)
	*/
	static void getAllFiles(std::string path, std::vector<std::string>& files);


	/*
	只读取某给定路径下的当前【文件夹名】(以下类似，只给出函数，调用案例同上)
	*/
	static int getJustCurrentDir(std::string path, std::vector<std::string>& files);


	/*
	只读取某给定路径下的当前【文件名】：
	*/
	static int getJustCurrentFile(std::string path, std::vector<std::string>& files);


	/*
	只读取某给定路径下的所有文件名(即包含当前目录及子目录的文件)：
	*/
	static void getFilesAll(std::string path, std::vector<std::string>& files);
private:

};