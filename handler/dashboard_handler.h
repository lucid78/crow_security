#ifndef DASHBOARD_HANDLER_H
#define DASHBOARD_HANDLER_H

#pragma once

#include <crow.h>

using namespace std;

void dashboard_handle(const crow::request /*&req*/, crow::response &res)
{
    res.write(crow::mustache::load("view/html/dashboard.html").render());
    res.end();
}

#endif // DASHBOARD_HANDLER_H
