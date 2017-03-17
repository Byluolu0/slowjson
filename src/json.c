
#include "json.h"
#include "common.h"
#include "parse.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>

void slow_init_null(slow_null_t* pn)
{
	assert(pn != NULL);
	pn->flag = 1;
}

void slow_init_false(slow_false_t* pf)
{
	assert(pf != NULL);
	pf->flag = 1;
}

void slow_init_true(slow_true_t* pt)
{
	assert(pt != NULL);
	pt->flag = 1;
}

void slow_init_number(slow_number_t* pn, double d)
{
	assert(pn != NULL);
	pn->d = d;
}

void slow_init_string(slow_string_t* ps)
{
	assert(ps != NULL);
	ps->p = (char*)malloc(STRING_INIT_SIZE);
	ps->size = STRING_INIT_SIZE;
	ps->offset = 0;
}

void slow_init_object(slow_object_t* po)
{
	assert(po != NULL);
	po->next = NULL;
}

void slow_init_array(slow_array_t* pa)
{
	assert(pa != NULL);
	pa->next = NULL;
}

void slow_init_kv(slow_kv_t* pkv)
{
	assert(pkv != NULL);
	slow_init_string(&pkv->key);
	slow_init_base(&pkv->value);
}

void slow_init_kv_list(slow_kv_list_t* pkvl)
{
	assert(pkvl != NULL);
	slow_init_kv(&pkvl->node);
	pkvl->next = NULL;
}

void slow_init_base(slow_base_t* pb)
{
	assert(pb != NULL);
	pb->type = ST_NONE;
	pb->p = NULL;
}

void slow_init_base_list(slow_base_list_t* pbl)
{
	assert(pbl != NULL);
	slow_init_base(&pbl->node);
	pbl->next = NULL;
}

void slow_string_pushs_len(slow_string_t* ps, const char* s, int len)
{
	assert(ps != NULL);
	assert(s != NULL);
	assert(len > 0);
	slow_string_check_size(ps, len);

	memcpy(ps->p + ps->offset, s, len);
	ps->offset += len;
}

void slow_string_pushs(slow_string_t* ps, const char* s)
{
	assert(ps != NULL);
	assert(s != NULL);
	int len = strlen(s);
	slow_string_pushs_len(ps, s, len);
}

void slow_string_pushc(slow_string_t* ps, char ch)
{
	assert(ps != NULL);
	slow_string_check_size(ps, 1);
	ps->p[ps->offset] = ch;
	ps->offset += 1;
}

void slow_string_check_size(slow_string_t* ps, int size)
{
	assert(size > 0);
	if (ps->offset + size <= ps->size) return;
	ps->size += ps->size >> 1;
	char* temp = (char*)realloc(ps->p, ps->size);
	ps->p = temp;
}

int slow_object_get_base(slow_object_t* po, const char* k, slow_base_t** b)
{
	assert(po != NULL);
	assert(k != NULL);
	assert(b != NULL);
	slow_kv_list_t* temp = po->next;
	while (temp)
	{
		slow_kv_t* node = &temp->node;
		if (slow_cmp_string(&node->key, k) == 0)
		{
			*b = &node->value;
			return SLOW_OK;
		}
		temp = temp->next;
	}
	return SLOW_KEY_NOT_FOUND;
}

int slow_object_get_null(slow_object_t* po, const char* k, int* n)
{
	assert(po != NULL);
	assert(k != NULL);

	slow_kv_list_t* temp = po->next;
	while (temp)
	{
		slow_kv_t* node = &temp->node;
		if (slow_cmp_string(&node->key, k) == 0 && node->value.type == ST_NULL)
		{
			*n = 1;
			return SLOW_OK;
		}
		temp = temp->next;
	}
	return SLOW_KEY_NOT_FOUND;
}

int slow_object_get_bool(slow_object_t* po, const char* k, int* b)
{
	assert(po != NULL);
	assert(k != NULL);

	slow_kv_list_t* temp = po->next;
	while (temp)
	{
		slow_kv_t* node = &temp->node;
		if (slow_cmp_string(&node->key, k) == 0 && node->value.type == ST_FALSE)
		{
			*b = 0;
			return SLOW_OK;
		}
		if (slow_cmp_string(&node->key, k) == 0 && node->value.type == ST_TRUE)
		{
			*b = 1;
			return SLOW_OK;
		}
		temp = temp->next;
	}
	return SLOW_KEY_NOT_FOUND;
}

int slow_object_get_number(slow_object_t* po, const char* k, double* d)
{
	assert(po != NULL);
	assert(k != NULL);

	slow_kv_list_t* temp = po->next;
	while (temp)
	{
		slow_kv_t* node = &temp->node;
		if (slow_cmp_string(&node->key, k) == 0 && node->value.type == ST_NUMBER)
		{
			*d = ((slow_number_t*)node->value.p)->d;
			return SLOW_OK;
		}
		temp = temp->next;
	}
	return SLOW_KEY_NOT_FOUND;
}

