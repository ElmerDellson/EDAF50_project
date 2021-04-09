/* myserver.cc: sample server program */
#include "connection.h"
#include "connectionclosedexception.h"
#include "server.h"
#include "database_memory.h"
#include "database.h"
#include "protocol.h"
#include "message_handler.h"

#include <cstdlib>
#include <iostream>
#include <memory>
#include <stdexcept>
#include <string>

using namespace std;

//Message_handler::Message_handler(){}
MessageHandler::MessageHandler(shared_ptr<Connection>& conn, DatabaseMemory& db)
                                : conn{conn}, database{db} {}
/*
 * Read an integer from a client.
 */
int MessageHandler::ReadNumber(const shared_ptr<Connection>& conn)
{
        unsigned char byte1 = conn->read();
        unsigned char byte2 = conn->read();
        unsigned char byte3 = conn->read();
        unsigned char byte4 = conn->read();
        return (byte1 << 24) | (byte2 << 16) | (byte3 << 8) | byte4;
}

/*
 * Send a string to a client.
 */
void MessageHandler::WriteString(const shared_ptr<Connection>& conn, const string& s)
{
        for (char c : s) {
                conn->write(c);
        }
        //conn->write('$');
}
void MessageHandler::WriteInt(const shared_ptr<Connection>& conn, const int& s)
{
    conn->write(s);
}

void MessageHandler::WriteDollar(const shared_ptr<Connection>& conn){
    conn->write('$');
}



bool MessageHandler::Handle(){
    try {
        Protocol nbr = static_cast<Protocol>(ReadNumber(conn));
        string answer = "";
        switch (nbr) {
            case Protocol::COM_LIST_NG:
                ListNewsgroups(answer);
                break;
            case Protocol::COM_CREATE_NG:
                CreateNewsgroup();
                break;
            case Protocol::COM_DELETE_NG:
                DeleteNewsgroup();
                break;
            case Protocol::COM_LIST_ART:
                ListArticle(answer);
                break;
            case Protocol::COM_CREATE_ART:
                CreateArticle();
                break;
            case Protocol::COM_DELETE_ART:
                DeleteArticle();
                break;
            case Protocol::COM_GET_ART:
                GetArticle(answer);
                break;
            default:
                cout << "Something went wrong!" << endl;
        }
        
        return true;
        
    } catch (const ConnectionClosedException&) {
            return false;            
    }
}

void MessageHandler::ListNewsgroups(string answer) {
    WriteInt(conn, static_cast<int>(Protocol::ANS_LIST_NG));
    WriteInt(conn, static_cast<int>(Protocol::PAR_NUM));
    WriteString(conn, " #numbOfGroups# "); //TODO: Return actual number of groups
    WriteInt(conn, static_cast<int>(Protocol::PAR_STRING));
    answer = database.ListNewsgroups();
    cout << "com_list_ng"<< endl;
    cout << "answer = "<< answer << endl;
    WriteString(conn, "here is the answer: ");
    WriteString(conn, answer);
    WriteInt(conn, static_cast<int>(Protocol::ANS_END));
    WriteDollar(conn);
}

void MessageHandler::CreateNewsgroup() {
    cout << "com_create_ng"<< endl;
    database.CreateNewsgroup("newsgroup");
    WriteInt(conn, static_cast<int>(Protocol::ANS_CREATE_NG));
    WriteInt(conn, static_cast<int>(Protocol::ANS_ACK));
    WriteInt(conn, static_cast<int>(Protocol::ANS_END));
    WriteString(conn, database.ListNewsgroups());
    WriteDollar(conn);
}

void MessageHandler::DeleteNewsgroup() {
    WriteInt(conn, static_cast<int>(Protocol::ANS_DELETE_NG));
    WriteInt(conn, static_cast<int>(Protocol::ANS_ACK));
    WriteInt(conn, static_cast<int>(Protocol::ANS_END));
    cout << "com_delete_ng"<< endl;
    database.DeleteNewsgroup(1);
    WriteString(conn, "deleted news group");
    WriteDollar(conn);
}

