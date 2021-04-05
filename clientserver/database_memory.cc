#include "database_memory.h"

DatabaseMemory::DatabaseMemory() {};

string DatabaseMemory::listNewsGroups() {
    return "getArticle";
}

void DatabaseMemory::createNewsGroup(string title) {};

void DatabaseMemory::deleteNewsGroup(int id) {}; 

string DatabaseMemory::listArticles(int id) {return "list articles";};

void DatabaseMemory::createArticle(int id, string author, string title, string text) {};

void DatabaseMemory::deleteArticle(int gid, int aid) {};

string DatabaseMemory::getArticle(int gid, int aid) {return "get article";};