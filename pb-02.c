#include<stdio.h>
#include<stdlib.h>
#include<stdarg.h>
#include<string.h>

#include <curl/curl.h>

#define TAB_SIZE 4
#define MEM_MAX 30000
#define PRC_MAX 256
#define STK_MAX 256

unsigned short s[STK_MAX]={0};
unsigned char m[MEM_MAX]={0};
unsigned char *c=NULL;
ssize_t p[PRC_MAX],sp,d;
size_t mp,cp,scp,n,ln,cl,i;
FILE *f;
int ch;



struct MemoryStruct {
  char *memory;
  size_t size;
};

static size_t
WriteMemoryCallback(void *contents, size_t size, size_t nmemb, void *userp)
{
  size_t realsize = size * nmemb;
  struct MemoryStruct *mem = (struct MemoryStruct *)userp;

  char *ptr = realloc(mem->memory, mem->size + realsize + 1);
  if(!ptr) {
    /* out of memory! */
    printf("not enough memory (realloc returned NULL)\n");
    return 0;
  }

  mem->memory = ptr;
  memcpy(&(mem->memory[mem->size]), contents, realsize);
  mem->size += realsize;
  mem->memory[mem->size] = 0;

  return realsize;
}



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

  CURL *curl_handle;
  CURLcode res;

  struct MemoryStruct chunk;

  chunk.memory = malloc(1);  /* will be grown as needed by the realloc above */
  chunk.size = 0;    /* no data at this point */

  curl_global_init(CURL_GLOBAL_ALL);

  /* init the curl session */
  curl_handle = curl_easy_init();

  /* specify URL to get */
  curl_easy_setopt(curl_handle, CURLOPT_URL, argv[1]);

  /* send all data to this function  */
  curl_easy_setopt(curl_handle, CURLOPT_WRITEFUNCTION, WriteMemoryCallback);

  /* we pass our 'chunk' struct to the callback function */
  curl_easy_setopt(curl_handle, CURLOPT_WRITEDATA, (void *)&chunk);

  /* some servers do not like requests that are made without a user-agent
     field, so we provide one */
  curl_easy_setopt(curl_handle, CURLOPT_USERAGENT, "libcurl-agent/1.0");

  /* get it! */
  res = curl_easy_perform(curl_handle);

  /* check for errors */
  if(res != CURLE_OK) {
    fprintf(stderr, "curl_easy_perform() failed: %s\n",
            curl_easy_strerror(res));
  }
  else {
    /*
     * Now, our chunk.memory points to a memory block that is chunk.size
     * bytes big and contains the remote file.
     *
     * Do something nice with it!
     */

//    printf("%lu bytes retrieved\n", (unsigned long)chunk.size);
  }

//	printf("%s\n",chunk.memory);

	chunk.memory[chunk.size]='\0';

	c=strdup(chunk.memory);
	n=chunk.size;
        
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
			default: break;
		}
		
		cp++;
		
	}

	free(c);

  /* cleanup curl stuff */
  curl_easy_cleanup(curl_handle);
	
  free(chunk.memory);

  /* we are done with libcurl, so clean it up */
  curl_global_cleanup();
	
	return 0;
}


