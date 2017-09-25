#ifndef USER_INFO_H
#define USER_INFO_H

#pragma once

#include <string>


using namespace std;

class USER_INFO
{
public:

    enum GROUP_TYPE : unsigned short
    {
        GROUP_ADMIN = 101,
        GROUP_USER,
    };

private:
    int id;
    string username;
    GROUP_TYPE group;
    string session;
    string session_create_time;
    string runtime;
    double session_sec;

public:

    const double& get_session_sec() const
    {
        return this->session_sec;
    }

    void set_session_sec(const string& sec)
    {
        string::size_type sz;
        this->session_sec = stod(sec, &sz);
    }

    const string& get_username() const
    {
        return username;
    }

    const GROUP_TYPE& get_group() const
    {
        return group;
    }

    void set_username(const string& _username)
    {
        username = _username;
    }

    void set_group(const GROUP_TYPE& type)
    {
        group = type;
    }

    void set_session(const string& _session)
    {
        session = _session;
    }

    const string& get_session() const
    {
        return session;
    }

    const string& get_session_create_time() const
    {
        return session_create_time;
    }

    void set_session_create_time(const string& _time)
    {
        session_create_time = _time;
    }

    void set_runtime(const string& _time)
    {
        runtime = _time;
    }

    const string& get_runtime() const
    {
        return runtime;
    }

    const int& get_id() const
    {
        return id;
    }

    void set_id(const int& _id)
    {
        id = _id;
    }


};

#endif // USER_INFO_H
