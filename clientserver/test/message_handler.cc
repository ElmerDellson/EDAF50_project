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
Message_handler::Message_handler(shared_ptr<Connection>& conn, DatabaseMemory& db)
                                : conn{conn}, database{db} {}
/*
 * Read an integer from a client.
 */
int Message_handler::readNumber(const shared_ptr<Connection>& conn)
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
void Message_handler::writeString(const shared_ptr<Connection>& conn, const string& s)
{
        for (char c : s) {
                conn->write(c);
        }
        //conn->write('$');
}
void Message_handler::writeInt(const shared_ptr<Connection>& conn, const int& s)
{
        
    conn->write(s);
}
void Message_handler::addDollar(const shared_ptr<Connection>& conn){
    conn->write('$');
}

bool Message_handler::handle(){
         
                        try {
                                Protocol  nbr = static_cast<Protocol>(readNumber(conn));
                                string answer = "";
                                switch (nbr)
                                {
                                case Protocol::COM_LIST_NG:
                                
                                    writeInt(conn, static_cast<int>(Protocol::ANS_LIST_NG));
                                    writeInt(conn, static_cast<int>(Protocol::PAR_NUM));
                                    writeString(conn, " #numbOfGroups# ");
                                    writeInt(conn, static_cast<int>(Protocol::PAR_STRING));
                                    answer = database.listNewsGroups();
                                    cout << "com_list_ng"<< endl;
                                    cout << "answer = "<< answer << endl;
                                    writeString(conn, "here is the answer: ");
                                    writeString(conn, answer);
                                    writeInt(conn, static_cast<int>(Protocol::ANS_END));
                                    addDollar(conn);
                                    break;
                                case Protocol::COM_CREATE_NG:
                                    
                                    cout << "com_create_ng"<< endl;
                                    database.createNewsGroup("newsgroup");
                                    writeInt(conn, static_cast<int>(Protocol::ANS_CREATE_NG));
                                    writeInt(conn, static_cast<int>(Protocol::ANS_ACK));
                                    writeInt(conn, static_cast<int>(Protocol::ANS_END));
                                    writeString(conn, database.listNewsGroups());
                                    addDollar(conn);
                                    break;
                                case Protocol::COM_DELETE_NG:
                                    writeInt(conn, static_cast<int>(Protocol::ANS_DELETE_NG));
                                    writeInt(conn, static_cast<int>(Protocol::ANS_ACK));
                                    writeInt(conn, static_cast<int>(Protocol::ANS_END));
                                    cout << "com_delete_ng"<< endl;
                                    database.deleteNewsGroup(1);
                                    writeString(conn, "deleted news group");
                                    addDollar(conn);
                                    break;
                                case Protocol::COM_LIST_ART:
                                    writeInt(conn, static_cast<int>(Protocol::ANS_LIST_ART));
                                    writeInt(conn, static_cast<int>(Protocol::ANS_ACK));
                                    writeInt(conn, static_cast<int>(Protocol::PAR_NUM));
                                    writeInt(conn, static_cast<int>(Protocol::PAR_STRING));
                                    answer = database.listArticles(0);
                                    cout << "com_list_art"<< endl;
                                    writeString(conn, answer);
                                    writeInt(conn, static_cast<int>(Protocol::ANS_END));
                                    addDollar(conn);
                                    break;
                                case Protocol::COM_CREATE_ART:
                                    writeInt(conn, static_cast<int>(Protocol::ANS_CREATE_ART));
                                    writeInt(conn, static_cast<int>(Protocol::ANS_ACK));
                                    writeInt(conn, static_cast<int>(Protocol::ANS_END));
                                    cout << "com_create_art"<< endl;
                                    database.createArticle(0, "jorgen", "life_of_jorgen", "he a good boy" );
                                    writeString(conn, "created article");
                                    addDollar(conn);
                                    break;
                                case Protocol::COM_DELETE_ART:
                                    writeInt(conn, static_cast<int>(Protocol::ANS_DELETE_ART));
                                    writeInt(conn, static_cast<int>(Protocol::ANS_ACK));
                                    writeInt(conn, static_cast<int>(Protocol::ANS_END));
                                    cout << "com_delete_art"<< endl;
                                    database.deleteArticle(0,3);
                                    writeString(conn, "deleted news group");
                                    break;
                                case Protocol::COM_GET_ART:
                                    writeInt(conn, static_cast<int>(Protocol::ANS_GET_ART));
                                    writeInt(conn, static_cast<int>(Protocol::ANS_ACK));
                                    writeInt(conn, static_cast<int>(Protocol::PAR_STRING));
                                    writeInt(conn, static_cast<int>(Protocol::PAR_STRING));
                                    writeInt(conn, static_cast<int>(Protocol::PAR_STRING));
                                    answer = database.getArticle(0,3);
                                    cout << "com_list_ng"<< endl;
                                    writeString(conn, answer);
                                    writeInt(conn, static_cast<int>(Protocol::ANS_END));
                                    break;
                                
                                default:
                                cout << "something went wrong" << endl;
                                    break;
                                } 
                                return true;
                            
                        } catch (ConnectionClosedException&) {
                                return false;
                                
                        }
                
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
