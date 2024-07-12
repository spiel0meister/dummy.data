#include <stdio.h>
#include <string.h>

char word[1024] = {0};

void begin_header(FILE* sink, const char* guard_name) {
    fprintf(sink, "#ifndef %s\n", guard_name);
    fprintf(sink, "#define %s\n\n", guard_name);
}

void end_header(FILE* sink, const char* guard_name) {
    fprintf(sink, "#endif // %s\n", guard_name);
}

int main(void) {
    const char* guard = "MOST_COMMON_ENGLISH_WORDS_H_";

    FILE* f = fopen("most_common_english_words.txt", "rb");
    size_t word_count = 0;
    begin_header(stdout, guard);
    printf("const char* en_words[] = {\n");
    while (fgets(word, 1024, f) != NULL) {
        *strchr(word, '\n') = 0;
        printf("\"%s\", ", word);
        if (word_count % 30 == 0) {
            printf("\n");
        }
        word_count++;
    }
    printf("};\n");
    printf("#define EN_WORDS_LEN (sizeof(words)/sizeof(words[0]))\n\n");
    end_header(stdout, guard);

    fclose(f);
    return 0;
}
