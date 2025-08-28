// hcpk_stub.c
// Self-extracting stub for .HCPK packages
// Usage: compiled stub with appended HCPK -> running the binary extracts files.

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <time.h>

#ifdef _WIN32
#include <direct.h>
#define mkdir_p(p) _mkdir(p)
#else
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>
#define mkdir_p(p) mkdir(p, 0755)
#endif

#define MAGIC "HCPK"
#define MAGIC_LEN 4
#define MARKER "HCPK_EOF:"   // marker placed before appended package to find it

static void perror_exit(const char *msg) {
    perror(msg);
    exit(1);
}

static uint32_t read_u32_le(FILE *f) {
    uint32_t v = 0;
    if (fread(&v, sizeof(uint32_t), 1, f) != 1) perror_exit("read_u32_le");
    return v;
}

int main(int argc, char **argv) {
    clock_t t0 = clock();
    printf("HolyPackager Self-extractor\n");

    // find path to own executable
    const char *out_dir = NULL;
    if (argc >= 2) out_dir = argv[1];
    else out_dir = "./hcpk_extracted";

    printf("Output directory: %s\n", out_dir);
    mkdir_p(out_dir);

    // open our own binary
    FILE *me = NULL;
#ifdef _WIN32
    // On Windows, argv[0] should work
    me = fopen(argv[0], "rb");
#else
    me = fopen(argv[0], "rb");
#endif
    if (!me) {
        // fallback: try /proc/self/exe (linux)
#ifdef __linux__
        me = fopen("/proc/self/exe", "rb");
#endif
        if (!me) perror_exit("opening self executable failed");
    }

    // Find marker "HCPK_EOF:" scanning from end
    fseek(me, 0, SEEK_END);
    long file_len = ftell(me);

    const size_t marker_len = strlen(MARKER);
    const size_t scan_len = 1024 * 64; // scan last 64KB for marker
    long start_scan = file_len - (long)scan_len;
    if (start_scan < 0) start_scan = 0;
    fseek(me, start_scan, SEEK_SET);

    char *buf = malloc((file_len - start_scan) + 1);
    if (!buf) perror_exit("malloc");
    size_t read_bytes = fread(buf, 1, file_len - start_scan, me);
    buf[read_bytes] = '\0';

    char *pos = NULL;
    for (long i = (long)read_bytes - (long)marker_len; i >= 0; --i) {
        if (memcmp(buf + i, MARKER, marker_len) == 0) { pos = buf + i; start_scan += i; break; }
    }

    if (!pos) {
        fprintf(stderr, "Marker not found in stub. Is the HCPK appended?\n");
        free(buf);
        fclose(me);
        return 1;
    }

    long marker_offset_in_file = start_scan;
    long package_offset = marker_offset_in_file + (long)marker_len;

    // Seek to package offset in original file and read package
    fseek(me, package_offset, SEEK_SET);

    // Read header: expecting MAGIC (4), version (u32), hc_size (u32)
    char magic[5] = {0};
    if (fread(magic, 1, 4, me) != 4) perror_exit("read magic");
    if (memcmp(magic, MAGIC, 4) != 0) {
        fprintf(stderr, "Invalid package magic. Found: %.4s\n", magic);
        free(buf);
        fclose(me);
        return 1;
    }

    uint32_t version = read_u32_le(me);
    uint32_t hc_size = read_u32_le(me);
    printf("Package version: %u\n", version);
    printf("HolyC size: %u bytes\n", hc_size);

    // read holyc code
    unsigned char *hc_buf = malloc(hc_size + 1);
    if (!hc_buf) perror_exit("malloc hc_buf");
    if (fread(hc_buf, 1, hc_size, me) != hc_size) perror_exit("read hc code");
    hc_buf[hc_size] = '\0';

    // write holyc file to out_dir
    char hc_path[1024];
    snprintf(hc_path, sizeof(hc_path), "%s/%s", out_dir, "extracted.hc");
    FILE *fhc = fopen(hc_path, "wb");
    if (!fhc) perror_exit("create extracted.hc");
    fwrite(hc_buf, 1, hc_size, fhc);
    fclose(fhc);
    printf("Wrote HolyC to: %s\n", hc_path);

    // read number of resources
    uint32_t num_resources = read_u32_le(me);
    printf("Resources: %u\n", num_resources);

    for (uint32_t r = 0; r < num_resources; ++r) {
        uint32_t name_len = read_u32_le(me);
        if (name_len == 0 || name_len > 1000) { fprintf(stderr, "suspicious name_len\n"); break; }
        char *name = malloc(name_len + 1);
        if (!name) perror_exit("malloc name");
        if (fread(name, 1, name_len, me) != name_len) perror_exit("read name");
        name[name_len] = '\0';

        uint32_t res_size = read_u32_le(me);
        unsigned char *res_buf = malloc(res_size);
        if (!res_buf) perror_exit("malloc res");
        if (fread(res_buf, 1, res_size, me) != res_size) perror_exit("read res data");

        // create path and write file
        char out_path[2048];
        snprintf(out_path, sizeof(out_path), "%s/%s", out_dir, name);
        // create subdirs if needed (naive: only top-level)
        FILE *fr = fopen(out_path, "wb");
        if (!fr) {
            fprintf(stderr, "Failed to create resource file: %s\n", out_path);
        } else {
            fwrite(res_buf, 1, res_size, fr);
            fclose(fr);
            printf("Wrote resource: %s (%u bytes)\n", out_path, res_size);
        }

        free(name);
        free(res_buf);
    }

    free(hc_buf);
    free(buf);
    fclose(me);

    clock_t t1 = clock();
    double elapsed = (double)(t1 - t0) / CLOCKS_PER_SEC;
    printf("Extraction completed in %.3f seconds\n", elapsed);

    printf("Next steps:\n");
    printf(" - You can open the extracted.hc in TempleOS or copy it into a TempleOS image.\n");
    printf(" - This stub does NOT execute HolyC natively (TempleOS required).\n");

    return 0;
}
