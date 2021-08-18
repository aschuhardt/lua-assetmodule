#include <miniz.h>
#include <stdio.h>

#define WRITE_SIZE 4096

int main(int argc, const char** argv) {
  FILE* file = fopen(argv[1], "rb");
  fseek(file, 0, SEEK_END);
  size_t file_len = ftell(file);
  fseek(file, 0, SEEK_SET);

  unsigned char* uncompressed = malloc(file_len * sizeof(unsigned char));
  fread(uncompressed, sizeof(unsigned char), file_len, file);

  size_t compressed_len = compressBound(file_len);
  unsigned char* compressed = malloc(compressed_len * sizeof(unsigned char));
  if (compress2(compressed, &compressed_len, uncompressed, file_len,
                MZ_UBER_COMPRESSION) != Z_OK) {
    printf("Failed to compress %s", argv[1]);
    return -1;
  }

  size_t remaining = compressed_len;
  do {
    size_t n = remaining < WRITE_SIZE ? remaining : WRITE_SIZE;
    remaining -= fwrite(&compressed[compressed_len - remaining],
                        sizeof(unsigned char), n, stdout);
  } while (remaining > 0);

  free(uncompressed);
  free(compressed);
  return 0;
}
