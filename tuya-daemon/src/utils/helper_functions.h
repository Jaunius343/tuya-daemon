#include <tuyalink_core.h>

#include "argp_parser.h"

#ifndef HELPER_FUNCTIONS_H_INCLUDED
#define HELPER_FUNCTIONS_H_INCLUDED

long int get_system_uptime();
void send_uptime(tuya_mqtt_context_t *context);
void send_free_memory(tuya_mqtt_context_t *context, struct ubus_context *ctx);
int parse_action_code(char *string, char *dest, size_t dest_s);
int check_user_input(struct arguments arguments);

#endif