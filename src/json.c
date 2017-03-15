#include "json.h"
#include "common.h"
#include "parser.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int slow_init_null(slow_null_t* ptrNull)
{
	if (ptrNull == NULL) return -1;
	ptrNull->flag = 1;
	return 0;
}

int slow_init_false(slow_false_t* ptrFalse)
{
	if (ptrFalse == NULL) return -1;
	ptrFalse->flag = 1;
	return 0;
}

int slow_init_true(slow_true_t* ptrTrue)
{
	if (ptrTrue == NULL) return -1;
	ptrTrue->flag = 1;
	return 0;
}

int slow_init_number(slow_number_t* ptrNumber, double d)
{
	if (ptrNumber == NULL) return -1;
	ptrNumber->d = d;
	return 0;
}

int slow_init_string(slow_string_t* ptrString, const char* s)
{
	if (ptrString == NULL || s == NULL) return -1;
	int len = strlen(s);
	ptrString->p = (char*)malloc(len + 1);
	if (ptrString->p == NULL) return -1;
	memcpy(ptrString->p, s, len);
	ptrString->p[len] = '\0';
	ptrString->len = len;
	return 0;
}

int slow_init_object(slow_object_t* ptrObject)
{
	if (ptrObject == NULL) return -1;
	ptrObject->next = NULL;
	return 0;
}

int slow_init_array(slow_array_t* ptrArray)
{
	if (ptrArray == NULL) return -1;
	ptrArray->next = NULL;
	return 0;
}

int slow_init_ret_string(slow_ret_string_t* ptr)
{
	if (ptr == NULL) return -1;
	ptr->offset = 0;
	ptr->size = RET_STRING_INIT_SIZE;
	ptr->p = (char*)malloc(ptr->size);
	if (ptr->p == NULL) return -1;
	return 0;
}

int slow_check_ret_string_size(slow_ret_string_t* ptr, int size)
{
	if (ptr == NULL || size < 0) return -1;
	if (ptr->offset + size <= ptr->size) return 0;
	char* temp = (char*)realloc(ptr->p, ptr->size * 2);
	if (temp == NULL) return -1;
	ptr->p = temp;
	ptr->size = ptr->size * 2;
	return 0;
}

int slow_object_get_base(slow_object_t* ptrObject, const char* k, slow_base_t** b)
{
	if (ptrObject == NULL || k == NULL) return -1;

	slow_kv_list_t* temp = ptrObject->next;
	while (1)
	{
		if (temp == NULL) return -1;
		slow_kv_t* node = &temp->node;
		if (slow_cmp_string(&node->key, k) == 0)
		{
			*b = &node->value;
			return 0;
		}
		temp = temp->next;
	}
	return -1;
}

int slow_object_get_null(slow_object_t* ptrObject, const char* k, int* n)
{
	if (ptrObject == NULL || k == NULL) return -1;

	slow_kv_list_t* temp = ptrObject->next;
	while (1)
	{
		if (temp == NULL) return -1;
		slow_kv_t* node = &temp->node;
		if (slow_cmp_string(&node->key, k) == 0 && node->value.type == ST_NULL)
		{
			*n = 1;
			return 0;
		}
		temp = temp->next;
	}
	return -1;
}

int slow_object_get_bool(slow_object_t* ptrObject, const char* k, int* b)
{
	if (ptrObject == NULL || k == NULL) return -1;

	slow_kv_list_t* temp = ptrObject->next;
	while (1)
	{
		if (temp == NULL) return -1;
		slow_kv_t* node = &temp->node;
		if (slow_cmp_string(&node->key, k) == 0 && node->value.type == ST_FALSE)
		{
			*b = 0;
			return 0;
		}
		if (slow_cmp_string(&node->key, k) == 0 && node->value.type == ST_TRUE)
		{
			*b = 1;
			return 0;
		}
		temp = temp->next;
	}
	return -1;
}

int slow_object_get_number(slow_object_t* ptrObject, const char* k, double* d)
{
	if (ptrObject == NULL || k == NULL) return -1;

	slow_kv_list_t* temp = ptrObject->next;
	while (1)
	{
		if (temp == NULL) return -1;
		slow_kv_t* node = &temp->node;
		if (slow_cmp_string(&node->key, k) == 0 && node->value.type == ST_NUMBER)
		{
			*d = ((slow_number_t*)node->value.p)->d;
			return 0;
		}
		temp = temp->next;
	}
	return -1;
}

