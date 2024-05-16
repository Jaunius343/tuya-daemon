#include <libubox/blobmsg_json.h>
#include <libubus.h>
#include <tuya_log.h>

#include "ubus_functions.h"

void ubus_cb(struct ubus_request *req, int type, struct blob_attr *msg)
{
	char **response = (char **)req->priv;

	// Convert to plain JSON
	char *json_str = blobmsg_format_json(msg, true);
	if (json_str) {
		log_debug("ESP payload: \'%s\'", json_str);
		// free(json_str);
	}

	*response = json_str;
	return;
}

void send_pin(struct ubus_context *ctx, char **response, char *port, int pin, const char *pin_status)
{
	struct blob_buf b = {};
	uint32_t id;

	blob_buf_init(&b, 0);
	blobmsg_add_string(&b, "port", port);
	blobmsg_add_u32(&b, "pin", pin);

	int rc, rc2;
	rc  = ubus_lookup_id(ctx, "esp", &id);
	rc2 = ubus_invoke(ctx, id, pin_status, b.head, ubus_cb, response, 5000);
	if (rc || rc2) {
		TY_LOGE("ubus_lookup_id(): %d, %s", rc, ubus_strerror(rc));
		TY_LOGE("ubus_invoke(): %d, %s", rc2, ubus_strerror(rc2));
		log_error("can't request data from ubus");
		blob_buf_free(&b);
		return;
	} else {
		// log_info("port: %s", device.port);
	}

	blob_buf_free(&b);
}

void get_devices(struct ubus_context *ctx, char **response)
{
	uint32_t id;

	int rc, rc2;
	rc  = ubus_lookup_id(ctx, "esp", &id);
	rc2 = ubus_invoke(ctx, id, "devices", NULL, ubus_cb, response, 5000);
	if (rc || rc2) {
		TY_LOGE("ubus_lookup_id(): %d, %s", rc, ubus_strerror(rc));
		TY_LOGE("ubus_invoke(): %d, %s", rc2, ubus_strerror(rc2));
		log_error("can't request data from ubus");
		return;
	} else {
		// log_info("port: %s", device.port);
	}
}