#include "api_msg.h"
 

int ApiGetRoomHistory(Room &room, MessageBatch &message_batch, const int msg_count)
{
    CacheManager *cache_manager = CacheManager::getInstance();
    CacheConn *cache_conn = cache_manager->GetCacheConn("msg");
    AUTO_REL_CACHECONN(cache_manager, cache_conn);

    std::string stream_ref = "+";
    if(!room.history_last_message_id.empty()) {
        stream_ref = "(" + room.history_last_message_id;
    }
    LOG_DEBUG << "stream_ref: " << stream_ref << ", msg_count:" << msg_count << ", room_id:" << room.room_id;
    // 消息id - 消息本身
    std::vector<std::pair<string, string>> msgs; 
    bool res = cache_conn->GetXrevrange(room.room_id, stream_ref, "-", msg_count, msgs);
    if(res) {
        for(int i = 0; i < msgs.size(); i++) {
            Message msg;
            #if 1  //性能测试时，可以不解析消息
            msg.id = msgs[i].first;   //这里保存的是消息id
            room.history_last_message_id =  msg.id;  // 保存最后一个消息的id
            bool res;
            Json::Value root;
            Json::Reader jsonReader;
            LOG_DEBUG << "msg: " << msgs[i].second;
            // {"content":"11111111","timestamp":1737193931885,"user_id":6}
            res = jsonReader.parse( msgs[i].second, root);
            if (!res) {
                LOG_ERROR << "parse redis msg failed ";
                return -1;
            }
            if (root["content"].isNull()) {
                LOG_ERROR << "content null";
                return -1;
            }
            msg.content = root["content"].asString();

            if (root["user_id"].isNull()) {
                LOG_ERROR << "user_id null";
                return -1;
            }
            msg.user_id = root["user_id"].asInt64();

            if (root["username"].isNull()) {
                LOG_ERROR << "username null";
                return -1;
            }
            msg.username = root["username"].asString();

            if (root["timestamp"].isNull()) {
                LOG_ERROR << "timestamp null";
                return -1;
            }
            msg.timestamp = root["timestamp"].asUInt64();
            #endif
            message_batch.messages.push_back(msg);
        }
        if(msgs.size() < msg_count)
            message_batch.has_more = false;     //读取到消息数量 比要求的少了，所以判定为没有更多数据可以读取了
        else
            message_batch.has_more = true;
        return 0;
    } else {
        return -1;
    }
}
string SerializeMessageToJson(const Message msg) {
    Json::Value root;
    root["content"] = msg.content;
    root["timestamp"] =(Json::UInt64) msg.timestamp;
    root["user_id"] =(Json::UInt64) msg.user_id;
    root["username"] = msg.username;
    Json::FastWriter writer;
    return writer.write(root);
    // return root.toStyledString();
}

int ApiStoreMessage(string room_id, std::vector<Message>  &msgs)
{
    //写入redis
    CacheManager *cache_manager = CacheManager::getInstance();
    CacheConn *cache_conn = cache_manager->GetCacheConn("msg");
    AUTO_REL_CACHECONN(cache_manager, cache_conn);

    for(int i = 0; i < msgs.size(); i++) {
        string json_msg = SerializeMessageToJson(msgs[i]);
        std::vector<std::pair<string, string>>  field_value_pairs;
        field_value_pairs.push_back(std::make_pair("payload", json_msg));
        LOG_DEBUG << "room_id:" << room_id << ", payload: " << json_msg;
        string id = "*";
        bool ret = cache_conn->Xadd(room_id, id, field_value_pairs);
        if(!ret) {
            LOG_ERROR << "Xadd failed";
            return -1;
        }
        // LOG_INFO << "room_id:" << room_id << ", msg.id:" << id << ", json_msg:" << json_msg;
        msgs[i].id = id;
    }
    return 0;
}