#ifndef MIDDLEWARE_H
#define MIDDLEWARE_H

#pragma once

#include <string>
#include <unordered_map>
#include <vector>


#include "crow.h"
#include "session.h"

using namespace std;

namespace SECURITY {

    const string LOGIN_PATH = "/login";
    const string DASHBOARD_PATH = "/";
    const string COOKIE_ID = "fortify_id";
    const string LOGIN_ID = "id";
    const string LOGIN_PWD = "password";
    const double SESSION_TIME_SECOND = 3600;

    unordered_map<string, bool> ignoreUrlMap;
    unordered_map<string, bool> adminUrlMap;
    unordered_map<string, bool> adminGroupMap;

    vector<string> ignore_url_array { "/css/bootstrap.css",
                                      "/images/index.png",
                                      "/script/jquery-1.12.4.min.js",
                                      "/login", "/login_proc", "/logout" };

    vector<string> admin_url_array { "/admin" };
    vector<string> admin_group_array { "admin" };

    struct SessionManager
    {
        SessionManager()
        {
            for (const string& s : ignore_url_array)
            {
                ignoreUrlMap.emplace(move(s), true);
            }

            for (const string& s : admin_url_array)
            {
                adminUrlMap.emplace(move(s), true);
            }

            for (const string&s : admin_group_array)
            {
                adminGroupMap.emplace(move(s), true);
            }
        }

        struct context {};

        void before_handle(crow::request& req, crow::response& res, context& /*ctx*/)
        {
            if (ignoreUrlMap[req.url])
            {
                return;
            }

            // get session from request
            const unordered_map<string, string> reqest_cookie_map = get_session_from_cookie(req.get_header_value("cookie"));
            if (reqest_cookie_map.find(COOKIE_ID) == reqest_cookie_map.end() || reqest_cookie_map.at(COOKIE_ID) == "")
            {
                res.code = 302;
                res.set_header("Set-Cookie", "");
                res.add_header("Location", LOGIN_PATH);
                res.end();
                return;
            }

            // get encryption session
            const string& request_enc_session = reqest_cookie_map.at(COOKIE_ID);

            // decryption session
            const string request_plain_session = Auth::decrypt_session(request_enc_session);

            // get id, session from decrypted request session
            const unordered_map<string, string> request_params = get_value_from_session(request_plain_session);
            const string& request_id = request_params.at(LOGIN_ID);
            const string& request_session = request_params.at(COOKIE_ID);

            // check admin accessable url
            if (adminUrlMap[req.url])
            {
                if (adminGroupMap.find(request_id) == adminGroupMap.end())
                {
                    res.code = 302;
                    res.add_header("Location", DASHBOARD_PATH);
                    res.end();
                    return;
                }
            }

            // check id's session
            if (!Auth::find_user(request_id))
            {
                res.code = 302;
                res.set_header("Set-Cookie", "");
                res.add_header("Location", LOGIN_PATH);
                res.end();
                return;
            }

            // get saved session
            const string& saved_enc_session = Auth::get_session(request_id);

            // decryption saved session
            const string& saved_plain_session = Auth::decrypt_session(saved_enc_session);

            // get id, session from decrypted saved session
            const unordered_map<string, string> saved_params = get_value_from_session(saved_plain_session);
            const string& saved_id = saved_params.at(LOGIN_ID);
            const string& saved_session = saved_params.at(COOKIE_ID);

            // check id, session
            if (saved_id.compare(request_id) != 0 && saved_session.compare(request_session) != 0)
            {
                Auth::remove_session(saved_id);

                res.code = 302;
                res.set_header("Set-Cookie", "");
                res.add_header("Location", LOGIN_PATH);
                res.end();
                return;
            }

            // check session time in one hour
            const double& create_time = Auth::get_session_second(saved_id);
            const double& current_time = Auth::get_current_time_seconds();
            const double runtime = current_time - create_time;
            if (runtime > SESSION_TIME_SECOND)
            {
                Auth::remove_session(saved_id);

                res.code = 302;
                res.set_header("Set-Cookie", "");
                res.add_header("Location", LOGIN_PATH);
                res.end();
                return;
            }
        }

        void after_handle(crow::request& /*req*/, crow::response& res, context& /*ctx*/)
        {
            res.set_header("X-Frame-Options", "DENY");
            res.set_header("X-XSS-Protection", "1");
            res.set_header("mode", "block");
            res.set_header("Cache-Control", "no-cache");
            res.set_header("Cache-Control", "no-store");
            res.set_header("Pragma", "no-cache");
        }

        unordered_map<string, string> get_value_from_session(const string& plain_req_session)
        {
            unordered_map<string, string> params;
            vector<string> v;
            stringstream iss(plain_req_session);
            string keyval, key, val;
            while(getline(iss, keyval, ';'))
            {
                stringstream iss(keyval);
                if (getline(getline(iss, key, '|'), val))
                {
                    params.emplace(move(COOKIE_ID), move(key));
                    params.emplace(move(LOGIN_ID), move(val));
                }
            }
            return params;
        }

        unordered_map<string, string> get_session_from_cookie(const string& cookie)
        {
            unordered_map<string, string> map;
            istringstream iss(cookie);
            string key, val;
            if (getline(getline(iss, key, '='), val))
            {
                map.emplace(move(key), move(val));
            }
            else
            {
                map.emplace(move(key), move(""));
            }

            return map;
        }

    };  // end of SessionManager

};

#endif // MIDDLEWARE_H
