#include "database_memory.h"
#include <algorithm>

DatabaseMemory::DatabaseMemory() { }

string DatabaseMemory::ListNewsgroups() {
    string result = "";
    for(pair<int , string> x : newsgroupsTitles){
        result.append(to_string(x.first));
        result.append(",");
        result.append(x.second);
        result.append(",");
    }
    return result;
}

void DatabaseMemory::CreateNewsgroup(string title) {
    int temp = currId++;
    newsgroupsTitles.insert(pair<int, string>(temp,title));
    newsgroupsarticles.emplace(temp, vector<Article>());
}

void DatabaseMemory::DeleteNewsgroup(int id) {
    newsgroupsTitles.erase(id);
    newsgroupsarticles.erase(id);
} 

string DatabaseMemory::ListArticles(int id) {
    string result = "";
    for(Article x : newsgroupsarticles.at(id)){
        result.append(to_string(x.getId()));
        result.append(",");
        result.append(x.getTitle());
        result.append(",");
    }
    return result;
}

void DatabaseMemory::CreateArticle(int id, string author, string title, string text) {
    int temp = currId ++;
    newsgroupsarticles.at(id).push_back(Article(temp, title, author, text));
}

void DatabaseMemory::DeleteArticle(int gid, int aid) {
           vector<Article>& v = newsgroupsarticles.at(gid);
           v.erase(remove_if(v.begin(), v.end(), [& aid](Article x){return x.getId() == aid;})); 
}

string DatabaseMemory::GetArticle(int gid, int aid) {
    string result = "";
    for(Article x : newsgroupsarticles.at(gid)){
        if(x.getId() == aid){
            result += x.getAuthor() + " , " +  x.getTitle() + " , " + x.getArticle();
        }
    }
    return result;
}