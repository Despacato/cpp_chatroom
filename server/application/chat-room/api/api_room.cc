#include "api_room.h"
#include <sstream>
bool ApiCreateRoom(const std::string& room_id, 
                        const std::string& room_name, 
                        int creator_id, 
                        std::string& error_msg)
{
    CDBManager* db_manager = CDBManager::getInstance();
    CDBConn* db_conn = db_manager->GetDBConn("chatroom_master");
    if (!db_conn) {
        error_msg = "无法获取数据库连接";
        return false;
    }
    AUTO_REL_DBCONN(db_manager, db_conn);
    std::stringstream ss;
    ss << "INSERT INTO room_info (room_id, room_name, creator_id) VALUES ('"
       << room_id << "', '"
       << room_name << "', "
       << creator_id << ")";
    
    if (!db_conn->ExecuteUpdate(ss.str().c_str(), true)) {
        error_msg = "创建聊天室失败";
        return false;
    }
    return true;
}

bool ApiGetRoomInfo(const std::string& room_id, 
                         std::string& room_name, 
                         int& creator_id,
                         std::string& create_time,
                         std::string& update_time,
                         std::string& error_msg)
 {
    CDBManager* db_manager = CDBManager::getInstance();
    CDBConn* db_conn = db_manager->GetDBConn("chatroom_slave");
    if (!db_conn) {
        error_msg = "无法获取数据库连接";
        return false;
    }
    AUTO_REL_DBCONN(db_manager, db_conn);

    
    std::stringstream ss;
    ss << "SELECT room_name, creator_id, create_time, update_time "
       << "FROM room_info WHERE room_id='" << room_id << "'";

    CResultSet* result_set = db_conn->ExecuteQuery(ss.str().c_str());
    if (!result_set) {
        error_msg = "查询聊天室信息失败";
        return false;
    }

    if (result_set->Next()) {
        room_name = result_set->GetString("room_name");
        creator_id = result_set->GetInt("creator_id");
        create_time = result_set->GetString("create_time");
        update_time = result_set->GetString("update_time");
        delete result_set;
        return true;
    }
    delete result_set;
    error_msg = "聊天室不存在";
    return false;
 }                        

bool ApiGetAllRooms(std::vector<Room>& rooms, 
                        std::string& error_msg,
                        const std::string& order_by)
{   
    CDBManager* db_manager = CDBManager::getInstance();
    CDBConn* db_conn = db_manager->GetDBConn("chatroom_slave");
    if (!db_conn) {
        error_msg = "无法获取数据库连接";
        return false;
    }
    AUTO_REL_DBCONN(db_manager, db_conn);

    
    std::stringstream ss;
    ss << "SELECT room_id, room_name, creator_id, create_time, update_time "
       << "FROM room_info ORDER BY " << order_by;

    CResultSet* result_set = db_conn->ExecuteQuery(ss.str().c_str());
    if (!result_set) {
        error_msg = "查询聊天室列表失败";
        return false;
    }

     while (result_set->Next()) {
        Room room;
        room.room_id = result_set->GetString("room_id");
        room.room_name = result_set->GetString("room_name");
        room.creator_id = result_set->GetInt("creator_id");
        room.create_time = result_set->GetString("create_time");
        room.update_time = result_set->GetString("update_time");
        rooms.push_back(room);
    }
    delete result_set;

    return true;
}