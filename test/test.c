#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <math.h>

#include "../src/parser.h"

static int total = 0;
static int pass = 0;

int mallocString(char** s, const char* str)
{
	if (s == NULL || str == NULL) return -1;
	int len = strlen(str);
	*s = (char*)malloc(len + 1);
	if (*s == NULL) return -1;
	memcpy(*s, str, len); (*s)[len] = '\0';
	return 0;
}

void testRemoveUseless()
{
	total++;
	char src[] = "  abd \trtq\n\n\rr   ";
	char dst[] = "abdrtqr";
	char *temp = NULL;
	if (removeUseless(src, &temp)) return;
	int result = strcmp(temp, dst);
	if (result == 0) pass++;
	free(temp);
}

void testParseNull()
{
	total++;
	char* s1 = NULL;
	if (mallocString(&s1, "null") != 0) return;
	char* tmp1 = s1;
	total++;
	char* s2 = NULL;
	if (mallocString(&s2, "nul") != 0) return;
	char* tmp2 = s2;
	total++;
	char* s3 = NULL;
	if (mallocString(&s3, "nxll") != 0) return;
	char* tmp3 = s3;
	total++;
	char* s4 = NULL;
	if (mallocString(&s4, "nullxxx") != 0) return;
	char* tmp4 = s4;

	JsonNull jn;
	if (parseNull(&s1, &jn) == 0) pass++;
	if (parseNull(&s2, &jn) != 0) pass++;
	if (parseNull(&s3, &jn) != 0) pass++;
	if (parseNull(&s4, &jn) == 0) pass++;

	free(tmp1);
	free(tmp2);
	free(tmp3);
	free(tmp4);
}

void testParseTrue()
{
	total++;
	char* s1 = NULL;
	if (mallocString(&s1, "true") != 0) return;
	char* tmp1 = s1;
	total++;
	char* s2 = NULL;
	if (mallocString(&s2, "tru") != 0) return;
	char* tmp2 = s2;
	total++;
	char* s3 = NULL;
	if (mallocString(&s3, "trxe") != 0) return;
	char* tmp3 = s3;
	total++;
	char* s4 = NULL;
	if (mallocString(&s4, "truexx") != 0) return;
	char* tmp4 = s4;

	JsonTrue jt;
	if (parseTrue(&s1, &jt) == 0) pass++;
	if (parseTrue(&s2, &jt) != 0) pass++;
	if (parseTrue(&s3, &jt) != 0) pass++;
	if (parseTrue(&s4, &jt) == 0) pass++;

	free(tmp1);
	free(tmp2);
	free(tmp3);
	free(tmp4);
}

void testParseFalse()
{
	total++;
	char* s1 = NULL;
	if (mallocString(&s1, "false") != 0) return;
	char* tmp1 = s1;
	total++;
	char* s2 = NULL;
	if (mallocString(&s2, "flase") != 0) return;
	char* tmp2 = s2;
	total++;
	char* s3 = NULL;
	if (mallocString(&s3, "fals") != 0) return;
	char* tmp3 = s3;
	total++;
	char* s4 = NULL;
	if (mallocString(&s4, "falsee") != 0) return;
	char* tmp4 = s4;

	JsonFalse jf;
	if (parseFalse(&s1, &jf) == 0) pass++;
	if (parseFalse(&s2, &jf) != 0) pass++;
	if (parseFalse(&s3, &jf) != 0) pass++;
	if (parseFalse(&s4, &jf) == 0) pass++;

	free(tmp1);
	free(tmp2);
	free(tmp3);
	free(tmp4);
}