void MessageHandler::ListArticle(string answer) {
    WriteInt(conn, static_cast<int>(Protocol::ANS_LIST_ART));
    WriteInt(conn, static_cast<int>(Protocol::ANS_ACK));
    WriteInt(conn, static_cast<int>(Protocol::PAR_NUM));
    WriteInt(conn, static_cast<int>(Protocol::PAR_STRING));
    answer = database.ListArticles(0);
    cout << "com_list_art"<< endl;
    WriteString(conn, answer);
    WriteInt(conn, static_cast<int>(Protocol::ANS_END));
    WriteDollar(conn);
}

void MessageHandler::CreateArticle() {
    WriteInt(conn, static_cast<int>(Protocol::ANS_CREATE_ART));
    WriteInt(conn, static_cast<int>(Protocol::ANS_ACK));
    WriteInt(conn, static_cast<int>(Protocol::ANS_END));
    cout << "com_create_art"<< endl;
    database.CreateArticle(0, "jorgen", "life_of_jorgen", "he a good boy" );
    WriteString(conn, "created article");
    WriteDollar(conn);
}

void MessageHandler::DeleteArticle() {
    WriteInt(conn, static_cast<int>(Protocol::ANS_DELETE_ART));
    WriteInt(conn, static_cast<int>(Protocol::ANS_ACK));
    WriteInt(conn, static_cast<int>(Protocol::ANS_END));
    cout << "com_delete_art"<< endl;
    database.DeleteArticle(0,3);
    WriteString(conn, "deleted news group");
}

void MessageHandler::GetArticle(string answer) {
    WriteInt(conn, static_cast<int>(Protocol::ANS_GET_ART));
    WriteInt(conn, static_cast<int>(Protocol::ANS_ACK));
    WriteInt(conn, static_cast<int>(Protocol::PAR_STRING));
    WriteInt(conn, static_cast<int>(Protocol::PAR_STRING));
    WriteInt(conn, static_cast<int>(Protocol::PAR_STRING));
    answer = database.GetArticle(0,3);
    cout << "com_list_ng"<< endl;
    WriteString(conn, answer);
    WriteInt(conn, static_cast<int>(Protocol::ANS_END));
}






/*int main(int argc, char* argv[])
{
        auto server = init(argc, argv);
        DatabaseMemory database = DatabaseMemory();

        while (true) {
                auto conn = server.waitForActivity();
                if (conn != nullptr) {
                        try {
                                Protocol  nbr = static_cast<Protocol>(readNumber(conn));
                                string answer = "";
                                switch (nbr)
                                {
                                case Protocol::COM_LIST_NG:
                                
                                    /* writeInt(conn, ANS_LIST_NG);
                                    writeInt(conn, PAR_NUM);*//*
                                    answer = database.listNewsGroups();
                                    cout << "com_list_ng"<< endl;
                                    writeString(conn, answer);
                                    break;
                                case Protocol::COM_CREATE_NG:
                                    
                                    cout << "com_create_ng"<< endl;
                                    database.createNewsGroup("newsgroup");
                                    writeString(conn, "created news group");
                                    break;
                                case Protocol::COM_DELETE_NG:
                                    cout << "com_delete_ng"<< endl;
                                    database.deleteNewsGroup(1);
                                    writeString(conn, "deleted news group");
                                    break;
                                case Protocol::COM_LIST_ART:
                                    answer = database.listArticles(0);
                                    cout << "com_list_art"<< endl;
                                    writeString(conn, answer);
                                    break;
                                case Protocol::COM_CREATE_ART:
                                    cout << "com_create_art"<< endl;
                                    database.createArticle(0, "jorgen", "life_of_jorgen", "he a good boy" );
                                    writeString(conn, "created article");
                                    break;
                                case Protocol::COM_DELETE_ART:
                                    cout << "com_delete_art"<< endl;
                                    database.deleteArticle(0,3);
                                    writeString(conn, "deleted news group");
                                    break;
                                case Protocol::COM_GET_ART:
                                    answer = database.getArticle(0,3);
                                    cout << "com_list_ng"<< endl;
                                    writeString(conn, answer);
                                    break;
                                
                                default:
                                cout << "something went wrong" << endl;
                                    break;
                                } 
                            
                        } catch (ConnectionClosedException&) {
                                server.deregisterConnection(conn);
                                cout << "Client closed connection" << endl;
                        }
                } else {
                        conn = make_shared<Connection>();
                        server.registerConnection(conn);
                        cout << "New client connects" << endl;
                }
        }
        return 0;
}*/
