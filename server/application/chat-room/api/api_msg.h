#ifndef __CHAT_ROOM_API_API_MSG_H__
#define __CHAT_ROOM_API_API_MSG_H__
#include "api_common.h"
#include "api_types.h"
const constexpr int k_message_batch_size = 30;  //聊天室登录后最多拉取的消息数量,这里并不是翻页的


int ApiGetRoomHistory(Room &room, MessageBatch &message_batch, const int msg_count = k_message_batch_size);
int ApiStoreMessage(string room_id, std::vector<Message>  &msgs);


#endif