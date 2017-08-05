#include <stdio.h>
#include <stdlib.h>

int
main(int argc, char *argv[])
{
	int	i, j, c;

	for(i=0; (c=getchar())!=EOF; i++){
		printf("(%d", (unsigned char)c);
		if(c == 0xa){
			for(j=0; j<=i; j++)
				printf("(%d@[", j);
			for(j=0; j<=i; j++)
				printf("(%d]", j);
			for(j=0; j<=i; j++)
				printf("$");
			printf("\n");
			i = -1;
		}
	}
	exit(0);
}
