
#include "json.h"
#include "common.h"
#include "parse.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int slow_init_null(slow_null_t* ptrNull)
{
	if (ptrNull == NULL) return SLOW_NULL_PTR;
	ptrNull->flag = 1;
	return SLOW_OK;
}

int slow_init_false(slow_false_t* ptrFalse)
{
	if (ptrFalse == NULL) return SLOW_NULL_PTR;
	ptrFalse->flag = 1;
	return SLOW_OK;
}

int slow_init_true(slow_true_t* ptrTrue)
{
	if (ptrTrue == NULL) return SLOW_NULL_PTR;
	ptrTrue->flag = 1;
	return SLOW_OK;
}

int slow_init_number(slow_number_t* ptrNumber, double d)
{
	if (ptrNumber == NULL) return SLOW_NULL_PTR;
	ptrNumber->d = d;
	return SLOW_OK;
}

int slow_init_string(slow_string_t* ptrString, const char* s)
{
	if (ptrString == NULL || s == NULL) return SLOW_NULL_PTR;
	int len = strlen(s);
	ptrString->p = (char*)malloc(len + 1);
	if (ptrString->p == NULL) return -1;
	memcpy(ptrString->p, s, len);
	ptrString->p[len] = '\0';
	ptrString->len = len;
	return SLOW_OK;
}

int slow_init_object(slow_object_t* ptrObject)
{
	if (ptrObject == NULL) return SLOW_NULL_PTR;
	ptrObject->next = NULL;
	return SLOW_OK;
}

int slow_init_array(slow_array_t* ptrArray)
{
	if (ptrArray == NULL) return SLOW_NULL_PTR;
	ptrArray->next = NULL;
	return SLOW_OK;
}

int slow_init_ret_string(slow_ret_string_t* ptr)
{
	if (ptr == NULL) return SLOW_NULL_PTR;
	ptr->offset = 0;
	ptr->size = RET_STRING_INIT_SIZE;
	ptr->p = (char*)malloc(ptr->size);
	if (ptr->p == NULL) return -1;
	return SLOW_OK;
}

int slow_check_ret_string_size(slow_ret_string_t* ptr, int size)
{
	if (ptr == NULL || size < 0) return SLOW_NULL_PTR;
	if (ptr->offset + size <= ptr->size) return 0;
	char* temp = (char*)realloc(ptr->p, ptr->size * 2);
	if (temp == NULL) return -1;
	ptr->p = temp;
	ptr->size = ptr->size * 2;
	return SLOW_OK;
}

int slow_object_get_base(slow_object_t* ptrObject, const char* k, slow_base_t** b)
{
	if (ptrObject == NULL || k == NULL) return SLOW_NULL_PTR;

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
	return SLOW_OK;
}

int slow_object_get_null(slow_object_t* ptrObject, const char* k, int* n)
{
	if (ptrObject == NULL || k == NULL) return SLOW_NULL_PTR;

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
	return SLOW_OK;
}

int slow_object_get_bool(slow_object_t* ptrObject, const char* k, int* b)
{
	if (ptrObject == NULL || k == NULL) return SLOW_NULL_PTR;

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
	return SLOW_OK;
}

int slow_object_get_number(slow_object_t* ptrObject, const char* k, double* d)
{
	if (ptrObject == NULL || k == NULL) return SLOW_NULL_PTR;

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
	return SLOW_OK;
}

int slow_object_get_string(slow_object_t* ptrObject, const char* k, slow_string_t** s)
{
	if (ptrObject == NULL || k == NULL) return SLOW_NULL_PTR;

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
	return SLOW_OK;
}

int slow_object_get_object(slow_object_t* ptrObject, const char* k, slow_object_t** o)
{
	if (ptrObject == NULL || k == NULL) return SLOW_NULL_PTR;

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
	return SLOW_OK;
}

int slow_object_get_array(slow_object_t* ptrObject, const char* k, slow_array_t** a)
{
	if (ptrObject == NULL || k == NULL) return SLOW_NULL_PTR;

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
	return SLOW_OK;
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

int slow_release_base(slow_base_t *ptrBase)
{
	if (ptrBase == NULL) return SLOW_NULL_PTR;

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
		return SLOW_TYPE_NOT_FOUND;
	}
	return SLOW_OK;
}

int slow_release_string(slow_string_t* ptrString)
{
	if (ptrString == NULL) return SLOW_NULL_PTR;

	if (ptrString->len > 0 && ptrString->p != NULL)
	{
		free(ptrString->p);
		ptrString->len = 0;
	}
	return SLOW_OK;
}

int slow_release_key_value(slow_kv_t* ptrKeyValue)
{
	if (ptrKeyValue == NULL) return SLOW_NULL_PTR;

	slow_release_base(&(ptrKeyValue->value));

	return SLOW_OK;
}

int slow_release_object(slow_object_t *ptrObject)
{
	if (ptrObject == NULL) return SLOW_NULL_PTR;

	while (ptrObject->next)
	{
		slow_kv_list_t* jkvl = ptrObject->next;
		ptrObject->next = jkvl->next;
		slow_release_key_value(&(jkvl->node));
		free(jkvl);
	}
	return SLOW_OK;
}

int slow_release_array(slow_array_t* ptrArray)
{
	if (ptrArray == NULL) return SLOW_NULL_PTR;

	while (ptrArray->next)
	{
		slow_base_list_t* jbl = ptrArray->next;
		ptrArray->next = jbl->next;
		slow_release_base(&(jbl->node));
		free(jbl);
	}
	return SLOW_OK;
}

int slow_release_ret_string(slow_ret_string_t* ptrRetString)
{
	if (ptrRetString == NULL) return SLOW_NULL_PTR;

	if (ptrRetString->p == NULL) return 0;

	free(ptrRetString->p);
	ptrRetString->offset = 0;
	ptrRetString->size = 0;
	return SLOW_OK;
}


