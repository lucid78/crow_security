#ifndef RESOURCE_HANDLER_H
#define RESOURCE_HANDLER_H

#pragma once

#include <boost/filesystem.hpp>
#include <boost/algorithm/string.hpp>

#include <unordered_map>
#include <stdlib.h>
#include <limits>
#include <iostream>

#include "crow/include/crow.h"
#include "resources.h"


using namespace std;
using res_type = unordered_map<string, pair<string,string>>;
using res = RESOURCES;

res_type resource_map;

string get_type(const string& ext)
{
    if (boost::iequals(ext, ".gif"))
    {
        return "image/gif";
    }
    else if (boost::iequals(ext, ".jpg"))
    {
        return "image/jpg";
    }
    else if (boost::iequals(ext, ".png"))
    {
        return "image/png";
    }
    else if (boost::iequals(ext, ".svg"))
    {
        return "image/svg+xml";
    }
    else if (boost::iequals(ext, ".js"))
    {
        return "application/javascript";
    }
    else if (boost::iequals(ext, ".css"))
    {
        return "text/css";
    }
    else
    {
        return "";
    }
}

bool read_all_resource()
{
    boost::filesystem::path full_path(boost::filesystem::current_path());
    cout << "current path is " << full_path.string() << endl;
    boost::filesystem::path base_dir = res::get().get_html_base_path();


    for (boost::filesystem::recursive_directory_iterator iter(base_dir), end; iter != end; ++iter)
    {
        if (!is_directory(iter->path()))
        {
            const string& path = iter->path().generic_string();
            const string& type = get_type(iter->path().extension().generic_string());
            size_t pos = path.rfind("/") + 1;
            const string& url_path = path.substr(pos, path.size() - pos);
            resource_map.emplace(url_path, make_pair(static_cast<const ostringstream&>(ostringstream{} << ifstream(path).rdbuf()).str(), type));
        }
    }

    return true;
}

void resource_handle(const crow::request /*&req*/, crow::response &res, const string& resource)
{
    res_type::const_iterator iter = resource_map.find(resource);
    if (iter != resource_map.end())
    {
        const string& content = iter->second.first;
        const string& type = iter->second.second;

        res.add_header("Content-type", type);
        res.write(content);
        res.end();
    }
    else
    {
        cerr << "resource not found: " << resource << endl;
        res.add_header("Content-type", "unknown");
        res.write("");
        res.end();
    }
}



#endif // RESOURCE_HANDLER_H
