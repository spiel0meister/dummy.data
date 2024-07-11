#include <stdio.h>
#include <string.h>

char password_buf[1024] = {0};

int main(void) {
    FILE* password_file = fopen("10000_most_common_passwords.txt", "rb");
    size_t password_count = 0;
    printf("#ifndef MOST_COMMON_PASSWORDS_H_\n");
    printf("#define MOST_COMMON_PASSWORDS_H_\n\n");
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
    printf("#endif // MOST_COMMON_PASSWORDS_H_");
    fclose(password_file);
    return 0;
}
