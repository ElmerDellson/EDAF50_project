#ifndef DATABASE_H
#define DATABASE_H

#include <string>

using namespace std;

class Database {
    public:
        virtual string ListNewsgroups() = 0;
        virtual void CreateNewsgroup(string title) = 0;
        virtual void DeleteNewsgroup(int id) = 0; 
        virtual string ListArticles(int id) = 0;
        virtual void CreateArticle(int id, string author, string title, string text) = 0;
        virtual void DeleteArticle(int gid, int aid) = 0;
        virtual string GetArticle(int gid, int aid) = 0;
};

#endif