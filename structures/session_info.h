#ifndef SESSION_INFO_H
#define SESSION_INFO_H

#pragma once

#include <string>

using namespace std;

class SESSION_INFO
{
private:
    string session_id;
    string session_value;
    string time;
    double seconds;

public:
    SESSION_INFO(){}
    SESSION_INFO(const string& _session_id, const string& _session_value, const string& _time, const double& _seconds)
        : session_id(_session_id), session_value(_session_value), time(_time), seconds(_seconds)
    {}

    const string& get_session_id() const
    {
        return session_id;
    }

    const string& get_session_value() const
    {
        return session_value;
    }

    const string& get_time()
    {
        return time;
    }

    double get_seconds()
    {
        return seconds;
    }
};

#endif // SESSION_INFO_H