int slow_object_get_string(slow_object_t* po, const char* k, slow_string_t** ps)
{
	assert(po != NULL);
	assert(k != NULL);

	slow_kv_list_t* temp = po->next;
	while (temp)
	{
		slow_kv_t* node = &temp->node;
		if (slow_cmp_string(&node->key, k) == 0 && node->value.type == ST_STRING)
		{
			*ps = (slow_string_t*)node->value.p;
			return SLOW_OK;
		}
		temp = temp->next;
	}
	return SLOW_KEY_NOT_FOUND;
}

int slow_object_get_object(slow_object_t* po, const char* k, slow_object_t** o)
{
	assert(po != NULL);
	assert(k != NULL);

	slow_kv_list_t* temp = po->next;
	while (temp)
	{
		slow_kv_t* node = &temp->node;
		if (slow_cmp_string(&node->key, k) == 0 && node->value.type == ST_OBJECT)
		{
			*o = (slow_object_t*)node->value.p;
			return SLOW_OK;
		}
		temp = temp->next;
	}
	return SLOW_KEY_NOT_FOUND;
}

int slow_object_get_array(slow_object_t* po, const char* k, slow_array_t** pa)
{
	assert(po != NULL);
	assert(k != NULL);

	slow_kv_list_t* temp = po->next;
	while (temp)
	{
		slow_kv_t* node = &temp->node;
		if (slow_cmp_string(&node->key, k) == 0 && node->value.type == ST_ARRAY)
		{
			*pa = (slow_array_t*)node->value.p;
			return SLOW_OK;
		}
		temp = temp->next;
	}
	return SLOW_KEY_NOT_FOUND;
}

int slow_array_get_size(slow_array_t* ptrArray)
{
	if (ptrArray == NULL) return 0;

	slow_base_list_t* temp = ptrArray->next;
	int count = 0;
	while (temp)
	{
		count++;
		temp = temp->next;
	}
	return count;
}

int slow_array_get_by_index(slow_array_t* ptrArray, int index, slow_base_t** b)
{
	if (ptrArray == NULL) return SLOW_NULL_PTR;

	slow_base_list_t* temp = ptrArray->next;
	int count = 0;
	while (temp)
	{
		if (index == count)
		{
			*b = &(temp->node);
			return SLOW_OK;
		}
		temp = temp->next;
		count++;
	}
	return SLOW_ELEMENT_NOT_FOUND;
}

void slow_release_base(slow_base_t *pb)
{
	assert(pb != NULL);

	if (pb->type == ST_NULL)
	{
		free((slow_null_t*)pb->p);
	}
	else if (pb->type == ST_FALSE)
	{
		free((slow_false_t*)pb->p);
	}
	else if (pb->type == ST_TRUE)
	{
		free((slow_true_t*)pb->p);
	}
	else if (pb->type == ST_NUMBER)
	{
		free((slow_number_t*)pb->p);
	}
	else if (pb->type == ST_STRING)
	{
		free((slow_string_t*)pb->p);
	}
	else if (pb->type == ST_KEY_VALUE)
	{
		slow_kv_t* jkv = (slow_kv_t*)pb->p;
		slow_release_kv(jkv);
		free(jkv);
	}
	else if (pb->type == ST_OBJECT)
	{
		slow_object_t* jo = (slow_object_t*)pb->p;
		slow_release_object(jo);
		free(jo);
	}
	else if (pb->type == ST_ARRAY)
	{
		slow_array_t* ja = (slow_array_t*)pb->p;
		slow_release_array(ja);
		free(ja);
	}
}

void slow_release_base_list(slow_base_list_t* pbl)
{
	assert(pbl != NULL);
	slow_release_base(&pbl->node);
	pbl->next = NULL;
}

void slow_release_string(slow_string_t* ps)
{
	assert(ps != NULL);

	free(ps->p);
	ps->offset = 0;
	ps->size = 0;
}

void slow_release_kv(slow_kv_t* pkv)
{
	assert(pkv != NULL);
	slow_release_string(&pkv->key);
	slow_release_base(&pkv->value);
}

void slow_release_kv_list(slow_kv_list_t* pkvl)
{
	assert(pkvl != NULL);
	slow_release_kv(&pkvl->node);
	pkvl->next = NULL;
}

void slow_release_object(slow_object_t *po)
{
	assert(po != NULL);

	while (po->next)
	{
		slow_kv_list_t* jkvl = po->next;
		po->next = jkvl->next;
		slow_release_kv(&(jkvl->node));
		free(jkvl);
	}
}

void slow_release_array(slow_array_t* pa)
{
	assert(pa != NULL);

	while (pa->next)
	{
		slow_base_list_t* jbl = pa->next;
		pa->next = jbl->next;
		slow_release_base(&jbl->node);
		free(jbl);
	}
}

