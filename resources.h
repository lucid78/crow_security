#ifndef RESOURCES_H
#define RESOURCES_H

#pragma once

#include <string>
#include <mutex>

#include "singleton.h"

using namespace std;

const string CONFIG_FILE_PATH = "/PATH/To/crow_security/config";

class RESOURCES : public Singleton<RESOURCES>
{
private:
    mutex file_locker;
    string project_base_path;
    string html_base_path;
    string dashboard_path;
    string session_time_second;
    string field_id;
    string field_pwd;
    string login_path;
    string cookie_id;
    string db_con;

public:
    RESOURCES()
    {
        project_base_path = get_config("PROJECT_BASE_PATH");
        html_base_path = get_config("HTML_BASE_PATH");
        dashboard_path = get_config("DASHBOARD_PATH");
        session_time_second = get_config("SESSION_TIME_SECOND");
        field_id = get_config("FIELD_ID");
        field_pwd = get_config("FIELD_PWD");
        login_path = get_config("LOGIN_PATH");
        cookie_id = get_config("COOKIE_ID");
        db_con = get_config("DB_CON");
    }

    ~RESOURCES(){}

    const string get_db_con() const
    {
        return this->db_con;
    }

    const string get_cookie_id() const
    {
        return this->cookie_id;
    }

    const string get_login_path() const
    {
        return this->login_path;
    }

    const string get_field_pwd() const
    {
        return this->field_pwd;
    }

    const string get_field_id() const
    {
        return this->field_id;
    }

    const string get_project_base_path() const
    {
        return this->project_base_path;
    }

    const string get_html_base_path() const
    {
        return this->html_base_path;
    }

    const string get_dashboard_path() const
    {
        return this->dashboard_path;
    }

    const string get_session_time_second() const
    {
        return this->session_time_second;
    }


private:

    const string get_config(const string& target)
    {
        this->file_locker.lock();
        const string result = read_file(target);
        this->file_locker.unlock();
        if (result.empty())
        {
            cerr << "not found <" << target << ">" << endl;
            exit(-123);
        }
        return result;
    }

    const string read_file(const string& target) const
    {
        string result="";
        string line;
        ifstream file(CONFIG_FILE_PATH);
        if(file.is_open())
        {
            string key;
            while(getline(file, line))
            {
                // ignore comment and empty line
                if (line.find("# ") != string::npos || line.empty())
                {
                    continue;
                }

                key.clear();
                result.clear();
                size_t begin = line.find("=");
                key = line.substr(0, begin);
                if(!key.compare(target))
                {
                    result = line.substr(begin+1);
                    break;
                }
            }
            file.close();
        }
        return result;
    }

};

#endif // RESOURCES_H
