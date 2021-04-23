#include "connection.h"
#include "connectionclosedexception.h"
#include "server.h"
#include "message_handler.h"

#include <cstdlib>
#include <iostream>
#include <memory>
#include <stdexcept>
#include <string>

using namespace std;

Server init(int argc, char* argv[])
{
    if (argc != 2) {
        cerr << "Usage: main_server port-number" << endl;
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
    Database* db;

    cout << "Please select Database Type.\n 1. In-memory\n 2. On disk" << endl;
    int selection;
    cin >> selection;

    switch(selection) {
        case 1:
            db = new DatabaseMemory{};
            cout << "Case 1: In-memory" << endl;
            break;
        default:
            db = new DatabaseDisk{};
            cout << "Case 2: On disk" << endl;
    };
    
    while (true) {
        auto conn = server.waitForActivity();
        MessageHandler mh{conn, db};
        if (conn != nullptr) {
            if (!mh.Handle()) {
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
