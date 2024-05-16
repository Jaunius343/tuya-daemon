#include <argp.h>

#ifndef ARGP_PARSER_H_INCLUDED
#define ARGP_PARSER_H_INCLUDED

/* Used by main to communicate with parse_opt. */
struct arguments {
	int silent, verbose, daemon;
	char output_file[128];
	char product_id[17];
	char device_id[23];
	char device_secret[17];
};

error_t parse_opt(int key, char *arg, struct argp_state *state);

#endif
