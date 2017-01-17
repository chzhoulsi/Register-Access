#include <stdio.h>        
#include <string.h>       
#include <stdlib.h>       
#include <stdbool.h>
#include <unistd.h>       
#include <unistd.h>       
#include <sys/mman.h>     
#include <errno.h>        
#include <fcntl.h>        
#include <stdint.h>       

#define start_value 0x30

void usage() {
    printf("-w|r stating_address number_of_bytes\n");
    printf("for example -r 0xc10000 10\n");
}


void hexdump(char *desc, void *addr, int len) {
    int i;
    unsigned char buff[17];
    unsigned char *pc = (unsigned char*)addr;

    //  Output description if given.
    if (desc != NULL)
        printf("%s:\n", desc);

    if (len == 0) {
        printf("  ZERO LENGTH\n");
        return;
    }
    if (len < 0) {
        printf("  NEGATIVE LENGTH: %i\n",len);
        return;
    }

    // Process every byte in the data.
    for (i = 0; i < len; i++) {
        // Multiple of 16 means new line (with line offset).

        if ((i % 16) == 0) {
            // Just don't print ASCII for the zeroth line.
            if (i != 0)
                printf("  %s\n", buff);

            // Output the offset.
            printf("  %04x ", i);
        }

        // Now the hex code for the specific character.
        printf(" %02x", pc[i]);

        // And store a printable ASCII character for later.
        if ((pc[i] < 0x20) || (pc[i] > 0x7e))
            buff[i % 16] = '.';
        else
            buff[i % 16] = pc[i];
        buff[(i % 16) + 1] = '\0';
    }

    // Pad out last line if not exactly 16 characters.
    while ((i % 16) != 0) {
        printf ("   ");
        i++;
    }

    // And print the final ASCII bit.
    printf ("  %s\n", buff);
}

int main(int argc, char *argv[]) {

    int64_t          dev_base = 0;
    int64_t          size = 0;

    unsigned long    alloc_size, page_mask, page_size;
    void *           mapped_base; 
    void *           virt_addr;
    uint32_t         i = 0;
    char             j = start_value;

    /* ifRead = 1, read; ifRead = 0, write */
    bool             ifRead = 0;
    int              opt = 0;


    while ((opt = getopt(argc, argv, "wr")) != -1) {
        switch (opt) {
            case 'w': 
                ifRead = 0;
                break;

            case 'r':
                ifRead = 1;
                break;

            default:
                usage();
                return 1; 
        }
    }

    if (argv[optind] == NULL || argv[optind + 1] == NULL) {
        usage();
        return 1;
    } else {
        dev_base = strtoll(argv[optind], NULL, 0);
        size = strtol(argv[optind+1], NULL, 0);
    }


    int memfd = open("/dev/mem", O_RDWR | O_SYNC);
    if(memfd == -1) {
        printf("Can't open /dev/mem\n");
        return -1;
    }

    page_size = sysconf(_SC_PAGESIZE);
    page_mask = (page_size - 1);
    alloc_size = (((size / page_size) + 1) * page_size);


    mapped_base = mmap(NULL,
            alloc_size,
            PROT_READ | PROT_WRITE,
            MAP_SHARED,
            memfd,
            (dev_base & ~page_mask)
            );

    if (mapped_base == MAP_FAILED)
    {  
        printf("MAP FAILED\n");
        return(-1);
    }

    virt_addr = (mapped_base + (dev_base & page_mask)); 

    if (ifRead == 1) {
        printf("hexdump from: \n\
                \tphysical addr: \t0x%llx\n\
                \tvirtual addr: \t0x%llx\n\
                \tsize: \t\t0x%llx\n"
                ,dev_base, virt_addr, size);
        hexdump(NULL, (void *)virt_addr, size);
    } else {
        for (i=0; i<size; i++) {
            memcpy(virt_addr++, (void *)&j, 1);
            j++;
        }
    }
    close(memfd);
    return 0;
}

