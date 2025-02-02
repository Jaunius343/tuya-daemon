#include <string.h>

#include "tuya_log.h"
#include "tuyalink_core.h"
#include "tuya_error_code.h"

#include "tuya_handler_func.h"
#include "helper_functions.h"

void on_connected(tuya_mqtt_context_t *context, void *user_data)
{
	TY_LOGI("on connected");

	send_uptime(context);
}

void on_disconnect(tuya_mqtt_context_t *context, void *user_data)
{
	TY_LOGI("on disconnect");
}

void on_messages(tuya_mqtt_context_t *context, void *user_data, const tuyalink_message_t *msg)
{
	TY_LOGI("on message id:%s, type:%d, code:%d", msg->msgid, msg->type, msg->code);
	switch (msg->type) {
	case THING_TYPE_MODEL_RSP:
		TY_LOGI("Model data:%s", msg->data_string);
		break;

	case THING_TYPE_PROPERTY_SET:
		TY_LOGI("property set:%s", msg->data_string);
		break;

	case THING_TYPE_PROPERTY_REPORT_RSP:
		break;

	case THING_TYPE_ACTION_EXECUTE:
		TY_LOGI("action received:%s", msg->data_string);

		char result[30];
		int rtn = parse_action_code(msg->data_string, result);

		// error parsing or action not found
		if (rtn == 1) {
			TY_LOGI("action code not found");
			break;
		}
		if (strcmp(result, "update") == 0) {
			TY_LOGI("sending uptime");
			send_uptime(context);
		} else {
			TY_LOGI("unsupported action");
		}

		break;

	default:
		break;
	}
	printf("\r\n");
}