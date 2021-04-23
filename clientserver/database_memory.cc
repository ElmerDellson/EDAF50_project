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

int DatabaseMemory::NoOfNewsGroups(){
    return newsgroupsTitles.size();
}

int DatabaseMemory::NoOfArticles(int id){
    return newsgroupsarticles.at(id).size();
}

bool DatabaseMemory::CreateNewsgroup(string title) {
    int id = currId++;

    for (auto it = newsgroupsTitles.begin(); it != newsgroupsTitles.end(); ++it ) {
        if (it->second == title)
            return false;
    }

    newsgroupsTitles.insert(pair<int, string>(id, title));
    newsgroupsarticles.emplace(id, vector<Article>());

    return true;
}

bool DatabaseMemory::DeleteNewsgroup(int id) {
    for (auto it = newsgroupsTitles.begin(); it != newsgroupsTitles.end(); ++it ) {
        if (it->first == id) {
            newsgroupsTitles.erase(id);
            newsgroupsarticles.erase(id);
            return true;
        }
    }

    return false;
} 

vector<string> DatabaseMemory::ListArticles(int id) {
    vector<string> result;

    for (Article x : newsgroupsarticles.at(id)){
        result.push_back(to_string(x.getId()));
        
        result.push_back(x.getTitle());
        
    }
    return result;
}

bool DatabaseMemory::CreateArticle(int id, string author, string title, string text) {
    int temp = currId ++;
    newsgroupsarticles.at(id).push_back(Article(temp, title, author, text));

    return true;
}

bool DatabaseMemory::DeleteArticle(int gid, int aid) {
           vector<Article>& v = newsgroupsarticles.at(gid);
           v.erase(remove_if(v.begin(), v.end(), [& aid](Article x){return x.getId() == aid;}));

           return true;
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