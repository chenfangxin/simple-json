#include <stdio.h>
#include <unistd.h>
#include "rte_json.h"
#include "rte_vmconfig.h"

struct vmconfig *create_vmconfig(const char *buf)
{
	struct rte_json *json = NULL;	
	json = rte_json_parse(buf);
	if(NULL==json){
		printf("JSON Parse Failed.\n");
		return NULL;
	}
	while(1)
		sleep(1);

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
