
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <math.h>
#include <string.h>

#include "../src/parse.h"
#include "../src/stringpify.h"
#include "../src/json.h"
#include "../src/common.h"

static int test_total = 0;
static int test_pass = 0;

#define MAX_SIZE 4096
char buffer[MAX_SIZE];

/*
 * base function. 
 */

void test_eq_int(int expect, int actual)
{
	test_total++;
	if (expect == actual) test_pass++;
}

void test_uneq_int(int expect, int actual)
{
	test_total++;
	if (expect != actual) test_pass++;
}

void test_eq_double(double expect, double actual)
{
	test_total++;
	if (fabs(expect - actual) < ACCURACY) test_pass++;
}

void test_eq_string(const char* expect, const char* actual, int len)
{
	test_total++;
	if (strlen(expect) == len && memcmp(expect, actual, len) == 0) test_pass++;
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
	slow_release_base(&sb);
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
	slow_release_base(&sb);
}

void test_parse_false()
{
	test_pass_parse_false("false");
	test_pass_parse_false("falseeeeee");
	test_unpass_parse("fxlse");
	test_unpass_parse("fals");
}

void test_pass_parse_true(const char* s)
{
	slow_base_t sb;
	slow_init_base(&sb);
	test_eq_int(SLOW_OK, slow_parse(s, &sb));
	test_eq_int(ST_TRUE, sb.type);
	test_eq_int(1, ((slow_true_t*)sb.p)->flag);
	slow_release_base(&sb);
}

void test_parse_true()
{
	test_pass_parse_true("true");
	test_pass_parse_true("trueeeee");
	test_unpass_parse("ture");
	test_unpass_parse("tru");
}

void test_number(const char* sn, double d)
{
	slow_base_t sb;
	slow_init_base(&sb);
	test_eq_int(SLOW_OK, slow_parse(sn, &sb));
	test_eq_int(ST_NUMBER, sb.type);
	test_eq_double(d, ((slow_number_t*)sb.p)->d);
	slow_release_base(&sb);
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
	slow_base_t sb;
	slow_init_base(&sb);
	test_eq_int(SLOW_OK, slow_parse(src, &sb));
	test_eq_int(ST_STRING, sb.type);
	slow_string_t* temp = (slow_string_t*)sb.p;
	test_eq_string(cmp, temp->p, temp->offset);
	slow_release_base(&sb);
}

void test_parse_string()
{
	test_string("\"key\"", "key");
	test_string("\"\\\"\"", "\"");
	test_string("\"as\twe\n\"", "as\twe\n");
}

void test_parse_object()
{
	int n;
	double d;
	slow_object_t* pso_out;
	slow_object_t* pso_in;
	slow_string_t* pss;
	slow_array_t* psa;
	slow_base_t* psb;

	slow_base_t sb;
	slow_init_base(&sb);
	test_eq_int(SLOW_OK, slow_parse(
		" { "
        "\"n\" : null , "
        "\"f\" : false , "
        "\"t\" : true , "
        "\"i\" : 123 , "
        "\"s\" : \"abc\", "
        "\"a\" : [ 1, 2, 3 ],"
        "\"o\" : { \"1\" : 1, \"2\" : 2, \"3\" : 3 }"
		" } ", 
	&sb));
	test_eq_int(ST_OBJECT, sb.type);
	
	pso_out = (slow_object_t*)sb.p;
	test_eq_int(SLOW_OK, slow_object_get_null(pso_out, "n", &n));
	test_eq_int(1, n);
	test_eq_int(SLOW_OK, slow_object_get_bool(pso_out, "f", &n));
	test_eq_int(0, n);
	test_eq_int(SLOW_OK, slow_object_get_bool(pso_out, "t", &n));
	test_eq_int(1, n);
	
	test_eq_int(SLOW_OK, slow_object_get_number(pso_out, "i", &d));
	test_eq_double(123, d);
	
	test_eq_int(SLOW_OK, slow_object_get_string(pso_out, "s", &pss));
	test_eq_string("abc", pss->p, pss->offset);

	test_eq_int(SLOW_OK, slow_object_get_array(pso_out, "a", &psa));
	test_eq_int(3, slow_array_get_size(psa));
	test_eq_int(SLOW_OK, slow_array_get_by_index(psa, 1, &psb));
	test_eq_int(ST_NUMBER, psb->type);
	test_eq_double(1, ((slow_number_t*)psb->p)->d);
	test_eq_int(SLOW_OK, slow_array_get_by_index(psa, 2, &psb));
	test_eq_int(ST_NUMBER, psb->type);
	test_eq_double(2, ((slow_number_t*)psb->p)->d);
	test_eq_int(SLOW_OK, slow_array_get_by_index(psa, 3, &psb));
	test_eq_int(ST_NUMBER, psb->type);
	test_eq_double(3, ((slow_number_t*)psb->p)->d);

	test_eq_int(SLOW_OK, slow_object_get_object(pso_out, "o", &pso_in));
	test_eq_int(SLOW_OK, slow_object_get_number(pso_in, "1", &d));
	test_eq_double(1, d);
	test_eq_int(SLOW_OK, slow_object_get_number(pso_in, "2", &d));
	test_eq_double(2, d);
	test_eq_int(SLOW_OK, slow_object_get_number(pso_in, "3", &d));
	test_eq_double(3, d);
	slow_release_base(&sb);
}

