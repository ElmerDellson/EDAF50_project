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

void PrintInstructions() {
    cout << "\nOptions:" << endl;
    cout << "List newsgroups: 1" << endl;
    cout << "Create newsgroups: 2" << endl;
    cout << "Delete newsgroups: 3" << endl;
    cout << "List articles: 4" << endl;
    cout << "Create article : 5" << endl;
    cout << "Delete article : 6" << endl;
    cout << "Get article : 7" << endl;
}

//Check if a string is a number
bool IsNumber(const std::string& s)
{
    string::const_iterator it = s.begin();

    while (it != s.end() && std::isdigit(*it)) 
        ++it;


    return !s.empty() && it == s.end();
}

bool IsPermittedInput(string s) {
    if (s.find_first_not_of("abcdefghijklmnopqrstuvxyzABCDEFGHIJKLMNOPQRSTUVXYZ0123456789 ") != string::npos
        && s.empty())
        return false;
    
    return true;
}

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
        
        for(char c: s)
            write(conn, c);
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


void handleListNewsgroupsResponse(const Connection& conn){
    Protocol p(static_cast<Protocol>(readChar(conn)));
    int num;
    
    if (p == Protocol::PAR_NUM){
            num = ReadNumber(conn);
            p = static_cast<Protocol>(readChar(conn));
            cout << "Number of groups: " << num << endl; 
        }
    
    while (p != Protocol::ANS_END) {
        if (p == Protocol::PAR_NUM) {
            num = ReadNumber(conn);
            cout << "ID: " << num << ", " ;
            p = static_cast<Protocol>(readChar(conn));
        }

        if (p == Protocol::PAR_STRING) {
            int i{ReadNumber(conn)};
            string temp = ""; //Do we need to initialize this?

            for (int j = 0; j<i; j++)
                temp += readChar(conn);

            cout << temp << endl;

            p = static_cast<Protocol>(readChar(conn));
        }
    }
}

void handleCreateNewsgroupsResponse(const Connection& conn){
    Protocol p(static_cast<Protocol>(readChar(conn)));

    while (p != Protocol::ANS_END) {
        
        if (p == Protocol::ANS_ACK) {
            cout << "Created succesfully" << endl;
            p = static_cast<Protocol>(readChar(conn));
        }

        if (p == Protocol::ANS_NAK) {
            p = static_cast<Protocol>(readChar(conn));

            if (p == Protocol::ERR_NG_ALREADY_EXISTS) {
                cout << "ERROR: Newsgroup already exist."  << endl;
                p = static_cast<Protocol>(readChar(conn));
            }
            else {
                cout << "ERROR: Error of unknown type" << endl;
                p = static_cast<Protocol>(readChar(conn));
            }
        }
    } 
}

void handleDeleteNewsgroupsResponse(const Connection& conn) {
    Protocol p(static_cast<Protocol>(readChar(conn)));

    while (p != Protocol::ANS_END) {
        if (p == Protocol::ANS_ACK) {
            cout << "Deleted succesfully." << endl;
            p = static_cast<Protocol>(readChar(conn));
        }

        if (p == Protocol::ANS_NAK) {
            p = static_cast<Protocol>(readChar(conn));
            
            if (p == Protocol::ERR_NG_DOES_NOT_EXIST) {
                cout << "ERROR: Newsgroup does not exist."  << endl;
                p = static_cast<Protocol>(readChar(conn));
            }
            else{
                cout << "ERROR: Error of unknown type" << endl;
                p = static_cast<Protocol>(readChar(conn));
            }
        }
    } 
}

void handleListArticlesResponse(const Connection& conn){
    Protocol p(static_cast<Protocol>(readChar(conn)));
    
    int num;
    bool first{true};

    while (p != Protocol::ANS_END) {
        if (p == Protocol::ANS_ACK) {
            p = static_cast<Protocol>(readChar(conn));

            while (p != Protocol::ANS_END) {    
                if (p == Protocol::PAR_NUM) {
                    num = ReadNumber(conn);
                    if (first) {
                        cout << "Number of articles: " << num << endl;
                        first = false;
                    }
                    else {
                        cout << "ID: " << num << ", ";
                    }

                    p = static_cast<Protocol>(readChar(conn));
                }

                if (p == Protocol::PAR_STRING) {
                    int i{ReadNumber(conn)};
                    string temp = ""; //Do we need to initialize?

                    for (int j = 0; j<i; j++)
                        temp += readChar(conn);

                    cout  << temp << endl;

                    p = static_cast<Protocol>(readChar(conn));
                }
            }
        }

        if (p == Protocol::ANS_NAK) {
            p = static_cast<Protocol>(readChar(conn));

            if (p == Protocol::ERR_NG_DOES_NOT_EXIST) {
                cout << "ERROR: Newsgroup does not exist."  << endl;
                p = static_cast<Protocol>(readChar(conn));
            }
            else {
                cout << "ERROR: Error of unknown type" << endl;
                p = static_cast<Protocol>(readChar(conn));
            }
        }
    }
}