void testParseNumber()
{
	total++;
	char* s1 = NULL;
	if (mallocString(&s1, "3.14") != 0) return;
	char* tmp1 = s1;
	total++;
	char* s2 = NULL;
	if (mallocString(&s2, "0.123") != 0) return;
	char* tmp2 = s2;
	total++;
	char* s3 = NULL;
	if (mallocString(&s3, ".12312312") != 0) return;
	char* tmp3 = s3;
	total++;
	char* s4 = NULL;
	if (mallocString(&s4, "1.2.2") != 0) return;
	char* tmp4 = s4;
	total++;
	char* s5 = NULL;
	if (mallocString(&s5, "1234.") != 0) return;
	char* tmp5 = s5;

	JsonNumber jn;
	if (parseNumber(&s1, &jn) == 0 && cmpJsonNumber(&jn, 3.14) == 0) pass++;
	if (parseNumber(&s2, &jn) == 0 && cmpJsonNumber(&jn, 0.123) == 0) pass++;
	if (parseNumber(&s3, &jn) != 0) pass++;
	if (parseNumber(&s4, &jn) == 0) pass++;
	if (parseNumber(&s5, &jn) != 0) pass++;

	free(tmp1);
	free(tmp2);
	free(tmp3);
	free(tmp4);
	free(tmp5);
}

void testParseString()
{
	total++;
	char* s1 = NULL;
	if (mallocString(&s1, "\"key\"") != 0) return;
	char* tmp1 = s1;
	total++;
	char* s2 = NULL;
	if (mallocString(&s2, "\"luoluo") != 0) return;
	char* tmp2 = s2;
	total++;
	char* s3 = NULL;
	if (mallocString(&s3, "luoluolu\"") != 0) return;
	char* tmp3 = s3;
	total++;
	char* s4 = NULL;
	if (mallocString(&s4, "\"\"") != 0) return;
	char* tmp4 = s4;
	total++;
	char* s5 = NULL;
	if (mallocString(&s5, "\"\"") != 0) return;
	char* tmp5 = s5;

	JsonString js; 
	if (parseString(&s1, &js, 1) == 0 && cmpJsonString(&js, "key") == 0) pass++;
	if (parseString(&s2, &js, 1) != 0) pass++;
	if (parseString(&s3, &js, 1) != 0) pass++;
	if (parseString(&s4, &js, 1) != 0) pass++;
	if (parseString(&s5, &js, 0) == 0 && js.len == 0) pass++;

	free(tmp1);
	free(tmp2);
	free(tmp3);
	free(tmp4);
	free(tmp5);
	releaseJsonString(&js);
}

void testParseObject()
{
	total++;
	char* s1 = NULL;
	if (mallocString(&s1, "{\"key\":null}") != 0) return;
	char* tmp1 = s1;
	JsonObject jo1;
	initJsonObject(&jo1);
	if (parseObject(&s1, &jo1) == 0 && jo1.next != NULL && jo1.next->next == NULL)
	{
		JsonKVList* jkv = jo1.next;
		if (cmpJsonString(&(jkv->node.key), "key") == 0 && jkv->node.value.type == JT_NULL) pass++;
	}
	releaseJsonObject(&jo1);
	free(tmp1);

	total++;
	char* s2 = NULL;
	if (mallocString(&s2, "{\"key\":\"value\"}") != 0) return;
	char* tmp2 = s2;
	JsonObject jo2;
	initJsonObject(&jo2);
	if (parseObject(&s2, &jo2) == 0 && jo2.next != NULL && jo2.next->next == NULL)
	{
		JsonKVList* jkv = jo2.next;
		if (cmpJsonString(&(jkv->node.key), "key") == 0 && jkv->node.value.type == JT_STRING
			&& cmpJsonString((JsonString*)jkv->node.value.p, "value") == 0) pass++;
	}
	releaseJsonObject(&jo2);
	free(tmp2);

	total++;
	char* s3 = NULL;
	if (mallocString(&s3, "{\"key\":1234.1234}") != 0) return;
	char* tmp3 = s3;
	JsonObject jo3;
	initJsonObject(&jo3);
	if (parseObject(&s3, &jo3) == 0 && jo3.next != NULL && jo3.next->next == NULL)
	{
		JsonKVList* jkv = jo3.next;
		if (cmpJsonString(&(jkv->node.key), "key") == 0 && jkv->node.value.type == JT_NUMBER
			&& cmpJsonNumber((JsonNumber*)jkv->node.value.p, 1234.1234) == 0) pass++;
	}
	releaseJsonObject(&jo3);
	free(tmp3);

	total++;
	char* s4 = NULL;
	if (mallocString(&s4, "{\"key\":null,\"key1\":123.123}") != 0) return;
	char* tmp4 = s4;
	JsonObject jo4;
	initJsonObject(&jo4);
	if (parseObject(&s4, &jo4) == 0 && jo4.next != NULL && jo4.next->next != NULL && jo4.next->next->next == NULL)
	{
		JsonKVList* jkv1 = jo4.next;
		if (cmpJsonString(&(jkv1->node.key), "key1") == 0 && jkv1->node.value.type == JT_NUMBER
			&& cmpJsonNumber((JsonNumber*)jkv1->node.value.p, 123.123) == 0)
		{
			JsonKVList* jkv2 = jkv1->next;
			if (cmpJsonString(&(jkv2->node.key), "key") == 0 && jkv2->node.value.type == JT_NULL) pass++;
		}
	}
	releaseJsonObject(&jo4);
	free(tmp4);

	total++;
	char* s5 = NULL;
	if (mallocString(&s5, "{\"key\":{\"key\":123.123}}") != 0) return;
	char* tmp5 = s5;
	JsonObject jo5;
	initJsonObject(&jo5);
	if (parseObject(&s5, &jo5) == 0 && jo5.next != NULL && jo5.next->next == NULL)
	{
		JsonKVList* jkv1 = jo5.next;
		if (cmpJsonString(&(jkv1->node.key), "key") == 0 && jkv1->node.value.type == JT_OBJECT)
		{
			JsonObject* tempJo = (JsonObject*)jkv1->node.value.p;
			if (tempJo->next != NULL && tempJo->next->next == NULL)
			{
				JsonKVList* jkv2 = tempJo->next;
				if (cmpJsonString(&(jkv2->node.key), "key") == 0 && jkv2->node.value.type == JT_NUMBER
					&& cmpJsonNumber((JsonNumber*)jkv2->node.value.p, 123.123) == 0) pass++;
			}
		}
	}
	releaseJsonObject(&jo5);
	free(tmp5);

}

