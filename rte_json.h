#ifndef __RTE_JSON_H__
#define __RTE_JSON_H__
enum json_type{
	JSON_NULL,	/* null */
	JSON_FALSE, /* false */
	JSON_TRUE,	/* true */
	JSON_INTEGER,/* 整数 */
	JSON_FLOAT,/* 浮点数 */ 
	JSON_STRING,/* 字符串 */
	JSON_OBJECT,/* 对象 */
	JSON_ARRAY, /* 数组 */
};

/* JSON的值类型 */
struct rte_json{
	struct rte_json *next, *prev; // 用于串联结构类型中并列的成员
	struct rte_json *member; // 指向Object或Array的成员 
	enum json_type type; // 类型
	char *name; //
	union{
		char *val_str;	
		long val_int;
		float val_flt;
	}u;
};

int is_valid_json(const char *buf, int len);

struct rte_json *rte_parse_json(const char *str);
int rte_destroy_json(struct rte_json *json);
int rte_traverse_json(struct rte_json *json);
char *rte_serialize_json(struct rte_json *json);

#endif
