/*
 * SPDX-FileCopyrightText: 2025 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Unlicense OR CC0-1.0
 */

#include <stdio.h>
#include <string.h>
#include <errno.h>
#include <ctype.h>
#include <dirent.h>
#include <sys/stat.h>

#include <inttypes.h>
#include "esp_console.h"
#include "argtable3/argtable3.h"
#include "freertos/task.h"
#include "cmd_system.h"

static struct {
    struct arg_str *path;
    struct arg_lit *append;
    struct arg_end *end;
} tee_args;


static int cmd_tee(int argc, char **argv)
{
    int nerrors = arg_parse(argc, argv, (void **)&tee_args);
    if (nerrors != 0) {
        arg_print_errors(stderr, tee_args.end, argv[0]);
        return 1;
    }

    if (tee_args.path->count != 1) {
        fprintf(stderr, "Please provide a single file path\n");
        return 1;
    }

    const char *path = tee_args.path->sval[0];
    const char *mode = (tee_args.append->count > 0) ? "a" : "w";

    FILE *file = fopen(path, mode);
    if (!file) {
        fprintf(stderr, "tee: %s: %s\n", path, strerror(errno));
        return 1;
    }

    char buf[256];
    while (fgets(buf, sizeof(buf), stdin)) {
        // Write to stdout
        fputs(buf, stdout);
        fflush(stdout);

        // Write to file
        fputs(buf, file);
    }

    fclose(file);
    return 0;
}

void register_system_shell_tee(void) {
    tee_args.path = arg_str1(NULL, NULL, "<path>", "File to write to");
    tee_args.append = arg_lit0("a", "append", "Append to file instead of overwriting");
    tee_args.end = arg_end(2);
    const esp_console_cmd_t tee_cmd = {
        .command = "tee",
        .help = "Read from stdin and write to file and stdout",
        .hint = NULL,
        .func = &cmd_tee,
        .argtable = &tee_args,
    };
    ESP_ERROR_CHECK(esp_console_cmd_register(&tee_cmd));
}


static struct {
    struct arg_str *path;
    struct arg_end *end;
} cat_args;

int cmd_cat(int argc, char **argv)
{
    int nerrors = arg_parse(argc, argv, (void **)&cat_args);
    if (nerrors != 0) {
        arg_print_errors(stderr, cat_args.end, argv[0]);
        return 1;
    }

    if (cat_args.path->count != 1) {
        fprintf(stderr, "Please provide a single file path\n");
        return 1;
    }
    const char *path = cat_args.path->sval[0];

    FILE *file = fopen(path, "r");
    if (!file) {
        fprintf(stderr, "File %s not found\n", path);
        return 2;
    }

    while (true) {
        char buf[32];
        size_t bytes = fread(buf, 1, sizeof(buf), file);
        if (bytes < 1)
            break;
        fwrite(buf, 1, bytes, stdout);
    }

    if (file) {
        fclose(file);
    }

    return 0;
}

void register_system_shell_cat(void) {
    cat_args.path = arg_str1(NULL, NULL, "<path>", "File to cat");
    cat_args.end = arg_end(2);
    const esp_console_cmd_t cat_cmd = {.command = "cat", .help = "cat file", .hint = NULL, .func = &cmd_cat, .argtable = &cat_args};
    ESP_ERROR_CHECK(esp_console_cmd_register(&cat_cmd));
}

static struct {
    struct arg_str *pattern;
    struct arg_file *files;
    struct arg_lit *ignore_case;
    struct arg_lit *line_number;
    struct arg_lit *invert_match;
    struct arg_end *end;
} grep_args;

static int strstr_case_insensitive(const char *haystack, const char *needle)
{
    if (!*needle) {
        return 1; // empty needle matches
    }

    for (const char *h = haystack; *h; h++) {
        const char *n = needle;
        const char *h_temp = h;

        while (*h_temp && *n && (tolower((unsigned char)*h_temp) == tolower((unsigned char)*n))) {
            h_temp++;
            n++;
        }

        if (!*n) {
            return 1; // found match
        }
    }
    return 0; // no match
}

static int grep_line(const char *line, const char *pattern, bool ignore_case, bool invert_match)
{
    int match;
    if (ignore_case) {
        match = strstr_case_insensitive(line, pattern);
    } else {
        match = (strstr(line, pattern) != NULL);
    }

    if (invert_match) {
        return !match;
    }
    return match;
}

static int grep_file(FILE *f, const char *pattern, bool ignore_case, bool line_number, bool invert_match, const char *filename_prefix)
{
    char line[256];
    int line_num = 0;
    int match_count = 0;

    while (fgets(line, sizeof(line), f)) {
        line_num++;
        // Remove trailing newline
        size_t len = strlen(line);
        if (len > 0 && line[len - 1] == '\n') {
            line[len - 1] = '\0';
        }

        if (grep_line(line, pattern, ignore_case, invert_match)) {
            if (filename_prefix != NULL) {
                printf("%s:", filename_prefix);
            }
            if (line_number) {
                printf("%d:", line_num);
            }
            printf("%s\n", line);
            fflush(stdout);
            match_count++;
        }
    }
    if (ferror(f)) {
        // Don't report pipe errors (EPIPE) - that's normal ending when input is finished
        if (errno != EPIPE) {
            fprintf(stderr, "Error %d reading file\n", ferror(f));
            return 1;
        }
    }

    return (match_count > 0) ? 0 : 1;
}

