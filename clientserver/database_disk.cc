#include "database_disk.h"
#include <algorithm>
#include <iostream>
#include <fstream>
#include <sys/stat.h>
#include <sys/types.h>
#include <stdio.h>
#include <dirent.h>

bool fileExists(const string& name) {
    if (FILE *file = fopen(name.c_str(), "r")) {
        fclose(file);
        return true;
    } else
        return false;
}

bool IsNumber(const std::string& s)
{
    std::string::const_iterator it = s.begin();
    while (it != s.end() && std::isdigit(*it)) 
        ++it;
    return !s.empty() && it == s.end();
}

int FindMax(const vector<int>& vec) {
    int max{0};
    for (unsigned int i = 0; i < vec.size(); i++) {
        if (vec[i] > max)
            max = vec[i];
    }

    return max;
}

//Find the path to the newsgroup with ID id
string GetPathToNGWithID(const int& id, const string& dbPath, const vector<string>& newsgroups) {
    string path{dbPath};

    for (int i = 0; i < newsgroups.size(); i+=2) {
        if (stoi(newsgroups[i]) == id) {
            path.append(to_string(id));
            path.append(" ");
            path.append(newsgroups[i+1]);
            break;
        }
    }

    return path;
}

DatabaseDisk::DatabaseDisk() { }

vector<string> DatabaseDisk::ListNewsgroups() {
    auto dirStream{opendir("database/newsgroups/")};
    struct dirent* dirEnt;
    vector<string> result;

    if (dirStream) {
        dirEnt = readdir(dirStream);
        while (dirEnt) {
            string dirName{dirEnt->d_name};

            result.push_back(dirName.substr(0, dirName.find(' ')));
            result.push_back(dirName.substr(dirName.find(' ') + 1));

            dirEnt = readdir(dirStream);
        }
    }

    result.erase(result.begin(), result.begin() + 4); //Get rid of "." and ".." directories

    return result;
}

int DatabaseDisk::NoOfNewsGroups() {
    auto dirStream{opendir("database/newsgroups/")};
    int noOfNgs{-2}; //compensating for "." and ".." directories

    if (dirStream) {
        auto dirEnt{readdir(dirStream)};
        while (dirEnt) {
            noOfNgs++;
            dirEnt = readdir(dirStream);
        }
    }

    return noOfNgs;
}

int DatabaseDisk::NoOfArticles(int id) {


    return newsgroupsArticles.at(id).size();
}

bool DatabaseDisk::CreateNewsgroup(string title) {
    string path{dbPath};
    
    auto dirStream{opendir("database/newsgroups/")};
        
    if (dirStream) {
        vector<int> IDs{0};
        vector<string> newsgroups{ListNewsgroups()};

        for (unsigned int i = 0; i < newsgroups.size(); i+=2) {
            IDs.push_back(stoi(newsgroups[i]));

            if (newsgroups[i+1] == title) {
                cout << "Couldn't create newsgroup: a group with that title already exists." << endl;
                return false;
            }
        }

        int highestID{FindMax(IDs) + 1};

        path.append(to_string(highestID));
        path.append(" ");
        path.append(title);
        path.append("/");

        if (mkdir(path.c_str(), 0777) == -1) {
            cout << "Couldn't create newsgroup: unable to create directory." << path << endl;
            return false;
        }
        
        return true;
    }
    else 
    {
        cout << "Couldn't create newsgroup: unable to open directory stream" << path << endl;
        return false;
    }
}

bool DatabaseDisk::DeleteNewsgroup(int id) {
    newsgroupsTitles.erase(id);
    newsgroupsArticles.erase(id);
    cout << "DeleteNewsGroup DatabaseDisk" << endl;

    return true;
} 

vector<string> DatabaseDisk::ListArticles(int id) {
    vector<string> result;
    for(Article x : newsgroupsArticles.at(id)){
        result.push_back(to_string(x.getId()));
        
        result.push_back(x.getTitle());
    }

    cout << "ListArticles DatabaseDisk" << endl;
    return result;
}

bool DatabaseDisk::CreateArticle(int id, string author, string title, string text) {
    /*int temp = currId++;
    newsgroupsArticles.at(id).push_back(Article(temp, title, author, text));

    cout << "CreateArticles DatabaseDisk" << endl;*/

    string path{GetPathToNGWithID(id, dbPath, ListNewsgroups())};
    path.append("/");
    path.append(to_string(id));
    path.append(" ");
    path.append(title);
    path.append(".txt");

    cout << "path: " << path << endl;

    ofstream stream;
    stream.open(path);

    if (!stream.is_open()) {
        cout << "Couldn't create file" << endl;
        return false;
    }

    stream << author << "\n" << endl;
    stream << text << endl;

    stream.close();

    return true;
}

bool DatabaseDisk::DeleteArticle(int gid, int aid) {
    vector<Article>& v = newsgroupsArticles.at(gid);
    v.erase(remove_if(v.begin(), v.end(), [& aid](Article x){return x.getId() == aid;})); 
    cout << "DeleteArticle DatabaseDisk" << endl;
    
    return true;
}

string DatabaseDisk::GetArticleTitle(int gid, int aid) {
    for(Article x : newsgroupsArticles.at(gid)){
        if(x.getId() == aid){
            return x.getTitle();
        }
    }
    return "";
}
string DatabaseDisk::GetArticleAuthor(int gid, int aid) {
    for(Article x : newsgroupsArticles.at(gid)){
        if(x.getId() == aid){
            return x.getAuthor();
        }
    }
    return "";
}
string DatabaseDisk::GetArticleText(int gid, int aid) {
    for(Article x : newsgroupsArticles.at(gid)){
        if(x.getId() == aid){
            return x.getArticle();
        }
    }
    return "";
}