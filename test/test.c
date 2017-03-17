
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <math.h>
#include <string.h>

#include "../src/parse.h"
#include "../src/stringpify.h"
#include "../src/json.h"
#include "../src/common.h"

static int total = 0;
static int pass = 0;

/*
 * base function. 
 */

void test_eq_int(int expect, int actual)
{
	total++;
	if (expect == actual) pass++;
}

void test_uneq_int(int expect, int actual)
{
	total++;
	if (expect != actual) pass++;
}

void test_eq_double(double expect, double actual)
{
	total++;
	if (fabs(expect - actual) < ACCURACY) pass++;
}

void test_eq_string(const char* expect, const char* actual, int len)
{
	total++;
	if (memcmp(expect, actual, len) == 0) pass++;
}

void test_unpass_parse(const char* s)
{
	slow_base_t sb;
	slow_init_base(&sb);
	test_uneq_int(SLOW_OK, slow_parse(s, &sb));
}

int test_malloc_string(char** s, const char* str)
{
	if (s == NULL || str == NULL) return -1;
	int len = strlen(str);
	*s = (char*)malloc(len + 1);
	if (*s == NULL) return -1;
	memcpy(*s, str, len); (*s)[len] = '\0';
	return 0;
}

/*
 * test parse type function.
 */

void test_pass_parse_null(const char* s)
{
	slow_base_t sb;
	slow_init_base(&sb);
	test_eq_int(SLOW_OK, slow_parse(s, &sb));
	test_eq_int(ST_NULL, sb.type);
	test_eq_int(1, ((slow_null_t*)sb.p)->flag);
}

void test_parse_null()
{
	test_pass_parse_null("null");
	test_pass_parse_null("nullllll");
	test_unpass_parse("nul");
	test_unpass_parse("nxll");
}

void test_pass_parse_false(const char* s)
{
	slow_base_t sb;
	slow_init_base(&sb);
	test_eq_int(SLOW_OK, slow_parse(s, &sb));
	test_eq_int(ST_FALSE, sb.type);
	test_eq_int(1, ((slow_false_t*)sb.p)->flag);
}

