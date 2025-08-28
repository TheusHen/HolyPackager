#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <time.h>

#define MAGIC "HCPK"
#define VERSION 1

typedef struct {
    char *name;
    uint32_t size;
    unsigned char *data;
} Resource;

unsigned char* read_file(const char* filename, uint32_t* out_size) {
    FILE* f = fopen(filename, "rb");
    if(!f) {
        perror("Error opening file");
        exit(1);
    }
    fseek(f, 0, SEEK_END);
    *out_size = ftell(f);
    fseek(f, 0, SEEK_SET);
    unsigned char *buffer = malloc(*out_size);
    if (!buffer) {
        perror("Memory allocation failed");
        fclose(f);
        exit(1);
    }
    fread(buffer, 1, *out_size, f);
    fclose(f);
    return buffer;
}

void write_uint32(FILE* f, uint32_t value) {
    fwrite(&value, sizeof(uint32_t), 1, f);
}

int main(int argc, char* argv[]) {
    clock_t start_time = clock();

    if(argc < 3) {
        printf("Usage: %s code.hc output.hcpk [resources...]\n", argv[0]);
        return 1;
    }

    const char* hc_file = argv[1];
    const char* out_file = argv[2];
    int num_resources = argc - 3;

    printf("Reading HolyC file: %s\n", hc_file);
    uint32_t hc_size;
    unsigned char* hc_data = read_file(hc_file, &hc_size);
    printf("HolyC file size: %u bytes\n", hc_size);

    Resource* resources = malloc(sizeof(Resource) * num_resources);
    for(int i = 0; i < num_resources; i++) {
        resources[i].name = argv[i+3];
        printf("Reading resource %d: %s\n", i+1, resources[i].name);
        resources[i].data = read_file(resources[i].name, &resources[i].size);
        printf("Resource size: %u bytes\n", resources[i].size);
    }

    printf("Creating package: %s\n", out_file);
    FILE* out = fopen(out_file, "wb");
    if(!out) {
        perror("Error creating output file");
        return 1;
    }

    // Header
    fwrite(MAGIC, 1, 4, out);            // Magic
    write_uint32(out, VERSION);           // Version
    write_uint32(out, hc_size);           // HolyC size
    fwrite(hc_data, 1, hc_size, out);    // HolyC code

    // Resources
    write_uint32(out, num_resources);
    for(int i = 0; i < num_resources; i++) {
        uint32_t name_len = strlen(resources[i].name);
        write_uint32(out, name_len);
        fwrite(resources[i].name, 1, name_len, out);
        write_uint32(out, resources[i].size);
        fwrite(resources[i].data, 1, resources[i].size, out);
        free(resources[i].data);
    }

    free(hc_data);
    free(resources);
    fclose(out);

    clock_t end_time = clock();
    double elapsed = (double)(end_time - start_time) / CLOCKS_PER_SEC;
    printf("Package %s created successfully!\n", out_file);
    printf("Elapsed time: %.3f seconds\n", elapsed);

    return 0;
}
