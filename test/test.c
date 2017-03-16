
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <math.h>

#include "../src/parse.h"
#include "../src/stringpify.h"

static int total = 0;
static int pass = 0;

int test_malloc_string(char** s, const char* str)
{
	if (s == NULL || str == NULL) return -1;
	int len = strlen(str);
	*s = (char*)malloc(len + 1);
	if (*s == NULL) return -1;
	memcpy(*s, str, len); (*s)[len] = '\0';
	return 0;
}

void test_remove_useless()
{
	total++;
	char src[] = "  abd \trtq\n\n\rr   ";
	char dst[] = "abdrtqr";
	char *temp = NULL;
	if (slow_remove_useless(src, &temp)) return;
	int result = strcmp(temp, dst);
	if (result == 0) pass++;
	free(temp);
}

void test_parse_null()
{
	total++;
	char* s1 = NULL;
	if (test_malloc_string(&s1, "null") != 0) return;
	char* tmp1 = s1;
	total++;
	char* s2 = NULL;
	if (test_malloc_string(&s2, "nul") != 0) return;
	char* tmp2 = s2;
	total++;
	char* s3 = NULL;
	if (test_malloc_string(&s3, "nxll") != 0) return;
	char* tmp3 = s3;
	total++;
	char* s4 = NULL;
	if (test_malloc_string(&s4, "nullxxx") != 0) return;
	char* tmp4 = s4;

	slow_null_t jn;
	if (slow_parse_null(&s1, &jn) == 0) pass++;
	if (slow_parse_null(&s2, &jn) != 0) pass++;
	if (slow_parse_null(&s3, &jn) != 0) pass++;
	if (slow_parse_null(&s4, &jn) == 0) pass++;

	free(tmp1);
	free(tmp2);
	free(tmp3);
	free(tmp4);
}

void test_parse_true()
{
	total++;
	char* s1 = NULL;
	if (test_malloc_string(&s1, "true") != 0) return;
	char* tmp1 = s1;
	total++;
	char* s2 = NULL;
	if (test_malloc_string(&s2, "tru") != 0) return;
	char* tmp2 = s2;
	total++;
	char* s3 = NULL;
	if (test_malloc_string(&s3, "trxe") != 0) return;
	char* tmp3 = s3;
	total++;
	char* s4 = NULL;
	if (test_malloc_string(&s4, "truexx") != 0) return;
	char* tmp4 = s4;

	slow_true_t jt;
	if (slow_parse_true(&s1, &jt) == 0) pass++;
	if (slow_parse_true(&s2, &jt) != 0) pass++;
	if (slow_parse_true(&s3, &jt) != 0) pass++;
	if (slow_parse_true(&s4, &jt) == 0) pass++;

	free(tmp1);
	free(tmp2);
	free(tmp3);
	free(tmp4);
}

void test_parse_false()
{
	total++;
	char* s1 = NULL;
	if (test_malloc_string(&s1, "false") != 0) return;
	char* tmp1 = s1;
	total++;
	char* s2 = NULL;
	if (test_malloc_string(&s2, "flase") != 0) return;
	char* tmp2 = s2;
	total++;
	char* s3 = NULL;
	if (test_malloc_string(&s3, "fals") != 0) return;
	char* tmp3 = s3;
	total++;
	char* s4 = NULL;
	if (test_malloc_string(&s4, "falsee") != 0) return;
	char* tmp4 = s4;

	slow_false_t jf;
	if (slow_parse_false(&s1, &jf) == 0) pass++;
	if (slow_parse_false(&s2, &jf) != 0) pass++;
	if (slow_parse_false(&s3, &jf) != 0) pass++;
	if (slow_parse_false(&s4, &jf) == 0) pass++;

	free(tmp1);
	free(tmp2);
	free(tmp3);
	free(tmp4);
}

void test_number(const char* sn, double d)
{
	total++;
	char* s = NULL;
	if (test_malloc_string(&s, sn) != 0) return;
	char* tmp = s;
	slow_number_t jn;
	if (slow_parse_number(&tmp, &jn) == 0 && slow_cmp_number(&jn, d) == 0) pass++;
	free(s);
}

