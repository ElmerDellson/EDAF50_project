#include "database_memory.h"
#include <iostream>

using namespace std;

int main()
{
    Database* db;

    string temp = db->getArticle(1, 1);

    cout << "temp:" << temp << endl;

    cout << "Hello" << endl;

    return 0;
}