#include <string.h>
#include <stdio.h>
#include <sys/sysinfo.h>
#include <libubus.h>

// Tuya SDK
// https://github.com/FreeRTOS/coreJSON
#include <core_json.h>
#include <tuyalink_core.h>
#include <tuya_log.h>

// Utils
#include "helper_functions.h"
#include "argp_parser.h"
#include "ubus_functions.h"

// Function to get system uptime in seconds
long int get_system_uptime()
{
	struct sysinfo info;

	if (sysinfo(&info) != 0) {
		return 0;
	}

	return info.uptime;
}

void send_uptime(tuya_mqtt_context_t *context)
{
	char json_string[50];

	long int value = get_system_uptime();
	if (value == 0) {
		TY_LOGE("Getting system uptime failed");
		return;
	}
	snprintf(json_string, sizeof(json_string), "{\"uptime\":{\"value\":\"%ld\"}}", value);

	tuyalink_thing_property_report(context, NULL, json_string);
}

void send_free_memory(tuya_mqtt_context_t *context, struct ubus_context *ctx)
{
	char json_string[50];
	struct MemData memory = { 0 };
	int ret;
	ret = read_memory(&memory, ctx);

	if (!ret) {
		TY_LOGE("Reading memory from UBUS failed");
		return;
	}

	snprintf(json_string, sizeof(json_string), "{\"memory\":{\"value\":%ld}}", memory.free);

	tuyalink_thing_property_report(context, NULL, json_string);
}

int parse_action_code(char *string, char *dest, size_t dest_s)
{
	JSONStatus_t result;
	int len = strlen(string);
	char buffer[len];
	size_t bufferLength   = sizeof(buffer) - 1;
	char queryKey[]	      = "actionCode";
	size_t queryKeyLength = sizeof(queryKey) - 1;
	char *value;
	size_t valueLength;

	// creating a copy to preserve original string in case modifications happen here
	strcpy(buffer, string);

	result = JSON_Search(buffer, bufferLength, queryKey, queryKeyLength, &value, &valueLength);

	if (result == JSONSuccess) {
		// The pointer "value" will point to a location in the "buffer".
		char save = value[valueLength];
		// After saving the character, set it to a null byte for printing.
		value[valueLength] = '\0';

		strncpy(dest, value, dest_s - 1);

		value[valueLength] = save;

		return 0;
	}
	return 1;
}

int check_user_input(struct arguments arguments)
{
	if (strlen(arguments.product_id) < (sizeof(arguments.product_id) - 1) ||
	    strlen(arguments.device_id) < (sizeof(arguments.device_id) - 1) ||
	    strlen(arguments.device_secret) < (sizeof(arguments.device_secret) - 1)) {
		return 1;
	}

	return 0;
}