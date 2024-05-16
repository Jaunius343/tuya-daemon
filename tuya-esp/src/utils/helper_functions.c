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

int parse_action_pin(char *response, char **port, int *pin)
{
	// Parse JSON string
	json_object *json = json_tokener_parse(response);
	if (json == NULL) {
		TY_LOGE("Error: Failed to parse JSON");
		return -1;
	}

	json_object *input_params_obj;
	if (!json_object_object_get_ex(json, "inputParams", &input_params_obj)) {
		TY_LOGE("Error: 'inputParams' key not found");
		json_object_put(json);
		return -1;
	}

	json_object *port_obj;
	if (!json_object_object_get_ex(input_params_obj, "port", &port_obj)) {
		TY_LOGE("Error: 'port' key not found");
		json_object_put(json);
		return -1;
	}

	*port = strdup(json_object_get_string(port_obj));

	json_object *pin_obj;
	if (!json_object_object_get_ex(input_params_obj, "pin", &pin_obj)) {
		TY_LOGE("Error: 'pin' key not found");
		json_object_put(json);
		return -1;
	}

	*pin = json_object_get_int(pin_obj);

	json_object_put(json);

	TY_LOGV("port: \'%s\', pin: %d", *port, *pin);

	return 0;
}

void handle_pin(tuya_mqtt_context_t *context, struct ubus_context *ctx, const tuyalink_message_t *msg,
		enum PIN_STATUS key)
{
	char *data = msg->data_string;
	char *port;
	int pin;
	char *response = NULL;
	int rc;

	rc = parse_action_pin(data, &port, &pin);
	if (rc != 0) {
		return;
	}

	switch (key) {
	case PIN_ON:
		TY_LOGV("send pin on");
		send_pin(ctx, &response, port, pin, PIN_STR[key]);

		if (response != NULL) {
			tuyalink_thing_property_report(context, NULL, response);
			TY_LOGI("response sent");
		}
		free(response);
		break;

	case PIN_OFF:
		TY_LOGV("send pin off");
		send_pin(ctx, &response, port, pin, PIN_STR[key]);

		if (response != NULL) {
			tuyalink_thing_property_report(context, NULL, response);
			TY_LOGI("response sent");
		}
		free(response);
		break;

	default:
		TY_LOGI("unsupported pin action");
		break;
	}

	free(port);
}

void handle_devices(tuya_mqtt_context_t *context, struct ubus_context *ctx, const tuyalink_message_t *msg)
{
	char *data     = msg->data_string;
	char *response = NULL;

	get_devices(ctx, &response);

	if (response != NULL) {
		tuyalink_thing_property_report(context, NULL, response);
		TY_LOGI("response sent");
	}
	free(response);
}