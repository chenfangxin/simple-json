#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "rte_vmconfig.h"

int main(int argc, char *argv[])
{
	char *filebuf=NULL;
	int fd=0;
	struct stat st;
	struct vmconfig *vmcfg=NULL;
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
	
#if 0	
	vmcfg = create_vmconfig(filebuf);
	if(NULL==vmcfg){
		goto out1;
	}
	new = serialize_vmconfig(vmcfg);
	if(NULL==new){
		goto out2;	
	}
	printf("New json = %s\n", new);

out2:
	destroy_vmconfig(vmcfg);
	free(new);
out1:
	free(filebuf);
#endif

	return 0;
}
