#include <stdio.h>
#include <stdlib.h>

int main(int argc, char *argv[])
{
	int i;

	for (i=0;i<atoi(argv[1]);i++) {
		fprintf(stdout,"%lf\t%lf\n",(double)i,0.0);
	}
}
