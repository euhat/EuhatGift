#include "../euhat/CjsonWrapper.h"

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

int memToFile(const char *path, const char *pData, int iLen)
{
	FILE *fp = fopen(path, "wb+");
	if (NULL == fp)
		return 0;

	fwrite(pData, 1, iLen, fp);

	fclose(fp);
	return 1;
}

int tstRead()
{
	unsigned int len;
	char *jsonStr = memFromWholeFile("test.json", &len);
	if (NULL == jsonStr)
	{
		printf("read test.json failed.\n");
		return 0;
	}

	typedef CjsonWrapper JTree;
	try
	{
		JTree tree;
		tree.parse(jsonStr);

		int intTst = tree.get<int>("intTst");
		string strTst = tree.get<string>("strTst");
		JTree nodeTst = tree.get<JTree>("nodeTst");
		double doubleTst = nodeTst.get<double>("doubleTst");

		JTree arrayTst = tree.get<JTree>("arrayTst");
		int i = 0;
		for (JTree::iterator it = arrayTst.begin(); it != arrayTst.end(); it++, i++)
		{
			printf("arrayTst[%d]:[%s]\n", i, it->get<string>("").c_str());
		}
		printf("intTst:[%d], strTst:[%s], doubleTst:[%f]\n", intTst, strTst.c_str(), (float)doubleTst);
	}
	catch (JTree::ParseException &e)
	{
		printf("json parser error: [%s]\n", e.what());
	}

	free(jsonStr);

	return 1;
}

int tstWrite()
{
	typedef CjsonWrapper JTree;
	JTree tree;
	tree.put("intTst", 123);
	tree.put("strTst", "hello, how are you?");
	JTree arrayTst;
	arrayTst.pushBack("Mike");
	arrayTst.pushBack("Jackson");
	arrayTst.pushBack("Marry");
	tree.put("arrayTst", &arrayTst);
	JTree nodeTst;
	nodeTst.put("doubleTst", 0.123);
	tree.put("nodeTst", &nodeTst);

	string jsonStr = tree.toString();
	memToFile("test.json", jsonStr.c_str(), jsonStr.length());
	return 1;
}

int main()
{
	tstWrite();
	tstRead();
	return 1;
}
