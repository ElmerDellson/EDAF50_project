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

bool isNumber(const std::string& s)
{
    std::string::const_iterator it = s.begin();
    while (it != s.end() && std::isdigit(*it)) 
        ++it;
    return !s.empty() && it == s.end();
}

int findMax(const vector<int>& vec) {
    int max{0};
    for (int i = 0; i < vec.size(); i++) {
        if (vec[i] > max)
            max = vec[i];
    }

    return max;
}

DatabaseDisk::DatabaseDisk() { 
    string metaFilePath{dbPath};
    metaFilePath.append("ng_meta.txt");

    if (!fileExists(metaFilePath)) {
        string metaFilePath{dbPath};
        metaFilePath.append("ng_meta.txt");

        ofstream stream(metaFilePath);

        stream << "0\n" << endl;
        stream.close();
    }
    
}

vector<string> DatabaseDisk::ListNewsgroups() {
    auto dirStream{opendir("database/newsgroups/")};
    struct dirent* dirEnt;

    vector<string> result;

    cout << "Testing" << endl;
    if (dirStream) {
        dirEnt = readdir(dirStream);
        while (dirEnt) {
            result.push_back(dirEnt->d_name);
            dirEnt = readdir(dirStream);
        }
    }









    string metaFilePath{dbPath};
    metaFilePath.append("ng_meta.txt");

    //vector<string> result;

    fstream readStream(metaFilePath);
    string helper;

    while (getline(readStream, helper)) {
        string name{helper};

        getline(readStream, helper);

        result.push_back(helper);
        result.push_back(name);
    }

    result.erase(result.begin(), result.begin()+2);

    return result;
}

int DatabaseDisk::NoOfNewsGroups() {
    string metaFilePath{dbPath};
    metaFilePath.append("ng_meta.txt");

    fstream readStream(metaFilePath);
    string helper;
    int noOfNgs{-2}; //compensating for lines that are not newsgroups

    while (getline(readStream, helper)) {
        noOfNgs++;
    }
    readStream.close();

    noOfNgs /= 2; //Since the meta file contains both names and IDs

    return noOfNgs;
}

int DatabaseDisk::NoOfArticles(int id) {
    return newsgroupsArticles.at(id).size();
}

bool DatabaseDisk::CreateNewsgroup(string title) {
    string path{dbPath};
    
    auto dirStream{opendir("database/newsgroups/")};
        
    if (dirStream) {
        struct dirent* dirEnt;
        vector<int> IDs{0};

        vector<string> dirNames;

        dirEnt = readdir(dirStream);
        while (dirEnt) {
            dirNames.push_back(dirEnt->d_name);
            dirEnt = readdir(dirStream);
        }

        dirNames.erase(dirNames.begin(), dirNames.begin() + 2);

        for (string s : dirNames) {
            string first{s.substr(0, s.find(' '))};

            if (isNumber(first))
                IDs.push_back(stoi(first));
        }            

        int highestID{findMax(IDs) + 1};

        cout << "highestID: " << highestID << endl;

        path.append("newsgroups/");
        path.append(to_string(highestID));
        path.append(" ");
        path.append(title);
        path.append("/");

        if (mkdir(path.c_str(), 0777) == -1) {
            cout << "Couldn't create directory " << path << endl;
            return false;
        }
    }
    
    
    /*else 
    {



        string metaFilePath{dbPath};
        metaFilePath.append("ng_meta.txt");

        fstream readStream(metaFilePath);
        string helper;
        string metaText;
        string highestIDString;

        getline(readStream, highestIDString);

        while (getline(readStream, helper)) {
            metaText.append(helper);
            metaText.append("\n");
        }
        readStream.close();

        ofstream stream(metaFilePath);

        int highestIDInt{stoi(highestIDString)};
        highestIDInt++;
        highestIDString = to_string(highestIDInt);

        metaText.append(title);
        metaText.append("\n");
        metaText.append(highestIDString);

        stream << highestIDString << endl;
        stream << metaText << endl;
        stream.close();

        cout << "Newsgroup " << title << " created" << endl;
    }*/

    return true;
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
    int temp = currId ++;
    newsgroupsArticles.at(id).push_back(Article(temp, title, author, text));

    cout << "CreateArticles DatabaseDisk" << endl;

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