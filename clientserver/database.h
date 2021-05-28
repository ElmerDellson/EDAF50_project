#ifndef DATABASE_H
#define DATABASE_H

#include <string>
#include <vector>

using namespace std;

class Database {
    public:
        virtual ~Database() =default;
        virtual vector<string> ListNewsgroups() = 0; //Alternatingly IDs and titles
        virtual int NoOfNewsGroups() = 0;
        virtual int NoOfArticles(int id) = 0;
        virtual bool CreateNewsgroup(string title) = 0;
        virtual bool DeleteNewsgroup(int id) = 0; 
        virtual vector<string> ListArticles(int id) = 0;
        virtual bool CreateArticle(int id, string author, string title, string text) = 0;
        virtual bool DeleteArticle(int gid, int aid) = 0;
        virtual string GetArticleTitle(int gid, int aid) = 0;
        virtual string GetArticleAuthor(int gid, int aid) = 0;
        virtual string GetArticleText(int gid, int aid) = 0;
};

#endif