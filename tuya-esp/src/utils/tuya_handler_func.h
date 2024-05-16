#ifndef TUYA_HANDLER_FUNC_H_INCLUDED
#define TUYA_HANDLER_FUNC_H_INCLUDED

void on_connected(tuya_mqtt_context_t *context, void *user_data);
void on_disconnect(tuya_mqtt_context_t *context, void *user_data);
void on_messages(tuya_mqtt_context_t *context, void *user_data, const tuyalink_message_t *msg);

#endif