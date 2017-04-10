
#include <unordered_map>

#include "crow.h"
#include "middleware.h"


using namespace SECURITY;

#define PROJECT_BASE "/Users/lucid7/workspace/crow_security/"

int main()
{
    crow::App<crow::CookieParser, SessionManager> app;
    crow::mustache::set_base(PROJECT_BASE);

    CROW_ROUTE(app, "/")([]{
        return "Hello World!";
    });

    CROW_ROUTE(app, "/login")([](const crow::request /*&req*/, crow::response &res){
        res.write(crow::mustache::load("html/login.html").render());
        res.end();
    });

    CROW_ROUTE(app, "/login_proc").methods("POST"_method)([](const crow::request &req, crow::response &res){
        unordered_map<string, string> request_parameter_map = Auth::get_login_info(req.body);
        if (request_parameter_map.find(LOGIN_ID) == request_parameter_map.end()
                || request_parameter_map.find(LOGIN_PWD) == request_parameter_map.end())
        {
            res.code = 302;
            res.add_header("Location", LOGIN_PATH);
            res.write("field name is error");
            res.end();
            return;
        }

        const string login_id = request_parameter_map.at(LOGIN_ID);
        const string login_pwd = request_parameter_map.at(LOGIN_PWD);

        if (!login_id.compare("") && !login_pwd.compare(""))
        {
            res.code = 302;
            res.add_header("Location", LOGIN_PATH);
            res.end();
            return;
        }

        //
        // modify this check user login.
        //
        if (!Auth::is_valid_user(login_id, login_pwd))
        {
            res.code = 302;
            res.add_header("Location", LOGIN_PATH);
            res.end();
            return;
        }

        const string& session = Auth::create_session(login_id);

        // set session to cookie
        const auto& authCTX = reinterpret_cast<crow::CookieParser::context *>(req.middleware_context);
        authCTX->set_cookie(COOKIE_ID, session);

        // save session to memory
        Auth::save_session(login_id, COOKIE_ID, session);

        res.code = 302;
        res.add_header("Location", DASHBOARD_PATH);
        res.end();

    });

    CROW_ROUTE(app, "/logout")([]{
        return "Hello World!";
    });

    app.port(18080).multithreaded().run();


    return 0;
}
