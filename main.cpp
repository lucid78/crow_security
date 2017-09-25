
#include <unordered_map>

#include "handler/resource_handler.h"
#include "handler/dashboard_handler.h"
#include "handler/login_handler.h"

#include "db/db_handler.h"
#include "auth/auth_handler.h"
#include "resources.h"

using namespace std;
using db = DB_HANDLER;
using res = RESOURCES;

int main()
{
    res::get();
    db::get();

    crow::App<crow::CookieParser, session::SessionManager> app;
    crow::mustache::set_base(res::get().get_project_base_path());

    CROW_ROUTE(app, "/")(dashboard_handle);
    CROW_ROUTE(app, "/login")(login);
    CROW_ROUTE(app, "/login_proc").methods("POST"_method)(login_proc);
    CROW_ROUTE(app, "/logout")(logout);

    app.port(18080).multithreaded().run();


    return 0;
}
