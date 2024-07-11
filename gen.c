#include <stdio.h>
#include <string.h>

char password_buf[1024] = {0};

void begin_header(FILE* sink, const char* guard_name) {
    fprintf(sink, "#ifndef %s\n", guard_name);
    fprintf(sink, "#define %s\n\n", guard_name);
}

void end_header(FILE* sink, const char* guard_name) {
    fprintf(sink, "#endif // %s\n", guard_name);
}

int main(void) {
    const char* guard = "MOST_COMMON_PASSWORDS_H_";

    FILE* password_file = fopen("10000_most_common_passwords.txt", "rb");
    size_t password_count = 0;
    begin_header(stdout, guard);
    printf("const char* passwords[] = {\n");
    while (fgets(password_buf, 1024, password_file) != NULL) {
        *strchr(password_buf, '\n') = 0;
        printf("\"%s\", ", password_buf);
        if (password_count % 10 == 0) {
            printf("\n");
        }
        password_count++;
    }
    printf("};\n");
    printf("#define PASSOWORDS_LEN (sizeof(passwords)/sizeof(passwords[0]))\n\n");
    end_header(stdout, guard);

    fclose(password_file);
    return 0;
}
