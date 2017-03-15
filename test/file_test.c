#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "../src/json.h"
#include "../src/interface.h"

#define MAX_SIZE 4096
char buffer[MAX_SIZE];

int main(int argc, char** argv)
{
	if (argc != 3)
	{
		printf_s("Usage: exe <inputFile> <outputFile>.\n");
		return 1;
	}

	FILE* inFile = NULL;
	if(fopen_s(&inFile, argv[1], "r") != 0 || inFile == NULL)
	{
		printf_s("open inFile %s error.\n", argv[1]);
		return 1;
	}

	FILE* outFile = NULL;
	if(fopen_s(&outFile, argv[2], "w") != 0 || outFile == NULL)
	{
		printf_s("open outFile %s error.\n", argv[2]);
		return 1;
	}

	int inSize = fread(buffer, sizeof(char), MAX_SIZE, inFile);

	if (inSize <= 0)
	{
		fclose(inFile);
		fclose(outFile);
		printf_s("fread error.\n");
		return 1;
	}

	if (inSize == MAX_SIZE)
	{
		fclose(inFile);
		fclose(outFile);
		printf_s("inFile too large.\n");
		return 1;
	}
	buffer[inSize] = '\0';

	JsonObject jo;
	initJsonObject(&jo);
	
	if (toJsonObject(buffer, &jo) != 0) 
	{
		fclose(inFile);
		fclose(outFile);
		printf_s("toJsonObject error.\n");
		return 1;
	}

	double d1 = 0;
	if (ObjectGetNumber(&jo, "s", &d1) == 0) printf("%lf\n", d1);

	double d2 = 0;
	if (ObjectGetNumber(&jo, "r", &d2) == 0) printf("%lf\n", d2);

	JsonString* s1;
	if (ObjectGetString(&jo, "k", &s1) == 0) printf("%s\n", s1->p);

	JsonObject* sjo = NULL;
	if (ObjectGetObject(&jo, "c", &sjo) == 0 && sjo != NULL)
	{
		int n1 = 0;
		if (ObjectGetNull(sjo, "c", &n1) == 0) printf("%d\n", n1);

		int n2 = 0;
		if (ObjectGetBool(sjo, "r", &n2) == 0) printf("%d\n", n2);

		JsonArray* ja1 = NULL;
		if (ObjectGetArray(sjo, "v", &ja1) == 0 && ja1 != NULL)
		{
			int count = ArraySize(ja1);
			int i;
			for (i = 0; i < count; ++i)
			{
				JsonBase* jb1 = NULL;
				if (ArrayGetByIndex(ja1, i, &jb1) == 0)
				{
					if (jb1->type == JT_NUMBER)
					{
						printf("%lf\n", ((JsonNumber*)jb1->p)->d);
					}
					else if (jb1->type == JT_TRUE)
					{
						printf("true\n");
					}
					else if (jb1->type == JT_ARRAY)
					{ }
				}
			}
			JsonBase* jb1 = NULL;
			if (ArrayGetByIndex(ja1, 0, &jb1) == 0)
			{

			}
		}
	}

	char* temp = NULL;
	if (OtoString(&temp, &jo) != 0)
	{
		fclose(inFile);
		fclose(outFile);
		printf_s("OtoString error.\n");
		return 1;
	}

	int len = strlen(temp);

	int outSize = fwrite(temp, sizeof(char), len, outFile);
	if (outSize < 0)
	{
		fclose(inFile);
		fclose(outFile);
		printf_s("fwrite error.\n");
		return 1;
	}

	free(temp);
	fclose(inFile);
	fclose(outFile);

	return 0;
}