void test_parse_number()
{
	test_number("0", 0);
	test_number("100", 100);
	test_number("1.234E+10", 1.234E+10);
	test_number("1.234E-10", 1.234E-10);
	test_number("-1.234E10", -1.234E10);
	test_number("-1.234E-10", -1.234E-10);
	test_number("1.0000000000000002", 1.0000000000000002);
}

void test_parse_string()
{
	total++;
	char* s1 = NULL;
	if (test_malloc_string(&s1, "\"key\"") != 0) return;
	char* tmp1 = s1;
	total++;
	char* s2 = NULL;
	if (test_malloc_string(&s2, "\"luoluo") != 0) return;
	char* tmp2 = s2;
	total++;
	char* s3 = NULL;
	if (test_malloc_string(&s3, "luoluolu\"") != 0) return;
	char* tmp3 = s3;
	total++;
	char* s4 = NULL;
	if (test_malloc_string(&s4, "\"\"") != 0) return;
	char* tmp4 = s4;
	total++;
	char* s5 = NULL;
	if (test_malloc_string(&s5, "\"\"") != 0) return;
	char* tmp5 = s5;

	slow_string_t js; 
	if (slow_parse_string(&s1, &js, 1) == 0 && slow_cmp_string(&js, "key") == 0) pass++;
	if (slow_parse_string(&s2, &js, 1) != 0) pass++;
	if (slow_parse_string(&s3, &js, 1) != 0) pass++;
	if (slow_parse_string(&s4, &js, 1) != 0) pass++;
	if (slow_parse_string(&s5, &js, 0) == 0 && js.len == 0) pass++;

	free(tmp1);
	free(tmp2);
	free(tmp3);
	free(tmp4);
	free(tmp5);
	slow_release_string(&js);
}

void test_parse_object()
{
	total++;
	char* s1 = NULL;
	if (test_malloc_string(&s1, "{\"key\":null}") != 0) return;
	char* tmp1 = s1;
	slow_object_t jo1;
	slow_init_object(&jo1);
	if (slow_parse_object(&s1, &jo1) == 0 && jo1.next != NULL && jo1.next->next == NULL)
	{
		slow_kv_list_t* jkv = jo1.next;
		if (slow_cmp_string(&(jkv->node.key), "key") == 0 && jkv->node.value.type == ST_NULL) pass++;
	}
	slow_release_object(&jo1);
	free(tmp1);

	total++;
	char* s2 = NULL;
	if (test_malloc_string(&s2, "{\"key\":\"value\"}") != 0) return;
	char* tmp2 = s2;
	slow_object_t jo2;
	slow_init_object(&jo2);
	if (slow_parse_object(&s2, &jo2) == 0 && jo2.next != NULL && jo2.next->next == NULL)
	{
		slow_kv_list_t* jkv = jo2.next;
		if (slow_cmp_string(&(jkv->node.key), "key") == 0 && jkv->node.value.type == ST_STRING
			&& slow_cmp_string((slow_string_t*)jkv->node.value.p, "value") == 0) pass++;
	}
	slow_release_object(&jo2);
	free(tmp2);

	total++;
	char* s3 = NULL;
	if (test_malloc_string(&s3, "{\"key\":1234.1234}") != 0) return;
	char* tmp3 = s3;
	slow_object_t jo3;
	slow_init_object(&jo3);
	if (slow_parse_object(&s3, &jo3) == 0 && jo3.next != NULL && jo3.next->next == NULL)
	{
		slow_kv_list_t* jkv = jo3.next;
		if (slow_cmp_string(&(jkv->node.key), "key") == 0 && jkv->node.value.type == ST_NUMBER
			&& slow_cmp_number((slow_number_t*)jkv->node.value.p, 1234.1234) == 0) pass++;
	}
	slow_release_object(&jo3);
	free(tmp3);

	total++;
	char* s4 = NULL;
	if (test_malloc_string(&s4, "{\"key\":null,\"key1\":123.123}") != 0) return;
	char* tmp4 = s4;
	slow_object_t jo4;
	slow_init_object(&jo4);
	if (slow_parse_object(&s4, &jo4) == 0 && jo4.next != NULL && jo4.next->next != NULL && jo4.next->next->next == NULL)
	{
		slow_kv_list_t* jkv1 = jo4.next;
		if (slow_cmp_string(&(jkv1->node.key), "key1") == 0 && jkv1->node.value.type == ST_NUMBER
			&& slow_cmp_number((slow_number_t*)jkv1->node.value.p, 123.123) == 0)
		{
			slow_kv_list_t* jkv2 = jkv1->next;
			if (slow_cmp_string(&(jkv2->node.key), "key") == 0 && jkv2->node.value.type == ST_NULL) pass++;
		}
	}
	slow_release_object(&jo4);
	free(tmp4);

	total++;
	char* s5 = NULL;
	if (test_malloc_string(&s5, "{\"key\":{\"key\":123.123}}") != 0) return;
	char* tmp5 = s5;
	slow_object_t jo5;
	slow_init_object(&jo5);
	if (slow_parse_object(&s5, &jo5) == 0 && jo5.next != NULL && jo5.next->next == NULL)
	{
		slow_kv_list_t* jkv1 = jo5.next;
		if (slow_cmp_string(&(jkv1->node.key), "key") == 0 && jkv1->node.value.type == ST_OBJECT)
		{
			slow_object_t* tempJo = (slow_object_t*)jkv1->node.value.p;
			if (tempJo->next != NULL && tempJo->next->next == NULL)
			{
				slow_kv_list_t* jkv2 = tempJo->next;
				if (slow_cmp_string(&(jkv2->node.key), "key") == 0 && jkv2->node.value.type == ST_NUMBER
					&& slow_cmp_number((slow_number_t*)jkv2->node.value.p, 123.123) == 0) pass++;
			}
		}
	}
	slow_release_object(&jo5);
	free(tmp5);

}

