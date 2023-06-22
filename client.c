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
    // long long sz;
    // char buf[1];
    // // char write_buf[] = "testing writing";
    // int offset = 100; /* TODO: try test something bigger than the limit */

    // int fd = open(FIB_DEV, O_RDWR);
    // if (fd < 0) {
    //     perror("Failed to open character device");
    //     exit(1);
    // }

    // for (int i = 0; i <= offset; i++) {
    //     sz = write(fd, write_buf, strlen(write_buf));
    //     printf("Writing to " FIB_DEV ", returned the sequence %lld\n", sz);
    // }

    // FILE *fptr = fopen("UserTimeTaken.txt", "w");
    // if (!fptr) {
    //     return 0;
    // }

    // for (int i = 0; i <= offset; i++) {
    //     long long sz;
    //     struct timespec t1, t2;
    //     lseek(fd, i, SEEK_SET);
    //     clock_gettime(CLOCK_MONOTONIC, &t1);
    //     sz = read(fd, buf, 1);
    //     clock_gettime(CLOCK_MONOTONIC, &t2);
    //     long long ut =
    //         ((t2.tv_sec * 1e9 + t2.tv_nsec) - (t1.tv_sec * 1e9 +
    //         t1.tv_nsec));
    //     printf("Reading from " FIB_DEV
    //            " at offset %d, returned the sequence "
    //            "%lld, taking time: %lld.\n",
    //            i, sz, ut);
    //     fprintf(fptr, "Userspace time taken at offset %d : %lld\n", i, ut);
    // }

    // for (int i = offset; i >= 0; i--) {
    //     lseek(fd, i, SEEK_SET);
    //     sz = read(fd, buf, 1);
    //     printf("Reading from " FIB_DEV
    //            " at offset %d, returned the sequence "
    //            "%lld.\n",
    //            i, sz);
    // }

    // close(fd);
    return 0;
}
