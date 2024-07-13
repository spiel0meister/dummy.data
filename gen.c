#include <stdio.h>
#include <string.h>
#include <stdbool.h>
#include <errno.h>

typedef struct {
    const char* txt_path;
    const char* header_path;

    const char* guard;
    const char* arr_var_name;
    const char* arr_len_var_name;
}ToGenerate;

void begin_header(FILE* sink, const char* guard_name) {
    fprintf(sink, "#ifndef %s\n", guard_name);
    fprintf(sink, "#define %s\n", guard_name);
}

void end_header(FILE* sink, const char* guard_name) {
    fprintf(sink, "#endif // %s\n", guard_name);
}

static char item[1024] = {0};
bool generate(ToGenerate gen, size_t items_per_line) {
    FILE* sink = fopen(gen.header_path, "wb");
    if (sink == NULL) {
        fprintf(stderr, "Couldn't open %s: %s\n", gen.header_path, strerror(errno));
        return false;
    }

    begin_header(sink, gen.guard);
    fprintf(sink, "#include <stddef.h>\n\n");

    FILE* f = fopen(gen.txt_path, "rb");
    if (f == NULL) {
        fprintf(stderr, "Couldn't open %s: %s\n", gen.txt_path, strerror(errno));
        return false;
    }

    size_t item_count = 0;
    fprintf(sink, "const char* %s[] = {\n    ", gen.arr_var_name);
    while (fgets(item, 1024, f) != NULL) {
        *strchr(item, '\n') = 0;
        fprintf(sink, "\"%s\", ", item);
        if (item_count != 0 && item_count % items_per_line == 0) {
            fprintf(sink, "\n    ");
        }
        item_count++;
    }
    fprintf(sink, "\n};\n");
    fprintf(sink, "const size_t %s = sizeof(%s)/sizeof((%s)[0]);\n\n", gen.arr_len_var_name, gen.arr_var_name, gen.arr_var_name);

    fclose(f);
    end_header(sink, gen.guard);
    fclose(sink);

    return true;
}

int main(void) {
    ToGenerate gens[] = {
        (ToGenerate) {
            .txt_path = "most_common_english_words.txt",
            .header_path = "most_common_english_words.h",
            .guard = "MOST_COMMON_ENGLISH_WORDS_H_",
            .arr_var_name = "en_words",
            .arr_len_var_name = "en_words_len"
        },
        (ToGenerate) {
            .txt_path = "10000_most_common_passwords.txt",
            .header_path = "10000_most_common_passwords.h",
            .guard = "MOST_COMMON_PASSWORDS_H_",
            .arr_var_name = "passwords",
            .arr_len_var_name = "passwords_len"
        },
        (ToGenerate) {
            .txt_path = "100_most_common_names_for_each_gender.txt",
            .header_path = "100_most_common_names_for_each_gender.h",
            .guard = "MOST_COMMON_NAMES_H_",
            .arr_var_name = "names",
            .arr_len_var_name = "names_len"
        },
    };

    for (size_t i = 0; i < sizeof(gens)/sizeof(gens[0]); ++i) {
        if (!generate(gens[i], 30)) {
            fprintf(stderr, "Couldn't generate %s from %s\n", gens[i].header_path, gens[i].txt_path);
        } else {
            printf("Generated %s from %s\n", gens[i].header_path, gens[i].txt_path);
        }
    }

    return 0;
}