void test_parse_kv()
{
	slow_kv_t skv;
	slow_src_t ss;
	slow_string_t* pss;
	slow_kv_t* pskv;

	slow_init_kv(&skv);
	slow_init_src(&ss, "\"key\":null");
	test_eq_int(SLOW_OK, slow_parse_kv(&ss, &skv));
	test_eq_string("key", skv.key.p, skv.key.offset);
	test_eq_int(ST_NULL, skv.value.type);
	test_eq_int(1, ((slow_null_t*)skv.value.p)->flag);
	slow_release_kv(&skv);
	
	slow_init_kv(&skv);
	slow_init_src(&ss, "\"keyq:null");
	test_uneq_int(SLOW_OK, slow_parse_kv(&ss, &skv));
	slow_release_kv(&skv);

	slow_init_kv(&skv);
	slow_init_src(&ss, "\"key\":true");
	test_eq_int(SLOW_OK, slow_parse_kv(&ss, &skv));
	test_eq_int(ST_TRUE, skv.value.type);
	test_eq_int(1, ((slow_true_t*)skv.value.p)->flag);
	slow_release_kv(&skv);

	slow_init_kv(&skv);
	slow_init_src(&ss, "\"key\":1234.1234");
	test_eq_int(SLOW_OK, slow_parse_kv(&ss, &skv));
	test_eq_string("key", skv.key.p, skv.key.offset);
	test_eq_int(ST_NUMBER, skv.value.type);
	test_eq_double(1234.1234, ((slow_number_t*)skv.value.p)->d);
	slow_release_kv(&skv);

	slow_init_kv(&skv);
	slow_init_src(&ss, "\"key\":\"value\"");
	test_eq_int(SLOW_OK, slow_parse_kv(&ss, &skv));
	test_eq_string("key", skv.key.p, skv.key.offset);
	test_eq_int(ST_STRING, skv.value.type);
	pss = (slow_string_t*)skv.value.p;
	test_eq_string("value", pss->p, pss->offset);
	slow_release_kv(&skv);

	slow_init_kv(&skv);
	slow_init_src(&ss, "\"key\":{\"key\":\"value\"}");
	test_eq_int(SLOW_OK, slow_parse_kv(&ss, &skv));
	test_eq_string("key", skv.key.p, skv.key.offset);
	test_eq_int(ST_OBJECT, skv.value.type);
	pskv = &(((slow_object_t*)skv.value.p)->next->node);
	test_eq_string("key", pskv->key.p, pskv->key.offset);
	test_eq_int(ST_STRING, pskv->value.type);
	test_eq_string("value", ((slow_string_t*)pskv->value.p)->p, ((slow_string_t*)pskv->value.p)->offset);
	slow_release_kv(&skv);
}