void testParseBase()
{
	total++;
	char* s1 = NULL;
	if (mallocString(&s1, "null") != 0) return;
	char* tmp1 = s1;
	JsonBase jb1;
	if (parseBase(&s1, &jb1) == 0 && jb1.type == JT_NULL) pass++;
	releaseJsonBase(&jb1);
	free(tmp1);

	total++;
	char* s2 = NULL;
	if (mallocString(&s2, "false") != 0) return;
	char* tmp2 = s2;
	JsonBase jb2;
	if (parseBase(&s2, &jb2) == 0 && jb2.type == JT_FALSE) pass++;
	releaseJsonBase(&jb2);
	free(tmp2);

	total++;
	char* s3 = NULL;
	if (mallocString(&s3, "true") != 0) return;
	char* tmp3 = s3;
	JsonBase jb3;
	if (parseBase(&s3, &jb3) == 0 && jb3.type == JT_TRUE) pass++;
	releaseJsonBase(&jb3);
	free(tmp3);

	total++;
	char* s4 = NULL;
	if (mallocString(&s4, "1234") != 0) return;
	char* tmp4 = s4;
	JsonBase jb4;
	if (parseBase(&s4, &jb4) == 0 && jb4.type == JT_NUMBER && cmpJsonNumber((JsonNumber*)jb4.p, 1234) == 0) pass++;
	releaseJsonBase(&jb4);
	free(tmp4);

	total++;
	char* s5 = NULL;
	if (mallocString(&s5, "1234.12") != 0) return;
	char* tmp5 = s5;
	JsonBase jb5;
	if (parseBase(&s5, &jb5) == 0 && jb5.type == JT_NUMBER 
		&& cmpJsonNumber((JsonNumber*)jb5.p, 1234.12) == 0) pass++;
	releaseJsonBase(&jb5);
	free(tmp5);

	total++;
	char* s6 = NULL;
	if (mallocString(&s6, "\"qwerqwe\"") != 0) return;
	char* tmp6 = s6;
	JsonBase jb6; 
	if (parseBase(&s6, &jb6) == 0 && jb6.type == JT_STRING && cmpJsonString((JsonString*)jb6.p, "qwerqwe") == 0) pass++;
	releaseJsonBase(&jb6);
	free(tmp6);
}

