#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>

#define FIB_DEV "/dev/fibonacci"

int main()
{

    long long sequence;
    char buf[1];
    int offset = 20;
    int wanted;

    int file_dest = open(FIB_DEV, O_RDWR);
    if (file_dest < 0) {
        perror("character device cannot be opened");
        exit(1);
    }

// section A : {

    printf("please enter your wanted offset: ");
    scanf("%d", &wanted);
    printf("Reading from " FIB_DEV "\n");
    printf("the sequence with offset %d is ", wanted);
    lseek(file_dest, wanted, SEEK_SET);
    long long temp = read(file_dest, buf, 1);
    printf("%lld\n", temp);

// }

// in case you want all the 20 sequences printed, uncomment this section (and comment section A)!

//    printf("offset \t\tsequence\n");
//    for (int i = 1; i <= offset; i++) {
//        lseek(file_dest, i, SEEK_SET);
//        sequence = read(file_dest, buf, 1);
//        printf("%d\t:\t%lld\n", i, sequence);
//    }

    close(file_dest);
    return 0;
}