void handleCreateArticleResponse(const Connection& conn){
    Protocol p(static_cast<Protocol>(readChar(conn)));

    while (p != Protocol::ANS_END) {
        if (p == Protocol::ANS_ACK) {
            cout << "article created successfuly" << endl;
            p = static_cast<Protocol>(readChar(conn));
        }

        if (p == Protocol::ANS_NAK) {
            p = static_cast<Protocol>(readChar(conn));

            if (p == Protocol::ERR_NG_DOES_NOT_EXIST) {
                cout << "ERROR: Newsgroup does not exist."  << endl;
                p = static_cast<Protocol>(readChar(conn));
            }
            else {
                cout << "ERROR: Error of unknown type" << endl;
                p = static_cast<Protocol>(readChar(conn));
            }
        }
    }
}

void handleDeleteArticleResponse(const Connection& conn){
    Protocol p(static_cast<Protocol>(readChar(conn)));

    while (p != Protocol::ANS_END) {
        if (p == Protocol::ANS_ACK) {
            cout << "Deleted succesfully." << endl;
            p = static_cast<Protocol>(readChar(conn));
        }

        if (p == Protocol::ANS_NAK) {
            p = static_cast<Protocol>(readChar(conn));

            if (p == Protocol::ERR_ART_DOES_NOT_EXIST) {
                cout << "ERROR: Article does not exist."  << endl;
                p = static_cast<Protocol>(readChar(conn));
            }
            else if (p == Protocol::ERR_NG_DOES_NOT_EXIST) {
                cout << "ERROR: Newsgroup does not exist."  << endl;
                p = static_cast<Protocol>(readChar(conn));
            }
            else {
                cout << "ERROR: Error of unknown type" << endl;
                p = static_cast<Protocol>(readChar(conn));
            }
        }
    } 
}

void handleGetArticleResponse(const Connection& conn){
    Protocol p(static_cast<Protocol>(readChar(conn)));
    
    while (p != Protocol::ANS_END) {
        if (p == Protocol::ANS_ACK) {
            p = static_cast<Protocol>(readChar(conn));        
            int i;

            while (p == Protocol::PAR_STRING) {
                i = ReadNumber(conn);
                string temp = "";

                for (int j = 0; j<i; j++)
                    temp += readChar(conn);
                
                cout << "Title: " << temp << endl;
                p = static_cast<Protocol>(readChar(conn));
                i = ReadNumber(conn);
                temp = "";

                for(int j = 0; j<i; j++)
                    temp += readChar(conn);
                
                cout << "Author: " << temp << endl;
                p = static_cast<Protocol>(readChar(conn));
                i = ReadNumber(conn);
                temp = "";
                
                for (int j = 0; j<i; j++)
                    temp += readChar(conn);
                
                cout << "Text: " << temp << endl;
                p = static_cast<Protocol>(readChar(conn));
            }
        }

        if (p == Protocol::ANS_NAK) {
            p = static_cast<Protocol>(readChar(conn));

            if (p == Protocol::ERR_ART_DOES_NOT_EXIST) {
                cout << "ERROR: Article does not exist."  << endl;
                p = static_cast<Protocol>(readChar(conn));
            }

            if (p == Protocol::ERR_NG_DOES_NOT_EXIST) {
                cout << "ERROR: Newsgroup does not exist."  << endl;
                p = static_cast<Protocol>(readChar(conn));
            }
            else {
                cout << "ERROR: Error of unknown type" << endl;
                p = static_cast<Protocol>(readChar(conn));
            }
        }
    }
}

void listNewsgroups(const Connection& conn) {
    writeProtocol(conn, Protocol::COM_LIST_NG);
    writeProtocol(conn, Protocol::COM_END);
}

void createNewsgroup(const Connection& conn) {
    string temp;
    writeProtocol(conn, Protocol::COM_CREATE_NG);
    cout << "Newsgroup title: ";

    getline(cin, temp);

    while  (!IsPermittedInput(temp)) {
        cout << "Please write only letters or numbers. " << endl;
        cout << "Newsgroup title: ";
        getline(cin, temp);
    }

    writeString(conn, temp);
    writeProtocol(conn, Protocol::COM_END);
}

void deleteNewsgroup(const Connection& conn) {
    string temp;

    writeProtocol(conn, Protocol::COM_DELETE_NG);
    writeProtocol(conn, Protocol::PAR_NUM);
    cout << "Enter the ID of the newsgroup to be deleted: ";
    
    getline(cin, temp);

    while (!IsNumber(temp)) {
        cout << "Please enter a valid ID number." << endl;
        cout << "Enter the ID of the newsgroup to be deleted: ";
        getline(cin, temp);
    }
    
    int id{stoi(temp)};

    writeNumber(conn, id);
    writeProtocol(conn, Protocol::COM_END);
}

void listArticles(const Connection& conn) {
    string temp;

    writeProtocol(conn, Protocol::COM_LIST_ART);
    writeProtocol(conn, Protocol::PAR_NUM);
    cout << "Enter newsgroup ID: ";

    getline(cin, temp);

    while (!IsNumber(temp)) {
        cout << "Please enter a valid ID number." << endl;                    
        cout << "Enter newsgroup ID: ";
        getline(cin, temp);
    }

    int id{stoi(temp)};

    writeNumber(conn, id);
    writeProtocol(conn, Protocol::COM_END);
}

