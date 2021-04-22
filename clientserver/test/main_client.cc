/* myclient.cc: sample client program */
#include "connection.h"
#include "connectionclosedexception.h"
#include "protocol.h"

#include <cstdlib>
#include <iostream>
#include <stdexcept>
#include <string>
#include <vector>

using namespace std;

/*
 * Send an integer to the server as four bytes.
 */
void writeNumber(const Connection& conn, int value)
{
        conn.write((value >> 24) & 0xFF);
        conn.write((value >> 16) & 0xFF);
        conn.write((value >> 8) & 0xFF);
        conn.write(value & 0xFF);
}

void write(const Connection& conn, unsigned char value)
{
        conn.write(value);
}

void writeProtocol(const Connection& conn, Protocol value)
{
        conn.write(static_cast<char>(value));
}

void writeString(const Connection& conn, string s)
{
        writeProtocol(conn, Protocol::PAR_STRING);
        writeNumber(conn, s.size());
        for(char c: s){
            write(conn, c);
        }
}

/*
 * Read a string from the server.
 */
string readString(const Connection& conn)
{
        string s;
        char   ch;
        while ((ch = conn.read()) != '$') {
                s += ch;
        }
        return s;
}

char readChar(const Connection& conn)
{
        return conn.read();
}

int ReadNumber(const Connection& conn)
{
        unsigned char byte1 = conn.read();
        unsigned char byte2 = conn.read();
        unsigned char byte3 = conn.read();
        unsigned char byte4 = conn.read();
        return (byte1 << 24) | (byte2 << 16) | (byte3 << 8) | byte4;
}

void listNewsGroups(const Connection& conn){
    Protocol p;
    p = static_cast<Protocol>(readChar(conn));
    int num;
    //vector<string> v;
    while(p != Protocol::ANS_END){
        if(p == Protocol::PAR_NUM){
            num = ReadNumber(conn);
            p = static_cast<Protocol>(readChar(conn));
        }
        if(p == Protocol::PAR_STRING){
            int i = ReadNumber(conn);
            string temp = "";
            for(int j = 0; j<i; j++){
                temp += readChar(conn);
            }
            cout << temp << endl;
            p = static_cast<Protocol>(readChar(conn));
        }
    }
}
void createNewsGroups(const Connection& conn){
    Protocol p;
    p = static_cast<Protocol>(readChar(conn));
    while(p != Protocol::ANS_END){
        
        if( p == Protocol::ANS_ACK){
            cout << "created succesfully" << endl;
            p = static_cast<Protocol>(readChar(conn));
        }
        if( p == Protocol::ANS_NAK){
            switch (static_cast<Protocol>(readChar(conn)))
            {
            case Protocol::ERR_NG_ALREADY_EXISTS :
                cout << "ERROR news group already exists" << endl;
                break;
            case Protocol::ERR_NG_DOES_NOT_EXIST :
                cout << "ERROR news group doesn't exist" << endl;
                break;
            case Protocol::ERR_ART_DOES_NOT_EXIST :
                cout << "ERROR article doesn't exist" << endl;
                break;
                
            
            default:
                break;
            }
        }
    } 
}
void deleteNewsGroups(const Connection& conn){
    Protocol p;
    p = static_cast<Protocol>(readChar(conn));
    while(p != Protocol::ANS_END){
        if( p == Protocol::ANS_ACK){
            cout << "deleted succesfully" << endl;
            p = static_cast<Protocol>(readChar(conn));
        }
        if( p == Protocol::ANS_NAK){
            cout << "things went to shit" << endl;
            cout << "error message" << endl;
            p = static_cast<Protocol>(readChar(conn));
        }
    } 
}
void listArticles(const Connection& conn){
    Protocol p;
    p = static_cast<Protocol>(readChar(conn));
    int num;
    cout << "i run" << endl;
    while(p != Protocol::ANS_END){
        if(p == Protocol::ANS_ACK){
            p = static_cast<Protocol>(readChar(conn));
            if(p == Protocol::PAR_NUM){
                num = ReadNumber(conn);
                cout << "num: " << num << endl;
                p = static_cast<Protocol>(readChar(conn));
            }
            if(p == Protocol::PAR_STRING){
                int i = ReadNumber(conn);
                string temp = "";
                for(int j = 0; j<i; j++){
                    temp += readChar(conn);
                }
                cout << "temp: " << temp << endl;
                p = static_cast<Protocol>(readChar(conn));
            }    
        }
        if(p == Protocol::ANS_NAK){
            cout << "error message" << endl;
        }
    }
}
void createArticle(const Connection& conn){
    Protocol p;
    p = static_cast<Protocol>(readChar(conn));
    while (p != Protocol::ANS_END){
        if(p == Protocol::ANS_ACK){
            cout << "article created successfuly" << endl;
            p = static_cast<Protocol>(readChar(conn));
        } 
    }
}
void deleteArticle(const Connection& conn){
    Protocol p;
    p = static_cast<Protocol>(readChar(conn));
    while(p != Protocol::ANS_END){
        if( p == Protocol::ANS_ACK){
            cout << "deleted succesfully" << endl;
            p = static_cast<Protocol>(readChar(conn));
        }
        if( p == Protocol::ANS_NAK){
            cout << "things went to shit" << endl;
            cout << "error message" << endl;
            p = static_cast<Protocol>(readChar(conn));
        }
    } 
}
void getArticle(const Connection& conn){
    Protocol p;
    p = static_cast<Protocol>(readChar(conn));
    int num;
    while(p != Protocol::ANS_END){
        if(p == Protocol::ANS_ACK){
            p = static_cast<Protocol>(readChar(conn));        
            int i;
            while(p == Protocol::PAR_STRING){
                
                i = ReadNumber(conn);
                string temp = "";
                for(int j = 0; j<i; j++){
                    temp += readChar(conn);
                }
                cout << "temp: " << temp << endl;
                p = static_cast<Protocol>(readChar(conn));
            
            }
            
        }
        if(p == Protocol::ANS_NAK){
            cout << "error message" << endl;
        }
    }
}



