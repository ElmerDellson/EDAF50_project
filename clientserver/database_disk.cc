#include "database_disk.h"
#include <algorithm>
#include <iostream>
#include <fstream>
#include <sys/stat.h>
#include <sys/types.h>
#include <stdio.h>
#include <dirent.h>

//Check if a file exists
bool FileExists(const std::string& name) {
    if (FILE *file = fopen(name.c_str(), "r")) {
        fclose(file);
        return true;
    } else 
        return false;
}

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

DatabaseDisk::DatabaseDisk() {
    auto dirStream{opendir(dbPath.c_str())};

    if (!dirStream) {
        dirStream = opendir("database/");

        if (!dirStream) {
            if (mkdir("database/", 0777) == -1) 
                cout << "ERROR: Unable to initialize database directory." << endl;
            else 
                cout << "Database directory created." << endl;
        }

        if (mkdir(dbPath.c_str(), 0777) == -1) 
            cout << "ERROR: Unable to initialize newsgroups directory." << endl;
        else 
            cout << "Newsgroups directory created." << endl;
    }

    string path{dbPath};
    string metaFilePath{path.append("meta.txt")};
    if (!FileExists(metaFilePath)) {
        ofstream stream(metaFilePath);

        //Initalize meta file with the highest newsgroup ID (0)
        if (stream.is_open()) {
            stream << "0" << endl;
            stream.close();
        }
        else
            cout << "ERROR: Couldn't create newsgroups meta file. Do you have permission to write to disk? (DatabaseDisk)" << endl;   
    }
}

vector<string> DatabaseDisk::ListNewsgroups() {
    auto dirStream{opendir("database/newsgroups/")};
    struct dirent* dirEnt;
    vector<string> result;

    if (dirStream) {
        vector<pair<int, string>> newsgroups;
        dirEnt = readdir(dirStream);
        
        while (dirEnt) {
            string dirName{dirEnt->d_name};

            string id{dirName.substr(0, dirName.find(' '))}; //Get newsgroup ID
            string name{dirName.substr(dirName.find(' ') + 1)}; //Get newsgroup title
            
            if ((IsNumber(id))) { //Only add to newsgroups if we managed to get an ID
                pair<int, string> newsgroup(stoi(id), name);
                newsgroups.push_back(newsgroup);
            }

            dirEnt = readdir(dirStream); 
        }

        sort(newsgroups.begin(), newsgroups.end()); //Sort the newsgroups by ID, ascending

        for (pair<int, string> ng : newsgroups) { //Construct result vector: IDs and titles
            result.push_back(to_string(ng.first));
            result.push_back(ng.second);
        }
    } else
        cout << "ERROR: Couldn't open newsgroups directory (ListNewsGroups)." << endl;
    
    return result;
}

int DatabaseDisk::NoOfNewsGroups() {
    auto dirStream{opendir("database/newsgroups/")};
    int noOfNgs{-3}; //compensating for "." and ".." directories, and meta file

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
    int noOfArts{-3}; //compensating for "." and ".." directories, and "meta.txt"

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
    auto dirStream{opendir(dbPath.c_str())};

    if (dirStream) {
        vector<int> IDs{0};
        vector<string> newsgroups{ListNewsgroups()};

        //Get newsgroup IDs, check if a newsgroup with this title already exists
        for (unsigned int i = 0; i < newsgroups.size(); i+=2) {
            if (newsgroups[i+1] == title) {
                cout << "ERROR: a newsgroup with that title already exists (CreateNewsGroup)." << endl;
                return false;
            }
        }

        string metaPath{dbPath};
        string highestID;
        ifstream metaStreamRead;

        metaStreamRead.open(metaPath.append("meta.txt").c_str());

        if (metaStreamRead.is_open()) {
            if (getline(metaStreamRead, highestID)) {
                cout << "highestID: " << highestID << endl;
                highestID = to_string(stoi(highestID) + 1); //Increment the highest ID
                metaStreamRead.close();

                ofstream metaStreaWrite;
                metaStreaWrite.open(metaPath.c_str(), fstream::out | fstream::trunc);
                metaStreaWrite.write(highestID.c_str(), highestID.length());
                metaStreaWrite.close();
            }   
        }
        else {
            cout << "ERROR: Couldn't open newsgroups meta file (CreateNewsGroup)." << endl;
            return false;
        }

        //Build the path
        string ngPath{dbPath};
        ngPath.append(highestID.append(" "));
        ngPath.append(title.append("/"));

        //Create the directory
        if (mkdir(ngPath.c_str(), 0777) == -1) {
            cout << "ERROR: Unable to create directory (CreateNewsGroup)." << ngPath << endl;
            return false;
        }

        string ngMetaPath{ngPath};
        ngMetaPath.append("meta.txt");

        ofstream ngMetaStreamWrite;
        ngMetaStreamWrite.open(ngMetaPath.c_str());
        ngMetaStreamWrite.write("0", 1);
        ngMetaStreamWrite.close();
        
        return true;
    }
    else 
    {
        cout << "ERROR: Unable to open directory stream " << dbPath << " (CreateNewsGroup)." << endl;
        return false;
    }
}

