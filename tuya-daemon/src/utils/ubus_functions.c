#include <libubox/blobmsg_json.h>
#include <libubus.h>
#include <tuya_log.h>

#include "ubus_functions.h"

void board_cb(struct ubus_request *req, int type, struct blob_attr *msg)
{
	struct MemData *memoryData = (struct MemData *)req->priv;
	struct blob_attr *tb[__INFO_MAX];
	struct blob_attr *memory[__MEMORY_MAX];

	blobmsg_parse(info_policy, __INFO_MAX, tb, blob_data(msg), blob_len(msg));

	if (!tb[MEMORY_DATA]) {
		TY_LOGE("No memory data received from UBUS");
		return;
	}

	blobmsg_parse(memory_policy, __MEMORY_MAX, memory, blobmsg_data(tb[MEMORY_DATA]),
		      blobmsg_data_len(tb[MEMORY_DATA]));

	memoryData->total    = blobmsg_get_u64(memory[TOTAL_MEMORY]);
	memoryData->free     = blobmsg_get_u64(memory[FREE_MEMORY]);
	memoryData->shared   = blobmsg_get_u64(memory[SHARED_MEMORY]);
	memoryData->buffered = blobmsg_get_u64(memory[BUFFERED_MEMORY]);
}

int read_memory(struct MemData *memory, struct ubus_context *ctx)
{
	uint32_t id;

	if (ubus_lookup_id(ctx, "system", &id) ||
	    ubus_invoke(ctx, id, "info", NULL, board_cb, memory, 3000)) {
		return -1;
	}
	TY_LOGI("Memory received from UBUS");
	return 0;
}