void createArticle(const Connection& conn) {
    string temp;
    writeProtocol(conn, Protocol::COM_CREATE_ART);
    cout << "Enter newsgroup ID: ";
    
    getline(cin, temp);

    while (!IsNumber(temp)) {
        cout << "Please enter a valid ID number." << endl;                    
        cout << "Enter newsgroup ID: ";
        getline(cin, temp);
    }

    int id{stoi(temp)};

    cout << "Title: ";
    string title;
    getline(cin, title);

    while  (!IsPermittedInput(title)) {
        cout << "Please write only letters or numbers. " << endl;
        cout << "Title: ";
        getline(cin, title);
    }

    cout << "Author: ";
    string author;
    getline(cin, author);

    cout << "Text: ";
    string text;
    getline(cin, text);
    
    writeProtocol(conn, Protocol::PAR_NUM);
    writeNumber(conn, id);
    writeString(conn, title);
    writeString(conn, author);
    writeString(conn, text);
    writeProtocol(conn, Protocol::COM_END);
}

void deleteArticle(const Connection& conn) {
    string temp;
    writeProtocol(conn, Protocol::COM_DELETE_ART);
    writeProtocol(conn, Protocol::PAR_NUM);
    cout << "Enter newsgroup ID: ";
    
    getline(cin, temp);

    while (!IsNumber(temp)) {
        cout << "Please enter a valid ID number." << endl;                    
        cout << "Enter newsgroup ID: ";
        getline(cin, temp);
    }

    int id{stoi(temp)};

    writeNumber(conn, id);
    writeProtocol(conn, Protocol::PAR_NUM);
    cout << "Enter article ID: ";
    
    getline(cin, temp);

    while (!IsNumber(temp)) {
        cout << "Please enter a valid ID number." << endl;                    
        cout << "Enter article ID: ";
        getline(cin, temp);
    }

    id = stoi(temp);

    writeNumber(conn, id);
    writeProtocol(conn, Protocol::COM_END);
}

void getArticle(const Connection& conn) {
    string temp;
    writeProtocol(conn, Protocol::COM_GET_ART);
    writeProtocol(conn, Protocol::PAR_NUM);
    cout << "Newsgroup ID: ";
    
    getline(cin, temp);

    while (!IsNumber(temp)) {
        cout << "Please enter a valid ID number." << endl;                    
        cout << "Enter article ID: ";
        getline(cin, temp);
    }

    int id{stoi(temp)};

    writeNumber(conn, id);
    writeProtocol(conn, Protocol::PAR_NUM);
    cout << "Article ID: ";
    
    getline(cin, temp);

    while (!IsNumber(temp)) {
        cout << "Please enter a valid ID number." << endl;                    
        cout << "Enter article ID: ";
        getline(cin, temp);
    }

    id = stoi(temp);
    
    writeNumber(conn, id);
    writeProtocol(conn, Protocol::COM_END);
}

int app(const Connection& conn)
{
    int nbr;
    int id;
    string temp;
    string title;
    string author;
    string text;

    PrintInstructions();

    while (getline(cin, temp)) {
        
        while (!IsNumber(temp)) {
            cout << "Please select a valid option." << endl;
            PrintInstructions();
            getline(cin, temp);
        }

        nbr = stoi(temp);
        switch (nbr) {
            case 1:
                listNewsgroups(conn);
                break;

            case 2:
                createNewsgroup(conn);
                break;

            case 3:
                deleteNewsgroup(conn);
                break;

            case 4:
                listArticles(conn);
                break;

            case 5:
                createArticle(conn);
                break;

            case 6:
                deleteArticle(conn);
                break;

            case 7:
                getArticle(conn);
                break;
            
            default:
                writeProtocol(conn, Protocol::COM_LIST_NG);
                writeProtocol(conn, Protocol::COM_END);
                break;
        }

        try {
            char reply = readChar(conn);
            
            switch (static_cast<Protocol>(reply)) {
                case Protocol::ANS_LIST_NG:
                    handleListNewsgroupsResponse(conn);
                    break;
                    
                case Protocol::ANS_CREATE_NG:
                    handleCreateNewsgroupsResponse(conn);
                    break;

                case Protocol::ANS_DELETE_NG:
                    handleDeleteNewsgroupsResponse(conn);
                    break;

                case Protocol::ANS_LIST_ART:
                    handleListArticlesResponse(conn);
                    break;

                case Protocol::ANS_CREATE_ART:
                    handleCreateArticleResponse(conn);
                    break;

                case Protocol::ANS_DELETE_ART:
                    handleDeleteArticleResponse(conn);
                    break;

                case Protocol::ANS_GET_ART:
                    handleGetArticleResponse(conn);
                    break;
                
                default:
                    break;
            }
            
        } catch (ConnectionClosedException&) {
                cout << "No reply from server. Exiting." << endl;
                return 1;
        }

        PrintInstructions();
    }

    return 0;
}

int main(int argc, char* argv[])
{
        Connection conn = init(argc, argv);
        return app(conn);
}