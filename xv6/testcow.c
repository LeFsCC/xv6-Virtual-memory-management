#include "types.h"
#include "stat.h"
#include "user.h"


/*
* Function: test copy on write
* Author : zhang jiacheng
* Time : 2019/12/23
*/

int buf = 0;


void test_share_page(){
	printf(1, "#----------------------------------------------------------------#\n");
	printf(1, "Test1: If child maintain free page number while data not changes.\n");
	printf(1, "Main process: [buf %d] [free page %d]\n", buf, fpgn());
	if(fork() == 0) {

		printf(1, "Child process[0] [BEFORE CHANGE]: [buf %d] [free page %d]\n", buf, fpgn());
		buf = 1;

		printf(1, "Child process[0] [AFTER CHANGE]: [buf %d] [free page %d]\n", buf, fpgn());
		exit();
	}
	sleep(100);
	printf(1, "Main process [BEFORE RECYCLE]: [buf %d] [free page %d]\n", buf, fpgn());
	wait();
	printf(1, "Main process [AFTER RECYCLE]: [buf %d] [free page %d]\n", buf, fpgn());
	printf(1, "Result: not changed.\n");
	printf(1, "#----------------------------------------------------------------#\n");
}

void test_main_changed_process() {

	printf(1, "\n");
	printf(1, "Test2: If parent maintain free page number while data changes.\n");
	printf(1, "Main process: [buf %d] [free page %d]\n", buf, fpgn());
	buf = 1;
	printf(1, "Main process: [buf %d] [free page %d]\n", buf, fpgn());
	printf(1, "Result: not changed.\n");
	printf(1, "#----------------------------------------------------------------#\n");
}

void test_main_unchanged_process() {

	printf(1, "\n");
	printf(1, "Test3: If parent maintain free page number while data not changes.\n");
	printf(1, "Main process: [buf %d] [free page %d]\n", buf, fpgn());
	buf = 0;
	printf(1, "Main process: [buf %d] [free page %d]\n", buf, fpgn());
	printf(1, "Result: not changed.\n");
	printf(1, "#----------------------------------------------------------------#\n");
}

void test_loop_fork() {

	printf(1, "\n");
	printf(1, "Test4: loop fork in child process.\n");
	printf(1, "Main process: [buf %d] [free page %d]\n", buf, fpgn());
	if(fork() == 0) {

		printf(1, "Child process[0] [BEFORE CHANGE]: [buf %d] [free page %d]\n", buf, fpgn());
		if(fork() == 0) {
			printf(1, "Child process[1] [BEFORE CHANGE]: [buf %d] [free page %d]\n", buf, fpgn());
			buf = 2;
			printf(1, "Child process[1] [AFTER CHANGE]: [buf %d] [free page %d]\n", buf, fpgn());
			exit();
		}
		wait();
		printf(1, "Child process[0]: [buf %d] [free page %d]\n", buf, fpgn());
		exit();
	}
	printf(1, "Main process [BEFORE RECYCLE]: [buf %d] [free page %d]\n", buf, fpgn());
	wait();
	printf(1, "Main process [AFTER RECYCLE]: [buf %d] [free page %d]\n", buf, fpgn());
	printf(1, "#----------------------------------------------------------------#\n");
}


void test_loop_fork2() {
	int pid[5] = {0};
	printf(1, "\n");
	printf(1, "Test5: loop fork in 5 child processes.\n");
	printf(1, "Main process: [buf %d] [free page %d]\n", buf, fpgn());
	for (int i = 0; i < 5; i++) {
		pid[i] = fork();
		sleep(50);
		if(pid[i] == 0) {
			printf(1, "Child process[%d]: [buf %d] [free page %d]\n", i, buf, fpgn());
			buf = i;
			printf(1, "Child process[%d]: [buf %d] [free page %d]\n", i, buf, fpgn());
			buf = i + 1;
			printf(1, "Child process[%d]: [buf %d] [free page %d]\n", i, buf, fpgn());
			printf(1, "\n");
			exit();
		}
	}

	for (int i = 0; i < 5; i++) {
		wait();
	}
	printf(1, "Main process [AFTER RECYCLE]: [buf %d] [free page %d]\n", buf, fpgn());
	printf(1, "#----------------------------------------------------------------#\n");
}

int main(int argc, char **argv){

	test_share_page();
	test_main_changed_process();
	test_main_unchanged_process();
	test_loop_fork();
	test_loop_fork2();

	return 0;
}
