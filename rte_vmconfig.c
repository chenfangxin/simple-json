#include <stdio.h>
#include <unistd.h>
#include "rte_json.h"
#include "rte_vmconfig.h"

struct vmconfig *create_vmconfig(const char *buf)
{
	struct rte_json *json = NULL;	
	json = rte_parse_json(buf);
	if(NULL==json){
		printf("JSON Parse Failed.\n");
		return NULL;
	}
	rte_destroy_json(json);
	return NULL;
}

char *serialize_vmconfig(struct vmconfig *vmcfg)
{
	return NULL;
}

int destroy_vmconfig(struct vmconfig *vmcfg)
{
	return 0;
}
