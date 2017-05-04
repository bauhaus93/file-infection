#include "checksum_generator.h"

int generate_checksums(const char* fileIn, const char* fileOut, const char* safeguard) {

  char buffer[BUFFER_SIZE];
  hashset_t* root = NULL;

  memset(buffer, 0, sizeof(char) * BUFFER_SIZE);

  FILE *fIn = fopen(fileIn, "r");
  if (fIn == NULL) {
    fprintf(stderr, "Could not open input file %s\n", fileIn);
    return 1;
  }
  while(fgets(buffer, BUFFER_SIZE, fIn) != NULL) {
    size_t buffLen = strlen(buffer);
    buffer[buffLen - 1] = 0;

    hashset_t* entry = malloc(sizeof(hashset_t));
    char* hashStr = malloc(sizeof(char) * buffLen);

    strncpy(hashStr, buffer, buffLen);
    entry->string = hashStr;
    entry->hash = checksum(hashStr);

    switch(check_uniqueness(entry, root)) {
      case 0: entry->next = root;
              root = entry;
              break;
      case 1: printf("Found duplicate string %s\n", entry->string);
              free(entry);
              break;
      case 2: fprintf(stderr, "hash collision %s with other in list\n", entry->string);
              free(entry);
              free_list(root);
              fclose(fIn);
              return 1;
      default:fprintf(stderr, "check_uniqueness unexpected value\n");
              break;
    }
  }
  fclose(fIn);

  FILE *fOut = fopen(fileOut, "w");
  if (fOut == NULL) {
    fprintf(stderr, "Could not open output file %s\n", fileOut);
    return 1;
  }

  fprintf(fOut, "#ifndef %s\n#define %s\n", safeguard, safeguard);

  for(hashset_t* ptr = root; ptr != NULL; ptr = ptr->next) {
    char line[BUFFER_SIZE];
    snprintf(line, BUFFER_SIZE, "#define CS_%s (0x%X)\n", ptr->string, ptr->hash);
    for(int i = 10; i < strlen(line); i++)
      line[i] = (char) toupper((int) line[i]);
    fputs(line, fOut);
  }

  fputs("#endif", fOut);

  fclose(fOut);

  free_list(root);

  return 0;
}

void free_list(hashset_t* root) {
  while (root != NULL) {
    hashset_t* next = root->next;
    free(root);
    root = next;
  }
}

int check_uniqueness(hashset_t* newEntry, hashset_t* root) {

  for(hashset_t* ptr = root; ptr != NULL; ptr = ptr->next) {
    if (strcmp(ptr->string, newEntry->string) == 0)
      return 1;
    if (ptr->hash == newEntry->hash)
      return 2;
  }
  return 0;
}

int main(int argc, char** argv) {
  if (argc != 4) {
    fprintf(stderr, "Need exactly 3 arguments: filenameIn filenameOut safeguard\n");
    return 1;
  }
  return generate_checksums(argv[1], argv[2], argv[3]);
}
