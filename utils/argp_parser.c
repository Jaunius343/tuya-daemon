#include <argp.h>
#include <string.h>

#include "argp_parser.h"

const char *argp_program_version     = "argp-ex3 1.0";
const char *argp_program_bug_address = "<bug-gnu-utils@gnu.org>";

/* Program documentation. */
char doc[] = "Options: ";

/* A description of the arguments we accept. */
char args_doc[] = "-P product_id  -D device_id  -S device_secret";

/* The options we understand. */
struct argp_option options[] = { { "verbose", 'v', 0, 0, "Produce verbose output" },
				 { "quiet", 'q', 0, 0, "Don't produce any output" },
				 { "silent", 's', 0, OPTION_ALIAS },
				 { "output", 'o', "FILE", 0, "Output to FILE instead of standard output" },
				 { "product id", 'P', "ID", 0, "variable for product ID" },
				 { "device id", 'D', "ID", 0, "variable for device ID" },
				 { "device secret", 'S', "ID", 0, "variable for device secret" },
				 { "daemon", 'd', 0, 0, "launch as daemon" },
				 { 0 } };

struct argp argp = { options, parse_opt, args_doc, doc };

/* Parse a single option. */
error_t parse_opt(int key, char *arg, struct argp_state *state)
{
	/* Get the input argument from argp_parse, which we
     know is a pointer to our arguments structure. */
	struct arguments *arguments = state->input;

	switch (key) {
	case 'q':
	case 's':
		arguments->silent = 1;
		break;

	case 'v':
		arguments->verbose = 1;
		break;

	case 'o':
		strncpy(arguments->output_file, arg, sizeof(arguments->output_file) - 1);
		break;

	case 'd':
		arguments->daemon = 1;
		break;

	case 'D':
		strncpy(arguments->device_id, arg, sizeof(arguments->device_id) - 1);
		break;

	case 'P':
		strncpy(arguments->product_id, arg, sizeof(arguments->product_id) - 1);
		break;

	case 'S':
		strncpy(arguments->device_secret, arg, sizeof(arguments->device_secret) - 1);
		break;

	// override default "too many arguments" handling.
	// Without this passing positional arguments gets memory allocated
	// and argp exits the program with memory leak.
	case ARGP_KEY_ARG:
		if (state->arg_num >= 0)
			;
		break;

	default:
		return ARGP_ERR_UNKNOWN;
	}
	return 0;
}
