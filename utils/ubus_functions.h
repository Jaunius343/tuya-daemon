#include <libubox/blobmsg_json.h>
#include <libubus.h>

#ifndef UBUS_FUNCTIONS_H_INCLUDED
#define UBUS_FUNCTIONS_H_INCLUDED

void ubus_cb(struct ubus_request *req, int type, struct blob_attr *msg);
void send_pin(struct ubus_context *ctx, char **response, char *port, int pin, const char *pin_status);
void get_devices(struct ubus_context *ctx, char **response);

#endif