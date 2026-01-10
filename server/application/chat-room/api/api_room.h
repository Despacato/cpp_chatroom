#ifndef API_ROOM_H_
#define API_ROOM_H_

#include <string>
#include <vector>
#include <ctime>
#include "db_pool.h"
#include "api_types.h"


bool ApiCreateRoom(const std::string& room_id, 
                        const std::string& room_name, 
                        int creator_id, 
                        std::string& error_msg);

bool ApiGetRoomInfo(const std::string& room_id, 
                        std::string& room_name, 
                        int& creator_id,
                        std::string& create_time,
                        std::string& update_time,
                        std::string& error_msg);

// 新增：获取所有聊天室列表
bool ApiGetAllRooms(std::vector<Room>& rooms, 
                        std::string& error_msg,
                        const std::string& order_by = "update_time DESC");

 
#endif // API_ROOM_H_
