#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <math.h>
#include <string.h>
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

static inline struct rte_json *new_json_item(void)
{
	struct rte_json *item = (struct rte_json *)malloc(sizeof(struct rte_json));
	if(NULL!=item){
		memset(item, 0, sizeof(struct rte_json));
	}

	return item;
}

static unsigned int parse_hex4(const char *str)
{
	unsigned int h=0;
	if (*str>='0' && *str<='9'){
		 h+=(*str)-'0';
	}else if (*str>='A' && *str<='F'){
		 h+=10+(*str)-'A'; 
	}else if (*str>='a' && *str<='f'){
		h+=10+(*str)-'a'; 
	}else {
		return 0;
	}

	h=h<<4;
	str++;
	if (*str>='0' && *str<='9'){ 
		h+=(*str)-'0'; 
	}else if (*str>='A' && *str<='F'){ 
		h+=10+(*str)-'A'; 
	}else if (*str>='a' && *str<='f'){ 
		h+=10+(*str)-'a'; 
	}else {
		return 0;
	}

	h=h<<4;
	str++;
	if (*str>='0' && *str<='9'){ 
		h+=(*str)-'0'; 
	}else if (*str>='A' && *str<='F'){ 
		h+=10+(*str)-'A'; 
	}else if (*str>='a' && *str<='f'){ 
		h+=10+(*str)-'a'; 
	}else{ 
		return 0;
	}

	h=h<<4;
	str++;
	if (*str>='0' && *str<='9'){ 
		h+=(*str)-'0'; 
	}else if (*str>='A' && *str<='F'){ 
		h+=10+(*str)-'A'; 
	}else if (*str>='a' && *str<='f'){ 
		h+=10+(*str)-'a'; 
	}else {
		return 0;
	}
	return h;
}

static const unsigned char firstByteMark[7] = { 0x00, 0x00, 0xC0, 0xE0, 0xF0, 0xF8, 0xFC };
static const char *json_parse_string(struct rte_json *json, const char *value)
{
	const char *ptr=value+1;
	char *ptr2=NULL;
	char *out=NULL;
	int len=0;
	unsigned int uc,uc2;

	if(*value!='\"'){ 
		err_point=value;
		return NULL;
	}
	
	while (*ptr!='\"' && *ptr && ++len){ /* 确定字符串的长度 */
		if(*ptr++ == '\\'){	/* Skip escaped quotes. */
			ptr++;
		}
	}
	
	out=(char*)malloc(len+1); /* 分配字符串所需的空间 */
	if (NULL==out){
		return NULL;
	}
		
	ptr=value+1;
	ptr2=out;
	while (*ptr!='\"' && *ptr){
		if (*ptr!='\\'){
			*ptr2++=*ptr++;
		} else {
			ptr++;
			switch (*ptr) {
				case 'b': *ptr2++='\b';	break;
				case 'f': *ptr2++='\f';	break;
				case 'n': *ptr2++='\n';	break;
				case 'r': *ptr2++='\r';	break;
				case 't': *ptr2++='\t';	break;
				case 'u':	 /* transcode utf16 to utf8. */
					uc=parse_hex4(ptr+1); /* get the unicode char. */
					ptr+=4;	
					if ((uc>=0xDC00 && uc<=0xDFFF) || uc==0){ /* check for invalid.	*/
						break;	
					}

					if (uc>=0xD800 && uc<=0xDBFF){ /* UTF16 surrogate pairs.	*/
						if (ptr[1]!='\\' || ptr[2]!='u'){ /* missing second-half of surrogate.	*/
							break;	
						}
						uc2=parse_hex4(ptr+3);
						ptr+=6;
						if (uc2<0xDC00 || uc2>0xDFFF){ /* invalid second-half of surrogate.	*/
							break;	
						}
						uc=0x10000 + (((uc&0x3FF)<<10) | (uc2&0x3FF));
					}

					len=4;
					if (uc<0x80){
						len=1;
					}else if (uc<0x800){
						len=2;
					}else if (uc<0x10000) {
						len=3; 
					}
					ptr2+=len;
					
					switch (len) {
						case 4: *--ptr2 =((uc | 0x80) & 0xBF); uc >>= 6;
						case 3: *--ptr2 =((uc | 0x80) & 0xBF); uc >>= 6;
						case 2: *--ptr2 =((uc | 0x80) & 0xBF); uc >>= 6;
						case 1: *--ptr2 =(uc | firstByteMark[len]);
					}
					ptr2+=len;
					break;
				default: 
					*ptr2++=*ptr; 
					break;
			}
			ptr++;
		}
	}
	*ptr2='\0';
	if (*ptr=='\"'){
		ptr++;
	}
	json->u.val_str=out;
	json->type=JSON_STRING;
	return ptr;
}

