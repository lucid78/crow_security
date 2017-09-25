QT -= core
QT -= gui

CONFIG += c++14

TARGET = crow_security
CONFIG += console
CONFIG -= app_bundle
TEMPLATE = app

SOURCES += main.cpp \
    db/MysqlCpp/Database.cpp \
    db/MysqlCpp/DbField.cpp \
    db/MysqlCpp/QueryObject.cpp \
    db/MysqlCpp/QueryResult.cpp
    main.cpp


# boost
INCLUDEPATH += /usr/local/Cellar/boost/1.64.0_1/include
LIBS += -L/usr/local/lib -lboost_system -lboost_filesystem -lpthread

# crow
INCLUDEPATH += crow/include/

# mysql
LIBS += -L/usr/local/mysql/lib/ -lmysqlclient
INCLUDEPATH += /usr/local/mysql/include
DEPENDPATH += /usr/local/mysql/include
PRE_TARGETDEPS += /usr/local/mysql/lib/libmysqlclient.a

HEADERS += \
    auth/auth_handler.h \
    auth/crypt_handler.h \
    crow/include/crow/app.h \
    crow/include/crow/ci_map.h \
    crow/include/crow/common.h \
    crow/include/crow/dumb_timer_queue.h \
    crow/include/crow/http_connection.h \
    crow/include/crow/http_parser_merged.h \
    crow/include/crow/http_request.h \
    crow/include/crow/http_response.h \
    crow/include/crow/http_server.h \
    crow/include/crow/json.h \
    crow/include/crow/logging.h \
    crow/include/crow/middleware.h \
    crow/include/crow/middleware_context.h \
    crow/include/crow/mustache.h \
    crow/include/crow/parser.h \
    crow/include/crow/query_string.h \
    crow/include/crow/routing.h \
    crow/include/crow/settings.h \
    crow/include/crow/socket_adaptors.h \
    crow/include/crow/TinySHA1.hpp \
    crow/include/crow/utility.h \
    crow/include/crow/websocket.h \
    crow/include/crow.h \
    crow/include/crow/app.h \
    crow/include/crow/ci_map.h \
    crow/include/crow/common.h \
    crow/include/crow/dumb_timer_queue.h \
    crow/include/crow/http_connection.h \
    crow/include/crow/http_parser_merged.h \
    crow/include/crow/http_request.h \
    crow/include/crow/http_response.h \
    crow/include/crow/http_server.h \
    crow/include/crow/json.h \
    crow/include/crow/logging.h \
    crow/include/crow/middleware.h \
    crow/include/crow/middleware_context.h \
    crow/include/crow/mustache.h \
    crow/include/crow/parser.h \
    crow/include/crow/query_string.h \
    crow/include/crow/routing.h \
    crow/include/crow/settings.h \
    crow/include/crow/socket_adaptors.h \
    crow/include/crow/TinySHA1.hpp \
    crow/include/crow/utility.h \
    crow/include/crow/websocket.h \
    crow/include/crow.h \
    db/MysqlCpp/Database.h \
    db/MysqlCpp/DbField.h \
    db/MysqlCpp/QueryObjects.h \
    db/MysqlCpp/QueryResult.h \
    db/MysqlCpp/SafeQueue.h \
    db/db_handler.h \
    handler/dashboard_handler.h \
    handler/login_handler.h \
    handler/resource_handler.h \
    structures/session_info.h \
    structures/user_info.h \
    singleton.h \
    resources.h

win32:CONFIG(release, debug|release): LIBS += -L$$PWD/cryptopp/release/ -lcryptopp
else:win32:CONFIG(debug, debug|release): LIBS += -L$$PWD/cryptopp/debug/ -lcryptopp
else:unix: LIBS += -L$$PWD/cryptopp/ -lcryptopp

INCLUDEPATH += $$PWD/cryptopp
DEPENDPATH += $$PWD/cryptopp

win32-g++:CONFIG(release, debug|release): PRE_TARGETDEPS += $$PWD/cryptopp/release/libcryptopp.a
else:win32-g++:CONFIG(debug, debug|release): PRE_TARGETDEPS += $$PWD/cryptopp/debug/libcryptopp.a
else:win32:!win32-g++:CONFIG(release, debug|release): PRE_TARGETDEPS += $$PWD/cryptopp/release/cryptopp.lib
else:win32:!win32-g++:CONFIG(debug, debug|release): PRE_TARGETDEPS += $$PWD/cryptopp/debug/cryptopp.lib
else:unix: PRE_TARGETDEPS += $$PWD/cryptopp/libcryptopp.a

DISTFILES += \
    config