void test_parse_array()
{
	slow_base_t sb;
	slow_array_t* psa_out;
	slow_array_t* psa_in;
	slow_string_t* pss;
	slow_base_t* psb_out;
	slow_base_t* psb_in;

	slow_init_base(&sb);
	test_eq_int(SLOW_OK, slow_parse("[ ]", &sb));
	test_eq_int(ST_ARRAY, sb.type);
	test_eq_int(0, slow_array_get_size((slow_array_t*)sb.p));
	slow_release_base(&sb);

	slow_init_base(&sb);
	test_eq_int(SLOW_OK, slow_parse("[ null , false , true , 123 , \"abc\" ]", &sb));
	test_eq_int(ST_ARRAY, sb.type);
	psa_out = (slow_array_t*)sb.p;
	test_eq_int(5, slow_array_get_size(psa_out));
	test_eq_int(SLOW_OK, slow_array_get_by_index(psa_out, 1, &psb_out)); // index 1
	test_eq_int(ST_NULL, psb_out->type);
	test_eq_int(1, ((slow_null_t*)psb_out->p)->flag);
	test_eq_int(SLOW_OK, slow_array_get_by_index(psa_out, 2, &psb_out)); // index 2
	test_eq_int(ST_FALSE, psb_out->type);
	test_eq_int(1, ((slow_false_t*)psb_out->p)->flag);
	test_eq_int(SLOW_OK, slow_array_get_by_index(psa_out, 3, &psb_out)); // index 3
	test_eq_int(ST_TRUE, psb_out->type);
	test_eq_int(1, ((slow_true_t*)psb_out->p)->flag);
	test_eq_int(SLOW_OK, slow_array_get_by_index(psa_out, 4, &psb_out)); // index 4
	test_eq_int(ST_NUMBER, psb_out->type);
	test_eq_double(123, ((slow_number_t*)psb_out->p)->d);
	test_eq_int(SLOW_OK, slow_array_get_by_index(psa_out, 5, &psb_out)); // index 5
	test_eq_int(ST_STRING, psb_out->type);
	pss = (slow_string_t*)psb_out->p;
	test_eq_string("abc", pss->p, pss->offset);
	slow_release_base(&sb);

	slow_init_base(&sb);
	test_eq_int(SLOW_OK, slow_parse("[ [ ] , [ 0 ] , [ 0 , 1 ] , [ 0 , 1 , 2 ] ]", &sb));
	test_eq_int(ST_ARRAY, sb.type);
	psa_out = (slow_array_t*)sb.p;
	test_eq_int(4, slow_array_get_size(psa_out));

	test_eq_int(SLOW_OK, slow_array_get_by_index(psa_out, 1, &psb_out)); // out index 1
	test_eq_int(ST_ARRAY, psb_out->type);
	psa_in = (slow_array_t*)psb_out->p;
	test_eq_int(0, slow_array_get_size(psa_in));

	test_eq_int(SLOW_OK, slow_array_get_by_index(psa_out, 2, &psb_out)); // out index 2
	test_eq_int(ST_ARRAY, psb_out->type);
	psa_in = (slow_array_t*)psb_out->p;
	test_eq_int(1, slow_array_get_size(psa_in));
	test_eq_int(SLOW_OK, slow_array_get_by_index(psa_in, 1, &psb_in)); // in index 1
	test_eq_int(ST_NUMBER, psb_in->type);
	test_eq_double(0, ((slow_number_t*)psb_in)->d);

	test_eq_int(SLOW_OK, slow_array_get_by_index(psa_out, 3, &psb_out)); // out index 3
	test_eq_int(ST_ARRAY, psb_out->type);
	psa_in = (slow_array_t*)psb_out->p;
	test_eq_int(2, slow_array_get_size(psa_in));
	test_eq_int(SLOW_OK, slow_array_get_by_index(psa_in, 1, &psb_in)); // in index 1
	test_eq_int(ST_NUMBER, psb_in->type);
	test_eq_double(0, ((slow_number_t*)psb_in->p)->d);
	test_eq_int(SLOW_OK, slow_array_get_by_index(psa_in, 2, &psb_in)); // in index 2
	test_eq_int(ST_NUMBER, psb_in->type);
	test_eq_double(1, ((slow_number_t*)psb_in->p)->d);

	test_eq_int(SLOW_OK, slow_array_get_by_index(psa_out, 4, &psb_out)); // out index 4
	test_eq_int(ST_ARRAY, psb_out->type);
	psa_in = (slow_array_t*)psb_out->p;
	test_eq_int(3, slow_array_get_size(psa_in));
	test_eq_int(SLOW_OK, slow_array_get_by_index(psa_in, 1, &psb_in)); // in index 1
	test_eq_int(ST_NUMBER, psb_in->type);
	test_eq_double(0, ((slow_number_t*)psb_in->p)->d);
	test_eq_int(SLOW_OK, slow_array_get_by_index(psa_in, 2, &psb_in)); // in index 2
	test_eq_int(ST_NUMBER, psb_in->type);
	test_eq_double(1, ((slow_number_t*)psb_in->p)->d);
	test_eq_int(SLOW_OK, slow_array_get_by_index(psa_in, 3, &psb_in)); // in index 3
	test_eq_int(ST_NUMBER, psb_in->type);
	test_eq_double(2, ((slow_number_t*)psb_in->p)->d);

	slow_release_base(&sb);
}

void test_to_string()
{
	test_total++;
	slow_null_t jn;
	slow_init_null(&jn);
	slow_string_t ps1;
	slow_init_string(&ps1);
	if (slow_null2string(&jn, &ps1) == 0 && ps1.offset == 4 && memcmp(ps1.p, "null", 4) == 0) test_pass++;
	slow_release_string(&ps1);

	test_total++;
	slow_false_t jf;
	slow_init_false(&jf);
	slow_string_t ps2;
	slow_init_string(&ps2);
	if (slow_false2string(&jf, &ps2) == 0 && ps2.offset == 5 && memcmp(ps2.p, "false", 5) == 0) test_pass++;
	slow_release_string(&ps2);

	test_total++;
	slow_true_t jt;
	slow_init_true(&jt);
	slow_string_t ps3;
	slow_init_string(&ps3);
	if (slow_true2string(&jt, &ps3) == 0 && ps3.offset == 4 && memcmp(ps3.p, "true", 4) == 0) test_pass++;
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
	test_total++;
	slow_string_t js;
	slow_init_string(&js);
	slow_string_pushs(&js, "tyuytyr");
	slow_string_t ps5;
	slow_init_string(&ps5);
	if (slow_string2string(&js, &ps5) == 0 && ps5.offset == 9 && memcmp(ps5.p, "\"tyuytyr\"", 9) == 0) test_pass++;
	slow_release_string(&ps5);
}

