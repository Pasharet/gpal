#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "cJSON.h"

int main(int argc, char *argv[]) {
    if (argc != 3 || strcmp(argv[1], "--file")) {
    // if (argc < 3) { // В этом случае нет проверки на наличие ключа "--file"
        printf("Usage: %s --file <filename.json>\n", argv[0]);
        return 1;
    }

    const char *input_file = argv[2];
    FILE *input_stream = fopen(input_file, "r");
    if (input_stream == NULL) {
        printf("Error: unable to open file %s\n", input_file);
        return 1;
    }

    fseek(input_stream, 0, SEEK_END);
    long size = ftell(input_stream);
    fseek(input_stream, 0, SEEK_SET);

    char *buffer = (char*) malloc(size);
    if (buffer == NULL) {
        fclose(input_stream);
        printf("Error: unable to allocate memory\n");
        return 1;
    }

    if (fread(buffer, 1, size, input_stream) != size) {
        fclose(input_stream);
        free(buffer);
        printf("Error: unable to read file %s\n", input_file);
        return 1;
    }

    fclose(input_stream);
    buffer[size] = '\0';

    cJSON *root = cJSON_Parse(buffer);
    if (root == NULL) {
        const char *error_ptr = cJSON_GetErrorPtr();
        if (error_ptr != NULL) {
            free(buffer);
            printf("Error: unable to parse JSON data: %s\n", error_ptr);
        }
        return 1;
    }

    FILE *output_file = fopen("output.txt", "w");
    if (output_file == NULL) {
        free(buffer);
        cJSON_Delete(root);
        printf("Error: unable to create file output.txt\n");
        return 1;
    }

    cJSON *item = NULL;
    cJSON_ArrayForEach(item, root) {
        cJSON *install = cJSON_GetObjectItem(item, "install");
        if (install != NULL) {
            cJSON *doc = cJSON_GetObjectItem(install, "doc");
            if (doc != NULL) {
                cJSON *title = cJSON_GetObjectItem(doc, "title");
                if (cJSON_IsString(title) && (title->valuestring != NULL)) {
                    fprintf(output_file, "%s\n", title->valuestring);
                }
            }
        }
    }

    fclose(output_file);
    free(buffer);
    cJSON_Delete(root);
    printf("JSON parsing done!");

    return 0;
}
