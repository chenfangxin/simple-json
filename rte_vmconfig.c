#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include "rte_json.h"
#include "rte_vmconfig.h"

/* 解析JSON文本，并根据其内容，初始化结构体 */
struct vmconfig *create_vmconfig(const char *buf)
{
	struct vmconfig *vmcfg=NULL;
	struct rte_json *json=NULL, *item=NULL;
	struct rte_json *tmp=NULL, *tmp1=NULL;
	struct ifset *ifset=NULL;
	int len, idx;
	json = rte_parse_json(buf);
	if(NULL==json){
		printf("JSON Parse Failed.\n");
		return NULL;
	}
	if(NULL==json->member){
		goto out;
	}
	vmcfg = (struct vmconfig *)malloc(sizeof(struct vmconfig));
	if(NULL==vmcfg){
		rte_destroy_json(json);
		return NULL;
	}

	memset(vmcfg, 0, sizeof(struct vmconfig));

	item=rte_object_get_item(json, "vmname");	
	if(NULL!=item){
		strncpy(vmcfg->vmname, item->u.val_str, MAX_NAME_LEN); 
	}

	item= rte_object_get_item(json, "cpuset");
	if(NULL!=item){
		len = rte_array_get_size(item);
		for(idx=0;idx<len;idx++) {
			tmp = rte_array_get_item(item, idx);
			vmcfg->cpu[idx] = tmp->u.val_int;
		}
	}

	item = rte_object_get_item(json, "memsize");
	if(NULL!=item){
		vmcfg->memsize = item->u.val_int;
	}

	item= rte_object_get_item(json, "disksize");
	if(NULL!=item){
		vmcfg->disksize = item->u.val_int;
	}

	item = rte_object_get_item(json, "mgtip");
	if(NULL!=item){
		strncpy(vmcfg->mgtip, item->u.val_str, MAX_NAME_LEN);
	}

	item = rte_object_get_item(json, "ifset");
	if(NULL!=item){
		len = rte_array_get_size(item);
		for(idx=0;idx<len;idx++){
			tmp = rte_array_get_item(item, idx);			
			ifset = (struct ifset *)malloc(sizeof(struct ifset));
			if(NULL==ifset){
				break;
			}
			memset(ifset, 0, sizeof(struct ifset));
			tmp1=rte_object_get_item(tmp, "ifname");			
			strncpy(ifset->ifname, tmp1->u.val_str, MAX_NAME_LEN);
			tmp1=rte_object_get_item(tmp, "vlanid");			
			ifset->vlanid = tmp1->u.val_int;
			vmcfg->ifset[idx]=ifset;
		}
	}	

	item= rte_object_get_item(json, "autorun");
	if(NULL!=item){
		vmcfg->autorun = item->u.val_int;
	}

out:
	rte_destroy_json(json);		
	return vmcfg;
}