int slow_object_get_string(slow_object_t* ptrObject, const char* k, slow_string_t** s)
{
	if (ptrObject == NULL || k == NULL) return -1;

	slow_kv_list_t* temp = ptrObject->next;
	while (1)
	{
		if (temp == NULL) return -1;
		slow_kv_t* node = &temp->node;
		if (slow_cmp_string(&node->key, k) == 0 && node->value.type == ST_STRING)
		{
			*s = (slow_string_t*)node->value.p;
			return 0;
		}
		temp = temp->next;
	}
	return -1;
}

int slow_object_get_object(slow_object_t* ptrObject, const char* k, slow_object_t** o)
{
	if (ptrObject == NULL || k == NULL) return -1;

	slow_kv_list_t* temp = ptrObject->next;
	while (1)
	{
		if (temp == NULL) return -1;
		slow_kv_t* node = &temp->node;
		if (slow_cmp_string(&node->key, k) == 0 && node->value.type == ST_OBJECT)
		{
			*o = (slow_object_t*)node->value.p;
			return 0;
		}
		temp = temp->next;
	}
	return -1;
}

int slow_object_get_array(slow_object_t* ptrObject, const char* k, slow_array_t** a)
{
	if (ptrObject == NULL || k == NULL) return -1;

	slow_kv_list_t* temp = ptrObject->next;
	while (1)
	{
		if (temp == NULL) return -1;
		slow_kv_t* node = &temp->node;
		if (slow_cmp_string(&node->key, k) == 0 && node->value.type == ST_ARRAY)
		{
			*a = (slow_array_t*)node->value.p;
			return 0;
		}
		temp = temp->next;
	}
	return -1;
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
	if (ptrArray == NULL) return -1;

	slow_base_list_t* temp = ptrArray->next;
	int count = 0;
	while (temp)
	{
		if (index == count)
		{
			*b = &(temp->node);
			return 0;
		}
		temp = temp->next;
		count++;
	}
	return -1;
}

int slow_release_base(slow_base_t *ptrBase)
{
	if (ptrBase == NULL) return -1;

	if (ptrBase->type == ST_NULL)
	{
		free((slow_null_t*)ptrBase->p);
	}
	else if (ptrBase->type == ST_FALSE)
	{
		free((slow_false_t*)ptrBase->p);
	}
	else if (ptrBase->type == ST_TRUE)
	{
		free((slow_true_t*)ptrBase->p);
	}
	else if (ptrBase->type == ST_NUMBER)
	{
		free((slow_number_t*)ptrBase->p);
	}
	else if (ptrBase->type == ST_STRING)
	{
		free((slow_string_t*)ptrBase->p);
	}
	else if (ptrBase->type == ST_KEY_VALUE)
	{
		slow_kv_t* jkv = (slow_kv_t*)ptrBase->p;
		slow_release_key_value(jkv);
		free(jkv);
	}
	else if (ptrBase->type == ST_OBJECT)
	{
		slow_object_t* jo = (slow_object_t*)ptrBase->p;
		slow_release_object(jo);
		free(jo);
	}
	else if (ptrBase->type == ST_ARRAY)
	{
		slow_array_t* ja = (slow_array_t*)ptrBase->p;
		slow_release_array(ja);
		free(ja);
	}
	else
	{
		return -1;
	}
	return 0;
}

int slow_release_string(slow_string_t* ptrString)
{
	if (ptrString == NULL) return -1;

	if (ptrString->len > 0 && ptrString->p != NULL)
	{
		free(ptrString->p);
		ptrString->len = 0;
	}
	return 0;
}

int slow_release_key_value(slow_kv_t* ptrKeyValue)
{
	if (ptrKeyValue == NULL) return -1;

	slow_release_base(&(ptrKeyValue->value));

	return 0;
}

int slow_release_object(slow_object_t *ptrObject)
{
	if (ptrObject == NULL) return -1;

	while (ptrObject->next)
	{
		slow_kv_list_t* jkvl = ptrObject->next;
		ptrObject->next = jkvl->next;
		slow_release_key_value(&(jkvl->node));
		free(jkvl);
	}
	return 0;
}

int slow_release_array(slow_array_t* ptrArray)
{
	if (ptrArray == NULL) return -1;

	while (ptrArray->next)
	{
		slow_base_list_t* jbl = ptrArray->next;
		ptrArray->next = jbl->next;
		slow_release_base(&(jbl->node));
		free(jbl);
	}
	return 0;
}

int slow_release_ret_string(slow_ret_string_t* ptrRetString)
{
	if (ptrRetString == NULL) return -1;

	if (ptrRetString->p == NULL) return 0;

	free(ptrRetString->p);
	ptrRetString->offset = 0;
	ptrRetString->size = 0;
	return 0;
}


