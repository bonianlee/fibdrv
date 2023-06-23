#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <time.h>
#include <unistd.h>

#define FIB_DEV "/dev/fibonacci"

int main()
{
    char buf[1000];
    char write_buf[] = "testing writing";
    int offset = 1000; /* TODO: try test something bigger than the limit */

    int fd = open(FIB_DEV, O_RDWR);
    if (fd < 0) {
        perror("Failed to open character device");
        exit(1);
    }
    FILE *fptr_kt = fopen("TimeTaken.txt", "w");
    if (!fptr_kt) {
        return 0;
    }
    FILE *fptr_ut = fopen("UserTimeTaken.txt", "w");
    if (!fptr_ut) {
        fclose(fptr_kt);
        return 0;
    }
    for (int i = 0; i <= offset; i++) {
        long long sz, kt;
        struct timespec t1, t2;
        lseek(fd, i, SEEK_SET);
        clock_gettime(CLOCK_MONOTONIC, &t1);
        sz = read(fd, buf, 1);
        clock_gettime(CLOCK_MONOTONIC, &t2);
        long long ut =
            ((t2.tv_sec * 1e9 + t2.tv_nsec) - (t1.tv_sec * 1e9 + t1.tv_nsec));
        printf("Reading from " FIB_DEV
               " at offset %d, returned the sequence "
               "%s.\n",
               i, buf);
        kt = write(fd, write_buf, 0);
        printf("Time taken to calculate the sequence %lld.\n", sz);
        fprintf(fptr_kt, "Time taken at offset %d : %lld\n", i, sz);
        fprintf(fptr_ut, "Time taken at offset %d : %lld\n", i, ut);
    }
    fclose(fptr_kt);
    fclose(fptr_ut);
    close(fd);
    return 0;
}