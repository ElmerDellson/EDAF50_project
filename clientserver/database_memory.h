#ifndef DATABASE_MEMORY_H
#define DATABASE_MEMORY_H

#include "database.h"
#include "article.h"

#include <map>
#include <vector>

using namespace std;

class DatabaseMemory: public Database {
    public:
        DatabaseMemory();
        string ListNewsgroups();
        void CreateNewsgroup(string title);
        void DeleteNewsgroup(int id); 
        string ListArticles(int id);
        void CreateArticle(int id, string author, string title, string text);
        void DeleteArticle(int gid, int aid);
        string GetArticle(int gid, int aid);
    private:
        map<int, vector<Article>> newsgroupsarticles;
        map<int, string> newsgroupsTitles;
        int currId{0};
};

#endif