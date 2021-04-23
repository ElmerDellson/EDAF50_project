#ifndef MESSAGE_HANDLER_H
#define MESSAGE_HANDLER_H

#include "database.h"
#include "database_memory.h"
#include "database_disk.h"
#include "connection.h"
#include "server.h"

using namespace std;

class MessageHandler {
    public:
        //Message_handler();
        MessageHandler(shared_ptr<Connection>& conn, Database* db);
        int ReadNumber(const shared_ptr<Connection>& conn);
        void WriteString(const shared_ptr<Connection>& conn, const string& s);
        void WriteInt(const shared_ptr<Connection>& conn, const int& s);
        bool Handle();
    private:
        void ListNewsgroups();
        void CreateNewsgroup();
        void DeleteNewsgroup();
        void ListArticle();
        void CreateArticle();
        void DeleteArticle();
        void GetArticle();
        void WriteDollar(const shared_ptr<Connection>& conn);
        Database* database;
        shared_ptr<Connection> conn;
};

#endif