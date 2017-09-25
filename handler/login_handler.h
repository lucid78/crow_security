#ifndef LOGIN_HANDLER_H
#define LOGIN_HANDLER_H

#pragma once

#include "crow.h"
#include "db/db_handler.h"

#include "structures/user_info.h"
#include "auth/crypt_handler.h"
#include "resources.h"

using namespace std;
using db = DB_HANDLER;
using ch = CRYPT_HANDLER;
using param_type = unordered_map<string, string>;
using res = RESOURCES;


const string get_time()
{
    std::time_t rawtime;
    std::time(&rawtime);
    std::tm* timeinfo = std::localtime(&rawtime);

    char buffer[80] = {0};
    std::strftime(buffer, 80, "%Y-%m-%d %H:%M:%S", timeinfo);
    return string(buffer);
}

bool get_user(const string& username, const string& password)
{
    const USER_INFO& user_info = db::get().get_user(username, password);
    if(user_info.get_username().empty())
    {
        return false;
    }
    return true;
}

void login(const crow::request /*&req*/, crow::response &res)
{
    res.write(crow::mustache::load("view/html/login.html").render());
    res.end();
}

void logout(const crow::request /*&req*/, crow::response &res)
{
    res.code = 302;
    res.add_header("Location", "logout");
    res.end();
}

void login_proc(const crow::request /*&req*/, crow::response &res)
{
    cout << "redirect to dashboard" << endl;
    res.code = 302;
    res.add_header("Location", "/");
    res.end();
}



#endif // LOGIN_HANDLER_H
