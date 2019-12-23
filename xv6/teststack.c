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
	Recur(4);
	printf(1, "success in allocate 4kb stack in my new framework!\n");
	Recur(8);
	printf(1, "success in allocate 8kb stack in my new framework!\n");
	Recur(1 * 1024);
	printf(1, "success in allocate 1mb stack in my new framework!\n");
	Recur(2 * 1024);
	printf(1, "success in allocate 2mb stack in my new framework!\n");
	Recur(4 * 1024);
	printf(1, "success in allocate 4mb stack in my new framework!\n");
	Recur(5 * 1024);
	printf(1, "success in allocate 5mb stack in my new framework!\n");
	return 0;
}