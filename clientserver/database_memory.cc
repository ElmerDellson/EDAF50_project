#include "database_memory.h"
#include <algorithm>
#include <vector>
#include <stdexcept>

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
    return newsgroupsArticles.at(id).size();
}

bool DatabaseMemory::CreateNewsgroup(string title) {
    for (auto it = newsgroupsTitles.begin(); it != newsgroupsTitles.end(); ++it ) {
        if (it->second == title)
            return false;
    }
    int id = currId++;
    newsgroupsTitles.insert(pair<int, string>(id, title));
    newsgroupsArticles.emplace(id, vector<Article>());
    articleIds.emplace(id, 1);

    return true;
}

bool DatabaseMemory::DeleteNewsgroup(int id) {
    for (auto it = newsgroupsTitles.begin(); it != newsgroupsTitles.end(); ++it ) {
        if (it->first == id) {
            newsgroupsTitles.erase(id);
            newsgroupsArticles.erase(id);
            return true;
        }
    }

    return false;
} 

vector<string> DatabaseMemory::ListArticles(int id) {
    vector<string> result;
    bool foundId = false;

    for (auto it = newsgroupsTitles.begin(); it != newsgroupsTitles.end(); ++it ) {
        if (it->first == id) {
            foundId = true;
        }
    }

    if(!foundId){
        throw invalid_argument("nonexsitant id");
    }

    for (Article x : newsgroupsArticles.at(id)){
        result.push_back(to_string(x.getId()));
        
        result.push_back(x.getTitle());
        
    }
    return result;
}

bool DatabaseMemory::CreateArticle(int id, string author, string title, string text) {
    int temp = articleIds.at(id) ++;
    for (auto it = newsgroupsTitles.begin(); it != newsgroupsTitles.end(); ++it ) {
        if (it->first == id) {
            newsgroupsArticles.at(id).push_back(Article(temp, title, author, text));
            return true;
        }
    }

    return false;
}

bool DatabaseMemory::DeleteArticle(int gid, int aid) {
        bool foundId = false;
        bool foundId2 = false;

        for (auto it = newsgroupsTitles.begin(); it != newsgroupsTitles.end(); ++it ) {
            if (it->first == gid) {
                foundId = true;
            }
        }

        if(!foundId){
            throw invalid_argument("nonexsitant group id");
        }
        vector<Article>& v = newsgroupsArticles.at(gid);

        for(Article a : v){
            if( a.getId() == aid)
                foundId2 = true;
        }

        if(!foundId2)
            return false;

        v.erase(remove_if(v.begin(), v.end(), [& aid](Article x){return x.getId() == aid;}));

        return true;
}

string DatabaseMemory::GetArticleTitle(int gid, int aid) {
    bool foundId = false;
    for (auto it = newsgroupsTitles.begin(); it != newsgroupsTitles.end(); ++it ) {
            if (it->first == gid) {
                foundId = true;
            }
        }

    if(!foundId){
        throw invalid_argument("nonexsitant group id");
    }

    vector<Article>& v = newsgroupsArticles.at(gid);
    
    for(Article a : v){
        if( a.getId() == aid)
            foundId = true;
    }

    if(!foundId){
        throw out_of_range("nonexsitant article id");
    }

    for(Article x : newsgroupsArticles.at(gid)){
        if(x.getId() == aid){
            return x.getTitle();
        }
    }
    return "";
}
string DatabaseMemory::GetArticleAuthor(int gid, int aid) {
    for(Article x : newsgroupsArticles.at(gid)){
        if(x.getId() == aid){
            return x.getAuthor();
        }
    }
    return "";
}
string DatabaseMemory::GetArticleText(int gid, int aid) {
    for(Article x : newsgroupsArticles.at(gid)){
        if(x.getId() == aid){
            return x.getArticle();
        }
    }
    return "";
}