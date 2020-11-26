#include "../euhat/IniOp.h"
#include <string>

using namespace std;

string iniReadString(const char *path, const char *section, const char *key, const char *defaultStr)
{
	char *buf = NULL;
	const char *valueStr = iniReadStringC(&buf, path, section, key, defaultStr);
	string out = valueStr;
	free(buf);
	return out;
}

int tstRead()
{
	string str = iniReadString("test.ini", "section", "key1", "what");
	printf("ini read string outputs [%s].\n", str.c_str());
	return 1;
}

int tstWrite()
{
	iniWriteStringC("test.ini", "section", "key1", "hello");
	return 1;
}

int main()
{
	tstWrite();
	tstRead();
	return 1;
}
