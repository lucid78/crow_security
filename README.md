# crow_security
this middleware is support session for crow framework.

## Required
cryptopp (https://github.com/weidai11/cryptopp)
MysqlCpp (https://github.com/gummy52/MysqlCpp)
Boost

## usage

1. open resources.h and change CONFIG_FILE_PATH variable to your PATH

```c++
const string CONFIG_FILE_PATH = "/PATH/To/crow_security/config";
```

2. open config config file and change below variables.
 - PROJECT_BASE_PATH : this is used crow::set_base function of main.cpp file.
 - HTML_BASE_PATH : this is used to search all resource files such in the handler/resource_handler.h file.
                    resources is javascript, images, ans so on.
                    set value to $PROJECT_BASE_PATH/view.
 - DB_CON : this is mysql database connect string used in MysqlCpp and consists of IP;PORT;ID;PASS;TABLE.
            
 and others set to default value.

## example

```c++
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
```

