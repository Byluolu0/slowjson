
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "../src/json.h"
#include "../src/interface.h"

#define MAX_SIZE 4096
char buffer[MAX_SIZE];

/*
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

	slow_object_t jo;
	slow_init_object(&jo);
	
	if (slow_raw2object(buffer, &jo) != 0) 
	{
		fclose(inFile);
		fclose(outFile);
		printf_s("toJsonObject error.\n");
		return 1;
	}

	double d1 = 0;
	if (slow_object_get_number(&jo, "s", &d1) == 0) printf("%lf\n", d1);

	double d2 = 0;
	if (slow_object_get_number(&jo, "r", &d2) == 0) printf("%lf\n", d2);

	slow_string_t* s1;
	if (slow_object_get_string(&jo, "k", &s1) == 0) printf("%s\n", s1->p);

	slow_object_t* sjo = NULL;
	if (slow_object_get_object(&jo, "c", &sjo) == 0 && sjo != NULL)
	{
		int n1 = 0;
		if (slow_object_get_null(sjo, "c", &n1) == 0) printf("%d\n", n1);

		int n2 = 0;
		if (slow_object_get_bool(sjo, "r", &n2) == 0) printf("%d\n", n2);

		slow_array_t* ja1 = NULL;
		if (slow_object_get_array(sjo, "v", &ja1) == 0 && ja1 != NULL)
		{
			int count = slow_array_get_size(ja1);
			int i;
			for (i = 0; i < count; ++i)
			{
				slow_base_t* jb1 = NULL;
				if (slow_array_get_by_index(ja1, i, &jb1) == 0)
				{
					if (jb1->type == ST_NUMBER)
					{
						printf("%lf\n", ((slow_number_t*)jb1->p)->d);
					}
					else if (jb1->type == ST_TRUE)
					{
						printf("true\n");
					}
					else if (jb1->type == ST_ARRAY)
					{ }
				}
			}
			slow_base_t* jb1 = NULL;
			if (slow_array_get_by_index(ja1, 0, &jb1) == 0)
			{

			}
		}
	}

	char* temp = NULL;
	if (slow_object2raw(&temp, &jo) != 0)
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
*/