/* 对结构体进行序列化，生成对应的JSON文本 */
char *serialize_vmconfig(struct vmconfig *vmcfg, int fmt)
{
	struct rte_json *json=NULL, *item=NULL;	
	struct rte_json *tmp=NULL, *tmp1=NULL;
	char *str=NULL;
	int idx=0, len=0, count=0;
	struct ifset *ifset=NULL;

	if(NULL==vmcfg){
		return NULL;
	}

	json = new_json_item();
	if(NULL==json){
		return NULL;
	}
	json->type = JSON_OBJECT;	

	item=new_json_item();
	if(NULL==item){
		goto err_out;		
	}
	item->type = JSON_STRING;
	len = strlen(vmcfg->vmname);
	item->u.val_str = (char *)malloc(len+1);
	if(NULL==item->u.val_str){
		rte_destroy_json(item);
		goto err_out;
	}
	memset(item->u.val_str, 0, len+1);
	strncpy(item->u.val_str, vmcfg->vmname, len);
	rte_object_add_item(json, "vmname", item);

	item=new_json_item();
	if(NULL==item){
		goto err_out;		
	}
	item->type=JSON_ARRAY;	
	for(idx=0,count=0;idx<MAX_CPU_NUM;idx++){
		if(vmcfg->cpu[idx]>0){
			count++;
			tmp=new_json_item();	
			if(NULL==tmp){
				rte_destroy_json(item);
				goto err_out;
			}
			tmp->type=JSON_INTEGER;
			tmp->u.val_int = vmcfg->cpu[idx];
			rte_array_add_item(item, tmp);
		}
	}
	if(count>0){
		rte_object_add_item(json, "cpuset", item);
	}else{
		rte_destroy_json(item);
	}

	item=new_json_item();
	if(NULL==item){
		goto err_out;
	}
	item->type=JSON_INTEGER;	
	item->u.val_int = vmcfg->memsize;
	rte_object_add_item(json, "memsize", item);

	item=new_json_item();
	if(NULL==item){
		goto err_out;
	}
	item->type=JSON_INTEGER;	
	item->u.val_int = vmcfg->disksize;
	rte_object_add_item(json, "disksize", item);

	item=new_json_item();
	if(NULL==item){
		goto err_out;
	}
	item->type=JSON_STRING;	
	len = strlen(vmcfg->mgtip);
	item->u.val_str = (char *)malloc(len+1);
	if(NULL==item->u.val_str){
		rte_destroy_json(item);
		goto err_out;
	}
	memset(item->u.val_str, 0, len+1);
	strncpy(item->u.val_str, vmcfg->mgtip, len);
	rte_object_add_item(json, "mgtip", item);

	item=new_json_item();
	if(NULL==item){
		goto err_out;
	}
	item->type=JSON_ARRAY;	
	for(idx=0,count=0;idx<MAX_IF_NUM;idx++){
		ifset = vmcfg->ifset[idx];
		if(NULL==ifset){
			continue;
		}
		count++;
		tmp = new_json_item();
		if(NULL==tmp){
			rte_destroy_json(item);	
			goto err_out;
		}
		tmp->type=JSON_OBJECT;
		
		tmp1 = new_json_item();
		if(NULL==tmp1){
			rte_destroy_json(tmp);
			rte_destroy_json(item);
			goto err_out;
		}
		tmp1->type=JSON_STRING;
		len=strlen(ifset->ifname);
		tmp1->u.val_str = (char *)malloc(len+1);
		if(NULL==tmp1->u.val_str){
			rte_destroy_json(tmp1);
			rte_destroy_json(tmp);
			rte_destroy_json(item);
			goto err_out;
		}
		strncpy(tmp1->u.val_str, ifset->ifname, len);
		rte_object_add_item(tmp, "ifname", tmp1);	

		tmp1 = new_json_item();
		if(NULL==tmp1){
			rte_destroy_json(tmp);
			rte_destroy_json(item);
			goto err_out;
		}
		tmp1->type=JSON_INTEGER;
		tmp1->u.val_int = ifset->vlanid;
		rte_object_add_item(tmp, "vlanid", tmp1);	

		rte_array_add_item(item, tmp);
	}
	if(count>0){
		rte_object_add_item(json, "ifset", item);
	}else{
		rte_destroy_json(item);
	}

	item=new_json_item();
	if(NULL==item){
		goto err_out;
	}
	item->type=JSON_INTEGER;
	item->u.val_int = vmcfg->autorun;
	rte_object_add_item(json, "autorun", item);

	str = rte_serialize_json(json, fmt);

//	printf("Result:\n%s\n", str);	

	return str;
err_out:
	rte_destroy_json(json);
	return NULL;
}

/* 释放结构体 */
int destroy_vmconfig(struct vmconfig *vmcfg)
{
	struct ifset *ifset=NULL;	
	int idx=0;
	if(NULL==vmcfg){
		return 0;
	}
	for(idx=0;idx<MAX_IF_NUM;idx++){
		ifset = vmcfg->ifset[idx];
		if(NULL==ifset){
			continue;
		}
		free(ifset);
	}
	free(vmcfg);
	return 0;
}