void test_parse_base()
{
	total++;
	char* s1 = NULL;
	if (test_malloc_string(&s1, "null") != 0) return;
	char* tmp1 = s1;
	slow_base_t jb1;
	if (slow_parse_base(&s1, &jb1) == 0 && jb1.type == ST_NULL) pass++;
	slow_release_base(&jb1);
	free(tmp1);

	total++;
	char* s2 = NULL;
	if (test_malloc_string(&s2, "false") != 0) return;
	char* tmp2 = s2;
	slow_base_t jb2;
	if (slow_parse_base(&s2, &jb2) == 0 && jb2.type == ST_FALSE) pass++;
	slow_release_base(&jb2);
	free(tmp2);

	total++;
	char* s3 = NULL;
	if (test_malloc_string(&s3, "true") != 0) return;
	char* tmp3 = s3;
	slow_base_t jb3;
	if (slow_parse_base(&s3, &jb3) == 0 && jb3.type == ST_TRUE) pass++;
	slow_release_base(&jb3);
	free(tmp3);

	total++;
	char* s4 = NULL;
	if (test_malloc_string(&s4, "1234") != 0) return;
	char* tmp4 = s4;
	slow_base_t jb4;
	if (slow_parse_base(&s4, &jb4) == 0 && jb4.type == ST_NUMBER && slow_cmp_number((slow_number_t*)jb4.p, 1234) == 0) pass++;
	slow_release_base(&jb4);
	free(tmp4);

	total++;
	char* s5 = NULL;
	if (test_malloc_string(&s5, "1234.12") != 0) return;
	char* tmp5 = s5;
	slow_base_t jb5;
	if (slow_parse_base(&s5, &jb5) == 0 && jb5.type == ST_NUMBER 
		&& slow_cmp_number((slow_number_t*)jb5.p, 1234.12) == 0) pass++;
	slow_release_base(&jb5);
	free(tmp5);

	total++;
	char* s6 = NULL;
	if (test_malloc_string(&s6, "\"qwerqwe\"") != 0) return;
	char* tmp6 = s6;
	slow_base_t jb6; 
	if (slow_parse_base(&s6, &jb6) == 0 && jb6.type == ST_STRING && slow_cmp_string((slow_string_t*)jb6.p, "qwerqwe") == 0) pass++;
	slow_release_base(&jb6);
	free(tmp6);
}

