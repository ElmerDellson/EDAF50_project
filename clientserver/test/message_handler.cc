/* myserver.cc: sample server program */
#include "connection.h"
#include "connectionclosedexception.h"
#include "server.h"
#include "database_memory.h"
#include "database.h"
#include "protocol.h"

#include <cstdlib>
#include <iostream>
#include <memory>
#include <stdexcept>
#include <string>

using namespace std;



/*
 * Read an integer from a client.
 */
int readNumber(const shared_ptr<Connection>& conn)
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
void writeString(const shared_ptr<Connection>& conn, const string& s)
{
        for (char c : s) {
                conn->write(c);
        }
        conn->write('$');
}
void writeInt(const shared_ptr<Connection>& conn, const int& s)
{
        
    conn->write(s);
}


Server init(int argc, char* argv[])
{
        if (argc != 2) {
                cerr << "Usage: myserver port-number" << endl;
                exit(1);
        }

        int port = -1;
        try {
                port = stoi(argv[1]);
        } catch (exception& e) {
                cerr << "Wrong format for port number. " << e.what() << endl;
                exit(2);
        }

        Server server(port);
        if (!server.isReady()) {
                cerr << "Server initialization error." << endl;
                exit(3);
        }
        return server;
}

int main(int argc, char* argv[])
{
        auto server = init(argc, argv);
        //DatabaseMemory database = DatabaseMemory();

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
                                    writeInt(conn, PAR_NUM);*/
                                    answer = "listed news groups";//database.listNewsGroups();
                                    cout << "com_list_ng"<< endl;
                                    writeString(conn, answer);
                                    break;
                                case Protocol::COM_CREATE_NG:
                                    
                                    cout << "com_list_ng"<< endl;
                                    writeString(conn, "created news group");
                                    break;
                                case Protocol::COM_DELETE_NG:
                                    cout << "com_delete_ng"<< endl;
                                    writeString(conn, "deleted news group");
                                    break;
                                case Protocol::COM_LIST_ART:
                                    answer = "listed articles";//database.listArticles(1);
                                    cout << "com_list_art"<< endl;
                                    writeString(conn, answer);
                                    break;
                                case Protocol::COM_CREATE_ART:
                                    cout << "com_create_art"<< endl;
                                    writeString(conn, "created article");
                                    break;
                                case Protocol::COM_DELETE_ART:
                                    cout << "com_delete_art"<< endl;
                                    writeString(conn, "deleted news group");
                                    break;
                                case Protocol::COM_GET_ART:
                                    answer = "get article";//database.getArticle(1,1);
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
}
