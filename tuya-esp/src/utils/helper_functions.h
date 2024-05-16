#include <tuyalink_core.h>
#include <json-c/json.h>

#include "argp_parser.h"

#ifndef HELPER_FUNCTIONS_H_INCLUDED
#define HELPER_FUNCTIONS_H_INCLUDED

enum PIN_STATUS {
	PIN_ON,
	PIN_OFF,
};

static const char *PIN_STR[] = { "on", "off" };

int parse_action_code(char *string, char *dest, size_t dest_s);
int check_user_input(struct arguments arguments);
void handle_pin(tuya_mqtt_context_t *context, struct ubus_context *ctx, const tuyalink_message_t *msg,
		enum PIN_STATUS key);
void handle_devices(tuya_mqtt_context_t *context, struct ubus_context *ctx, const tuyalink_message_t *msg);

#endif