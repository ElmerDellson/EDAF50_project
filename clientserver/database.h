#ifndef DATABASE_H
#define DATABASE_H

#include <string>

using namespace std;

class Database {
    public:
        virtual string getNewsGroups() = 0;
        virtual void createNewsGroup(string title) = 0;
        virtual void deleteNewsGroup(int id) = 0; 
        virtual string listArticles(int id) = 0;
        virtual void createArticle(int id, string author, string title, string text) = 0;
        virtual void deleteArticle(int gid, int aid) = 0;
        virtual string getArticle(int gid, int aid) = 0;
};

#endif