#ifndef AUTH_HANDLER_H
#define AUTH_HANDLER_H


#pragma once

#include <boost/algorithm/string/trim.hpp>

#include "crow/http_request.h"
#include "crow/http_response.h"
#include "crypt_handler.h"
#include "resources.h"
#include "structures/user_info.h"

using namespace std;
using res = RESOURCES;

using ch = CRYPT_HANDLER;
using param_type = unordered_map<string, string>;
using url_type = unordered_map<string, bool>;

namespace session
{
    class SessionManager
    {
    private:
        url_type ignore_urls;
        url_type admin_urls;

        const vector<string> ignore_url_array { "/css/bootstrap.css",
                                                "/script/jquery-1.12.4.min.js",
                                                "/login",
                                                "/logout"
                                              };

        // for admin only access urls
        vector<string> admin_url_array { "/admin" };

    public:
        SessionManager()
        {
            for(const string& s : ignore_url_array)
            {
                ignore_urls.emplace(s, true);
            }

            for (const string& s : admin_url_array)
            {
                admin_urls.emplace(move(s), true);
            }
        }

        struct context{};

        void before_handle(crow::request& req, crow::response& res, context& /*ctx*/)
        {
            if (ignore_urls[req.url])
            {
                return;
            }

            // if try to login -> check id/password
            if (!req.url.compare("/login_proc"))
            {
                const param_type& requests = ch::get().get_login_info(req.body);
                const string& login_id = requests.at(res::get().get_field_id());
                const string& login_pwd = requests.at(res::get().get_field_pwd());

                if (login_id.empty() && login_pwd.empty())
                {
                    cerr << "id or password is null" << endl;
                    res.code = 302;
                    res.add_header("Location", res::get().get_login_path());
                    res.end();
                    return;
                }

                if(!get_user(login_id, login_pwd))
                {
                    cerr << "id or password is not valid, no user" << endl;
                    res.code = 302;
                    res.add_header("Location", res::get().get_login_path());
                    res.end();
                    return;
                }

                const string& session = ch::get().create_session(login_id);

                // cookie
                const auto& authCTX = (crow::CookieParser::context *)req.middleware_context;
                authCTX->set_cookie(res::get().get_cookie_id(), session);

                // store to db
                db::get().init_session(login_id, login_pwd, session, get_time());
                return;
            }

            const string& session = get_session(req.get_header_value("Cookie"));
            if(session.empty())
            {
                res.code = 302;
                res.set_header("Set-Cookie", "");
                res.add_header("Location", res::get().get_login_path());
                res.end();
                return;
            }

            // get user
            const USER_INFO User = db::get().get_user(session);

            // get id from request session
            const param_type& requests = get_value(ch::get().decrypt(session));
            const string& id = requests.at(res::get().get_field_id());

            // compare id in request session and id in stroed session
            if (User.get_username().compare(id) != 0)
            {
                db::get().delete_session(User.get_username());
                res.code = 302;
                res.set_header("Set-Cookie", "");
                res.add_header("Location", res::get().get_login_path());
                res.end();
                return;
            }

            // check session time for 1 hour
            string::size_type sz;
            const double expires = stod(res::get().get_session_time_second(), &sz);
            if (User.get_session_sec() > expires)
            {
                db::get().delete_session(User.get_username());
                res.code = 302;
                res.set_header("Set-Cookie", "");
                res.add_header("Location", res::get().get_login_path());
                res.end();
                return;
            }

            // check admin only accessable url
            if (admin_urls[req.url])
            {
                if (User.get_group() != USER_INFO::GROUP_TYPE::GROUP_ADMIN)
                {
                    res.code = 302;
                    res.add_header("Location", res::get().get_dashboard_path());
                    res.end();
                    return;
                }
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

    private:

        string get_session(const string& cookie)
        {
            if(cookie.empty()) return "";
            size_t begin = cookie.find(res::get().get_cookie_id());
            if(begin == string::npos)
            {
                return "";
            }

            begin += res::get().get_cookie_id().length() + 1;
            size_t end = cookie.find(";", begin + 1);
            if(end == string::npos) end = cookie.length();

            return cookie.substr(begin, end-begin);
        }

        param_type get_value(const string& plain_req_session)
        {
            param_type params;
            stringstream iss(plain_req_session);
            string keyval, key, val;
            while(getline(iss, keyval, ';'))
            {
                stringstream iss(keyval);
                if (getline(getline(iss, key, '|'), val))
                {
                    params.emplace(res::get().get_cookie_id(), move(key));
                    params.emplace(res::get().get_field_id(), move(val));
                }
            }
            return params;
        }

        double get_time_sec()
        {
            using Clock = std::chrono::high_resolution_clock;
            return std::chrono::duration<double>(Clock::now().time_since_epoch()).count();
        }

    };
}



#endif // AUTH_HANDLER_H
