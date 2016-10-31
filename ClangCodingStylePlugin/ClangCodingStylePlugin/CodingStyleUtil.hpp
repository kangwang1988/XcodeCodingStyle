//
//  CodingStyleUtil.hpp
//  ClangCodingStylePlugin
//
//  Created by KyleWong on 14/10/2016.
//
//

#ifndef CodingStyleUtil_hpp
#define CodingStyleUtil_hpp
#include <string>
#include<algorithm>
#include <functional>
#include <string>
#include <fstream>
#include <vector>
#include <map>

using namespace std;

static vector<string> split(const string &s, char delim) {
    vector<string> elems;
    stringstream ss;
    ss.str(s);
    string item;
    while (getline(ss, item, delim)) {
        elems.push_back(item);
    }
    return elems;
}

static inline void remove_char_from_string(string &str,char ch)
{
    str.erase(remove(str.begin(), str.end(), ch), str.end());
}

static inline void remove_blank(string &str)
{
    remove_char_from_string(str,' ');
    remove_char_from_string(str,'\t');
    remove_char_from_string(str,'\r');
    remove_char_from_string(str,'\n');
}

static inline bool has_suffix(const std::string &str, const std::string &suffix)
{
    return str.size() >= suffix.size() &&
    str.compare(str.size() - suffix.size(), suffix.size(), suffix) == 0;
}

static inline string &ltrim(std::string &s) {
    s.erase(s.begin(), find_if(s.begin(), s.end(),
                               not1(ptr_fun<int, int>(isspace))));
    return s;
}

// trim from end
static inline string &rtrim(string &s) {
    s.erase(find_if(s.rbegin(), s.rend(),
                    not1(ptr_fun<int, int>(isspace))).base(), s.end());
    return s;
}

// trim from both ends
static inline string &trim(string &s) {
    return ltrim(rtrim(s));
}

static string formatPropertyDelegateType(string name){
    string newName(name);
    if(name.find("<")!=string::npos && name.find(">")!=string::npos){
        replace(newName.begin(),newName.end(),'\t',' ');
        replace(newName.begin(),newName.end(),'\r',' ');
        replace(newName.begin(),newName.end(),'\n',' ');
        newName.erase (std::remove(newName.begin(), newName.end(), ' '), newName.end());
        int npos = newName.find(")");
        if(npos==string::npos){
            npos = newName.find("@property");
            if(npos!=string::npos)
                return string(name);
            npos+=string("@property").length();
        }
        newName.insert(npos+1," ");
        newName.insert(newName.length()," ");
    }
    return string(newName);
}

static string formatObjcMethodName(string name){
    replace(name.begin(),name.end(),'\r',' ');
    replace(name.begin(),name.end(),'\n',' ');
    replace(name.begin(),name.end(),'\t',' ');
    vector<string> vec = split(name,':');
    vector<string> lineVec;
    string lPart,rPart;
    size_t nMaxDotPos = 0,pos;
    for(int i=0;i<vec.size();i++){
        string str = vec.at(i);
        trim(str);
        if(i==0){
            lPart = str;
            continue;
        }
        string tmpStr;
        pos = str.find(")");
        rPart=str.substr(0,pos+1);
        tmpStr = str.substr(pos+1,str.length()-pos-1);
        trim(tmpStr);
        pos = tmpStr.find(" ");
        if(pos != string::npos){
            rPart+=tmpStr.substr(0,pos+1);
            lineVec.push_back(trim(lPart)+":"+trim(rPart));
            nMaxDotPos = (lPart.length()>nMaxDotPos?lPart.length():nMaxDotPos);
            lPart = tmpStr.substr(pos+1,tmpStr.length()-pos-1);
        }
        else{
            rPart+=tmpStr;
            lineVec.push_back(trim(lPart)+":"+trim(rPart));
        }
    }
    if(!lineVec.size() && lPart.length()){
        lineVec.push_back(trim(lPart));
    }
    string joinedStr;
    if(lineVec.size()==1){
        joinedStr = lineVec.at(0);
    }
    else{
        for(size_t i=0;i!=lineVec.size();i++){
            string line = lineVec.at(i);
            pos = line.find(":");
            string tmpStr(i==0?0:nMaxDotPos-pos,' ');
            joinedStr+=tmpStr+line;
            if(i!=lineVec.size()-1)
                joinedStr+="\n";
        }
    }
    return joinedStr;
}

static string formatObjcMessageCall(string name){
    replace(name.begin(),name.end(),'\r',' ');
    replace(name.begin(),name.end(),'\n',' ');
    replace(name.begin(),name.end(),'\t',' ');
    vector<string> vec = split(name,':');
    vector<string> lineVec;
    string lPart,rPart;
    size_t nMaxDotPos = 0,pos;
    for(int i=0;i<vec.size();i++){
        string str = vec.at(i);
        trim(str);
        if(i==0){
            lPart = str;
            continue;
        }
        pos = str.find(" ");
        if(pos != string::npos){
            rPart=str.substr(0,pos+1);
            lineVec.push_back(trim(lPart)+":"+trim(rPart));
            nMaxDotPos = (lPart.length()>nMaxDotPos?lPart.length():nMaxDotPos);
            lPart = str.substr(pos+1,str.length()-pos-1);
        }
        else{
            rPart=str;
            lineVec.push_back(trim(lPart)+":"+trim(rPart));
        }
    }
    if(!lineVec.size() && lPart.length()){
        lineVec.push_back(trim(lPart));
    }
    string joinedStr;
    if(lineVec.size()==1){
        joinedStr = lineVec.at(0);
    }
    else{
        for(size_t i=0;i!=lineVec.size();i++){
            string line = lineVec.at(i);
            pos = line.find(":");
            string tmpStr(i==0?0:nMaxDotPos-pos,' ');
            joinedStr+=tmpStr+line;
            if(i!=lineVec.size()-1)
                joinedStr+="\n";
        }
    }
    return joinedStr;
}
#endif /* CodingStyleUtil_hpp */
