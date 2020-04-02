#pragma once
#include <cocos2d.h>

class MyFileFind
{
public:
	/*
	�ж��ļ��Ƿ����
	*/
	static bool isExist(std::string path);

	/*
	�ú�����������������һ��Ϊ·���ַ���(string���ͣ����Ϊ����·��)��
	�ڶ�������Ϊ�ļ������ļ����ƴ洢����(vector����, ���ô���)��
	*/
	static int getFiles(std::string path, std::vector<std::string>& files);

	/*
	�ݹ�(��������ǰĿ¼����Ŀ¼���ļ�)
	*/
	static void getAllFiles(std::string path, std::vector<std::string>& files);


	/*
	ֻ��ȡĳ����·���µĵ�ǰ���ļ�������(�������ƣ�ֻ�������������ð���ͬ��)
	*/
	static int getJustCurrentDir(std::string path, std::vector<std::string>& files);


	/*
	ֻ��ȡĳ����·���µĵ�ǰ���ļ�������
	*/
	static int getJustCurrentFile(std::string path, std::vector<std::string>& files);


	/*
	ֻ��ȡĳ����·���µ������ļ���(��������ǰĿ¼����Ŀ¼���ļ�)��
	*/
	static void getFilesAll(std::string path, std::vector<std::string>& files);
private:

};