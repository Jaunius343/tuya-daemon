#include <string.h>
#include <stdio.h>
#include <sys/sysinfo.h>

// https://github.com/FreeRTOS/coreJSON
#include "core_json.h"
#include "tuyalink_core.h"
#include "helper_functions.h"
#include "argp_parser.h"

// Function to get system uptime in seconds
long int get_system_uptime()
{
	struct sysinfo info;

	if (sysinfo(&info) != 0) {
		perror("sysinfo");
		return 0;
	}

	return info.uptime;
}

void send_uptime(tuya_mqtt_context_t *context)
{
	char json_string[50];

	long int value = get_system_uptime();
	snprintf(json_string, sizeof(json_string), "{\"uptime\":{\"value\":\"%ld\"}}", value);

	tuyalink_thing_property_report(context, NULL, json_string);
}

int parse_action_code(char *string, char *dest)
{
	JSONStatus_t result;
	int len = strlen(string);
	char buffer[len];
	size_t bufferLength   = sizeof(buffer) - 1;
	char queryKey[]	      = "actionCode";
	size_t queryKeyLength = sizeof(queryKey) - 1;
	char *value;
	size_t valueLength;

	strcpy(buffer, string);

	result = JSON_Search(buffer, bufferLength, queryKey, queryKeyLength, &value, &valueLength);

	if (result == JSONSuccess) {
		// The pointer "value" will point to a location in the "buffer".
		char save = value[valueLength];
		// After saving the character, set it to a null byte for printing.
		value[valueLength] = '\0';

		strncpy(dest, value, sizeof(dest) - 1);

		// Restore the original character.
		value[valueLength] = save;

		return 0;
	}
	return 1;
}

int handle_user_input(struct arguments arguments, char *product_id, size_t p_size, char *device_id,
		      size_t d_size, char *device_secret, size_t s_size)
{
	if (arguments.product_id == NULL || arguments.device_id == NULL || arguments.device_secret == NULL) {
		return 1;
	}

	strncpy(product_id, arguments.product_id, p_size - 1);
	product_id[p_size - 1] = '\0';

	strncpy(device_id, arguments.device_id, d_size - 1);
	device_id[d_size - 1] = '\0';

	strncpy(device_secret, arguments.device_secret, s_size - 1);
	device_secret[s_size - 1] = '\0';

	if (strlen(product_id) < (p_size - 1) || strlen(device_id) < (d_size - 1) ||
	    strlen(device_secret) < (s_size - 1)) {
		return 1;
	}

	return 0;
}