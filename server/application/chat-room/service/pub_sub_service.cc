#include "pub_sub_service.h"

// static std::vector<Room> s_room_list = {
//     {"0001", "程序员老廖2", 1, "", "", ""},
//     {"0002", "码农mark2", 2, "", "", ""},
//     {"0003", "程序员yt2", 3, "", "", ""},
//     {"0004", "老周讲golang2", 4, "", "", ""},
//     {"0005", "绝顶哥编程vico2", 5, "", "", ""}
// };

static std::vector<Room> s_room_list = {
    {"0001", "房间1", 1, "", "", ""},
    {"0002", "房间2", 2, "", "", ""},
    {"0003", "房间3", 3, "", "", ""},
    {"0004", "房间4", 4, "", "", ""},
    {"0005", "房间5", 5, "", "", ""},
    {"0006", "房间6", 6, "", "", ""},
    {"0007", "房间7", 7, "", "", ""},
    {"0008", "房间8", 8, "", "", ""}};

static std::mutex s_metux_room_list;

std::vector<Room> &PubSubService::GetRoomList()
{
    // 加锁
    std::lock_guard<std::mutex> lock(s_metux_room_list);
    return s_room_list;
}

int PubSubService::AddRoom(const Room &room)
{
    std::lock_guard<std::mutex> lock(s_metux_room_list);
    // 判断是否存在
    for (const auto &r : s_room_list)
    {
        if (r.room_id == room.room_id)
        {
            LOG_WARN << "room already exists";
            return -1;
        }
    }
    s_room_list.push_back(room);

    return 0;
}
