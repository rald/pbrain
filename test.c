#include <stdio.h>

void gotoxy(int x,int y) {
	printf("\x1B[%d;%dH",y,x);
}

int main(void) {
	for(int j=0;j<16;j++) {
		for(int i=0;i<16;i++) {
			gotoxy(i,j);
			putchar('X');
		}
	}
}
