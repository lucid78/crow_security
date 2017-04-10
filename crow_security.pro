QT -= core
QT -= gui

CONFIG += c++14

TARGET = crow_security
CONFIG += console
CONFIG -= app_bundle
TEMPLATE = app

SOURCES += main.cpp

# The following define makes your compiler emit warnings if you use
# any feature of Qt which as been marked deprecated (the exact warnings
# depend on your compiler). Please consult the documentation of the
# deprecated API in order to know how to port your code away from it.
DEFINES -= QT_DEPRECATED_WARNINGS

# You can also make your code fail to compile if you use deprecated APIs.
# In order to do so, uncomment the following line.
# You can also select to disable deprecated APIs only up to a certain version of Qt.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

# boost
INCLUDEPATH += /usr/local/Cellar/boost/1.63.0/include
LIBS += -L/usr/local/lib -lboost_system -lboost_filesystem -lpthread

# cryptopp
LIBS += -L/usr/local/lib -lcryptopp
INCLUDEPATH += /usr/local/include

INCLUDEPATH += crow/include

HEADERS += \
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
    middleware.h \
    session.h


