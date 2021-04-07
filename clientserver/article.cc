#include "article.h"

Article::Article(){}

Article::Article(int id, string title, string author, string article) : id{id}, title{title}, author{author}, article{article} {}

string Article::getArticle(){
    return article;
}

string Article::getAuthor(){
    return author;
}

string Article::getTitle(){
    return title;
}

int Article::getId(){
    return id;
}