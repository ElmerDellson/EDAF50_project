#ifndef DATABASE_H
#define DATABASE_H

#include <string>

using namespace std;

class Database {
    public:
        virtual ~Database() =default;
        virtual string ListNewsgroups() = 0;
        virtual int NewsGroupNumber() = 0;
        virtual int ArticleNumber(int id) = 0;
        virtual void CreateNewsgroup(string title) = 0;
        virtual void DeleteNewsgroup(int id) = 0; 
        virtual string ListArticles(int id) = 0;
        virtual void CreateArticle(int id, string author, string title, string text) = 0;
        virtual void DeleteArticle(int gid, int aid) = 0;
        virtual string GetArticleTitle(int gid, int aid) = 0;
        virtual string GetArticleAuthor(int gid, int aid) = 0;
        virtual string GetArticleText(int gid, int aid) = 0;
};

#endif