# crow_security
support session middleware for crow framework

this middleware is support session for crow framework.

## example

```c++
#include "crow.h"
#include "middleware.h"

using namespace SECURITY;

int main()
{
    crow::App<crow::CookieParser, SessionManager> app;
    
    CROW_ROUTE(app, "/login")([](const crow::request /*&req*/, crow::response &res){
      ...
    });
    
    CROW_ROUTE(app, "/login_proc").methods("POST"_method)([](const crow::request &req, crow::response &res){
      ...
    });
    
    CROW_ROUTE(app, "/logout")([]{
      ...
    });
    
    app.port(18080).multithreaded().run();
}
```


## Login Process (/login_proc)

```c++

// create session
Auth::create_session(login_id);

// set session to cookie
const auto& authCTX = reinterpret_cast<crow::CookieParser::context *>(req.middleware_context);
authCTX->set_cookie(COOKIE_ID, session);

// save session to memory
Auth::save_session(login_id, COOKIE_ID, session);

```


## Session Check
session value is checked in before_handle function.

1. get encryption session from cookie.
2. decrypt 1.
3. get id, session value from 2.
4. get saved session from memory.
5. decrypt 4.
6. get id, session value from 5.
7. compare 3 to 6
8. check session time out


session time_out is 3600 seconds.

````c++
const double SESSION_TIME_SECOND = 3600;
````


