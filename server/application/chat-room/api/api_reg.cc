#include "api_reg.h"





int decodeRegisterJson(const std::string &str_json, string &username,
                       string &email, string &password)
{   
    bool res;
    Json::Value root;
    Json::Reader jsonReader;
    res = jsonReader.parse(str_json, root);    
    if (!res) {
        LOG_ERROR << "parse reg json failed ";
        return -1;
    }
    // 用户名
    if (root["username"].isNull()) {
        LOG_ERROR << "username null";
        return -1;
    }
    username = root["username"].asString();

   //邮箱  
    if (root["email"].isNull()) {
        LOG_WARN << "email null";
    } else {
        email = root["email"].asString();
    }

    //密码
    if (root["password"].isNull()) {
        LOG_ERROR << "password null";
        return -1;
    }
    password = root["password"].asString();

    return 0;
}

void encdoeRegisterJson(api_error_id input, string message, string &str_json) {
  
    Json::Value root;
    root["id"] = api_error_id_to_string(input);
    root["message"] = message;
    Json::FastWriter writer;
    str_json = writer.write(root);
}

int registerUser(string &username, string &email, string &password, api_error_id &error_id) {
    int ret = -1;
    CDBManager *db_manager = CDBManager::getInstance();
    CDBConn *db_conn = db_manager->GetDBConn("chatroom_master");
    AUTO_REL_DBCONN(db_manager, db_conn);
    if(!db_conn) {
        LOG_ERROR << "GetDBConn(chatroom_master) failed" ;
        return 1;
    }
    // 先查询 用户名  邮箱是否存在 如果存在就报错
    string str_sql = FormatString("select id, username, email from users where username='%s' or email='%s' ", username.c_str(), email.c_str());

    CResultSet *result_set = db_conn->ExecuteQuery(str_sql.c_str());
    if(result_set && result_set->Next()) {
        if(result_set->GetString("username")) {
            if(result_set->GetString("username") == username)  {
                error_id = api_error_id::username_exists;
                LOG_WARN << "id: " << result_set->GetInt("id") << ", username: " <<  username <<  "  已经存在";
            }
           
        }

        if(result_set->GetString("email")) {
            if(result_set->GetString("email") == email) {
                error_id = api_error_id::email_exists;
                LOG_WARN << "id: " << result_set->GetInt("id") << ", email: " <<  email <<  "  已经存在";
            }
        }
        delete result_set;
        return -1;
    }

    //注册账号
    // 随机数生成盐值
    string salt = RandomString(16);  //随机混淆码, 混淆码对字符串唯一性要求没有那么高
    MD5 md5(password+salt);
    string password_hash = md5.toString();
    LOG_INFO << "salt: " << salt;

    //插入语句
    str_sql = "insert into users  (`username`,`email`,`password_hash`,`salt`) values(?,?,?,?)";
    LOG_INFO << "执行: " <<  str_sql;
    // 预处理方式写入数据
    CPrepareStatement *stmt = new CPrepareStatement();
    if (stmt->Init(db_conn->GetMysql(), str_sql)) {
        uint32_t index = 0;
        stmt->SetParam(index++, username);
        stmt->SetParam(index++, email);
        stmt->SetParam(index++, password_hash);
        stmt->SetParam(index++, salt);
        bool bRet = stmt->ExecuteUpdate(); //真正提交要写入的数据
        if (bRet) {     //提交正常返回 true
            ret = 0;
            LOG_INFO << "insert user_id: " <<  db_conn->GetInsertId() <<  ", username: " <<  username ;
        } else {
            LOG_ERROR << "insert users failed. " <<  str_sql;
            ret = 1;
        }
    }
    delete stmt;


    return ret;
}

int ApiRegisterUser(std::string &post_data, std::string &resp_data){
    string username;
    string email;
    string password;
    // json 
    //解析 json username email password
    //json反序列化
    int ret = decodeRegisterJson(post_data, username, email, password);
    if(ret < 0) {
        encdoeRegisterJson(api_error_id::bad_request, "请求参数不全", resp_data);
        return -1;
    }


    // 封装registerUser（username email password）
    api_error_id error_id = api_error_id::bad_request;
    ret = registerUser(username, email, password, error_id);
    //返回注册结果
    if(ret == 0) {
        // 注册成功 是需要产生cookie
        ApiSetCookie(email, resp_data);
    } else {
        encdoeRegisterJson(error_id, api_error_id_to_string(error_id), resp_data);
    }


    //正常注册  返回cookie

    //异常 {}   400 Bad Request  {"id":"USERNAME_EXISTS","message":""}   EMAIL_EXISTS
    return ret;

}