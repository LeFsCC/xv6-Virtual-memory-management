#include "types.h"
#include "stat.h"
#include "user.h"


/*
* Function: test copy on write
* Author : zhang jiacheng
* Time : 2019/12/24
*/
void Recur(int n) {

	if(n <= 0)
		return;
	// In case huge parameter attack
	if(n > 100)
		return;

	// About 1kb stack
	char stack[950];
	memset(stack, '0', sizeof(stack));
	Recur(n-1);
}

int main() {

	printf(1, "Test1: allocate 4kb stack in my new framework!\n");
	Recur(4);
	printf(1, "success in allocate 4kb stack in my new framework!\n");

	printf(1, "Test2: allocate 8kb stack in my new framework!\n");
	Recur(8);
	printf(1, "success in allocate 8kb stack in my new framework!\n");

	printf(1, "Test3: allocate 1mb stack in my new framework!\n");
	Recur(1 * 1024);
	printf(1, "success in allocate 1mb stack in my new framework!\n");

	printf(1, "Test4: allocate 2mb stack in my new framework!\n");
	Recur(2 * 1024);
	printf(1, "success in allocate 2mb stack in my new framework!\n");

	printf(1, "Test5: allocate 4mb stack in my new framework!\n");
	Recur(4 * 1024);
	printf(1, "success in allocate 4mb stack in my new framework!\n");

	printf(1, "Test6: allocate 5mb stack in my new framework!\n");
	Recur(5 * 1024);
	printf(1, "success in allocate 5mb stack in my new framework!\n");



	printf(1, "Test7: allocate 221mb stack in my new framework!\n");
	char b[221][1024][1024];
	for(int i = 0; i < 1024;i++) {
		for(int j = 0; j < 1024; j++) {
			b[0][i][j] = '1';
		}
	}
	printf(1, "success in allocate 221mb stack in my new framework!%c\n", b[0][0][0]);

	return 0;
}