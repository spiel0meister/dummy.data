#include <stdio.h>
#include <string.h>
#include <stdbool.h>
#include <assert.h>
#include <errno.h>

// For some reason it's not in string.h
char* strdup(const char* s);

typedef struct {
    const char* txt_path;
    const char* header_path;
    const char* json_path;

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

#define TMP_BUF_SIZE (1 << 12)
char tmp_buf[TMP_BUF_SIZE] = {0};
size_t tmp_buf_size = 0;

char* tmp_end() {
    return tmp_buf + tmp_buf_size;
}

char* tmp_alloc(size_t size) {
    assert(tmp_buf_size + size < TMP_BUF_SIZE);

    char* s = tmp_end();
    tmp_buf_size += size;

    return s;
}

char* tmp_strdup(const char* s) {
    int n = strlen(s);
    char* copy = tmp_alloc(n + 1);
    memcpy(copy, s, n + 1);
    return copy;
}

#define tmp_reset() tmp_buf_size = 0

ToGenerate create_to_gen(const char* txt_path, const char* guard, const char* arr_name, const char* arr_len_name) {
    char* dot = strchr(txt_path, '.');
    assert(dot != NULL);

    char header_path[1024] = {0};
    sprintf(header_path, "%.*s.h", (int)(dot - txt_path), txt_path);

    char json_path[1024] = {0};
    sprintf(json_path, "%.*s.json", (int)(dot - txt_path), txt_path);

    return (ToGenerate) {
        .txt_path = txt_path,
        .header_path = tmp_strdup(header_path),
        .json_path = tmp_strdup(json_path),
        .guard = guard,
        .arr_var_name = arr_name,
        .arr_len_var_name = arr_len_name
    };
}

static char item[1024] = {0};
bool generate(ToGenerate gen, size_t items_per_line) {
    FILE* header = fopen(gen.header_path, "w");
    if (header == NULL) {
        fprintf(stderr, "Couldn't open %s: %s\n", gen.header_path, strerror(errno));
        return false;
    }

    FILE* json = fopen(gen.json_path, "w");
    if (json == NULL) {
        fprintf(stderr, "Couldn't open %s: %s\n", gen.header_path, strerror(errno));
        return false;
    }
    fprintf(json, "[\n");

    begin_header(header, gen.guard);
    fprintf(header, "#include <stddef.h>\n\n");

    FILE* f = fopen(gen.txt_path, "rb");
    if (f == NULL) {
        fprintf(stderr, "Couldn't open %s: %s\n", gen.txt_path, strerror(errno));
        return false;
    }

    size_t item_count = 0;
    fprintf(header, "const char* %s[] = {\n    ", gen.arr_var_name);
    while (fgets(item, 1024, f) != NULL) {
        *strchr(item, '\n') = 0;
        if (item_count != 0) {
            fprintf(json, ", \n");
        }
        fprintf(header, "\"%s\", ", item);
        fprintf(json, "    \"%s\"", item);
        if (item_count != 0 && item_count % items_per_line == 0) {
            fprintf(header, "\n    ");
        }
        item_count++;
    }
    fclose(f);

    fprintf(header, "\n};\n");
    fprintf(header, "const size_t %s = sizeof(%s)/sizeof((%s)[0]);\n\n", gen.arr_len_var_name, gen.arr_var_name, gen.arr_var_name);
    end_header(header, gen.guard);

    fprintf(json, "\n]\n");

    fclose(header);
    fclose(json);

    return true;
}

int main(void) {
    ToGenerate gens[] = {
        create_to_gen("most_common_english_words.txt", "MOST_COMMON_ENGLISH_WORDS_H_", "en_words", "en_words_len"),
        create_to_gen("10000_most_common_passwords.txt", "MOST_COMMON_PASSWORDS_H_", "passwords", "passwords_len"),
        create_to_gen("100_most_common_names_for_each_gender.txt", "MOST_COMMON_NAMES_H_", "names", "names_len"),
        create_to_gen("fruits.txt", "FRUITS_H_", "fruits", "fruits_len"),
        create_to_gen("vegetables.txt", "VEGETABLES_H_", "vegetables", "vegetables_len"),
    };

    for (size_t i = 0; i < sizeof(gens)/sizeof(gens[0]); ++i) {
        if (!generate(gens[i], 30)) {
            fprintf(stderr, "Couldn't generate %s from %s\n", gens[i].header_path, gens[i].txt_path);
            fprintf(stderr, "Couldn't generate %s from %s\n", gens[i].json_path, gens[i].txt_path);
        } else {
            printf("Generated %s from %s\n", gens[i].header_path, gens[i].txt_path);
            printf("Generated %s from %s\n", gens[i].json_path, gens[i].txt_path);
        }
    }

    return 0;
}
