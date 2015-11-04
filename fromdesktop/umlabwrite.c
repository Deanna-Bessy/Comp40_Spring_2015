#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "assert.h"
#include <fmt.h>
#include "seq.h"

extern void Um_write_sequence(FILE *output, Seq_T asm);

extern void emit_halt_test(Seq_T asm);
extern void emit_goto_test(Seq_T asm);
extern void emit_cond_move_zero_test(Seq_T asm);
extern void emit_cond_move_nonzero_test(Seq_T stream);
extern void emit_read_put_char_test(Seq_T asm);
extern void emit_print_digit(Seq_T asm);
extern void emit_nand_print_digit(Seq_T asm);

static struct test_info {
        const char *name;
        const char *test_input;            /* NULL means no input needed */
        const char *expected_output;
        /* writes instructions into sequence */
        void (*emit_test) (Seq_T stream);
} tests[] = {
        {"halt", NULL, "", emit_halt_test},
        {"goto", NULL, "GOTO passed.\n", emit_goto_test},
	{"cond_move_zero_test", NULL, "Conditional move on zero register passed.\n",
		emit_cond_move_zero_test},
	{"cond_move_nonzero_test", NULL, "Conditional move on nonzero register passed\n",
		emit_cond_move_nonzero_test},
	{"read_put_char", "a", "a", emit_read_put_char_test},
	{"print_digit", "5", "5", emit_print_digit},
	{"nand_print_digit", "2", "2", emit_nand_print_digit}
};

#define NTESTS (sizeof(tests)/sizeof(tests[0]))
/*
 * open file 'path' for writing, then free the pathname;
 * if anything fails, checked runtime error
 */
static FILE *open_and_free_pathname(char *path);

/*
 * if contents is NULL or empty, remove the given 'path', 
 * otherwise write 'contents' into 'path'.  Either way, free 'path'.
 */
static void write_or_remove_file(char *path, const char *contents);

static void write_test_files(struct test_info *test)
{
	char *s = malloc(512);
	sprintf(s, "%s.um", test->name);
        //FILE *binary = open_and_free_pathname(Fmt_string("%s.um", test->name));
        FILE *binary = open_and_free_pathname(s);
        Seq_T asm = Seq_new(0);
        test->emit_test(asm);
        Um_write_sequence(binary, asm);
        Seq_free(&asm);
        fclose(binary);


	s = malloc(512);
	sprintf(s, "%s.0", test->name);
        //write_or_remove_file(Fmt_string("%s.0", test->name), test->test_input);
        write_or_remove_file(s, test->test_input);

	s = malloc(512);
	sprintf(s, "%s.1", test->name);
        //write_or_remove_file(Fmt_string("%s.1", test->name),
        //                     test->expected_output);
        write_or_remove_file(s, test->expected_output);
}

int main(int argc, char *argv[])
{
        bool failed = false;
        if (argc == 1)
                for (unsigned i = 0; i < NTESTS; i++) {
                        printf("***** Writing test '%s'.\n", tests[i].name);
                        write_test_files(&tests[i]);
        } else
                for (int j = 1; j < argc; j++) {
                        bool tested = false;
                        for (unsigned i = 0; i < NTESTS; i++)
                                if (!strcmp(tests[i].name, argv[j])) {
                                        tested = true;
                                        write_test_files(&tests[i]);
                                }
                        if (!tested) {
                                failed = true;
                                fprintf(stderr,
                                        "***** No test named %s *****\n",
                                        argv[j]);
                        }
                }
        return failed;   /* failed nonzero == exit nonzero == failure */
}

static void write_or_remove_file(char *path, const char *contents)
{
        if (contents == NULL || *contents == '\0') {
                remove(path);
        } else {
                FILE *input = fopen(path, "wb");
                assert(input);
                fputs(contents, input);
                fclose(input);
        }
        free(path);
}

static FILE *open_and_free_pathname(char *path)
{
        FILE *fp = fopen(path, "wb");
        assert(fp != NULL);

        free(path);
        return fp;
}
