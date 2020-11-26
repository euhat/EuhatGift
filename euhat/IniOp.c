/*
 * Copyright (c) 2020 euhat.com
 */
#include "IniOp.h"
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#ifdef WIN32
#define INI_STRDUP(_str) _strdup(_str)
#define INI_STRCMP(_l,_r) _stricmp(_l, _r)
#else
#define INI_STRDUP(_str) strdup(_str)
#define INI_STRCMP(_l,_r) strcasecmp(_l, _r)
#endif

char *memFromWholeFile(const char *path, unsigned int *len)
{
	FILE *fp = fopen(path, "rb");
	if (NULL == fp)
		return NULL;

	fseek(fp, 0, SEEK_END);
	*len = ftell(fp);
	fseek(fp, 0, SEEK_SET);
	
	char *pData = (char *)malloc(*len + 1);
	fread(pData, 1, *len, fp);
	pData[*len] = 0;

	fclose(fp);
	return pData;
}

typedef struct IniParseParam
{
	char *buf;
	char *p;
	char *valueOut;
	char *sectionStart;
	char *keyStart;
	char *valueStart;
} IniParseParam;

static int gotoChar(char **p, char ch)
{
	while (0 != **p)
	{
		if (ch == **p)
			return 1;
		(*p)++;
	}
	return 0;
}

static int nextLine(char **p)
{
	if (!gotoChar(p, '\n'))
		return 0;
	**p = 0;
	(*p)++;
	return 1;
}

static char *nextSection(IniParseParam *param)
{
	if (!gotoChar(&param->p, '['))
		return NULL;

	param->p++;
	param->sectionStart = param->p;

	if (!gotoChar(&param->p, ']'))
		return NULL;

	*param->p++ = 0;
	nextLine(&param->p);
	return param->sectionStart;
}

static char *nextKey(IniParseParam *param)
{
	param->keyStart = param->p;
	nextLine(&param->p);

	char *p = param->keyStart;
	if (!gotoChar(&p, '='))
		return NULL;

	*p++ = 0;
	param->valueStart = p;
	return param->keyStart;
}

static void iniReadStringMem(IniParseParam *param, const char *section, const char *key)
{
	unsigned int len;

	param->p = param->buf;
	while (NULL != nextSection(param))
	{
		if (INI_STRCMP(param->sectionStart, section) == 0)
		{
			while (NULL != nextKey(param))
			{
				if (INI_STRCMP(param->keyStart, key) == 0)
				{
					param->valueOut = param->valueStart;
					break;
				}
			}
			break;
		}
	}
}

const char *iniReadStringC(char **base, const char *path, const char *section, const char *key, const char *defaultStr)
{
	unsigned int len;
	IniParseParam param = {0};

	param.buf = memFromWholeFile(path, &len);
	if (NULL == param.buf)
		goto returnDefault;

	iniReadStringMem(&param, section, key);

	if (NULL == param.valueOut)
	{
		free(param.buf);
		goto returnDefault;
	}

	*base = param.buf;
	return param.valueOut;

returnDefault:
	return defaultStr;
}

int iniWriteStringC(const char *path, const char *section, const char *key, const char *value)
{
	unsigned int len;
	char *pattern;
	IniParseParam param = {0};
	FILE *fp;
	
	pattern = memFromWholeFile(path, &len);
	if (NULL == pattern)
	{
		fp = fopen(path, "w+");
		if (NULL == fp)
			return 0;
		fprintf(fp, "[%s]\n%s=%s\n", section, key, value);
		fclose(fp);
		return 1;
	}
		
	param.buf = (char *)malloc(len + 1);
	strcpy(param.buf, pattern);
	
	iniReadStringMem(&param, section, key);
	
	fp = fopen(path, "w+");
	if (NULL == param.valueOut)
	{
		if (NULL == param.sectionStart)
		{
			fwrite(pattern, 1, len, fp);
			fprintf(fp, "[%s]\n%s=%s\n", section, key, value);
		}
		else
		{
			param.p = pattern + (param.sectionStart - param.buf);
			gotoChar(&param.p, '\n');
			fwrite(pattern, 1, param.p - pattern, fp);
			fprintf(fp, "\n%s=%s", key, value);
			fwrite(param.p, 1, len - (param.p - pattern), fp);
		}
	}
	else
	{
		fwrite(pattern, 1, param.valueOut - param.buf, fp);
		fprintf(fp, "%s\n", value);
		fwrite(pattern + (param.p - param.buf), 1, len - (param.p - param.buf), fp);
		
	}
	fclose(fp);
	
	free(pattern);
	free(param.buf);
	
	return 1;
}

