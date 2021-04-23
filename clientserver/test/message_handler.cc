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
MessageHandler::MessageHandler(shared_ptr<Connection>& conn, Database* db)
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

Protocol MessageHandler::ReadProtocol(const shared_ptr<Connection>& conn)
{
        return static_cast<Protocol>(conn->read());
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
void MessageHandler::WriteInt(const shared_ptr<Connection>& conn, const int& value)
{
    conn->write((value >> 24) & 0xFF);
        conn->write((value >> 16) & 0xFF);
        conn->write((value >> 8) & 0xFF);
        conn->write(value & 0xFF);
}

void MessageHandler::WriteProtocol(const shared_ptr<Connection>& conn, const Protocol c){
     conn->write(static_cast<char>(c));
}
void MessageHandler::WriteChar(const shared_ptr<Connection>& conn, const char& c){
    conn->write(c);
}

void MessageHandler::WriteDollar(const shared_ptr<Connection>& conn){
    conn->write('$');
}



bool MessageHandler::Handle(){
    try {
        Protocol nbr = ReadProtocol(conn);
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
    vector<string> vec = database->ListNewsgroups();
    Protocol p = ReadProtocol(conn);
    if(p == Protocol::COM_END)  cout << "alles gut " << endl;
    WriteProtocol(conn, Protocol::ANS_LIST_NG);
    WriteProtocol(conn,Protocol::PAR_NUM);
    WriteInt(conn, database->NewsGroupNumber()); //TODO: Return actual number of groups
    for (int i = 0; i < database->NewsGroupNumber(); i++){
        WriteProtocol(conn,Protocol::PAR_NUM);
        WriteInt(conn, stoi(vec.at(i*2)));
        WriteProtocol(conn, Protocol::PAR_STRING);
        WriteInt(conn, vec.at((i*2)+1).length());
        for(char x :vec.at((i*2)+1)){
            WriteChar(conn, x);
        };
    }
    
    
    cout << "com_list_ng"<< endl;
    cout << "answer = "<< answer << endl;
    WriteProtocol(conn, Protocol::ANS_END);
}

void MessageHandler::CreateNewsgroup() {
    cout << "com_create_ng"<< endl;
    Protocol p = ReadProtocol(conn);
    while(p != Protocol::COM_END){
        string temp = "";
        int size = ReadNumber(conn);
        for(int i = 0; i < size; i++){
            temp += conn->read();
        }
        database->CreateNewsgroup(temp);
        cout << "news group name: " << temp << endl;
        WriteProtocol(conn,Protocol::ANS_CREATE_NG);
        WriteProtocol(conn, Protocol::ANS_ACK);
        WriteProtocol(conn, Protocol::ANS_END);
        p = ReadProtocol(conn);
    }
    if(p ==Protocol::COM_END) cout << "recieved com_end" << endl;
    
}

void MessageHandler::DeleteNewsgroup() {
    Protocol p = ReadProtocol(conn);
    while(p != Protocol::COM_END){
        if(p == Protocol::PAR_NUM) cout << "alles gut" << endl;
        int id = ReadNumber(conn);
        cout << "id : " << id << endl;
        WriteProtocol(conn, Protocol::ANS_DELETE_NG);
        WriteProtocol(conn, Protocol::ANS_ACK);
        WriteProtocol(conn, Protocol::ANS_END);
        cout << "com_delete_ng"<< endl;
        database->DeleteNewsgroup(id);
        p = ReadProtocol(conn);
    }
    
}

void MessageHandler::ListArticle(string answer) {
    Protocol p = ReadProtocol(conn);
    
    if(p == Protocol::PAR_NUM) cout << "par_num recieved" << endl;
    int id = ReadNumber(conn);
    cout << "id : " << id << endl;
    p = ReadProtocol(conn);
    if(p == Protocol::COM_END)  cout << "com_end recieved " << endl;
    vector<string> vec = database->ListArticles(id);
    cout << "com_list_art"<< endl;
    WriteProtocol(conn, Protocol::ANS_LIST_ART);
    WriteProtocol(conn, Protocol::ANS_ACK);
    WriteProtocol(conn,Protocol::PAR_NUM);
    WriteInt(conn, database->ArticleNumber(id)); //TODO: Return actual number of groups
    for (int i = 0; i < database->ArticleNumber(id); i++){
        WriteProtocol(conn,Protocol::PAR_NUM);
        WriteInt(conn, stoi(vec.at(i*2)));
        WriteProtocol(conn, Protocol::PAR_STRING);
        WriteInt(conn, vec.at((i*2)+1).length());
        for(char x :vec.at((i*2)+1)){
            WriteChar(conn, x);
        };
    }
    WriteProtocol(conn, Protocol::ANS_END);
}

void MessageHandler::CreateArticle() {
    Protocol p = ReadProtocol(conn);
    if(p == Protocol::PAR_NUM) cout << "par_num recieved" << endl;
    int id;
    int size;
    string title = "";
    string author = "";
    string text = "";
    id = ReadNumber(conn);
    cout << "id : " << id << endl;
    cout << "com_create_art"<< endl;
    p = ReadProtocol(conn);
    if(p == Protocol::PAR_STRING) cout << "Par_string 1 recieved" << endl;
    size = ReadNumber(conn); 
    for(int i = 0; i < size; i++){
        title += conn->read();
    }
    cout << "title done" << endl;
    p = ReadProtocol(conn);
    if(p == Protocol::PAR_STRING) cout << "Par_string 2 recieved" << endl;
    size = ReadNumber(conn); 
    for(int i = 0; i < size; i++){
        author += conn->read();
    }
    cout << "author done" << endl;
    p = ReadProtocol(conn);
    if(p == Protocol::PAR_STRING) cout << "Par_string 3 recieved" << endl;
    size = ReadNumber(conn); 
    for(int i = 0; i < size; i++){
        text += conn->read();
    }
    cout << "text done" << endl;
    p = ReadProtocol(conn);
    if(p == Protocol::COM_END)  cout << "com_end recieved " << endl;
    database->CreateArticle(id, author, title, text );
    WriteProtocol(conn, Protocol::ANS_CREATE_ART);
    WriteProtocol(conn, Protocol::ANS_ACK);
    WriteProtocol(conn, Protocol::ANS_END);
    
}

void MessageHandler::DeleteArticle() {
    int id;
    int id2;
    Protocol p = ReadProtocol(conn);
    if(p == Protocol::PAR_NUM) cout << "alles gut" << endl;
    id = ReadNumber(conn);
    cout << "id : " << id << endl;
    cout << "com_create_art"<< endl;
    p = ReadProtocol(conn);
    if(p == Protocol::PAR_NUM) cout << "alles gut" << endl;
    id2 = ReadNumber(conn);
    cout << "id2 : " << id2 << endl;
    p = ReadProtocol(conn);
    if(p == Protocol::COM_END)  cout << "com_end recieved " << endl;
    database->DeleteArticle(id,id2);
    WriteProtocol(conn, Protocol::ANS_DELETE_ART);
    WriteProtocol(conn, Protocol::ANS_ACK);
    WriteProtocol(conn, Protocol::ANS_END);
    
}

void MessageHandler::GetArticle(string answer) {
    int id;
    int id2;
    Protocol p = ReadProtocol(conn);
    if(p == Protocol::PAR_NUM) cout << "alles gut" << endl;
    id = ReadNumber(conn);
    cout << "id : " << id << endl;
    cout << "com_create_art"<< endl;
    p = ReadProtocol(conn);
    if(p == Protocol::PAR_NUM) cout << "alles gut" << endl;
    id2 = ReadNumber(conn);
    cout << "id2 : " << id2 << endl;
    p = ReadProtocol(conn);
    if(p == Protocol::COM_END)  cout << "com_end recieved " << endl;
    WriteProtocol(conn, Protocol::ANS_GET_ART);
    WriteProtocol(conn, Protocol::ANS_ACK);
    answer = database->GetArticleTitle(id,id2);
    WriteProtocol(conn, Protocol::PAR_STRING);
    WriteInt(conn, answer.length());
    for(char x :answer){
        WriteChar(conn, x);
    };
    answer = database->GetArticleAuthor(id,id2);
    WriteProtocol(conn, Protocol::PAR_STRING);
    WriteInt(conn, answer.length());
    for(char x :answer){
        WriteChar(conn, x);
    };
    answer = database->GetArticleText(id,id2);
    WriteProtocol(conn, Protocol::PAR_STRING);
    WriteInt(conn, answer.length());
    for(char x :answer){
        WriteChar(conn, x);
    };
    
    cout << "com_list_ng"<< endl;
    WriteProtocol(conn, Protocol::ANS_END);
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