/* Creates a client for the given args, if possible.
 * Otherwise exits with error code.
 */
Connection init(int argc, char* argv[])
{
        if (argc != 3) {
                cerr << "Usage: myclient host-name port-number" << endl;
                exit(1);
        }

        int port = -1;
        try {
                port = stoi(argv[2]);
        } catch (exception& e) {
                cerr << "Wrong port number. " << e.what() << endl;
                exit(2);
        }

        Connection conn(argv[1], port);
        if (!conn.isConnected()) {
                cerr << "Connection attempt failed" << endl;
                exit(3);
        }

        return conn;
}

int app(const Connection& conn)
{
    
    cout << "Options:" << endl;
    cout << "List newsgroups: 1" << endl;
    cout << "Create newsgroups: 2" << endl;
    cout << "Delete newsgroups: 3" << endl;
    cout << "List articles: 4" << endl;
    cout << "Create article : 5" << endl;
    cout << "Delete article : 6" << endl;
    cout << "Get article : 7" << endl;

    int nbr;
    int id;
    string temp;
    string title;
    string author;
    string text;
        while (cin >> nbr) {
            switch (nbr)
            {
            case 1:
                writeProtocol(conn, Protocol::COM_LIST_NG);
                break;
            case 2:
                writeProtocol(conn, Protocol::COM_CREATE_NG);
                cin >> temp;
                writeString(conn, temp);
                break;
            case 3:
                writeProtocol(conn, Protocol::COM_DELETE_NG);
                writeProtocol(conn, Protocol::PAR_NUM);
                cin >> id;
                writeNumber(conn, id);
                break;
            case 4:
                writeProtocol(conn, Protocol::COM_LIST_ART);
                writeProtocol(conn, Protocol::PAR_NUM);
                cin >> id;
                writeNumber(conn, id);
                break;
            case 5:
                writeProtocol(conn, Protocol::COM_CREATE_ART);
                cin >> id;
                cin >> title;
                cin >> author;
                cin >> text;
                writeProtocol(conn, Protocol::PAR_NUM);
                writeNumber(conn, id);
                writeString(conn, title);
                writeString(conn, author);
                writeString(conn, text);
                break;
            case 6:
                writeProtocol(conn, Protocol::COM_DELETE_ART);
                writeProtocol(conn, Protocol::PAR_NUM);
                cin >> id;
                writeNumber(conn, id);
                writeProtocol(conn, Protocol::PAR_NUM);
                cin >> id;
                writeNumber(conn, id);
                break;
            case 7:
                writeProtocol(conn, Protocol::COM_GET_ART);
                writeProtocol(conn, Protocol::PAR_NUM);
                cin >> id;
                writeNumber(conn, id);
                writeProtocol(conn, Protocol::PAR_NUM);
                cin >> id;
                writeNumber(conn, id);
                break;
            
            default:
                break;
            }
                try {
                        
                        char reply = readChar(conn);
                        
                        switch (static_cast<Protocol>(reply))
                        {
                        case Protocol::ANS_LIST_NG :
                            listNewsGroups(conn);
                            break;
                        case Protocol::ANS_CREATE_NG :
                            createNewsGroups(conn);
                            break;
                        case Protocol::ANS_DELETE_NG :
                            deleteNewsGroups(conn);
                            break;
                        case Protocol::ANS_LIST_ART :
                            listArticles(conn);
                            break;
                        case Protocol::ANS_CREATE_ART :
                            createArticle(conn);
                            break;
                        case Protocol::ANS_DELETE_ART :
                            deleteArticle(conn);
                            break;
                        case Protocol::ANS_GET_ART :
                            getArticle(conn);
                            break;
                        
                        default:
                            break;
                        }
                        //cout << " " << reply << endl;
                        cout << "give another command: ";
                } catch (ConnectionClosedException&) {
                        cout << " no reply from server. Exiting." << endl;
                        return 1;
                }
        }

        return 0;

       /*    
        cout << "Type a number: ";
        int nbr;
        while (cin >> nbr) {
                try {
                        cout << nbr << " is ...";
                        writeNumber(conn, nbr);
                        string reply = readString(conn);
                        cout << " " << reply << endl;
                        cout << "Type another number: ";
                } catch (ConnectionClosedException&) {
                        cout << " no reply from server. Exiting." << endl;
                        return 1;
                }
        }
        cout << "\nexiting.\n";
        return 0;*/
}

int main(int argc, char* argv[])
{
        Connection conn = init(argc, argv);
        return app(conn);
}