void test_parse_false()
{
	test_pass_parse_false("false");
	test_pass_parse_false("falseeeeee");
	test_unpass_parse("fxlse");
	test_unpass_parse("fals");
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

void test_string(const char* src, const char* cmp)
{
	total++;
	char* s = NULL;
	if (test_malloc_string(&s, src) != 0) return;
	char* tmp = s;
	slow_string_t ss;
	slow_init_string(&ss);
	if (slow_parse_string(&tmp, &ss) == 0)
	{
		if (slow_cmp_string_len(&ss, cmp, strlen(cmp)) == 0) pass++;
	}
	slow_release_string(&ss);
	free(s);
}

void test_parse_string()
{
	test_string("\"key\"", "key");
	test_string("\"\\\"\"", "\"");
	test_string("\"as\twe\n\"", "as\twe\n");
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
		if (slow_cmp_string_len(&(jkv->node.key), "key", 3) == 0 && jkv->node.value.type == ST_NULL) pass++;
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
		if (slow_cmp_string_len(&(jkv->node.key), "key", 3) == 0 && jkv->node.value.type == ST_STRING
			&& slow_cmp_string_len((slow_string_t*)jkv->node.value.p, "value", 5) == 0) pass++;
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
		if (slow_cmp_string_len(&(jkv->node.key), "key", 3) == 0 && jkv->node.value.type == ST_NUMBER
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
		if (slow_cmp_string_len(&(jkv1->node.key), "key1", 4) == 0 && jkv1->node.value.type == ST_NUMBER
			&& slow_cmp_number((slow_number_t*)jkv1->node.value.p, 123.123) == 0)
		{
			slow_kv_list_t* jkv2 = jkv1->next;
			if (slow_cmp_string_len(&(jkv2->node.key), "key", 3) == 0 && jkv2->node.value.type == ST_NULL) pass++;
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
		if (slow_cmp_string_len(&(jkv1->node.key), "key", 3) == 0 && jkv1->node.value.type == ST_OBJECT)
		{
			slow_object_t* tempJo = (slow_object_t*)jkv1->node.value.p;
			if (tempJo->next != NULL && tempJo->next->next == NULL)
			{
				slow_kv_list_t* jkv2 = tempJo->next;
				if (slow_cmp_string_len(&(jkv2->node.key), "key", 3) == 0 && jkv2->node.value.type == ST_NUMBER
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
	slow_init_base(&jb1);
	if (slow_parse_base(&s1, &jb1) == 0 && jb1.type == ST_NULL) pass++;
	slow_release_base(&jb1);
	free(tmp1);

	total++;
	char* s2 = NULL;
	if (test_malloc_string(&s2, "false") != 0) return;
	char* tmp2 = s2;
	slow_base_t jb2;
	slow_init_base(&jb2);
	if (slow_parse_base(&s2, &jb2) == 0 && jb2.type == ST_FALSE) pass++;
	slow_release_base(&jb2);
	free(tmp2);

	total++;
	char* s3 = NULL;
	if (test_malloc_string(&s3, "true") != 0) return;
	char* tmp3 = s3;
	slow_base_t jb3;
	slow_init_base(&jb3);
	if (slow_parse_base(&s3, &jb3) == 0 && jb3.type == ST_TRUE) pass++;
	slow_release_base(&jb3);
	free(tmp3);

	total++;
	char* s4 = NULL;
	if (test_malloc_string(&s4, "1234") != 0) return;
	char* tmp4 = s4;
	slow_base_t jb4;
	slow_init_base(&jb4);
	if (slow_parse_base(&s4, &jb4) == 0 && jb4.type == ST_NUMBER && slow_cmp_number((slow_number_t*)jb4.p, 1234) == 0) pass++;
	slow_release_base(&jb4);
	free(tmp4);

	total++;
	char* s5 = NULL;
	if (test_malloc_string(&s5, "1234.12") != 0) return;
	char* tmp5 = s5;
	slow_base_t jb5;
	slow_init_base(&jb5);
	if (slow_parse_base(&s5, &jb5) == 0 && jb5.type == ST_NUMBER 
		&& slow_cmp_number((slow_number_t*)jb5.p, 1234.12) == 0) pass++;
	slow_release_base(&jb5);
	free(tmp5);

	total++;
	char* s6 = NULL;
	if (test_malloc_string(&s6, "\"qwerqwe\"") != 0) return;
	char* tmp6 = s6;
	slow_base_t jb6; 
	slow_init_base(&jb6);
	if (slow_parse_base(&s6, &jb6) == 0 && jb6.type == ST_STRING)
	{
		slow_string_t* temps = (slow_string_t*)jb6.p;
		slow_string_pushc(temps, '\0');
		if (slow_cmp_string((slow_string_t*)jb6.p, "qwerqwe") == 0) pass++;
	}
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
	slow_init_kv(&jkv1);
	if (slow_parse_kv(&s1, &jkv1) == 0 && slow_cmp_string_len(&jkv1.key, "key", 3) == 0
		&& jkv1.value.type == ST_NULL) pass++;
	slow_release_kv(&jkv1);
	free(tmp1);

	total++;
	char* s2 = NULL;
	if (test_malloc_string(&s2, "\"keyq:null") != 0) return;
	char* tmp2 = s2;
	slow_kv_t jkv2;
	slow_init_kv(&jkv2);
	if (slow_parse_kv(&s2, &jkv2) != 0) pass++;
	slow_release_kv(&jkv2);
	free(tmp2);

	total++;
	char* s3 = NULL;
	if (test_malloc_string(&s3, "\"key\":false") != 0) return;
	char* tmp3 = s3;
	slow_kv_t jkv3;
	slow_init_kv(&jkv3);
	if (slow_parse_kv(&s3, &jkv3) == 0 && slow_cmp_string_len(&jkv3.key, "key", 3) == 0
		&& jkv3.value.type == ST_FALSE) pass++;
	slow_release_kv(&jkv3);
	free(tmp3);

	total++;
	char* s4 = NULL;
	if (test_malloc_string(&s4, "\"key\":true") != 0) return;
	char* tmp4 = s4;
	slow_kv_t jkv4;
	slow_init_kv(&jkv4);
	if (slow_parse_kv(&s4, &jkv4) == 0 && slow_cmp_string_len(&jkv4.key, "key", 3) == 0
		&& jkv4.value.type == ST_TRUE) pass++;
	slow_release_kv(&jkv4);
	free(tmp4);

	total++;
	char* s5 = NULL;
	if (test_malloc_string(&s5, "\"key\":1234.1234") != 0) return;
	char* tmp5 = s5;
	slow_kv_t jkv5;
	slow_init_kv(&jkv5);
	if (slow_parse_kv(&s5, &jkv5) == 0 && slow_cmp_string_len(&jkv5.key, "key", 3) == 0
		&& jkv5.value.type == ST_NUMBER && slow_cmp_number((slow_number_t*)jkv5.value.p, 1234.1234) == 0) pass++;
	slow_release_kv(&jkv5);
	free(tmp5);

	total++;
	char* s6 = NULL;
	if (test_malloc_string(&s6, "\"key\":\"value\"") != 0) return;
	char* tmp6 = s6;
	slow_kv_t jkv6;
	slow_init_kv(&jkv6);
	if (slow_parse_kv(&s6, &jkv6) == 0 && slow_cmp_string_len(&jkv6.key, "key", 3) == 0
		&& jkv6.value.type == ST_STRING && slow_cmp_string_len((slow_string_t*)jkv6.value.p, "value", 5) == 0) pass++;
	slow_release_kv(&jkv6);
	free(tmp6);

	total++;
	char* s7 = NULL;
	if (test_malloc_string(&s7, "\"key\":{\"key\":\"value\"}") != 0) return;
	char* tmp7 = s7;
	slow_kv_t jkv7;
	slow_init_kv(&jkv7);
	if (slow_parse_kv(&s7, &jkv7) == 0 && slow_cmp_string_len(&jkv7.key, "key", 3) == 0
		&& jkv7.value.type == ST_OBJECT && ((slow_object_t*)jkv7.value.p)->next != NULL)
	{
		slow_kv_list_t *jkvl = ((slow_object_t*)jkv7.value.p)->next;
		if (jkvl->next == NULL && slow_cmp_string_len(&(jkvl->node.key), "key", 3) == 0
			&& jkvl->node.value.type == ST_STRING && slow_cmp_string_len((slow_string_t*)jkvl->node.value.p, "value", 5) == 0) pass++;
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
			if (slow_cmp_string_len(&(tempJo1->next->node.key), "key", 3) == 0 && tempJo1->next->node.value.type == ST_NUMBER
				&& slow_cmp_number((slow_number_t*)tempJo1->next->node.value.p, 123.123) == 0)
			{
				// 第四层true
				slow_string_t* tempJs1 = (slow_string_t*)ja3.next->next->next->next->node.p;
				if (slow_cmp_string_len(tempJs1, "key", 3) == 0) pass++;
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
	slow_string_t ps1;
	slow_init_string(&ps1);
	if (slow_null2string(&jn, &ps1) == 0 && ps1.offset == 4 && memcmp(ps1.p, "null", 4) == 0) pass++;
	slow_release_string(&ps1);

	total++;
	slow_false_t jf;
	slow_init_false(&jf);
	slow_string_t ps2;
	slow_init_string(&ps2);
	if (slow_false2string(&jf, &ps2) == 0 && ps2.offset == 5 && memcmp(ps2.p, "false", 5) == 0) pass++;
	slow_release_string(&ps2);

	total++;
	slow_true_t jt;
	slow_init_true(&jt);
	slow_string_t ps3;
	slow_init_string(&ps3);
	if (slow_true2string(&jt, &ps3) == 0 && ps3.offset == 4 && memcmp(ps3.p, "true", 4) == 0) pass++;
	slow_release_string(&ps3);
	/*
	total++;
	slow_number_t jn1;
	slow_init_number(&jn1, 12345.12345);
	slow_string_t ps4;
	slow_init_string(&ps4);
	if (slow_number2string(&jn1, &ps4) == 0 && ps4.offset == 11 && memcmp(ps4.p, "12345.12345", 11) == 0) pass++;
	slow_release_string(&ps4);
	*/
	total++;
	slow_string_t js;
	slow_init_string(&js);
	slow_string_pushs(&js, "tyuytyr");
	slow_string_t ps5;
	slow_init_string(&ps5);
	if (slow_string2string(&js, &ps5) == 0 && ps5.offset == 9 && memcmp(ps5.p, "\"tyuytyr\"", 9) == 0) pass++;
	slow_release_string(&ps5);
}

int main()
{
	test_parse_null();
	test_parse_false();
	/*
	test_parse_number();
	test_parse_string();
	test_parse_base();
	test_parse_object();
	test_parse_kv();
	test_parse_array();

	test_to_string();
	*/
	printf("total: %d pass: %d\n", total, pass);
}
