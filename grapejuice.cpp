#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#define BLOCK_LENGTH 16

typedef enum {
    LITTLE_ENDIAN,
    BIG_ENDIAN
} Endianness;

typedef struct {
    bool is_64_bit;
    Endianness endianness;
    ABI abi;
} ELF_header;

typedef enum {
    ABI_SYSTEM_V,
    ABI_HP_UX,
    ABI_NETBSD,
    ABI_LINUX,
    ABI_GNU_HURD,
    ABI_SOLARIS,
    ABI_AIX,
    ABI_IRIX,
    ABI_FREEBSD,
    ABI_TRU64,
    ABI_NOVELL_MODESTO,
    ABI_OPENBSD,
    ABI_OPENVMS,
    ABI_NONSTOP_KERNEL,
    ABI_AROS,
    ABI_FENIX_OS,
    ABI_CLOUD_ABI,
    ABI_STRATUS
} ABI;

ELF_header elf_header;

// returns the position of the last byte parsed, or -1 if an error is found
int parse_header(unsigned char* data) {
    elf_header.is_64_bit = data[4] == 2;
    elf_header.endianness = data[5] == 1 ? LITTLE_ENDIAN : BIG_ENDIAN;
    elf_header.abi = data[7];
    // TODO: continue from here
    //elf_header.abi_version = data[8]; ??????????????????????????
}

bool parse_elf(unsigned char *data) {
    int offset = parse_header(data);
    if (offset > 0) {
        fprintf(stderr, "Error: Failed to parse ELF header.\n");
        return false;
    }

    return true;
}

int main(int argc, char **argv)
{
    int max_lines = INT_MAX;
    if (argc > 2) {
        max_lines = atoi(argv[2]);
    }

    FILE* file = fopen(argv[1], "rb");

    if (!file) {
        fprintf(stderr, "Error: Unexistent file.\n");
        return -1;
    }

    fseek(file, 0, SEEK_END);
    int file_length = ftell(file);

    rewind(file);

    unsigned char* out = (unsigned char *) malloc(file_length);
    int bytes_read = fread(out, 1, file_length, file);

    if (bytes_read != file_length) {
        fprintf(stderr, "Error: Couldn't read the whole file.\n");
        return -1;
    }

    printf("File size: %d\n", file_length);
    printf("ASCII\t\t\t\tHex\n");
    for (int i = 0; i < file_length && i / BLOCK_LENGTH < max_lines; i++) {
        for (int j = 0; j < BLOCK_LENGTH; j++) {
            if (out[i + j] >= 32 && out[i + j] <= 126) {
                printf("%c", out[i + j]);
            } else {
                printf(".");
            }
        }
        printf("\t\t");
        for (int j = 0; j < BLOCK_LENGTH; j++) {
            if (j == BLOCK_LENGTH / 2) {
                printf(" ");
            }
            printf("%02x", out[i+j]);
        }
        i += BLOCK_LENGTH;
        printf("\n");
    }

    parse_elf(out);

    return 0;
}

