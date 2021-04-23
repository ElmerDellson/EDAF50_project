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
        vector<string> ListNewsgroups() override;
        int NoOfNewsGroups() override;
        int NoOfArticles(int id) override;
        bool CreateNewsgroup(string title) override;
        bool DeleteNewsgroup(int id) override; 
        vector<string> ListArticles(int id) override;
        bool CreateArticle(int id, string author, string title, string text) override;
        bool DeleteArticle(int gid, int aid) override;
        string GetArticleTitle(int gid, int aid) override;
        string GetArticleAuthor(int gid, int aid) override;
        string GetArticleText(int gid, int aid) override;
    private:
        map<int, vector<Article>> newsgroupsarticles;
        map<int, string> newsgroupsTitles;
        int currId{0};
};

#endif