void testParseKeyValue()
{
	total++;
	char* s1 = NULL;
	if (mallocString(&s1, "\"key\":null") != 0) return;
	char* tmp1 = s1;
	JsonKeyValue jkv1; 
	if (parseKeyValue(&s1, &jkv1) == 0 && cmpJsonString(&jkv1.key, "key") == 0
		&& jkv1.value.type == JT_NULL) pass++;
	releaseJsonKeyValue(&jkv1);
	free(tmp1);

	total++;
	char* s2 = NULL;
	if (mallocString(&s2, "\"keyq:null") != 0) return;
	char* tmp2 = s2;
	JsonKeyValue jkv2;
	if (parseKeyValue(&s2, &jkv2) != 0) pass++;
	releaseJsonKeyValue(&jkv2);
	free(tmp2);

	total++;
	char* s3 = NULL;
	if (mallocString(&s3, "\"key\":false") != 0) return;
	char* tmp3 = s3;
	JsonKeyValue jkv3;
	if (parseKeyValue(&s3, &jkv3) == 0 && cmpJsonString(&jkv3.key, "key") == 0
		&& jkv3.value.type == JT_FALSE) pass++;
	releaseJsonKeyValue(&jkv3);
	free(tmp3);

	total++;
	char* s4 = NULL;
	if (mallocString(&s4, "\"key\":true") != 0) return;
	char* tmp4 = s4;
	JsonKeyValue jkv4; 
	if (parseKeyValue(&s4, &jkv4) == 0 && cmpJsonString(&jkv4.key, "key") == 0
		&& jkv4.value.type == JT_TRUE) pass++;
	releaseJsonKeyValue(&jkv4);
	free(tmp4);

	total++;
	char* s5 = NULL;
	if (mallocString(&s5, "\"key\":1234.1234") != 0) return;
	char* tmp5 = s5;
	JsonKeyValue jkv5;
	if (parseKeyValue(&s5, &jkv5) == 0 && cmpJsonString(&jkv5.key, "key") == 0
		&& jkv5.value.type == JT_NUMBER && cmpJsonNumber((JsonNumber*)jkv5.value.p, 1234.1234) == 0) pass++;
	releaseJsonKeyValue(&jkv5);
	free(tmp5);

	total++;
	char* s6 = NULL;
	if (mallocString(&s6, "\"key\":\"value\"") != 0) return;
	char* tmp6 = s6;
	JsonKeyValue jkv6;
	if (parseKeyValue(&s6, &jkv6) == 0 && cmpJsonString(&jkv6.key, "key") == 0
		&& jkv6.value.type == JT_STRING && cmpJsonString((JsonString*)jkv6.value.p, "value") == 0) pass++;
	releaseJsonKeyValue(&jkv6);
	free(tmp6);

	total++;
	char* s7 = NULL;
	if (mallocString(&s7, "\"key\":{\"key\":\"value\"}") != 0) return;
	char* tmp7 = s7;
	JsonKeyValue jkv7;
	if (parseKeyValue(&s7, &jkv7) == 0 && cmpJsonString(&jkv7.key, "key") == 0
		&& jkv7.value.type == JT_OBJECT && ((JsonObject*)jkv7.value.p)->next != NULL)
	{
		JsonKVList *jkvl = ((JsonObject*)jkv7.value.p)->next;
		if (jkvl->next == NULL && cmpJsonString(&(jkvl->node.key), "key") == 0
			&& jkvl->node.value.type == JT_STRING && cmpJsonString((JsonString*)jkvl->node.value.p, "value") == 0) pass++;
	}
	releaseJsonKeyValue(&jkv7);
	free(tmp7);
}

