#include<stdio.h>
#include<stdlib.h>

#define TAB_SIZE 4
#define MEM_MAX 65536
#define PRC_MAX 256
#define STK_MAX 256

unsigned short s[STK_MAX]={0};
unsigned char m[MEM_MAX]={0};
unsigned char *c;
ssize_t mp,cp,d,scp,p[PRC_MAX],sp;
size_t n,ln,cl,i;
FILE *f;
int ch;

void getpos(size_t scp) {
	ssize_t i;
	ln=1;cl=1;
	for(i=0;i<scp;i++) {
		if(c[i]=='\n') {ln++;cl=0;}
		if(c[i]=='\t') cl+=TAB_SIZE; else cl++;
	}
}

int main(int argc,char **argv) {
	
	if(argc<2) {
		printf("usage: %s filename\n",argv[0]);
		return 1;
	}
        
	if((f=fopen(argv[1],"rb"))==NULL) {
		printf("error opening file %s\n",argv[1]);
		return 2;
	} 
        
	fseek(f,0L,SEEK_END);
	n=ftell(f);
	rewind(f);
	
	if((c=malloc(sizeof(*c)*n))==NULL) {
		printf("error allocating memory\n");
		return 3;
	}

	if(fread(c,sizeof(*c),n,f)!=n) {
		printf("error reading file\n");
		return 4;
	}

	fclose(f);
	
	for(i=0;i<PRC_MAX;i++) {
		p[i]=-1;
	}
        
	mp=0;
	cp=0;
	ln=1;
	cl=1;
	sp=-1;

	do {
		d=1;
		switch(c[cp]) {
			case '+': m[mp]++; break;
			case '-': m[mp]--; break;
			case '<': mp--; if(mp<0) { getpos(cp); printf("error memory ln %lu col %lu: pointer below MIN\n",(unsigned long)ln,(unsigned long)cl); exit(5); } break;
			case '>': mp++; if(mp>=MEM_MAX) { getpos(cp); printf("error ln %lu col %lu: memory pointer above MAX\n",(unsigned long)ln,(unsigned long)cl); exit(6); } break;
			case '.': putchar(m[mp]); break;
			case ',': m[mp]=((ch=getchar())==EOF?0:ch); break;
			case '[':
				scp=cp;
				while(d!=0) {
					cp++;
					if(cp>=n) { getpos(scp); printf("error ln %lu col %lu: unmatched [\n",(unsigned long)ln,(unsigned long)cl); exit(7); }
					d-=(c[cp]==']')-(c[cp]=='[');
				}
				if(m[mp]!=0) {
					cp=scp;
				}
				break;
			case ']':
				scp=cp;
				while(d!=0) {
					cp--;
					if(cp<0) { getpos(scp); printf("error ln %lu col %lu: unmatched ]\n",(unsigned long)ln,(unsigned long)cl);  exit(8); }
					d-=(c[cp]=='[')-(c[cp]==']');
				}
				if(m[mp]==0) {
					cp=scp;
				} 
				break;
			case '(': 
				scp=cp;
				while(d!=0) {
					cp++;
					if(cp>=n) { getpos(scp); printf("error ln %lu col %lu: unmatched (\n",(unsigned long)ln,(unsigned long)cl); exit(9); }
					d-=(c[cp]==')')-(c[cp]=='(');
				}
				p[m[mp]]=scp;
				break;
			case ')': 
				scp=cp;
				while(d!=0) {
					cp--;
					if(cp<0) { getpos(scp); printf("error ln %lu col %lu: unmatched )\n",(unsigned long)ln,(unsigned long)cl); exit(10); }
					d-=(c[cp]=='(')-(c[cp]==')');
				}
				cp=scp;
				if(sp>=0) { cp=s[sp]; sp--; } else { getpos(scp); printf("error ln %lu col %lu: stack is empty\n",(unsigned long)ln,(unsigned long)cl); exit(11); }
				break;
			case ':':
				scp=cp;
				if(p[m[mp]]==-1) { getpos(scp); printf("error ln %lu col %lu: invalid procedure call %u\n",(unsigned long)ln,(unsigned long)cl,(unsigned int)m[mp]); exit(12); }
				if(sp<STK_MAX-1) { sp++; s[sp]=cp; cp=p[m[mp]]; } else { getpos(scp); printf("error ln %lu col %lu: stack is full\n",(unsigned long)ln,(unsigned long)cl); exit(13); }
				break;
			default: break;
		}
		
		cp++;
		
	} while(cp<n);
	
	free(c);
	
	return 0;
}
