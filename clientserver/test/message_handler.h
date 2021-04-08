#ifndef MESSAGE_HANDLER_H
#define MESSAGE_HANDLER_H

#include "database.h"
#include "database_memory.h"
#include "connection.h"
#include "server.h"

using namespace std;

class Message_handler {
    public:
        Message_handler();
        Message_handler(shared_ptr<Connection>& conn, DatabaseMemory& db);
        int readNumber(const shared_ptr<Connection>& conn);
        void writeString(const shared_ptr<Connection>& conn, const string& s);
        void writeInt(const shared_ptr<Connection>& conn, const int& s);
        bool handle();
    private:
        DatabaseMemory database;
        shared_ptr<Connection> conn;


};


#endif