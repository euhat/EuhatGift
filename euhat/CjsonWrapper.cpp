/*
 * Copyright (c) 2020 euhat.com
 */
#include "CjsonWrapper.h"
#include <stdlib.h>

CjsonWrapper::CjsonWrapper(int isArray)
{
	node_ = NULL;
	newNode(0);
}

CjsonWrapper::CjsonWrapper(const CjsonWrapper &from)
{
	node_ = NULL;
	newNode(0, from.node_);
}

CjsonWrapper::~CjsonWrapper()
{
	deleteNode();
}

void CjsonWrapper::newNode(int isRoot, cJSON *node)
{
	if (node_ == node)
		return;

	deleteNode();

	isRoot_ = isRoot;
	node_ = node;

	isArray_ = 0;
	arraySize_ = 0;
	
	if (NULL != node_)
	{
		arraySize_ = cJSON_GetArraySize(node_);
		if (arraySize_ > 0)
			isArray_ = 1;
	}
}

cJSON *CjsonWrapper::getNode()
{
	if (NULL != node_)
		return node_;
	node_ = cJSON_CreateObject();
	isRoot_ = 1;
	return node_;
}

void CjsonWrapper::deleteNode()
{
	if (NULL == node_)
		return;
	if (!isRoot_)
		return;
	cJSON_Delete(node_);
	node_ = NULL;
	isRoot_ = 0;
	isArray_ = 0;
}

int CjsonWrapper::parse(const string &jsonStr)
{
	cJSON *node = cJSON_Parse(jsonStr.c_str());
	if (NULL == node)
	{
		string err = "parse str [" + jsonStr + "] error!";
		throw ParseException(err);
	}
	newNode(1, node);
	return node_ != NULL;
}

string CjsonWrapper::toString(int isFormatted)
{
	char *jsonStr;
	if (isFormatted)
		jsonStr = cJSON_Print(node_);
	else
		jsonStr = cJSON_PrintUnformatted(node_);
	if (NULL == jsonStr)
		return "";

	string out = jsonStr;
	free(jsonStr);
	return out;
}

cJSON *CjsonWrapper::getSubNode(const string &name)
{
	if (name.empty())
		return node_;
	cJSON *subNode = cJSON_GetObjectItem(node_, name.c_str());
	if (NULL == subNode)
	{
		string err = "Can't find node [" + name + "].";
		throw ParseException(err);
	}
	return subNode;
}

template<>
string CjsonWrapper::get(const string name)
{
	return getSubNode(name)->valuestring;
}

template<>
int CjsonWrapper::get(const string name)
{
	return getSubNode(name)->valueint;
}

template<>
double CjsonWrapper::get(const string name)
{
	return getSubNode(name)->valuedouble;
}

template<>
CjsonWrapper CjsonWrapper::get(const string name)
{
	CjsonWrapper wrapper;
	wrapper.newNode(0, getSubNode(name));
	return wrapper;
}

template<>
void CjsonWrapper::put(const string name, string val)
{
	cJSON_AddStringToObject(getNode(), name.c_str(), val.c_str());
}

template<>
void CjsonWrapper::put(const string name, const char *val)
{
	put(name, string(val));
}

template<>
void CjsonWrapper::put(const string name, int val)
{
	cJSON_AddNumberToObject(getNode(), name.c_str(), val);
}

template<>
void CjsonWrapper::put(const string name, float val)
{
	cJSON_AddNumberToObject(getNode(), name.c_str(), val);
}

template<>
void CjsonWrapper::put(const string name, double val)
{
	cJSON_AddNumberToObject(getNode(), name.c_str(), val);
}

template<>
void CjsonWrapper::put(const string name, CjsonWrapper *child)
{
	child->isRoot_ = 0;
	if (child->stringArray_.size() > 0)
	{
		const char **strings = (const char **)malloc(child->stringArray_.size() * sizeof(char *));
		int i = 0;
		for (list<string>::iterator it = child->stringArray_.begin(); it != child->stringArray_.end(); it++, i++)
		{
			strings[i] = it->c_str();
		}
		cJSON *node = cJSON_CreateStringArray(strings, child->stringArray_.size());
		free(strings);
		cJSON_AddItemToObject(getNode(), name.c_str(), node);
		return;
	}
	if (child->intArray_.size() > 0)
	{
		int *numbers = (int *)malloc(child->intArray_.size() * sizeof(int));
		int i = 0;
		for (list<int>::iterator it = child->intArray_.begin(); it != child->intArray_.end(); it++, i++)
		{
			numbers[i] = *it;
		}
		cJSON *node = cJSON_CreateIntArray(numbers, child->intArray_.size());
		free(numbers);
		cJSON_AddItemToObject(getNode(), name.c_str(), node);
		return;
	}
	if (child->floatIntArray_.size() > 0)
	{
		float *numbers = (float *)malloc(child->floatIntArray_.size() * sizeof(float));
		int i = 0;
		for (list<float>::iterator it = child->floatIntArray_.begin(); it != child->floatIntArray_.end(); it++, i++)
		{
			numbers[i] = *it;
		}
		cJSON *node = cJSON_CreateFloatArray(numbers, child->floatIntArray_.size());
		free(numbers);
		cJSON_AddItemToObject(getNode(), name.c_str(), node);
		return;
	}
	if (child->doubleIntArray_.size() > 0)
	{
		double *numbers = (double *)malloc(child->doubleIntArray_.size() * sizeof(double));
		int i = 0;
		for (list<double>::iterator it = child->doubleIntArray_.begin(); it != child->doubleIntArray_.end(); it++, i++)
		{
			numbers[i] = *it;
		}
		cJSON *node = cJSON_CreateDoubleArray(numbers, child->doubleIntArray_.size());
		free(numbers);
		cJSON_AddItemToObject(getNode(), name.c_str(), node);
		return;
	}
	if (child->nodeArray_.size() > 0)
	{
		cJSON *parent = cJSON_CreateArray();
		for (list<cJSON *>::iterator it = child->nodeArray_.begin(); it != child->nodeArray_.end(); it++)
		{
			cJSON_AddItemToArray(parent, *it);
		}
		cJSON_AddItemToObject(getNode(), name.c_str(), parent);
		return;
	}
	cJSON_AddItemToObject(getNode(), name.c_str(), child->node_);
}

template<>
void CjsonWrapper::pushBack(string val)
{
	stringArray_.push_back(val);
}

template<>
void CjsonWrapper::pushBack(const char *val)
{
	pushBack(string(val));
}

template<>
void CjsonWrapper::pushBack(int val)
{
	intArray_.push_back(val);
}

template<>
void CjsonWrapper::pushBack(float val)
{
	floatIntArray_.push_back(val);
}

template<>
void CjsonWrapper::pushBack(double val)
{
	doubleIntArray_.push_back(val);
}

template<>
void CjsonWrapper::pushBack(CjsonWrapper *member)
{
	member->isRoot_ = 0;
	nodeArray_.push_back(member->node_);
}

CjsonWrapper CjsonWrapper::getArrayMember(int idx)
{
	CjsonWrapper wrapper;
	wrapper.newNode(0, cJSON_GetArrayItem(node_, idx));
	return wrapper;
}

CjsonWrapper::iterator CjsonWrapper::begin()
{
	return CjsonWrapper::iterator(0, this);
}

CjsonWrapper::iterator CjsonWrapper::end()
{
	return CjsonWrapper::iterator(arraySize_, this);
}
