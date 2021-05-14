#include "database_disk.h"
#include <algorithm>
#include <iostream>
#include <fstream>
#include <sys/stat.h>
#include <sys/types.h>
#include <stdio.h>
#include <dirent.h>

//Check if a string is a number
bool IsNumber(const std::string& s)
{
    string::const_iterator it = s.begin();

    while (it != s.end() && std::isdigit(*it)) 
        ++it;
    
    return !s.empty() && it == s.end();
}

//Return the largest int in a vector. Only works for positive integers!
int FindMax(const vector<int>& vec) {
    int max{0};

    for (unsigned int i = 0; i < vec.size(); i++) {
        if (vec[i] > max)
            max = vec[i];
    }

    return max;
}

//Find the path to the newsgroup with ID id
string GetPathToNg(const int& id, const string& dbPath, const vector<string>& newsgroups) {
    string path{dbPath};

    for (unsigned int i = 0; i < newsgroups.size(); i+=2) {
        if (stoi(newsgroups[i]) == id) {
            path.append(to_string(id));
            path.append(" ");
            path.append(newsgroups[i+1]);
            break;
        }
    }

    if (path == dbPath) {
        cout << "ERROR: Newsgroup with ID " << id << " not found." << endl;
        return "";
    }

    return path;
}

//Find the path to the article with ID id in newsgroup specified by ng
string GetPathToArt(const int& id, string pathToNg, vector<string> articlesInNg) {
    string path{pathToNg.append("/")};

    for (unsigned int i = 0; i < articlesInNg.size(); i+=2) {
        if (stoi(articlesInNg[i]) == id) {
            path.append(to_string(id).append(" "));
            path.append(articlesInNg[i+1].append(".txt"));
            break;
        }
    }

    if (path == pathToNg) {
        cout << "ERROR: Article with id " << id << " in newsgroup " << pathToNg << " not found." << endl;
        return "";
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
    auto dirStream{opendir(GetPathToNg(id, dbPath, ListNewsgroups()).c_str())};
    int noOfArts{-2}; //compensating for "." and ".." directories

    if (dirStream) {
        auto fileEnt{readdir(dirStream)};

        while (fileEnt) {
            noOfArts++;
            fileEnt = readdir(dirStream);
        }
    }

    return noOfArts;
}

bool DatabaseDisk::CreateNewsgroup(string title) {
    string path{dbPath};
    auto dirStream{opendir(path.c_str())};
    
    if (dirStream) {
        vector<int> IDs{0};
        vector<string> newsgroups{ListNewsgroups()};

        //Get newsgroup IDs, check if a newsgroup with this title already exists
        for (unsigned int i = 0; i < newsgroups.size(); i+=2) {
            IDs.push_back(stoi(newsgroups[i]));

            if (newsgroups[i+1] == title) {
                cout << "ERROR: a newsgroup with that title already exists." << endl;
                return false;
            }
        }

        //Increment the highest current newsgroup ID
        int highestID{FindMax(IDs) + 1};

        //Build the path
        path.append(to_string(highestID).append(" "));
        path.append(title.append("/"));

        //Create the directory
        if (mkdir(path.c_str(), 0777) == -1) {
            cout << "ERROR: Unable to create directory." << path << endl;
            return false;
        }
        
        return true;
    }
    else 
    {
        cout << "ERROR: Unable to open directory stream" << path << endl;
        return false;
    }
}

bool DatabaseDisk::DeleteNewsgroup(int id) {
    string path{GetPathToNg(id, dbPath, ListNewsgroups())};
    
    if (remove(path.c_str()) == 0) {
        cout << "Newsgroup deleted successfully: " << path.c_str() << endl;
        return true;
    } else {
        cout << "ERROR: Couldn't delete newsgroup." << endl;
        return false;
    }

    return true;
} 

vector<string> DatabaseDisk::ListArticles(int id) {
    auto dirStream{opendir( GetPathToNg(id, dbPath, ListNewsgroups()).c_str() )};

    struct dirent* fileEnt;
    vector<string> result;

    if (dirStream) {
        fileEnt = readdir(dirStream);
        
        while (fileEnt) {
            string dirName{fileEnt->d_name};

            string artID{dirName.substr(0, dirName.find(' '))};

            result.push_back(artID);
            result.push_back(dirName.substr(dirName.find(' ') + 1, 
                             dirName.length() - (5 + artID.length())));

            fileEnt = readdir(dirStream);
        }
    }

    result.erase(result.begin(), result.begin() + 4); //Get rid of "." and ".." directories

    return result;
}

bool DatabaseDisk::CreateArticle(int id, string author, string title, string text) {
    string path{GetPathToNg(id, dbPath, ListNewsgroups()).append("/")};

    vector<int> IDs{0};
    vector<string> articles{ListArticles(id)};

    //Get article IDs in newsgroup 
    for (unsigned int i = 0; i < articles.size(); i+=2)
        IDs.push_back(stoi(articles[i]));

    //Increment the highest ID
    int highestID{FindMax(IDs) + 1};

    //Build path
    path.append(to_string(highestID).append(" "));
    path.append(title.append(".txt"));    

    ofstream stream(path);

    //Write article author and contents to file
    if (stream.is_open()) {
        stream << author << "\n" << endl;
        stream << text << endl;
        stream.close();
    }
    else {
        cout << "ERROR: Couldn't create file. Do you have permission to write to disk?" << endl;
        return false;
    }

    return true;
}

bool DatabaseDisk::DeleteArticle(int gid, int aid) {
    string path{GetPathToArt(aid, GetPathToNg(gid, dbPath, ListNewsgroups()), ListArticles(gid))};
    
    if (remove(path.c_str()) == 0) {
        cout << "Article deleted successfully: " << path.c_str() << endl;
        return true;
    } else {
        cout << "ERROR: Couldn't delete article." << endl;
        return false;
    }
}

string DatabaseDisk::GetArticleTitle(int gid, int aid) {
    string title;
    vector<string> articlesInNg{ListArticles(gid)};
    
    for (unsigned int i = 0; i < articlesInNg.size(); i+=2) {
        if (stoi(articlesInNg[i]) == aid) {
            title = articlesInNg[i+1];
            break;
        }
    }

    if (title.length() == 0) {
        cout << "ERROR: Article with id " << aid << " not found in newsgroup with id " << gid << endl;
        return "";
    }
    
    return title;
}

string DatabaseDisk::GetArticleAuthor(int gid, int aid) {
    string author;
    string path{GetPathToArt(aid, GetPathToNg(gid, dbPath, ListNewsgroups()), ListArticles(gid))};

    ifstream stream;
    stream.open(path.c_str());

    if (stream.is_open()) {
        getline(stream, author);

        return author;
    } 
    else {
        cout << "ERROR: Couldn't open article." << endl;
        return "";
    }
}

string DatabaseDisk::GetArticleText(int gid, int aid) {
    string text;
    string path{GetPathToArt(aid, GetPathToNg(gid, dbPath, ListNewsgroups()), ListArticles(gid))};

    ifstream stream;
    stream.open(path.c_str());

    if (stream.is_open()) {
        string helper;

        getline(stream, helper);
        getline(stream, helper);

        while (getline(stream, helper) ) {
            text.append(helper);
            text.append("\n");
        }
        
        return text;

    } else {
        cout << "ERROR: Couldn't open article." << endl;
        return "";
    }   
}