#include<stdio.h>
#include<stdlib.h>
#include<stdarg.h>

#define TAB_SIZE 4
#define MEM_MAX 30000
#define PRC_MAX 256
#define STK_MAX 256

unsigned short s[STK_MAX]={0};
unsigned char m[MEM_MAX]={0};
unsigned char *c;
ssize_t p[PRC_MAX],sp,d;
size_t mp,cp,scp,n,ln,cl,i;
FILE *f;
int ch;

void getpos(ssize_t scp) {
	ssize_t i;
	if(scp>=0) {
		ln=1;cl=1;
		for(i=0;i<scp;i++) {
			if(c[i]=='\n') {ln++;cl=0;}
			if(c[i]=='\t') cl+=TAB_SIZE; else cl++;
		}
		printf(" AT %zd:%zd\n",ln,cl);
	}
}

void die(ssize_t scp,const char *fmt,...) {
	va_list args;
	printf("\n");
	va_start(args,fmt);
	vprintf(fmt,args);
	va_end(args);
	if(scp>=0) getpos(scp); else printf("\n");  
	exit(1);
}

int main(int argc,char **argv) {
	
	if(argc<2) {
		die(-1,"usage: %s filename\n",argv[0]);
	}
        
	if((f=fopen(argv[1],"rb"))==NULL) {
		die(-1,"EFOPEN");
	} 
        
	fseek(f,0L,SEEK_END);
	n=ftell(f);
	rewind(f);
	
	if((c=malloc(sizeof(*c)*n))==NULL) {
		die(-1,"EMALLOC");
	}

	if(fread(c,sizeof(*c),n,f)!=n) {
		die(-1,"EFREAD");
	}

	fclose(f);
	
	for(i=0;i<PRC_MAX;i++) {
		p[i]=-1;
	}
        
	mp=0;
	cp=0;
	ln=1;
	cl=1;
	sp=STK_MAX;

	while(cp<n) {
		d=1;
		switch(c[cp]) {
			case '+': m[mp]++; break;
			case '-': m[mp]--; break;
			case '<': if(mp==0) die(cp,"PBEBLWMIN"); else mp--; break;
			case '>': if(mp==MEM_MAX-1) die(cp,"PBEABVMAX"); else mp++; break;
			case '.': putchar(m[mp]); break;
			case ',': m[mp]=((ch=getchar())==EOF?0:ch); break;
			case '[':
				scp=cp;
				while(d!=0) {
					if(cp==n-1) die(scp,"PBEOBNOMAT");
					cp++;
					d-=(c[cp]==']')-(c[cp]=='[');
				}
				if(m[mp]!=0) {
					cp=scp;
				}
				break;
			case ']':
				scp=cp;
				while(d!=0) {
					if(cp==0) die(scp,"PBECBNOMAT");
					cp--;
					d-=(c[cp]=='[')-(c[cp]==']');
				}
				if(m[mp]==0) {
					cp=scp;
				} 
				break;
			case '(':
				scp=cp;
				while(d!=0) {
					if(cp==n-1) die(scp,"PBEOPNOMAT");
					cp++;
					d-=(c[cp]==')')-(c[cp]=='(');
				}
				p[m[mp]]=scp;
				break;
			case ')': 
				scp=cp;
				while(d!=0) {
					if(cp==0) die(scp,"PBECPNOMAT");
					cp--;
					d-=(c[cp]=='(')-(c[cp]==')');
				}
				cp=scp;
				if(sp==STK_MAX) die(scp,"PBESUNDFLW"); else  cp=s[sp++];
				break;
			case ':':
				if(p[m[mp]]==-1) die(cp,"PBENOPROC");
				if(sp==0) die(cp,"PBESOVRFLW"); else { s[--sp]=cp; cp=p[m[mp]]; }			
				break;
			case '#':
				while(cp<n-1 && c[cp]!='\n') {
					cp++;
				}
				break;
			case '@':
				exit(m[mp]);
				break;
			default: break;
		}
		
		cp++;
		
	}
	
	free(c);
	
	return 0;
}



