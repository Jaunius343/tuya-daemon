/* This program uses the same features as example 2, and uses options and
   arguments.

   We now use the first four fields in ARGP, so here’s a description of them:
     OPTIONS  – A pointer to a vector of struct argp_option (see below)
     PARSER   – A function to parse a single option, called by argp
     ARGS_DOC – A string describing how the non-option arguments should look
     DOC      – A descriptive string about this program; if it contains a
                 vertical tab character (\v), the part after it will be
                 printed *following* the options

   The function PARSER takes the following arguments:
     KEY  – An integer specifying which option this is (taken
             from the KEY field in each struct argp_option), or
             a special key specifying something else; the only
             special keys we use here are ARGP_KEY_ARG, meaning
             a non-option argument, and ARGP_KEY_END, meaning
             that all arguments have been parsed
     ARG  – For an option KEY, the string value of its
             argument, or NULL if it has none
     STATE– A pointer to a struct argp_state, containing
             various useful information about the parsing state; used here
             are the INPUT field, which reflects the INPUT argument to
             argp_parse, and the ARG_NUM field, which is the number of the
             current non-option argument being parsed
   It should return either 0, meaning success, ARGP_ERR_UNKNOWN, meaning the
   given KEY wasn’t recognized, or an errno value indicating some other
   error.

   Note that in this example, main uses a structure to communicate with the
   parse_opt function, a pointer to which it passes in the INPUT argument to
   argp_parse.  Of course, it’s also possible to use global variables
   instead, but this is somewhat more flexible.

   The OPTIONS field contains a pointer to a vector of struct argp_option’s;
   that structure has the following fields (if you assign your option
   structures using array initialization like this example, unspecified
   fields will be defaulted to 0, and need not be specified):
     NAME   – The name of this option’s long option (may be zero)
     KEY    – The KEY to pass to the PARSER function when parsing this option,
               *and* the name of this option’s short option, if it is a
               printable ascii character
     ARG    – The name of this option’s argument, if any
     FLAGS  – Flags describing this option; some of them are:
                 OPTION_ARG_OPTIONAL – The argument to this option is optional
                 OPTION_ALIAS        – This option is an alias for the
                                        previous option
                 OPTION_HIDDEN       – Don’t show this option in –help output
     DOC    – A documentation string for this option, shown in –help output

   An options vector should be terminated by an option with all fields zero. */

// #include <stdlib.h>
#include <argp.h>

#include "argp_parser.h"

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
		arguments->output_file = arg;
		break;
	case 'd':
		arguments->daemon = 1;
		break;
	case 'D':
		arguments->device_id = arg;
		break;
	case 'P':
		arguments->product_id = arg;
		break;
	case 'S':
		arguments->device_secret = arg;
		break;

		// case ARGP_KEY_ARG:
		// 	if (state->arg_num >= 3)
		// 		/* Too many arguments. */
		// 		argp_usage(state);

		// 	arguments->args[state->arg_num] = arg;

		// 	break;

		// case ARGP_KEY_END:
		// 	if (state->arg_num < 3)
		// 		/* Not enough arguments. */
		// 		argp_usage(state);
		// 	break;

	default:
		return ARGP_ERR_UNKNOWN;
	}
	return 0;
}
