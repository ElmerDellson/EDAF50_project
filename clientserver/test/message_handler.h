#ifndef MESSAGE_HANDLER_H
#define MESSAGE_HANDLER_H

#include "database.h"
#include "database_memory.h"
#include "database_disk.h"
#include "connection.h"
#include "server.h"
#include "protocol.h"

using namespace std;

class MessageHandler {
    public:
        //Message_handler();
        MessageHandler(shared_ptr<Connection>& conn, Database* db);
        int ReadNumber(const shared_ptr<Connection>& conn);
        Protocol ReadProtocol(const shared_ptr<Connection>& conn);
        void WriteString(const shared_ptr<Connection>& conn, const string& s);
        void WriteInt(const shared_ptr<Connection>& conn, const int& value);
        void WriteProtocol(const shared_ptr<Connection>& conn, const Protocol c);
        void WriteChar(const shared_ptr<Connection>& conn, const char& c);
        bool Handle();
    private:
        void ListNewsgroups(string);
        void CreateNewsgroup();
        void DeleteNewsgroup();
        void ListArticle(string);
        void CreateArticle();
        void DeleteArticle();
        void GetArticle(string);
        void WriteDollar(const shared_ptr<Connection>& conn);
        Database* database;
        shared_ptr<Connection> conn;
};

#endif