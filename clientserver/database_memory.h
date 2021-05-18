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
        ~DatabaseMemory() =default;
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
        map<int, vector<Article>> newsgroupsArticles;
        map<int, string> newsgroupsTitles;
        map<int, int> articleIds;
        int currId{1};
};

#endif