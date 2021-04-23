#include "database_memory.h"
#include <algorithm>
#include <vector>

DatabaseMemory::DatabaseMemory() { }

vector<string> DatabaseMemory::ListNewsgroups() {
    vector<string> result;
    for(pair<int , string> x : newsgroupsTitles){
        result.push_back(to_string(x.first));
        result.push_back(x.second);
    }
    return result;
}

int DatabaseMemory::NewsGroupNumber(){
    return newsgroupsTitles.size();
}

int DatabaseMemory::ArticleNumber(int id){
    return newsgroupsarticles.at(id).size();
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

vector<string> DatabaseMemory::ListArticles(int id) {
    vector<string> result;
    for(Article x : newsgroupsarticles.at(id)){
        result.push_back(to_string(x.getId()));
        
        result.push_back(x.getTitle());
        
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

string DatabaseMemory::GetArticleTitle(int gid, int aid) {
    for(Article x : newsgroupsarticles.at(gid)){
        if(x.getId() == aid){
            return x.getTitle();
        }
    }
    return "";
}
string DatabaseMemory::GetArticleAuthor(int gid, int aid) {
    for(Article x : newsgroupsarticles.at(gid)){
        if(x.getId() == aid){
            return x.getAuthor();
        }
    }
    return "";
}
string DatabaseMemory::GetArticleText(int gid, int aid) {
    for(Article x : newsgroupsarticles.at(gid)){
        if(x.getId() == aid){
            return x.getArticle();
        }
    }
    return "";
}