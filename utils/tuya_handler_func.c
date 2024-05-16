#include <string.h>
#include <libubus.h>

// Tuya SDK
#include <tuya_log.h>
#include <tuyalink_core.h>
#include <tuya_error_code.h>

// Utils
#include "tuya_handler_func.h"
#include "helper_functions.h"
#include "ubus_functions.h"

void on_messages(tuya_mqtt_context_t *context, void *user_data, const tuyalink_message_t *msg)
{
	TY_LOGI("on message id:%s, type:%d, code:%d", msg->msgid, msg->type, msg->code);
	switch (msg->type) {
	case THING_TYPE_ACTION_EXECUTE:
		TY_LOGI("action received:%s", msg->data_string);

		char result[30];
		int rtn = parse_action_code(msg->data_string, result, sizeof(result));

		// error parsing or action not found
		if (rtn == 1) {
			TY_LOGI("action code not found");
			break;
		}

		if (strcmp(result, "pin_on") == 0) {
			TY_LOGI("turning pin on");
			struct ubus_context *ubus_ctx = user_data;
			handle_pin(context, ubus_ctx, msg, PIN_ON);

		} else if (strcmp(result, "pin_off") == 0) {
			TY_LOGI("turning pin off");
			struct ubus_context *ubus_ctx = user_data;
			handle_pin(context, ubus_ctx, msg, PIN_OFF);

		} else if (strcmp(result, "devices") == 0) {
			TY_LOGI("getting devices");
			struct ubus_context *ubus_ctx = user_data;
			handle_devices(context, ubus_ctx, msg);

		} else {
			TY_LOGI("unsupported action");
		}

		break;

	default:
		break;
	}
	printf("\r\n");
}