#ifndef __CHAT_ROOM_API_API_COMMON_H__
#define __CHAT_ROOM_API_API_COMMON_H__
#include "muduo/base/Logging.h"
#include "muduo/base/md5.h"

#include <json/json.h>
#include <memory>

#include "db_pool.h"
#include "cache_pool.h"


using std::string;
enum class api_error_id
{
    bad_request = 0,
    login_failed,
    email_exists,
    username_exists,
};


template <typename... Args>
std::string FormatString(const std::string &format, Args... args) {
    auto size = std::snprintf(nullptr, 0, format.c_str(), args...) +
                1; // Extra space for '\0'
    std::unique_ptr<char[]> buf(new char[size]);
    std::snprintf(buf.get(), size, format.c_str(), args...);
    return std::string(buf.get(), buf.get() + size - 1); // We don't want the '\0' inside
}
 
string RandomString(const int len); /*参数为字符串的长度*/
 
int ApiSetCookie(string  email, string &cookie);
std::string api_error_id_to_string(api_error_id input);

int ApiGetUserInfoByCookie(string &username, int32_t &userid, string  &email, string cookie);
string generateUUID();
#endif