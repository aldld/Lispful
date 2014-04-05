#include <stdio.h>
#include <stdlib.h>

#include <editline/readline.h>

#include "mpc.h"


long eval_op(long x, char* op, long y) {
	if (strcmp(op, "+") == 0) return x + y;
	if (strcmp(op, "-") == 0) return x - y;
	if (strcmp(op, "*") == 0) return x * y;
	if (strcmp(op, "/") == 0) return x / y;
	return 0;
}


long eval(mpc_ast_t* t) {
	// If tagged as number return it directly, otherwise expression.
	if (strstr(t->tag, "number"))
		return atoi(t->contents);

	// The operator is always the second child
	char* op = t->children[1]->contents;

	long x = eval(t->children[2]);

	// Iterate over remaining children, combining using the specified operator
	int i = 3;
	while (strstr(t->children[i]->tag, "expr")) {
		x = eval_op(x, op, eval(t->children[i]));
		i++;
	}

	return x;
}


int main(int argc, char** argv) {

	// Create parsers for Polish notation
	mpc_parser_t* Number   = mpc_new("number");
	mpc_parser_t* Operator = mpc_new("operator");
	mpc_parser_t* Expr     = mpc_new("expr");
	mpc_parser_t* Lispful  = mpc_new("lispful");

	// Define parsers as follows
	mpca_lang(MPC_LANG_DEFAULT,
		"                                                      \
			number   : /-?[0-9]+/ ;                            \
			operator : '+' | '-' | '*' | '/' ;                 \
			expr     : <number> | '(' <operator> <expr>+ ')' ; \
			lispful  : /^/ <operator> <expr>+ /$/ ;            \
		",
		Number, Operator, Expr, Lispful);

	// Print Version and Exit information
	puts("Lispful Version 0.0.0.0.1");
	puts("Press Ctrl+c to Exit\n");


	while (1) {
		char* input = readline("lispful> ");
		add_history(input);

		// Attempt to parse the user input
		mpc_result_t r;
		if (mpc_parse("<stdin>", input, Lispful, &r)) {
			/*
			// On success print the AST
			mpc_ast_print(r.output);
			mpc_ast_delete(r.output);
			*/

			long result = eval(r.output);
			printf("%li\n", result);
			mpc_ast_delete(r.output);
		} else {
			// Otherwise print the error
			mpc_err_print(r.error);
			mpc_err_delete(r.error);
		}

		free(input);
	}

	// Undefine and delete parsers
	mpc_cleanup(4, Number, Operator, Expr, Lispful);

	return 0;

}
