#pragma once
#include "../pch.h"

template<class T>
class SingletonBase
{
protected:
	SingletonBase() = default;
	virtual ~SingletonBase() {}//��� ���� ��ü�� ���ؼ� �Ҹ��� ȣ��
	//�̱����̱� ������, ���簡 �Ͼ�� �� ��.
	SingletonBase(const SingletonBase&) = delete;
	SingletonBase& operator=(const SingletonBase&) = delete;
public:
	static T& GetInstance()
	{
		//thread-safe??
		static T instance;
		return instance;
	}
};