static const char *json_parse_number(struct rte_json *json, const char *value)
{
	float n=0.0, sign=1.0, scale=0.0;
	int subscale=0, signsubscale=1;
	int flt_flag=0; 
	if(*value=='-'){ // 负数
		sign=-1.0;
		value++;
	}
	if(*value=='0'){ //
		value++;
	}
	if (*value>='1' && *value<='9'){
		do{
			n=(n*10.0)+(*value++ -'0');
		}while (*value>='0' && *value<='9');	/* Number? */
	}
	if (*value=='.' && value[1]>='0' && value[1]<='9'){
		flt_flag = 1;
		value++;
		do{
			n=(n*10.0)+(*value++ -'0');
			scale--;
		}while (*value>='0' && *value<='9');
	}	/* Fractional part? */

	if (*value=='e' || *value=='E'){	/* Exponent? */
		flt_flag = 1;
		value++;
		if (*value=='+'){
			value++;
		}else if (*value=='-'){
			signsubscale=-1;
			value++;		/* With sign? */
		}
		while (*value>='0' && *value<='9'){
			subscale=(subscale*10)+(*value++ - '0');	/* Number? */
		}
	}

	n=sign*n*pow(10.0,(scale+subscale*signsubscale));	/* number = +/- number.fraction * 10^+/- exponent */
	
	if(flt_flag){
		json->type=JSON_FLOAT;
		json->u.val_flt=n;
	}else {
		json->type=JSON_INTEGER;
		json->u.val_int=(int)n;
	}
	return value;
}

