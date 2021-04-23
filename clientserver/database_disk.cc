#include "database_disk.h"
#include <algorithm>

DatabaseDisk::DatabaseDisk() { }

string DatabaseDisk::ListNewsgroups() {
    string result = "";
    for(pair<int , string> x : newsgroupsTitles){
        result.append(to_string(x.first));
        result.append(",");
        result.append(x.second);
        result.append(",");
    }

    cout << "ListNewsGroup DatabaseDisk" << endl;
    return result;
}

void DatabaseDisk::CreateNewsgroup(string title) {
    int temp = currId++;
    newsgroupsTitles.insert(pair<int, string>(temp,title));
    newsgroupsarticles.emplace(temp, vector<Article>());
    cout << "CreateNewsGroup DatabaseDisk" << endl;
}

void DatabaseDisk::DeleteNewsgroup(int id) {
    newsgroupsTitles.erase(id);
    newsgroupsarticles.erase(id);
    cout << "DeleteNewsGroup DatabaseDisk" << endl;
} 

string DatabaseDisk::ListArticles(int id) {
    string result = "";
    for(Article x : newsgroupsarticles.at(id)){
        result.append(to_string(x.getId()));
        result.append(",");
        result.append(x.getTitle());
        result.append(",");
    }

    cout << "ListArticles DatabaseDisk" << endl;
    return result;
}

void DatabaseDisk::CreateArticle(int id, string author, string title, string text) {
    int temp = currId ++;
    newsgroupsarticles.at(id).push_back(Article(temp, title, author, text));

    cout << "CreateArticles DatabaseDisk" << endl;
}

void DatabaseDisk::DeleteArticle(int gid, int aid) {
    vector<Article>& v = newsgroupsarticles.at(gid);
    v.erase(remove_if(v.begin(), v.end(), [& aid](Article x){return x.getId() == aid;})); 
    cout << "DeleteArticle DatabaseDisk" << endl;
}

string DatabaseDisk::GetArticle(int gid, int aid) {
    string result = "";
    for(Article x : newsgroupsarticles.at(gid)){
        if(x.getId() == aid){
            result += x.getAuthor() + " , " +  x.getTitle() + " , " + x.getArticle();
        }
    }

    cout << "GetArticle DatabaseDisk" << endl;
    return result;
}