void test_parse_kv()
{
	total++;
	char* s1 = NULL;
	if (test_malloc_string(&s1, "\"key\":null") != 0) return;
	char* tmp1 = s1;
	slow_kv_t jkv1; 
	if (slow_parse_kv(&s1, &jkv1) == 0 && slow_cmp_string(&jkv1.key, "key") == 0
		&& jkv1.value.type == ST_NULL) pass++;
	slow_release_kv(&jkv1);
	free(tmp1);

	total++;
	char* s2 = NULL;
	if (test_malloc_string(&s2, "\"keyq:null") != 0) return;
	char* tmp2 = s2;
	slow_kv_t jkv2;
	if (slow_parse_kv(&s2, &jkv2) != 0) pass++;
	slow_release_kv(&jkv2);
	free(tmp2);

	total++;
	char* s3 = NULL;
	if (test_malloc_string(&s3, "\"key\":false") != 0) return;
	char* tmp3 = s3;
	slow_kv_t jkv3;
	if (slow_parse_kv(&s3, &jkv3) == 0 && slow_cmp_string(&jkv3.key, "key") == 0
		&& jkv3.value.type == ST_FALSE) pass++;
	slow_release_kv(&jkv3);
	free(tmp3);

	total++;
	char* s4 = NULL;
	if (test_malloc_string(&s4, "\"key\":true") != 0) return;
	char* tmp4 = s4;
	slow_kv_t jkv4; 
	if (slow_parse_kv(&s4, &jkv4) == 0 && slow_cmp_string(&jkv4.key, "key") == 0
		&& jkv4.value.type == ST_TRUE) pass++;
	slow_release_kv(&jkv4);
	free(tmp4);

	total++;
	char* s5 = NULL;
	if (test_malloc_string(&s5, "\"key\":1234.1234") != 0) return;
	char* tmp5 = s5;
	slow_kv_t jkv5;
	if (slow_parse_kv(&s5, &jkv5) == 0 && slow_cmp_string(&jkv5.key, "key") == 0
		&& jkv5.value.type == ST_NUMBER && slow_cmp_number((slow_number_t*)jkv5.value.p, 1234.1234) == 0) pass++;
	slow_release_kv(&jkv5);
	free(tmp5);

	total++;
	char* s6 = NULL;
	if (test_malloc_string(&s6, "\"key\":\"value\"") != 0) return;
	char* tmp6 = s6;
	slow_kv_t jkv6;
	if (slow_parse_kv(&s6, &jkv6) == 0 && slow_cmp_string(&jkv6.key, "key") == 0
		&& jkv6.value.type == ST_STRING && slow_cmp_string((slow_string_t*)jkv6.value.p, "value") == 0) pass++;
	slow_release_kv(&jkv6);
	free(tmp6);

	total++;
	char* s7 = NULL;
	if (test_malloc_string(&s7, "\"key\":{\"key\":\"value\"}") != 0) return;
	char* tmp7 = s7;
	slow_kv_t jkv7;
	if (slow_parse_kv(&s7, &jkv7) == 0 && slow_cmp_string(&jkv7.key, "key") == 0
		&& jkv7.value.type == ST_OBJECT && ((slow_object_t*)jkv7.value.p)->next != NULL)
	{
		slow_kv_list_t *jkvl = ((slow_object_t*)jkv7.value.p)->next;
		if (jkvl->next == NULL && slow_cmp_string(&(jkvl->node.key), "key") == 0
			&& jkvl->node.value.type == ST_STRING && slow_cmp_string((slow_string_t*)jkvl->node.value.p, "value") == 0) pass++;
	}
	slow_release_kv(&jkv7);
	free(tmp7);
}