static int cmd_grep(int argc, char **argv)
{
    int nerrors = arg_parse(argc, argv, (void **)&grep_args);
    if (nerrors != 0) {
        arg_print_errors(stderr, grep_args.end, argv[0]);
        return 1;
    }

    const char *pattern = grep_args.pattern->sval[0];
    bool ignore_case = grep_args.ignore_case->count > 0;
    bool line_number = grep_args.line_number->count > 0;
    bool invert_match = grep_args.invert_match->count > 0;

    // If no files specified, read from stdin
    if (grep_args.files->count == 0) {
        return grep_file(stdin, pattern, ignore_case, line_number, invert_match, NULL);
    }

    // Process files
    int ret = 0;
    bool show_filename = (grep_args.files->count > 1);

    for (int i = 0; i < grep_args.files->count; i++) {
        const char *filename = grep_args.files->filename[i];
        FILE *f = fopen(filename, "r");
        if (f == NULL) {
            fprintf(stderr, "grep: %s: No such file or directory\n", filename);
            ret = 1;
            continue;
        }

        int file_ret = grep_file(f, pattern, ignore_case, line_number, invert_match, show_filename ? filename : NULL);
        fclose(f);

        if (file_ret != 0) {
            ret = file_ret;
        }
    }

    return ret;
}

void register_system_shell_grep(void) {
    grep_args.pattern = arg_str1(NULL, NULL, "PATTERN", "Search pattern");
    grep_args.files = arg_filen(NULL, NULL, "FILE", 0, 10, "Files to search (stdin if omitted)");
    grep_args.ignore_case = arg_lit0("i", "ignore-case", "Case-insensitive search");
    grep_args.line_number = arg_lit0("n", "line-number", "Print line numbers");
    grep_args.invert_match = arg_lit0("v", "invert-match", "Select non-matching lines");
    grep_args.end = arg_end(2);

    const esp_console_cmd_t cmd = {
        .command = "grep",
        .help = "Search for PATTERN in files or stdin",
        .hint = NULL,
        .func = &cmd_grep,
        .argtable = &grep_args,
    };
    esp_console_cmd_register(&cmd);
}

static int cmd_echo(int argc, char **argv)
{
    for (int i = 1; i < argc; i++) {
        if (i == argc - 1)
            printf("%s\n", argv[i]);
        else
            printf("%s ", argv[i]);
    }
    return 0;
}

void register_system_shell_echo(void) {
    const esp_console_cmd_t cmd = {
        .command = "echo",
        .help = "Echo arguments to stdout",
        .hint = NULL,
        .func = &cmd_echo,
    };
    esp_console_cmd_register(&cmd);
}

static int cmd_true(int argc, char **argv) {
    return 0;
}

void register_system_shell_true(void) {
    const esp_console_cmd_t cmd = {
        .command = "true",
        .help = "Do nothing, successfully",
        .hint = NULL,
        .func = &cmd_true,
    };
    esp_console_cmd_register(&cmd);
}

static int cmd_false(int argc, char **argv) {
    return 1;
}

void register_system_shell_false(void) {
    const esp_console_cmd_t cmd = {
        .command = "false",
        .help = "Do nothing, unsuccessfully",
        .hint = NULL,
        .func = &cmd_false,
    };
    esp_console_cmd_register(&cmd);
}

static struct {
    struct arg_str *path;
    struct arg_end *end;
} ls_args;

int cmd_ls(int argc, char **argv)
{
    int nerrors = arg_parse(argc, argv, (void **)&ls_args);
    if (nerrors != 0) {
        arg_print_errors(stderr, ls_args.end, argv[0]);
        return 1;
    }

    const char *path = ls_args.path->sval[0];
    if (!path)
        return 1;

    DIR *dir = opendir(path);
    if (!dir) {
        printf("Directory %s not found\n", path);
        return 2;
    }
    while (true) {
        struct dirent *de = readdir(dir);
        if (!de) {
            break;
        }
        char file_path[512];
        struct stat file_stat = {};
        snprintf(file_path, sizeof(file_path), "%s/%s", path, de->d_name);
        stat(file_path, &file_stat);
        printf(" %s %s %ld\n", file_path, de->d_type == DT_REG ? "<file>" : "<dir>", file_stat.st_size);
    }
    closedir(dir);

    return 0;
}

void register_system_shell_ls(void) {
    ls_args.path = arg_str1(NULL, NULL, "<path>", "Path to list files in");
    ls_args.end = arg_end(2);
    const esp_console_cmd_t ls_cmd = {.command = "ls", .help = "list files", .hint = NULL, .func = &cmd_ls, .argtable = &ls_args};
    esp_console_cmd_register(&ls_cmd);
}
void register_system_shell_common(void) {

    register_system_shell_tee();
    register_system_shell_cat();
    register_system_shell_grep();
    register_system_shell_echo();
    register_system_shell_true();
    register_system_shell_false();
    register_system_shell_ls();
}
