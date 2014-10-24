#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include "rte_json.h"
#include "rte_vmconfig.h"

/* 
 * 利用json结构，给结构体struct vmconfig赋值
 * */
static struct vmconfig *parse_vmconfig_from_json(struct rte_json *json)
{
	struct vmconfig *vmcfg = (struct vmconfig *)malloc(sizeof(struct vmconfig));
	if(NULL==vmcfg){
		return NULL;
	}
	
	return vmcfg;
}

static struct rte_json *persist_vmconfig_to_json(struct vmconfig *vmcfg)
{
	return NULL;
}

struct vmconfig *create_vmconfig(const char *buf)
{
	struct vmconfig *vmcfg=NULL;
	struct rte_json *json = NULL;	
	json = rte_parse_json(buf);
	if(NULL==json){
		printf("JSON Parse Failed.\n");
		return NULL;
	}
	rte_traverse_json(json);
	vmcfg = parse_vmconfig_from_json(json);
	rte_destroy_json(json);
	return vmcfg;
}

char *serialize_vmconfig(struct vmconfig *vmcfg)
{
	struct rte_json *json=NULL;	
	json = persist_vmconfig_to_json(vmcfg);

	return NULL;
}

int destroy_vmconfig(struct vmconfig *vmcfg)
{
	return 0;
}
