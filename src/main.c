#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <argp.h>

#include <signal.h>

#include "tuya_cacert.h"
#include "tuya_log.h"
#include "tuya_error_code.h"
#include "tuyalink_core.h"
#include "helper_functions.h"
#include "argp_parser.h"
#include "become_daemon.h"
#include "tuya_handler_func.h"

// MUST BE BELOW "tuya_log.h" to avoid type conflicts.
#include <syslog.h>

//-----------------------------------------------------------------------------
//								Signal handler
//-----------------------------------------------------------------------------

int flag = 1;

void sig_handler(int signum)
{
	flag = 0;
	// syslog(LOG_USER | LOG_INFO, "closing");
	// signal(SIGTERM, SIG_DFL); // Re Register signal handler for default action
	// exit(signum);
}

//-----------------------------------------------------------------------------
//									Arg parser
//-----------------------------------------------------------------------------

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

//-----------------------------------------------------------------------------
//								Tuya SDK
//-----------------------------------------------------------------------------

// tuya_mqtt_context_t client_instance;

int main(int argc, char **argv)
{
	signal(SIGTERM, sig_handler);
	signal(SIGINT, sig_handler);

	struct arguments arguments;

	/* Default values. */
	arguments.silent  = 0;
	arguments.verbose = 0;
	arguments.daemon  = 0;

	char product_id[17];
	char device_id[23];
	char device_secret[17];

	tuya_mqtt_context_t client_instance;
	tuya_mqtt_context_t *client = &client_instance;

	int ret;

	/* Parse our arguments; every option seen by parse_opt will
     be reflected in arguments. */
	argp_parse(&argp, argc, argv, 0, 0, &arguments);

	ret = handle_user_input(arguments, product_id, sizeof(product_id), device_id, sizeof(device_id),
				device_secret, sizeof(device_secret));

	if (ret == 1) {
		printf("at least one of the required parameters is incorrect\n");
		printf("see \'tuya_daemon --help\'\n");
		return EXIT_FAILURE;
	}

	if (arguments.daemon) {
		printf("trying to run as daemon\n");
		const char *LOGNAME = "DAEMON_EXAMPLE";

		// turn this process into a daemon
		ret = become_daemon(0);
		if (ret) {
			syslog(LOG_USER | LOG_ERR, "error starting");
			closelog();
			return EXIT_FAILURE;
		}

		// we are now a daemon!
		// printf now will go to /dev/null

		// open up the system log
		openlog(LOGNAME, LOG_PID, LOG_USER);
		syslog(LOG_USER | LOG_INFO, "starting");
	}

	ret = tuya_mqtt_init(client, &(const tuya_mqtt_config_t){ .host		 = "m1.tuyacn.com",
								  .port		 = 8883,
								  .cacert	 = tuya_cacert_pem,
								  .cacert_len	 = sizeof(tuya_cacert_pem),
								  .device_id	 = device_id,
								  .device_secret = device_secret,
								  .keepalive	 = 100,
								  .timeout_ms	 = 2000,
								  .on_connected	 = on_connected,
								  .on_disconnect = on_disconnect,
								  .on_messages	 = on_messages });

	if (ret != OPRT_OK)
		flag = 0;

	ret = tuya_mqtt_connect(client);
	if (ret != OPRT_OK)
		flag = 0;

	while (flag) {
		/* Loop to receive packets, and handles client keepalive */
		tuya_mqtt_loop(client);
	}

	tuya_mqtt_disconnect(client);
	tuya_mqtt_deinit(client);

	syslog(LOG_USER | LOG_INFO, "closing");

	// return ret;

	return EXIT_SUCCESS;
}