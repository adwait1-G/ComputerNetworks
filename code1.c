int main() {

	int fv = fork();

	while(1) {
	
		sleep(1);

	if(fv == 0)
		printf("I am child, pid = %d, ppid = %d\n", getpid(), getppid());
	else if(fv != 0) {
		printf("I am parent, pid = %d, ppid = %d\n", getpid(), getppid());
		exit(1);
	}

	}

	return 0;
}
