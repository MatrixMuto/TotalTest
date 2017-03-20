/*
 *
 *
 *
 *
 *
 *
 */


#include <inttypes.h>
#include <stdio.h>
#include <stdint.h>

#define MKTAG(a, b, c, d) ((a) | ((b) << 8) | ((c) << 16) | ((unsigned)(d) << 24))


uint32_t type_mov = MKTAG('m', 'o', 'o', 'v');

char *filename;
FILE *in;
uint8_t *buf, *end, *ptr, *next;
uint8_t buffer[4096];

int64_t offset;
int depth = 0;

typedef struct MOVAtom {
    uint32_t type;
    int64_t size; /* total size (excluding the size and type fields) */
} MOVAtom;

typedef struct {

} XXIOContext;


void xxio_open(XXIOContext **pb) {

}

/* XXX: put an inline version */
int avio_r8(XXIOContext *s) {
    unsigned char t;
    fread(&t, 1, 1, in);
    return t;
}

unsigned int avio_rb16(XXIOContext *s) {
    unsigned int val;
    val = avio_r8(s) << 8;
    val |= avio_r8(s);
    return val;
}

unsigned int avio_rb32(XXIOContext *s) {
    unsigned int val;
    val = avio_rb16(s) << 16;
    val |= avio_rb16(s);
    return val;
}

uint64_t avio_rb64(XXIOContext *s) {
    uint64_t val;
    val = (uint64_t) avio_rb32(s) << 32;
    val |= (uint64_t) avio_rb32(s);
    return val;
}

unsigned int avio_rl16(XXIOContext *s) {
    unsigned int val;
    val = avio_r8(s);
    val |= avio_r8(s) << 8;
    return val;
}

unsigned int avio_rl32(XXIOContext *s) {
    unsigned int val;
    val = avio_rl16(s);
    val |= avio_rl16(s) << 16;
    return val;
}


void mov_read_default(XXIOContext *pb, MOVAtom atom) {
    MOVAtom a;
    int64_t total_size = 0;

    depth++;
    while (total_size + 8 <= atom.size && !feof(in)) {
        a.size = avio_rb32(NULL);
        a.type = avio_rl32(NULL);
        if (feof(in)) {
            printf("end of file\n");
            return ;
        }
        total_size += 8;

        if (a.size == 0) {

        } else if (a.size == 1) {
            a.size = avio_rb64(NULL) - 8;
            total_size += 8;
        }
        a.size -= 8;

        ptr = (char *) &a.type;
        printf("%d '%.4s' %"PRId64"\n",
               depth,
               (char*)&a.type,
               a.size);

        if (a.type == MKTAG('m', 'o', 'o', 'v')) {
            mov_read_default(NULL, a);
        } else if (a.type == MKTAG('t','r','a','k')) {
            mov_read_default(NULL, a);
//        } else if (a.type == MKTAG('u','u','i','d')) {
//            mov_read_default(NULL, a);
//        } else if (a.type == MKTAG('t','k','h','d')) {
//
        } else if (a.type == MKTAG('m','d','i','a')) {
            mov_read_default(NULL, a);
        } else if (a.type == MKTAG('m','i','n','f')) {
            mov_read_default(NULL, a);
        } else if (a.type == MKTAG('s','t','b','l')) {
            mov_read_default(NULL, a);
        } else {
            if (fseek(in, a.size, SEEK_CUR)) {
                printf("fseek eror\n");
                break;
            };
        }

        total_size += a.size;
    }

    if (total_size < atom.size) {
        printf("atom no \n");
    }

    depth--;
}

int main(int argc, char *argv[]) {
    if (argc < 2) {
        perror("too few para\n");
        return -1;
    }

    in = fopen(argv[1], "r");
    if (in == NULL) {
        printf("failed open %s\n", argv[1]);
        return -1;
    }

    MOVAtom root;
    root.size = INT64_MAX;
    root.type = MKTAG('r', 'o', 'o', 't');

    mov_read_default(NULL, root);

    fclose(in);
    return 0;
}