#include <stdio.h>
#include <string.h>

#include "hpchain.h"

HPElem *HPChain_read(FILE *fp){
	char buffer[2001];
	char *chain;
	int retval;
	
	retval = fscanf(fp, " %2000[HP]", buffer);

	if(retval == 1){
		chain = strdup(buffer);
	} else {
		chain = NULL;
	}

	return chain;
}
