#ifndef DATABASE_DISK_H
#define DATABASE_DISK_H

#include "database.h"
#include "article.h"

#include <iostream>
#include <map>
#include <vector>

using namespace std;

class DatabaseDisk: public Database {
    public:
        DatabaseDisk();
        ~DatabaseDisk() =default;
        string ListNewsgroups() override;
        int NewsGroupNumber() override;
        void CreateNewsgroup(string title) override;
        void DeleteNewsgroup(int id) override; 
        string ListArticles(int id) override;
        void CreateArticle(int id, string author, string title, string text) override;
        void DeleteArticle(int gid, int aid) override;
        string GetArticle(int gid, int aid) override;
    private:
        map<int, vector<Article>> newsgroupsarticles;
        map<int, string> newsgroupsTitles;
        int currId{0};
};

#endif