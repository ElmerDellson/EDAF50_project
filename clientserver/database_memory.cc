#include "database_memory.h"

DatabaseMemory::DatabaseMemory() {


}

string DatabaseMemory::listNewsGroups() {
    string result = "";
    for(pair<int , string> x : newsgroupsTitles){
        result.append("hej" );
        result.append(",");
        result.append(x.second);
        result.append(",");
    }
    return result;
}

void DatabaseMemory::createNewsGroup(string title) {
    int temp = currId++;
    newsgroupsTitles.insert(pair<int, string>(temp,title));
    newsgroupsarticles.emplace(temp, vector<Article>());
}

void DatabaseMemory::deleteNewsGroup(int id) {
    newsgroupsTitles.erase(id);
    newsgroupsarticles.erase(id);
} 

string DatabaseMemory::listArticles(int id) {
    string result = "";
    for(Article x : newsgroupsarticles.at(id)){
        result.append("hej");
        result.append(",");
        result.append(x.getTitle());
        result.append(",");
    }
    return result;
    }

void DatabaseMemory::createArticle(int id, string author, string title, string text) {
    int temp = currId ++;
    newsgroupsarticles.at(id).push_back(Article(temp, title, author, text));
}

void DatabaseMemory::deleteArticle(int gid, int aid) {
           
}

string DatabaseMemory::getArticle(int gid, int aid) {
    return "get article";
    }