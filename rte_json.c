#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include "rte_json.h"

static const char *err_point=NULL;
static const char *json_parse_value(struct rte_json *json, const char *value);

/* 跳过无用的空格和CR/LF字符 */
static inline const char *json_skip(const char *in)
{
	while(in && *in && (unsigned char)*(in)<=32){
		in++;
	}
	return in;
}

/* 
 * 判断字符串是否为合法的JSON结构
 * */
int is_valid_json(const char *buf, int len)
{
	return 0;
}


static inline struct rte_json *new_json_item(void)
{
	return (struct rte_json *)malloc(sizeof(struct rte_json));
}

static const char *json_parse_string(struct rte_json *json, const char *value)
{
	const char *ptr=value+1;
	char *ptr2;
	char *out;
	int len=0;
	unsigned uc, uc2;

	if(*value!='\"'){
		err_point = value;
		return NULL;
	}
}

static const char *json_parse_array(struct rte_json *json, const char *value)
{
	struct rte_json *child = NULL;
	if(*value != '['){
		err_point = value;
		return NULL;
	}
	json->type = JSON_ARRAY;	
	value = json_skip(value+1);		
	if(*value==']'){ // 空Array
		return value+1;
	}
	json->child = child = new_json_item();
	if(NULL==child){
		return NULL;
	}
	value = json_skip(json_parse_value(child, json_skip(value)));
	if(NULL==value){
		return NULL;
	}
	while(*value==','){
		struct rte_json *item=new_json_item();
		if(NULL==item){
			return NULL;
		}
		child->next=item;
		item->prev=child;
		child = item;
		value = json_skip(json_parse_value(child, json_skip(value+1)));
		if(NULL==value){
			return NULL;
		}
	}
	if(*value==']'){
		return value+1;
	}

	err_point = value;
	return NULL;
}

static const char *json_parse_object(struct rte_json *json, const char *value)
{
	struct rte_json *child=NULL;	
	if(*value!='{'){
		err_point = value;
		return NULL;
	}
	json->type = JSON_OBJECT;
	value = json_skip(value+1);
	if(*value=='}'){ // 空Object
		return value + 1;
	}
	json->child = child = new_json_item();
	if(NULL==child){
		return NULL;
	}
	value = json_skip(json_parse_string(child, json_skip(value)));
	if(NULL==value){
		return NULL;
	}
	child->name = child->u.val_str;
	child->u.val_str = NULL;
	if(*value!=':'){
		err_point = value;
		return NULL;
	}
	value = json_skip(json_parse_value(child, json_skip(value+1)));
	if(NULL==NULL){
		return NULL;
	}
	while(*value==','){
		struct rte_json *item= new_json_item();
		if(NULL==item){
			return NULL;
		}
		child->next=item;
		item->prev=child;
		child=item;
		value = json_skip(json_parse_string(child, json_skip(value+1))); // 解析成员的名字
		if(NULL==value){
			return NULL;
		}
		child->name = child->u.val_str;
	   	child->u.val_str = NULL; 	
		if(*value!=':'){
			err_point = value;
			return NULL;
		}
		value=json_skip(json_parse_value(child, json_skip(value+1)));
		if(NULL==value){
			return NULL;
		}
	}
	if(*value=='}'){
		return value+1;
	}
	err_point = value;
	return NULL;
}

static const char *json_parse_value(struct rte_json *json, const char *value)
{
	if(NULL==value){
		return NULL;
	}
	if(!strncmp(value, "null", 4)){
		json->type = JSON_NULL;
		return value+4;
	}
	if(!strncmp(value, "false", 5)){
		json->type = JSON_FALSE;
		return value+5;
	}
	if(!strncmp(value, "true", 4)){
		json->type = JSON_TRUE;
		return value+4;
	}
	if(*value=='\"'){
		return json_parse_string(json, value);
	}
	if(*value=='['){
		return json_parse_array(json, value);
	}
	if(*value=='{'){
		return json_parse_object(json, value);
	}
}

struct rte_json *rte_json_parse(const char *str)
{
	const char *end=NULL;
	struct rte_json *json=NULL;
	
	json = new_json_item();
	if(NULL==json){
		return NULL;
	}
	
	end = json_parse_value(json, json_skip(str));	
}
