#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <unistd.h>

#define FIB_DEV "/dev/fibonacci"

int main()
{
    char buf[1];
    char write_buf[] = "testing writing";
    int offset = 100; /* TODO: try test something bigger than the limit */

    int fd = open(FIB_DEV, O_RDWR);
    if (fd < 0) {
        perror("Failed to open character device");
        exit(1);
    }
    FILE *fptr = fopen("TimeTaken.txt", "w");
    if (!fptr) {
        return 0;
    }
    for (int i = 0; i <= offset; i++) {
        long long sz, kt;
        lseek(fd, i, SEEK_SET);
        sz = read(fd, buf, 1);
        printf("Reading from " FIB_DEV
               " at offset %d, returned the sequence "
               "%lld.\n",
               i, sz);
        kt = write(fd, write_buf, 0);
        printf("Time taken to calculate the sequence %lld.\n", kt);
        fprintf(fptr, "Time taken at offset %d : %lld.\n", i, kt);
    }
    close(fd);
    return 0;
}