void test_parse_array()
{
	total++;
	char* s1 = NULL;
	if (test_malloc_string(&s1, "[]") != 0) return;
	char* tmp1 = s1;
	slow_array_t ja1;
	slow_init_array(&ja1);
	if (slow_parse_array(&s1, &ja1) == 0 && ja1.next == NULL) pass++;
	slow_release_array(&ja1);
	free(tmp1);

	total++;
	char* s2 = NULL;
	if (test_malloc_string(&s2, "[1123.321]") != 0) return;
	char* tmp2 = s2;
	slow_array_t ja2;
	slow_init_array(&ja2);
	if (slow_parse_array(&s2, &ja2) == 0 && ja2.next != NULL && ja2.next->next == NULL && ja2.next->node.type == ST_NUMBER)
	{
		slow_number_t* tempJn = (slow_number_t*)ja2.next->node.p;
		if (slow_cmp_number(tempJn, 1123.321) == 0) pass++;
	}
	slow_release_array(&ja2);
	free(tmp2);

	total++;
	char* s3;
	if (test_malloc_string(&s3, "[\"key\",true,{\"key\":123.123},[12,false]]") != 0) return;
	char* tmp3 = s3;
	slow_array_t ja3;
	slow_init_array(&ja3);
	if (slow_parse_array(&s3, &ja3) == 0 && ja3.next != NULL && ja3.next->node.type == ST_ARRAY && ja3.next->next != NULL && ja3.next->next->node.type == ST_OBJECT
		&& ja3.next->next->next != NULL && ja3.next->next->next->node.type == ST_TRUE
		&& ja3.next->next->next->next != NULL && ja3.next->next->next->next->node.type == ST_STRING
		&& ja3.next->next->next->next->next == NULL)
	{
		slow_array_t* tempJa1 = (slow_array_t*)ja3.next->node.p;
		// 第一层array
		if (tempJa1->next != NULL && tempJa1->next->next != NULL && tempJa1->next->next->next == NULL
			&& tempJa1->next->node.type == ST_FALSE && tempJa1->next->next->node.type == ST_NUMBER
			&& slow_cmp_number(tempJa1->next->next->node.p, 12) == 0)
		{
			// 第二层object
			slow_object_t* tempJo1 = (slow_object_t*)ja3.next->next->node.p;
			if (slow_cmp_string(&(tempJo1->next->node.key), "key") == 0 && tempJo1->next->node.value.type == ST_NUMBER
				&& slow_cmp_number((slow_number_t*)tempJo1->next->node.value.p, 123.123) == 0)
			{
				// 第四层true
				slow_string_t* tempJs1 = (slow_string_t*)ja3.next->next->next->next->node.p;
				if (slow_cmp_string(tempJs1, "key") == 0) pass++;
			}
		}
	}
	slow_release_array(&ja3);
	free(tmp3);
}

void test_to_string()
{
	total++;
	slow_null_t jn;
	slow_init_null(&jn);
	slow_ret_string_t jrs1;
	slow_init_ret_string(&jrs1);
	if (slow_null2string(&jn, &jrs1) == 0 && jrs1.offset == 4 && memcmp(jrs1.p, "null", 4) == 0) pass++;
	slow_release_ret_string(&jrs1);

	total++;
	slow_false_t jf;
	slow_init_false(&jf);
	slow_ret_string_t jrs2;
	slow_init_ret_string(&jrs2);
	if (slow_false2string(&jf, &jrs2) == 0 && jrs2.offset == 5 && memcmp(jrs2.p, "false", 5) == 0) pass++;
	slow_release_ret_string(&jrs2);

	total++;
	slow_true_t jt;
	slow_init_true(&jt);
	slow_ret_string_t jrs3;
	slow_init_ret_string(&jrs3);
	if (slow_true2string(&jt, &jrs3) == 0 && jrs3.offset == 4 && memcmp(jrs3.p, "true", 4) == 0) pass++;
	slow_release_ret_string(&jrs3);

	total++;
	slow_number_t jn1;
	slow_init_number(&jn1, 12345.12345);
	slow_ret_string_t jrs4;
	slow_init_ret_string(&jrs4);
	if (slow_number2string(&jn1, &jrs4) == 0 && jrs4.offset == 11 && memcmp(jrs4.p, "12345.12345", 11) == 0) pass++;
	slow_release_ret_string(&jrs4);

	total++;
	slow_string_t js;
	slow_init_string(&js, "tyuytyr");
	slow_ret_string_t jrs5;
	slow_init_ret_string(&jrs5);
	if (slow_string2string(&js, &jrs5) == 0 && jrs5.offset == 9 && memcmp(jrs5.p, "\"tyuytyr\"", 9) == 0) pass++;
	slow_release_ret_string(&jrs5);
}

int main()
{
	test_remove_useless();
	test_parse_null();
	test_parse_true();
	test_parse_false();
	test_parse_number();
	test_parse_string();
	test_parse_base();
	test_parse_object();
	test_parse_kv();
	test_parse_array();

	test_to_string();

	printf("total: %d pass: %d\n", total, pass);
}
