#include "tuyalink_core.h"
#include "core_json.h"
#include "argp_parser.h"

#ifndef HELPER_FUNCTIONS_H_INCLUDED
#define HELPER_FUNCTIONS_H_INCLUDED

long int get_system_uptime();
void send_uptime(tuya_mqtt_context_t *context);
int parse_action_code(char *string, char *dest);
int handle_user_input(struct arguments arguments, char *product_id, size_t p_size, char *device_id,
		      size_t d_size, char *device_secret, size_t s_size);

#endif