void test_stringpify()
{
	slow_src_t raw_ss;
	slow_string_t ss;

	slow_null_t sn;
	slow_init_null(&sn);
	slow_init_src(&raw_ss, "null");
	test_eq_int(SLOW_OK, slow_parse_null(&raw_ss, &sn));
	slow_init_string(&ss);
	test_eq_int(SLOW_OK, slow_null2string(&sn, &ss));
	test_eq_string("null", ss.p, ss.offset);
	slow_release_string(&ss);

	slow_false_t sf;
	slow_init_false(&sf);
	slow_init_src(&raw_ss, "false");
	test_eq_int(SLOW_OK, slow_parse_false(&raw_ss, &sf));
	slow_init_string(&ss);
	test_eq_int(SLOW_OK, slow_false2string(&sf, &ss));
	test_eq_string("false", ss.p, ss.offset);
	slow_release_string(&ss);

	slow_true_t st;
	slow_init_true(&st);
	slow_init_src(&raw_ss, "true");
	test_eq_int(SLOW_OK, slow_parse_true(&raw_ss, &st));
	slow_init_string(&ss);
	test_eq_int(SLOW_OK, slow_true2string(&st, &ss));
	test_eq_string("true", ss.p, ss.offset);
	slow_release_string(&ss);

	slow_string_t sstr;
	slow_init_string(&sstr);
	slow_init_src(&raw_ss, "\"key13fadf\"");
	test_eq_int(SLOW_OK, slow_parse_string(&raw_ss, &sstr));
	slow_init_string(&ss);
	test_eq_int(SLOW_OK, slow_string2string(&sstr, &ss));
	test_eq_string("\"key13fadf\"", ss.p, ss.offset);
	slow_release_string(&ss);
	slow_release_string(&sstr);
}

void test_file_parse_stringpify()
{
	char* in_name = "in.txt";
	char* out_name = "out.txt";

	FILE* in_file = NULL;
	if (fopen_s(&in_file, in_name, "r") != 0 || in_file == NULL)
	{
		printf_s("open in_file %s error.\n", in_name);
		return;
	}

	FILE* out_file = NULL;
	if (fopen_s(&out_file, out_name, "w") != 0 || out_file == NULL)
	{
		printf_s("open out_file %s error.\n", out_name);
		return;
	}

	int in_size = fread(buffer, sizeof(char), MAX_SIZE, in_file);

	if (in_size <= 0)
	{
		fclose(in_file);
		fclose(out_file);
		printf_s("fread error.\n");
		return;
	}

	if (in_size == MAX_SIZE)
	{
		fclose(in_file);
		fclose(out_file);
		printf_s("in_file too large.\n");
		return;
	}
	buffer[in_size] = '\0';

	slow_base_t sb;
	slow_init_base(&sb);

	if (slow_parse(buffer, &sb) != SLOW_OK)
	{
		slow_release_base(&sb);
		fclose(in_file);
		fclose(out_file);
		printf_s("slow_parse error.\n");
		return;
	}

	slow_string_t ss;
	slow_init_string(&ss);
	if (slow_base2string(&sb, &ss) != SLOW_OK)
	{
		slow_release_base(&sb);
		slow_release_string(&ss);
		fclose(in_file);
		fclose(out_file);
		printf_s("slow_base2string error.\n");
		return;
	}

	int out_size = fwrite(ss.p, sizeof(char), ss.offset, out_file);
	if (out_size < 0)
	{
		slow_release_base(&sb);
		slow_release_string(&ss);
		fclose(in_file);
		fclose(out_file);
		printf_s("fwrite error.\n");
		return;
	}

	slow_release_base(&sb);
	slow_release_string(&ss);
	fclose(in_file);
	fclose(out_file);

	return;
}


int main()
{
	test_parse_null();
	test_parse_false();
	test_parse_true();
	test_parse_number();
	test_parse_string();
	test_parse_kv();
	test_parse_object();
	test_parse_array();
	test_stringpify();
	test_file_parse_stringpify();
	printf("test_total: %d test_pass: %d\n", test_total, test_pass);
}
