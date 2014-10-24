#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include "rte_json.h"
#include "rte_vmconfig.h"

/* 
 * 利用json结构，给结构体struct vmconfig赋值
 * 返回值：
 * 	0 -- 成功
 * 	-1 --失败
 * */
static int parse_vmconfig_from_json(struct vmconfig *vmcfg, struct rte_json *json)
{
	return 0;
}

struct vmconfig *create_vmconfig(const char *buf)
{
	struct vmconfig *vmcfg=NULL;
	struct rte_json *json = NULL;	
	int ret=0;
	json = rte_parse_json(buf);
	if(NULL==json){
		printf("JSON Parse Failed.\n");
		return NULL;
	}
	vmcfg = (struct vmconfig *)malloc(sizeof(struct vmconfig));
	if(NULL==vmcfg){
		rte_destroy_json(json);
		return NULL;
	}
	ret = parse_vmconfig_from_json(vmcfg, json);
	if(ret){
		free(vmcfg);
		rte_destroy_json(json);
		return NULL;
	}
	rte_destroy_json(json);
	return vmcfg;
}

char *serialize_vmconfig(struct vmconfig *vmcfg)
{
	return NULL;
}

int destroy_vmconfig(struct vmconfig *vmcfg)
{
	return 0;
}
