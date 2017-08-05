/* stacki:	A stack-based interpreter that borrows all opcodes from
 * 		Whitespace <http://compsoc.dur.ac.uk/whitespace>
 * Author:	Huidae Cho <https://idea.isnew.info>
 * Since:	May 2, 2003
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define	BUFFER	1024
#define	STACK	1024

#define	rm_blank()	if(i == ';'){\
				is = 1;\
				continue;\
			}else\
			if(is){\
				if(i == '\n')\
					is = 0;\
				continue;\
			}else\
			if(strchr(" \t\r\n", i))\
				continue;

#define	inc_is()	do{if(!((++is)%STACK))\
				s = (int *)realloc(s, (is/STACK+1)*STACK*t0);\
			}while(0)

#define	dec_is()	do{if(!((is--)%STACK))\
				s = (int *)realloc(s, (is/STACK+1)*STACK*t0);\
			}while(0)

int
main(int argc, char *argv[])
{
	FILE	*fp;
	char	*src, **g, *p, *p2, *p3, *p4, *q;
	int	lsrc, ig, i, l, *s, *h, is, ih, t, t0, t1;

	src = NULL;
	lsrc = 0;
	if(argc > 1){
		for(t0=1; t0<argc; t0++){
			if((fp = fopen(argv[t0], "r"))){
				fseek(fp, 0, SEEK_END);
				l = ftell(fp);
				if(l > 0){
					fseek(fp, 0, SEEK_SET);
					src = (char *)realloc(src, lsrc+l+1);
					is = 0;
					for(t=0; t<l&&(i=fgetc(fp))!=EOF; t++){
						rm_blank();
						src[lsrc++] = i;
					}
					src[lsrc] = 0;
				}
				fclose(fp);
			}
		}
	}else{
		is = 0;
		src = (char *)malloc(BUFFER);
		for(; (i=getchar())!=EOF; ){
			rm_blank();
			src[lsrc] = i;
			if(!((++lsrc)%STACK))
				src = (char *)realloc(src,
						(lsrc/BUFFER+1)*BUFFER);
		}
		src[lsrc] = 0;
	}
	p = src;

	ig = 0; g = NULL;
	is = 0; s = NULL;
	ih = -1; h = NULL;
	if(p && *p){
		t0 = sizeof(int);
		t1 = sizeof(char *);
		s = (int *)malloc(STACK*t0);
#ifdef	DEBUG
		printf("%s}\n", p);
#endif
	}

	while(p && *p){
		switch(*p++){
		case '(':
			for(p2=p+(*p=='-'); *p2>='0'&&*p2<='9'; p2++);
			s[is] = atoi(p);
			p = p2;
			inc_is();
			break;
		case '=':
			if(is){
				s[is] = s[is-1];
				inc_is();
			}
			break;
		case '@':
			if(is > 1){
				t = s[is-1];
				s[is-1] = s[is-2];
				s[is-2] = t;
			}
			break;
		case ')':
			if(is)
				dec_is();
			break;
		case '+':
			if(is > 1){
				s[is-2] += s[is-1];
				dec_is();
			}
			break;
		case '-':
			if(is > 1){
				s[is-2] -= s[is-1];
				dec_is();
			}
			break;
		case '*':
			if(is > 1){
				s[is-2] *= s[is-1];
				dec_is();
			}
			break;
		case '/':
			if(is > 1 && s[is-1]){
				s[is-2] /= s[is-1];
				dec_is();
			}
			break;
		case '%':
			if(is > 1 && s[is-1]){
				s[is-2] %= s[is-1];
				dec_is();
			}
			break;
		case '[':
			if(is > 1 && (t=s[is-2]) >= 0){
				if(t > ih){
					ih = t;
					h = (int *)realloc(h, (ih+1)*t0);
				}
				h[t] = s[is-1];
				--is;
				dec_is();
			}
			break;
		case ']':
			if(is && (t=s[is-1]) >= 0 && t <= ih)
				s[is-1] = h[t];
			break;
		case ':':
			for(; *p>='0'&&*p<='9'; p++);
			break;
		case '&':
		case '^':
		case '!':
		case '~':
			for(p2=p; *p2>='0'&&*p2<='9'; p2++);
			t = *p2;
			*p2 = 0;
			q = (char *)malloc(p2-p+2);
			sprintf(q, ":%s", p);
			*p2 = t;
			for(p3=src; p3<src+lsrc-(p2-p) &&
					(p4 = strstr(p3, q)) &&
					*(p4+(p2-p+1)) >= '0' &&
					*(p4+(p2-p+1)) <= '9'; )
				p3 = p4+(p2-p+1);
			if(p4){
				t = *(p-1);
				p = p4+(p2-p+1);
				switch(t){
				case '&':
					if(!(ig%STACK))
						g = (char **)realloc(g,
							(ig/STACK+1)*STACK*t1);
					g[ig++] = p2;
					break;
				case '!':
					if(!is || s[is-1])
						p = p2;
					if(is)
						dec_is();
					break;
				case '~':
					if(!is || s[is-1] >= 0)
						p = p2;
					if(is)
						dec_is();
					break;
				}
			}else
				p = p2;
			break;
		case '|':
			if(ig){
				p = g[--ig];
				if(!ig){
					free(g);
					g = NULL;
				}else
				if(!(ig%STACK))
					g = (char **)realloc(g,
							ig/STACK*STACK*t1);
			}
			break;
		case '.':
			p = NULL;
			break;
		case '$':
			if(is){
				putchar(s[is-1]);
				dec_is();
			}
			break;
		case '#':
			if(is){
				printf("%d", s[is-1]);
				dec_is();
			}
			break;
		case '"':
			s[is] = getchar();
			inc_is();
			break;
		case '\'':
			scanf("%d", s+is);
			inc_is();
			break;
		}
	}

	if(src)
		free(src);
	if(g)
		free(g);
	if(s)
		free(s);
	if(h)
		free(h);
	exit(0);
}
