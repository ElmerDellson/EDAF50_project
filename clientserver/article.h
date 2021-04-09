#ifndef ARTICLE_H
#define ARTICLE_H

#include <string>

using namespace std;

class Article{
    public:
        Article();
        Article(int id, string title, string author, string article);
        int getId();
        string getTitle();
        string getAuthor();
        string getArticle();
    private:
        int id;
        string title;
        string author;
        string article;
};

#endif