void testParseArray()
{
	total++;
	char* s1 = NULL;
	if (mallocString(&s1, "[]") != 0) return;
	char* tmp1 = s1;
	JsonArray ja1;
	initJsonArray(&ja1);
	if (parseArray(&s1, &ja1) == 0 && ja1.next == NULL) pass++;
	releaseJsonArray(&ja1);
	free(tmp1);

	total++;
	char* s2 = NULL;
	if (mallocString(&s2, "[1123.321]") != 0) return;
	char* tmp2 = s2;
	JsonArray ja2;
	initJsonArray(&ja2);
	if (parseArray(&s2, &ja2) == 0 && ja2.next != NULL && ja2.next->next == NULL && ja2.next->node.type == JT_NUMBER)
	{
		JsonNumber* tempJn = (JsonNumber*)ja2.next->node.p;
		if (cmpJsonNumber(tempJn, 1123.321) == 0) pass++;
	}
	releaseJsonArray(&ja2);
	free(tmp2);

	total++;
	char* s3;
	if (mallocString(&s3, "[\"key\",true,{\"key\":123.123},[12,false]]") != 0) return;
	char* tmp3 = s3;
	JsonArray ja3;
	initJsonArray(&ja3);
	if (parseArray(&s3, &ja3) == 0 && ja3.next != NULL && ja3.next->node.type == JT_ARRAY && ja3.next->next != NULL && ja3.next->next->node.type == JT_OBJECT
		&& ja3.next->next->next != NULL && ja3.next->next->next->node.type == JT_TRUE
		&& ja3.next->next->next->next != NULL && ja3.next->next->next->next->node.type == JT_STRING
		&& ja3.next->next->next->next->next == NULL)
	{
		JsonArray* tempJa1 = (JsonArray*)ja3.next->node.p;
		// 第一层array
		if (tempJa1->next != NULL && tempJa1->next->next != NULL && tempJa1->next->next->next == NULL
			&& tempJa1->next->node.type == JT_FALSE && tempJa1->next->next->node.type == JT_NUMBER
			&& cmpJsonNumber(tempJa1->next->next->node.p, 12) == 0)
		{
			// 第二层object
			JsonObject* tempJo1 = (JsonObject*)ja3.next->next->node.p;
			if (cmpJsonString(&(tempJo1->next->node.key), "key") == 0 && tempJo1->next->node.value.type == JT_NUMBER
				&& cmpJsonNumber((JsonNumber*)tempJo1->next->node.value.p, 123.123) == 0)
			{
				// 第四层true
				JsonString* tempJs1 = (JsonString*)ja3.next->next->next->next->node.p;
				if (cmpJsonString(tempJs1, "key") == 0) pass++;
			}
		}
	}
	releaseJsonArray(&ja3);
	free(tmp3);
}

void testToString()
{
	total++;
	JsonNull jn;
	initJsonNull(&jn);
	JsonRetString jrs1;
	initJsonRetString(&jrs1);
	if (nullToString(&jn, &jrs1) == 0 && jrs1.offset == 4 && memcmp(jrs1.p, "null", 4) == 0) pass++;
	releaseJsonRetString(&jrs1);

	total++;
	JsonFalse jf;
	initJsonFalse(&jf);
	JsonRetString jrs2;
	initJsonRetString(&jrs2);
	if (falseToString(&jf, &jrs2) == 0 && jrs2.offset == 5 && memcmp(jrs2.p, "false", 5) == 0) pass++;
	releaseJsonRetString(&jrs2);

	total++;
	JsonTrue jt;
	initJsonTrue(&jt);
	JsonRetString jrs3;
	initJsonRetString(&jrs3);
	if (trueToString(&jt, &jrs3) == 0 && jrs3.offset == 4 && memcmp(jrs3.p, "true", 4) == 0) pass++;
	releaseJsonRetString(&jrs3);

	total++;
	JsonNumber jn1;
	initJsonNumber(&jn1, 12345.12345);
	JsonRetString jrs4;
	initJsonRetString(&jrs4);
	if (numberToString(&jn1, &jrs4) == 0 && jrs4.offset == 11 && memcmp(jrs4.p, "12345.12345", 11) == 0) pass++;
	releaseJsonRetString(&jrs4);

	total++;
	JsonString js;
	initJsonString(&js, "tyuytyr");
	JsonRetString jrs5;
	initJsonRetString(&jrs5);
	if (stringToString(&js, &jrs5) == 0 && jrs5.offset == 9 && memcmp(jrs5.p, "\"tyuytyr\"", 9) == 0) pass++;
	releaseJsonRetString(&jrs5);
}
/*
int main()
{
	testRemoveUseless();
	testParseNull();
	testParseTrue();
	testParseFalse();
	testParseNumber();
	testParseString();
	testParseBase();
	testParseObject();
	testParseKeyValue();
	testParseArray();

	testToString();

	printf("total: %d pass: %d\n", total, pass);
}
*/