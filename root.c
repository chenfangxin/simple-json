#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "rte_json.h"
#include "rte_vmconfig.h"

int main(int argc, char *argv[])
{
	char *filebuf=NULL;
	int fd=0;
	struct stat st;
	struct vmconfig *vmcfg=NULL;
	struct rte_json *json=NULL;
	char *new=NULL;

	memset(&st, 0, sizeof(struct stat));
	if(stat("vmconfig.json", &st)){
		return -1;
	}

	fd = open("vmconfig.json", O_RDWR);
	if(fd<0){
		return -1;
	}

	filebuf = malloc(st.st_size);
	if(NULL==filebuf){
		close(fd);
		return -1;
	}

	read(fd, filebuf, st.st_size);
	close(fd);
	printf("%s", filebuf);
#if 1
	vmcfg = create_vmconfig(filebuf);
	if(NULL==vmcfg){
		goto out1;
	}
	
	new = serialize_vmconfig(vmcfg);
	if(NULL==new){
		goto out2;	
	}
#endif
#if 0
	json = rte_parse_json(filebuf);
	rte_object_del_item(json, "vmname");
	new = rte_serialize_json(json);
#endif
	printf("New json = %s\n", new);
out2:
	destroy_vmconfig(vmcfg);
	free(new);
out1:
	free(filebuf);

	return 0;
}
