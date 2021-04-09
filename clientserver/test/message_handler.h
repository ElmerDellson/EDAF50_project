#ifndef MESSAGE_HANDLER_H
#define MESSAGE_HANDLER_H

#include "database.h"
#include "database_memory.h"
#include "connection.h"
#include "server.h"

using namespace std;

class MessageHandler {
    public:
        //Message_handler();
        MessageHandler(shared_ptr<Connection>& conn, DatabaseMemory& db);
        int ReadNumber(const shared_ptr<Connection>& conn);
        void WriteString(const shared_ptr<Connection>& conn, const string& s);
        void WriteInt(const shared_ptr<Connection>& conn, const int& s);
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
        DatabaseMemory& database;
        shared_ptr<Connection> conn;
};

#endif