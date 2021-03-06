/*
 * This file is a part of Jing.
 *
 * Copyright (c) 2015 The Jing Authors.
 *
 * For the full copyright and license information, please view the LICENSE file
 * that was distributed with this source code.
 */

#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <string.h>
#include <unistd.h>

#include "jing.h"
#include "strbuf.h"

/* AST root node */
struct node *
ntop = NULL;

static char *
infile = NULL;

static char
outfile[256] = { 0 };

static int
parse_opt(int argc, char **argv);

static void
usage(void);

/*
 * Display error with line number.
 */
void
yyerror(const char *s, ...)
{
    va_list ap;

    ++yynerrs;

    va_start(ap, s);
    fprintf(stderr, "%s:%d: error: ", infile, yylineno);
    vfprintf(stderr, s, ap);
    fprintf(stderr, "\n");
    va_end(ap);
}

/*
 * Display error with location information.
 */
void
yyerrorl(YYLTYPE t, const char *s, ...)
{
    va_list ap;

    ++yynerrs;

    va_start(ap, s);
    if (t.first_line) {
        fprintf(stderr, "%s:%d:%d %d:%d error: ", infile, t.first_line,
                t.first_column, t.last_line, t.last_column);
    }
    vfprintf(stderr, s, ap);
    fprintf(stderr, "\n");
    va_end(ap);
}

/*
 * Destroy AST and symbol table.
 */
void
jing_destroy(void)
{
    node_free(ntop);
    symtab_free();
    emitter_free();
}

/*
 * Exit with error status
 */
int
error_exit(void)
{
    jing_destroy();
    exit(EXIT_FAILURE);
}

int
main(int argc, char **argv)
{
    char *ptr;
    FILE *fp_out = stdout;

    parse_opt(argc, argv);

    if (!(yyin = fopen(infile, "r"))) {
        perror("fopen");
        exit(EXIT_FAILURE);
    }

    ptr = strrchr(infile, '/');
    if (ptr) {
        infile = ++ptr;
    }

    emitter_init(strbuf_new());

    yyparse();
    /* must be called before `yylex_destroy` */
    fclose(yyin);
    yylex_destroy();

    if (yynerrs) {
        error_exit();
    }

    if ('\0' != outfile[0]) {
        if (!(fp_out = fopen(outfile, "w"))) {
            fprintf(stderr, "error: %s ", outfile);
            perror(NULL);
            error_exit();
        }
    }

    fprintf(fp_out, "%s", emitter_get_str());
    jing_destroy();

    return 0;
}

/*
 * Parse commandline options.
 */
static int
parse_opt(int argc, char **argv)
{
    int ch;

    if (argc < 2) {
        usage();
        exit(EXIT_FAILURE);
    }

    while ((ch = getopt(argc, argv, "ho:v")) != -1) {
        switch (ch) {
        case 'h':
            usage();
            exit(EXIT_SUCCESS);
            break;
        case 'o':
            strncpy(outfile, optarg, 255);
            break;
        case 'v':
            printf("jing2indigo %s\n", JING_VERSION);
            exit(EXIT_SUCCESS);
            break;
        case '?':
        default:
            usage();
            exit(EXIT_FAILURE);
        }
    }

    argc -= optind;
    /* check infile is specified */
    if (argc < 1) {
        usage();
        exit(EXIT_FAILURE);
    }
    infile = argv[optind];

    return optind;
}

/*
 * Display program usage.
 */
static void
usage(void)
{
    fprintf(stderr, "Usage: jing2indigo [OPTIONS] file.jing\n");
    fprintf(stderr, "\nOptions:\n"
            "  -h           Display this message\n"
            "  -o <file>    Write output to file\n"
            "  -v           Display version and exit\n\n");
}