bool DatabaseDisk::DeleteNewsgroup(int id) {
    string path{GetPathToNg(id, dbPath, ListNewsgroups())};
    string commandString{"rm -r '"};
    commandString.append(path.append("'"));
    
    int status{system(commandString.c_str())};

    if (status == 0)
        return true;
    else
        return false;
} 

vector<string> DatabaseDisk::ListArticles(int id) {
    auto dirStream{opendir( GetPathToNg(id, dbPath, ListNewsgroups()).c_str() )};
    struct dirent* fileEnt;
    vector<string> result;

    if (dirStream) {
        vector<pair<int, string>> articles;
        fileEnt = readdir(dirStream);
        
        while (fileEnt) {
            string dirName{fileEnt->d_name};

            string artID{dirName.substr(0, dirName.find(' '))}; //Get article ID
            string artTitle{dirName.substr(dirName.find(' ') + 1, 
                            dirName.length() - (5 + artID.length()))}; //Get article title

            if (IsNumber(artID)) {
                pair<int, string> article(stoi(artID), artTitle);
                articles.push_back(article);
            }

            fileEnt = readdir(dirStream);
        }

        sort(articles.begin(), articles.end());

        for (pair<int, string> art : articles) {
            result.push_back(to_string(art.first));
            result.push_back(art.second);
        }
    } else 
        cout << "ERROR: Couldn't open newsgroup (ListArticles)." << endl;

    return result;
}

bool DatabaseDisk::CreateArticle(int id, string author, string title, string text) {
    string path{GetPathToNg(id, dbPath, ListNewsgroups()).append("/")};
    string metaPath{path};
    string highestID;
    ifstream metaStreamRead;

    metaStreamRead.open(metaPath.append("meta.txt").c_str());

    if (metaStreamRead.is_open()) {
        if (getline(metaStreamRead, highestID)) {
            highestID = to_string(stoi(highestID) + 1); //Increment the highest ID
            metaStreamRead.close();

            ofstream metaStreamWrite;
            metaStreamWrite.open(metaPath.c_str(), fstream::out | fstream::trunc);
            metaStreamWrite.write(highestID.c_str(), highestID.length()); //Replace the highestID in meta.txt
            metaStreamWrite.close();
        }   
    }
    else {
        cout << "ERROR: Couldn't open newsgroup meta file (CreateArticle)." << endl;
        return false;
    }

    //Build path
    path.append(highestID.append(" "));
    path.append(title.append(".txt"));    

    //Write article author and contents to file
    ofstream stream(path);
    
    if (stream.is_open()) {
        stream << author << "\n" << endl;
        stream << text << endl;
        stream.close();
    }
    else {
        cout << "ERROR: Couldn't create file. Do you have permission to write to disk? (CreateArticle)" << endl;
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
        cout << "ERROR: Couldn't delete article (DeleteArticle)." << endl;
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
        cout << "ERROR: Couldn't open artile (GetArticleTitle)." << endl;
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
        cout << "ERROR: Couldn't open article (GetArticleAuthor)." << endl;
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

        //Get rid of the author and the extra line
        getline(stream, helper);
        getline(stream, helper);

        while (getline(stream, helper) ) {
            text.append(helper);
            text.append("\n");
        }
        
        return text;

    } else {
        cout << "ERROR: Couldn't open article (GetArticleText)." << endl;
        return "";
    }   
}