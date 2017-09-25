#ifndef DB_HANDLER_H
#define DB_HANDLER_H

#pragma once

#include <vector>
#include <memory>
#include <iostream>
#include <fstream>
#include <vector>

#include <boost/asio/ip/host_name.hpp>
#include <boost/algorithm/string/replace.hpp>

#include "MysqlCpp/Database.h"
#include "structures/user_info.h"
#include "singleton.h"
#include "resources.h"

using namespace std;
using param_type = unordered_map<string, string>;
using res = RESOURCES;

class DB_HANDLER : public Singleton<DB_HANDLER>
{
private:
    unique_ptr<Database> db = make_unique<Database>();

public:
    DB_HANDLER()
    {
        cout << "create database session: " << endl;

        if(!db->Initialize(res::get().get_db_con().c_str()))
        {
            cerr << "db initialize error" << endl;
            exit(-12);
        }

        create_table();
        insert_default_value();
    }

    ~DB_HANDLER()
    {
        db->Uninitialise();
        cout << "destroy database session" << endl;
    }


    const USER_INFO get_user(const string& username, const string& password) const
    {
        string query = "SELECT *, unix_timestamp(now()) - unix_timestamp(session_create_time) as timeout, sec_to_time(unix_timestamp(now()) - unix_timestamp(session_create_time)) as runtime FROM crow.members WHERE username='";
        query.append(username);
        query.append("' and password='");
        query.append(password);
        query.append("'");

        USER_INFO row;
        if (std::shared_ptr<QueryResult> result = db->Query(query.c_str()))
        {
            do
            {
                DbField* pFields = result->fetchCurrentRow();
                row.set_id(pFields[0].getInt32());
                row.set_username(pFields[1].getCppString());
                switch(pFields[2].getUInt32())
                {
                case USER_INFO::GROUP_TYPE::GROUP_ADMIN:
                    row.set_group(USER_INFO::GROUP_TYPE::GROUP_ADMIN);
                    break;
                case USER_INFO::GROUP_TYPE::GROUP_USER:
                    row.set_group(USER_INFO::GROUP_TYPE::GROUP_USER);
                    break;
                }
                row.set_session(pFields[4].getCppString());
                row.set_session_create_time(pFields[5].getCppString());
                row.set_session_sec(pFields[6].getCppString());
                row.set_runtime(pFields[7].getCppString());
            }
            while(result->NextRow());
        }
        return row;
    }

    const USER_INFO get_user(const string& session) const
    {
        string query = "SELECT *, unix_timestamp(now()) - unix_timestamp(session_create_time) as timeout, sec_to_time(unix_timestamp(now()) - unix_timestamp(session_create_time)) as runtime FROM crow.members WHERE session='";
        query.append(session);
        query.append("'");

        USER_INFO row;
        if (std::shared_ptr<QueryResult> result = db->Query(query.c_str()))
        {
            DbField* pFields = result->fetchCurrentRow();
            row.set_id(pFields[0].getInt32());
            row.set_username(pFields[1].getCppString());
            switch(pFields[2].getUInt32())
            {
            case USER_INFO::GROUP_TYPE::GROUP_ADMIN:
                row.set_group(USER_INFO::GROUP_TYPE::GROUP_ADMIN);
                break;
            case USER_INFO::GROUP_TYPE::GROUP_USER:
                row.set_group(USER_INFO::GROUP_TYPE::GROUP_USER);
                break;
            }
            row.set_session(pFields[4].getCppString());
            row.set_session_create_time(pFields[5].getCppString());
            row.set_session_sec(pFields[6].getCppString());
            row.set_runtime(pFields[7].getCppString());
        }
        return row;
    }

    void delete_session(const string& id)
    {
        string query = "UPDATE members SET session='' WHERE id='";
        query.append(id);
        query.append("'");
        db->ExecuteQueryInstant(query.c_str());
    }

    void init_session(const string& username, const string& password, const string& session, const string& time)
    {
        string query = "UPDATE members SET session = '";
        query.append(session);
        query.append("', session_create_time='");
        query.append(time);
        query.append("' WHERE username='");
        query.append(username);
        query.append("' and password='");
        query.append(password);
        query.append("'");
        db->ExecuteQueryInstant(query.c_str());
    }

private:

    void create_table()
    {
        const string query = "CREATE TABLE IF NOT EXISTS members (  id INT UNSIGNED NOT NULL AUTO_INCREMENT,\
                                                                    username VARCHAR(32) NOT NULL,\
                                                                    groups INT UNSIGNED NOT NULL,\
                                                                    password VARCHAR(128) NOT NULL,\
                                                                    session VARCHAR(128) NULL,\
                                                                    session_create_time DATETIME,\
                                                                    PRIMARY KEY (id),\
                                                                    UNIQUE INDEX id_UNIQUE (id ASC),\
                                                                    UNIQUE INDEX username_UNIQUE (username ASC)) \
                                                        ENGINE = InnoDB \
                                                        DEFAULT CHARACTER SET = utf8";
        db->ExecuteQueryInstant(query.c_str());
    }

    void insert_default_value()
    {
        const string test_user = "INSERT INTO members (id, username, groups, password) VALUES ('1', 'test', '102', 'test')";
        db->ExecuteQueryInstant(test_user.c_str());
        const string admin_user = "INSERT INTO members (id, username, groups, password) VALUES ('2', 'admin', '101', 'admin')";
        db->ExecuteQueryInstant(admin_user.c_str());
    }

};

#endif // DB_HANDLER_H
