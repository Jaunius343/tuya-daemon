#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <argp.h>
#include <signal.h>
#include <libubox/blobmsg_json.h>
#include <libubus.h>
#include <syslog.h>

// Tuya SDK
#include <tuya_error_code.h>
#include <tuyalink_core.h>

// Utils
#include "tuya_cacert.h"
#include "helper_functions.h"
#include "argp_parser.h"
#include "become_daemon.h"
#include "tuya_handler_func.h"
#include "ubus_functions.h"

//-----------------------------------------------------------------------------
//								Signal handler
//-----------------------------------------------------------------------------

int flag = 1;

void sig_handler(int signum)
{
	flag = 0;
}

int main(int argc, char **argv)
{
	signal(SIGTERM, sig_handler);
	signal(SIGINT, sig_handler);

	int ret;
	const char *LOGNAME = "TUYA_DAEMON";
	struct ubus_context *ctx;
	struct arguments arguments;
	tuya_mqtt_context_t client_instance;
	tuya_mqtt_context_t *client = &client_instance;

	/* Default values. */
	arguments.silent  = 0;
	arguments.verbose = 0;
	arguments.daemon  = 0;

	ctx = ubus_connect(NULL);
	if (!ctx) {
		printf("Failed to connect to ubus\n");
		return EXIT_FAILURE;
	}

	/* Parse our arguments; every option seen by parse_opt will
     be reflected in arguments. */
	argp_parse(&argp, argc, argv, 0, 0, &arguments);

	ret = check_user_input(arguments);

	if (ret == 1) {
		printf("at least one of the required parameters is incorrect\n");
		printf("see \'tuya_daemon --help\'\n");
		return EXIT_FAILURE;
	}

	if (arguments.daemon) {
		printf("trying to run as daemon\n");

		// turn this process into a daemon
		ret = become_daemon(0);
		if (ret) {
			syslog(LOG_USER | LOG_ERR, "error starting Tuya program as daemon");
			closelog();
			return EXIT_FAILURE;
		}

		// printf now will go to /dev/null

		openlog(LOGNAME, LOG_PID, LOG_USER);
		syslog(LOG_USER | LOG_INFO, "started Tuya program as daemon");
	}

	ret = tuya_mqtt_init(client, &(const tuya_mqtt_config_t){ .host		 = "m1.tuyacn.com",
								  .port		 = 8883,
								  .cacert	 = tuya_cacert_pem,
								  .cacert_len	 = sizeof(tuya_cacert_pem),
								  .device_id	 = arguments.device_id,
								  .device_secret = arguments.device_secret,
								  .keepalive	 = 100,
								  .timeout_ms	 = 2000,
								  .on_messages	 = on_messages,
								  .user_data	 = ctx });

	if (ret != OPRT_OK)
		flag = 0;
	if (arguments.daemon)
		syslog(LOG_USER | LOG_INFO, "Tuya program initialised");

	ret = tuya_mqtt_connect(client);
	if (ret != OPRT_OK)
		flag = 0;
	if (arguments.daemon)
		syslog(LOG_USER | LOG_INFO, "Tuya program connected to Tuya IoT cloud");

	while (flag) {
		/* Loop to receive packets, and handles client keepalive */
		tuya_mqtt_loop(client);
	}

	tuya_mqtt_disconnect(client);
	tuya_mqtt_deinit(client);

	ubus_free(ctx);

	if (arguments.daemon) {
		syslog(LOG_USER | LOG_INFO, "closing Tuya program");
		closelog();
	}

	return EXIT_SUCCESS;
}