#include "api_common.h"
#include <uuid/uuid.h>

string RandomString(const int len) /*参数为字符串的长度*/
{
    /*初始化*/
    string str; /*声明用来保存随机字符串的str*/
    char c;     /*声明字符c，用来保存随机生成的字符*/
    int idx;    /*用来循环的变量*/
    /*循环向字符串中添加随机生成的字符*/
    for (idx = 0; idx < len; idx++) {
        /*rand()%26是取余，余数为0~25加上'a',就是字母a~z,详见asc码表*/
        c = 'a' + rand() % 26;
        str.push_back(c); /*push_back()是string类尾插函数。这里插入随机字符c*/
    }
    return str; /*返回生成的随机字符串*/
} 


std::string api_error_id_to_string(api_error_id input)
{
    switch (input)
    {
        case api_error_id::login_failed: return "LOGIN_FAILED";
        case api_error_id::username_exists: return "USERNAME_EXISTS";
        case api_error_id::email_exists: return "EMAIL_EXISTS";
        case api_error_id::bad_request:return "";
        default: return "BAD_REQUEST";
    }
}

std::string generateUUID() {
    uuid_t uuid;
    uuid_generate_time_safe(uuid);  //调用uuid的接口
 
    char uuidStr[40] = {0};
    uuid_unparse(uuid, uuidStr);     //调用uuid的接口
 
    return std::string(uuidStr);
}

int ApiSetCookie(string  email, string &cookie){
    // 获取redis连接池
    CacheManager *cache_manager = CacheManager::getInstance();
    CacheConn *cache_conn = cache_manager->GetCacheConn("token");  //
    AUTO_REL_CACHECONN(cache_manager, cache_conn);

    if(!cache_conn) {
        LOG_ERROR << "get cache conn failed";
        return -1;
    }
    
    cookie = generateUUID();    //需要唯一性
    string ret = cache_conn->SetEx(cookie, 86400, email);
    if(!ret.empty()) {
        return 0;
    } else {
        return -1;
    }
}

int GetUsernameAndUseridByEmail(string email, string &username, int32_t &userid) {
    int ret = 0;
    CDBManager *db_manager = CDBManager::getInstance();
    CDBConn *db_conn = db_manager->GetDBConn("chatroom_slave");
    AUTO_REL_DBCONN(db_manager, db_conn);   //析构时自动归还连接

   //获取用户id, 用户名
    string strSql = FormatString("select id, username from users where email='%s'", email.c_str());
    CResultSet *result_set = db_conn->ExecuteQuery(strSql.c_str());
    if (result_set && result_set->Next()) {  
        // 存在在返回
        username = result_set->GetString("username");
        userid = result_set->GetInt("id");
        
        LOG_INFO <<"username: " << username;
        ret = 0;
    } else {                        // 说明用户不存在
        ret = -1;
    }

    delete result_set;

    return ret;
}


int ApiGetUserInfoByCookie(string &username, int32_t &userid, string  &email, string cookie) {
    CacheManager *cache_manager = CacheManager::getInstance();
    CacheConn *cache_conn = cache_manager->GetCacheConn("token");
    AUTO_REL_CACHECONN(cache_manager, cache_conn);
    if (cache_conn) {
        email  = cache_conn->Get(cookie);    //校验cookie和用户名的关系
        LOG_INFO << "cookie: " << cookie << ", email: " << email;
        if(email.empty()) {
            return -1;
        } else {
            return GetUsernameAndUseridByEmail(email, username, userid);
        }
    } else {
        return -1;
    }
}