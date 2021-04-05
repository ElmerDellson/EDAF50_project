#ifndef DATABASE_MEMORY_H
#define DATABASE_MEMORY_H

#include "database.h"

using namespace std;

class DatabaseMemory: public Database {
    public:
        DatabaseMemory();
        string listNewsGroups();
        void createNewsGroup(string title);
        void deleteNewsGroup(int id); 
        string listArticles(int id);
        void createArticle(int id, string author, string title, string text);
        void deleteArticle(int gid, int aid);
        string getArticle(int gid, int aid);
};

#endif