#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/ioctl.h>
#include <err.h>

#define HIBOMA_GET_VERSION 0
#define HIBOMA_OPEN_FD     1

int main()
{
	int r;
	int fd = open("/dev/hiboma", O_RDONLY|O_WRONLY);
	if ( fd < 0) {
		perror("failed to open /dev/hiboma");
		return 1;
	}

	r = ioctl(fd, HIBOMA_GET_VERSION);
	if (r < 0 ) {
		perror("ioctl");
		return EXIT_FAILURE;
	}
	printf("version = %d\n", r);

	r = ioctl(fd, HIBOMA_OPEN_FD);
	if (r < 0 ) {
		perror("ioctl");
		return EXIT_FAILURE;
	}
	printf("fd = %d\n", r);

	r = ioctl(fd, HIBOMA_OPEN_FD);
	if (r < 0 ) {
		perror("ioctl");
		return EXIT_FAILURE;
	}
	printf("fd = %d\n", r);

	char command[32];
snprintf(command, sizeof(command), "ls -hal /proc/%d/fd", getpid());
	system(command);

	exit(EXIT_SUCCESS);
}