static const char *json_parse_array(struct rte_json *json, const char *value)
{
	struct rte_json *member= NULL;
	if(*value != '['){
		err_point = value;
		return NULL;
	}
	json->type = JSON_ARRAY;	
	value = json_skip(value+1);		
	if(*value==']'){ // 空Array
		return value+1;
	}
	member = new_json_item();
	if(NULL==member){
		return NULL;
	}
	json->member=member;
	value = json_skip(json_parse_value(member, json_skip(value)));
	if(NULL==value){
		return NULL;
	}
	while(*value==','){
		struct rte_json *item=new_json_item();
		if(NULL==item){
			return NULL;
		}
		member->next=item; // 添加到链表中 
		item->prev=member;
		member=item;
		value = json_skip(json_parse_value(member, json_skip(value+1)));
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
	struct rte_json *member=NULL;	
	if(*value!='{'){
		err_point = value;
		return NULL;
	}
	json->type = JSON_OBJECT;
	value = json_skip(value+1);
	if(*value=='}'){ // 空Object
		return value + 1;
	}
	member = new_json_item();
	if(NULL==member){
		return NULL;
	}
	json->member=member;
	value = json_skip(json_parse_string(member, json_skip(value))); // 解析成员的名字
	if(NULL==value){
		return NULL;
	}
	member->name=member->u.val_str;
	member->u.val_str = NULL;
	if(*value!=':'){
		err_point = value;
		return NULL;
	}
	value = json_skip(json_parse_value(member, json_skip(value+1)));
	if(NULL==value){
		return NULL;
	}
	while(*value==','){
		struct rte_json *item= new_json_item();
		if(NULL==item){
			return NULL;
		}
		member->next=item;
		item->prev=member;
		member=item;

		value = json_skip(json_parse_string(member, json_skip(value+1))); // 解析成员的名字
		if(NULL==value){
			return NULL;
		}

		member->name = member->u.val_str;
	   	member->u.val_str = NULL; 	
		if(*value!=':'){
			err_point = value;
			return NULL;
		}
		value=json_skip(json_parse_value(member, json_skip(value+1)));
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
	if(*value=='\"'){ // 字符串
		return json_parse_string(json, value);
	}
	if(*value=='-'||(*value>='0' && *value<='9')){ // 数值
		return json_parse_number(json, value);
	}
	if(*value=='['){ // Array
		return json_parse_array(json, value);
	}
	if(*value=='{'){ // Object
		return json_parse_object(json, value);
	}
	err_point = value;
	return NULL;
}

struct rte_json *rte_parse_json(const char *str)
{
	const char *end=NULL;
	struct rte_json *json=NULL;
	
	json = new_json_item();
	if(NULL==json){
		return NULL;
	}
	
	end = json_parse_value(json, json_skip(str));	
	if(NULL==end){
		rte_destroy_json(json);
		return NULL;
	}

	return json;
}

int rte_destroy_json(struct rte_json *json)
{
	struct rte_json *next=NULL;
	while(json){
		next = json->next;
		if(json->member){
			rte_destroy_json(json->member);
		}	
		if(json->type==JSON_STRING){
			free(json->u.val_str);
		}
		if(json->name){
			free(json->name);
		}
		free(json);
		json = next;
	}
	return 0;
}

int rte_traverse_json(struct rte_json *json)
{
	struct rte_json *next=NULL;
	while(json){
		next = json->next;
		if(json->name){
			printf("%s:", json->name);
		}
		if(json->member){
			rte_traverse_json(json->member);
		}
		if(json->type==JSON_STRING){
			printf("%s ", json->u.val_str);
		}
		if(json->type==JSON_INTEGER){
			printf("%ld ", json->u.val_int);
		}
		if(json->type==JSON_FLOAT){
			printf("%f ", json->u.val_flt);
		}
		json = next;
	}
	return 0;	
}

static char *json_strdup(const char *str)
{
	int len;
	char *copy=NULL;

	len = strlen(str) + 1;
	copy = (char *)malloc(len);
	if(NULL==copy){
		return NULL;
	}
	memset(copy, 0, len*sizeof(char));
	memcpy(copy, str, len);
	return copy;
}

static char *print_number(struct rte_json *json)
{
	char *str=NULL;
	str = (char *)malloc(21);	
	if(NULL==str){
		return NULL;
	}
	memset(str, 0, 21*sizeof(char));

	if(json->type==JSON_INTEGER){
		sprintf(str, "%ld", json->u.val_int);
	}else if(json->type==JSON_FLOAT){
		sprintf(str, "%f", json->u.val_flt);
	}

	return str;
}

static char *print_string(struct rte_json *json)
{
	const char *ptr;
	char *ptr2, *out;
	int len=0;
	unsigned char token;
	const char *str = json->u.val_str;
	if(NULL==str){
		return json_strdup("");
	}

	ptr = str;	
	while((token=*ptr) && ++len){
		if(strchr("\"\\\b\f\n\r\t", token)){
			len++;
		}else if(token<32){
			len+=5;
		}
		ptr++;
	}
	out = (char *)malloc(len+3);
	if(NULL==out){
		return NULL;
	}
	ptr2 = out;
	ptr = str;
	*ptr2++='\"';
	while(*ptr){
		if(((unsigned char)*ptr>31) && 
				(*ptr!='\"') &&
				(*ptr!='\\')){
			*ptr2++=*ptr++;
		}else{
			*ptr2++='\\';
			switch (token=*ptr++) {
				case '\\':
					*ptr2++='\\';	
					break;
				case '\"':
					*ptr2++='\"';
					break;
				case '\b':
					*ptr2++='b';
					break;
				case '\f':
					*ptr2++='f';
					break;
				case '\n':
					*ptr2++='n';
					break;
				case '\r':
					*ptr2++='r';
					break;
				case '\t':
					*ptr2++='t';
					break;
				default: 
					sprintf(ptr2,"u%04x",token);
					ptr2+=5;
					break;
			}

		}
	}
	*ptr2++='\"';
	*ptr2++='\0';
	return out;
}

static char *print_array(struct rte_json *json, int depth)
{
	char *out=NULL;
	return out;
}

static char *print_object(struct rte_json *json, int depth)
{
	char *out=NULL;
	return out;
}

static char *print_value(struct rte_json *json, int depth)
{
	char *out=0;	
	if(NULL==json){
		return NULL;
	}

	switch(json->type & 0xff){
		case JSON_NULL:
			out = json_strdup("null");
			break;
		case JSON_FALSE:
			out = json_strdup("false");
			break;
		case JSON_TRUE:
			out = json_strdup("true");
			break;
		case JSON_INTEGER:
		case JSON_FLOAT:
			out = print_number(json);
			break;
		case JSON_STRING:
			out = print_string(json);
			break;
		case JSON_OBJECT:
			out = print_object(json, depth);
			break;
		case JSON_ARRAY:
			out = print_array(json, depth);
			break;
	}
	return out;
}

/*
 * 根据JSON结构体中的内容， 输出JSON文本
 * */
char *rte_serialize_json(struct rte_json *json)
{
	return print_value(json, 0);
}

int rte_array_get_size(struct rte_json *array)
{
	int size=0;
	struct rte_json *item=array->member;
	while(item){
		size++;
		item = item->next;
	}	
	return size;
}

struct rte_json *rte_array_get_item(struct rte_json *array, int idx)
{
	struct rte_json *item = array->member;	
	while(item && idx>0){
		idx--;
		item = item->next;
	}
	return item;
}

int rte_array_add_item(struct rte_json *array, struct rte_json *item)
{
	struct rte_json *tmp=NULL;
	if(NULL==item){
		return 0;
	}
	if(NULL==array->member){
		array->member = item;
	}else{
		tmp=array->member;
		while(tmp && tmp->next){
			tmp = tmp->next;
		}
		tmp->next=item;
		item->prev=tmp;
	}
	return 0;	
}

int rte_array_del_item(struct rte_json *array, int idx)
{
	struct rte_json *item = array->member;	
	while(item && idx>0){
		item = item->next;
		idx--;
	}
	if(NULL==item){
		return 0;
	}
	if(item->prev){
		item->prev->next=item->next;
	}
	if(item->next){
		item->next->prev=item->prev;
	}
	if(item==array->member){
		array->member=item->next;
	}
	item->prev=item->next=NULL;

	rte_destroy_json(item);

	return 0;
}

struct rte_json *rte_object_get_item(struct rte_json *object, const char *name)
{
	struct rte_json *item = object->member;	
	while(item && strcmp(item->name, name)){
		item = item->next;
	}
	return item;
}

int rte_object_add_item(struct rte_json *object, const char *name, struct rte_json *item)
{
	int len=0;
	if(NULL==item){
		return 0;
	}
	if(NULL!=item->name){
		free(item->name);
	}
	len = strlen(name)+1;
	item->name = (char *)malloc(len);
	if(NULL==item->name){
		return -1;
	}
	memset(item->name, 0, len);
	strcpy(item->name, name);

	rte_array_add_item(object, item);	
	return 0;
}

int rte_object_del_item(struct rte_json *json, const char *name)
{
	return 0;
}
