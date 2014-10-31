#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include "rte_json.h"
#include "rte_vmconfig.h"

/* 
 * 利用json结构，给结构体struct vmconfig赋值
 * */
static struct vmconfig *parse_vmconfig_from_json(struct rte_json *json)
{
	struct rte_json *item=NULL, *tmp=NULL, *tmp1;
	int idx=0,len=0;
	struct ifset *ifset=NULL;
	struct vmconfig *vmcfg = (struct vmconfig *)malloc(sizeof(struct vmconfig));
	if(NULL==vmcfg){
		return NULL;
	}
	memset(vmcfg, 0, sizeof(struct vmconfig));

	item= rte_object_get_item(json, "vmname");	
	if(NULL==item){
		goto err_out;
	}
	strncpy(vmcfg->vmname, item->u.val_str, MAX_NAME_LEN); 

	item= rte_object_get_item(json, "cpuset");
	if(NULL==item){
		goto err_out;
	}
	len = rte_array_get_size(item);
	for(idx=0;idx<len;idx++) {
		tmp = rte_array_get_item(item, idx);
		vmcfg->cpu[idx] = tmp->u.val_int;
	}

	item = rte_object_get_item(json, "memsize");
	if(NULL==item){
		goto err_out;
	}
	vmcfg->memsize = item->u.val_int;

	item= rte_object_get_item(json, "disksize");
	if(NULL==item){
		goto err_out;
	}
	vmcfg->disksize = item->u.val_int;

	item = rte_object_get_item(json, "mgtip");
	if(NULL==item){
		goto err_out;
	}
	strncpy(vmcfg->mgtip, item->u.val_str, MAX_NAME_LEN);

	item = rte_object_get_item(json, "ifset");
	if(NULL==item){
		goto err_out;
	}
	len = rte_array_get_size(item);
	for(idx=0;idx<len;idx++){
		tmp = rte_array_get_item(item, idx);			
		ifset = (struct ifset *)malloc(sizeof(struct ifset));
		if(NULL==ifset){
		}
		memset(ifset, 0, sizeof(struct ifset));
		tmp1=rte_object_get_item(tmp, "ifname");			
		strncpy(ifset->ifname, tmp1->u.val_str, MAX_NAME_LEN);
		tmp1=rte_object_get_item(tmp, "vlanid");			
		ifset->vlanid = tmp1->u.val_int;
		vmcfg->ifset[idx]=ifset;
	}

	item= rte_object_get_item(json, "autorun");
	if(NULL==item){
		goto err_out;
	}
	vmcfg->autorun = item->u.val_int;

	return vmcfg;

err_out:
	free(vmcfg);
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
	vmcfg = parse_vmconfig_from_json(json);

	printf("serialize: %s\n", rte_serialize_json(json));

	rte_destroy_json(json);
	return vmcfg;
}

char *serialize_vmconfig(struct vmconfig *vmcfg)
{
	// struct rte_json *json=NULL;	

	return NULL;
}

int destroy_vmconfig(struct vmconfig *vmcfg)
{
	return 0;
}
