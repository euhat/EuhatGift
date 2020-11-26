/*
 * Copyright (c) 2020 euhat.com
 */

#pragma once

#include "cJSON.h"
#include <string>
#include <list>

using namespace std;

class CjsonWrapper
{
	void newNode(int isRoot, cJSON *node = NULL);
	cJSON *getNode();
	void deleteNode();
	cJSON *getSubNode(const string &name);
	CjsonWrapper getArrayMember(int idx);

	cJSON *node_;
	int isRoot_;
	int isArray_;
	int arraySize_;
	list<string> stringArray_;
	list<double> doubleIntArray_;
	list<float> floatIntArray_;
	list<int> intArray_;
	list<cJSON *> nodeArray_;
public:
	class ParseException : public exception
	{
		string msg_;
	public:
		ParseException(const string &msg)
		{
			msg_ = msg;
		}
		const char *what() const noexcept override
		{
			return msg_.c_str();
		}
	};
	class iterator
	{
		int idx_;
		CjsonWrapper *parent_;
		CjsonWrapper *tmp_;
	public:
		iterator()
		{
			iterator(0, NULL);
		}
		iterator(int idx, CjsonWrapper *parent)
		{
			idx_ = idx;
			parent_ = parent;
			tmp_ = NULL;
		}
		iterator(const iterator &other)
		{
			iterator(other.idx_, other.parent_);
		}
		~iterator()
		{
			delete tmp_;
		}
		iterator &operator++()
		{
			idx_++;
			return *this;
		}
		bool operator!=(iterator &other)
		{
			return idx_ != other.idx_;
		}
		CjsonWrapper operator*()
		{
			return parent_->getArrayMember(idx_);
		}
		CjsonWrapper *operator->()
		{
			if (NULL == tmp_)
				tmp_ = new CjsonWrapper();
			*tmp_ = parent_->getArrayMember(idx_);
			return tmp_;
		}
	};
	CjsonWrapper(int isArray = 0);
	CjsonWrapper(const CjsonWrapper &from);
	~CjsonWrapper();

	int parse(const string &jsonStr);
	string toString(int isFormatted = 1);

	template<class T>
	T get(const string name);

	template<typename T>
	void put(const string name, T t);

	template<class T>
	void pushBack(T t);

	iterator begin();
	iterator end();
};
