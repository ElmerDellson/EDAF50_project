#include "database_disk.h"
#include <algorithm>

DatabaseDisk::DatabaseDisk() { }

vector<string> DatabaseDisk::ListNewsgroups() {
    vector<string> result;
    for(pair<int , string> x : newsgroupsTitles){
        result.push_back(to_string(x.first));
        result.push_back(x.second);
    }
    return result;
}

int DatabaseDisk::NoOfNewsGroups(){
    return newsgroupsTitles.size();
}

int DatabaseDisk::NoOfArticles(int id){
    return newsgroupsarticles.at(id).size();
}

bool DatabaseDisk::CreateNewsgroup(string title) {
    int temp = currId++;
    newsgroupsTitles.insert(pair<int, string>(temp,title));
    newsgroupsarticles.emplace(temp, vector<Article>());
    cout << "CreateNewsGroup DatabaseDisk" << endl;

    return true;
}

bool DatabaseDisk::DeleteNewsgroup(int id) {
    newsgroupsTitles.erase(id);
    newsgroupsarticles.erase(id);
    cout << "DeleteNewsGroup DatabaseDisk" << endl;

    return true;
} 

vector<string> DatabaseDisk::ListArticles(int id) {
    vector<string> result;
    for(Article x : newsgroupsarticles.at(id)){
        result.push_back(to_string(x.getId()));
        
        result.push_back(x.getTitle());
        
    }

    cout << "ListArticles DatabaseDisk" << endl;
    return result;
}

bool DatabaseDisk::CreateArticle(int id, string author, string title, string text) {
    int temp = currId ++;
    newsgroupsarticles.at(id).push_back(Article(temp, title, author, text));

    cout << "CreateArticles DatabaseDisk" << endl;

    return true;
}

bool DatabaseDisk::DeleteArticle(int gid, int aid) {
    vector<Article>& v = newsgroupsarticles.at(gid);
    v.erase(remove_if(v.begin(), v.end(), [& aid](Article x){return x.getId() == aid;})); 
    cout << "DeleteArticle DatabaseDisk" << endl;
    
    return true;
}

string DatabaseDisk::GetArticleTitle(int gid, int aid) {
    for(Article x : newsgroupsarticles.at(gid)){
        if(x.getId() == aid){
            return x.getTitle();
        }
    }
    return "";
}
string DatabaseDisk::GetArticleAuthor(int gid, int aid) {
    for(Article x : newsgroupsarticles.at(gid)){
        if(x.getId() == aid){
            return x.getAuthor();
        }
    }
    return "";
}
string DatabaseDisk::GetArticleText(int gid, int aid) {
    for(Article x : newsgroupsarticles.at(gid)){
        if(x.getId() == aid){
            return x.getArticle();
        }
    }
    return "";
}