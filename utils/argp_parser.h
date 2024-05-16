

#include <argp.h>

#ifndef ARGP_PARSER_H_INCLUDED
#define ARGP_PARSER_H_INCLUDED

/* Used by main to communicate with parse_opt. */
struct arguments {
	// char *args[3];
	int silent, verbose, daemon;
	char *output_file;
	char *product_id;
	char *device_id;
	char *device_secret;
};

error_t parse_opt(int key, char *arg, struct argp_state *state);

#endif
