#ifndef __RTE_JSON_H__
#define __RTE_JSON_H__
enum json_type{
	JSON_NULL,	/* null */
	JSON_FALSE, /* false */
	JSON_TRUE,	/* true */
	JSON_OBJECT,/* 对象 */
	JSON_ARRAY, /* 数组 */
	JSON_INTEGER,/* 整数 */
	JSON_REAL,	/* 实数 */
	JSON_STRING,/* 字符串 */
};

struct rte_json{
	enum json_type type;
	union{
		
	}u;
};
int rte_valid_json(const char *buf, int len);

#endif
