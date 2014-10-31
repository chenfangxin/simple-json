#ifndef __RTE_VMCONFIG_H__
#define __RTE_VMCONFIG_H__
#define MAX_NAME_LEN 16
#define MAX_CPU_NUM 4
#define MAX_IP_LEN 20
#define MAX_IF_NUM 20

struct ifset{
	char ifname[MAX_NAME_LEN+1];
	int vlanid;
};

struct vmconfig{
	char vmname[MAX_NAME_LEN+1];
	char cpu[MAX_CPU_NUM];	
	int memsize;
	int disksize;
	char mgtip[MAX_IP_LEN];
	struct ifset *ifset[MAX_IF_NUM];
	int autorun;
};

struct vmconfig *create_vmconfig(const char *);
char *serialize_vmconfig(struct vmconfig *);
int destroy_vmconfig(struct vmconfig *);

#endif
