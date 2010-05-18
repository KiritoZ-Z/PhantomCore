/* 
 * Copyright (C) 2005-2008 MaNGOS <http://www.mangosproject.org/>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 */

#include "Config/Config.h"

#ifndef DOTCONFPP_H
#define DOTCONFPP_H

#include <list>

#include <stdarg.h>
#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include <errno.h>

#include <sys/types.h>
#include <sys/stat.h>

#ifdef WIN32
#define PATH_MAX _MAX_PATH
#define snprintf _snprintf
#define strcasecmp stricmp
#define realpath(path,resolved_path) _fullpath(resolved_path, path, _MAX_PATH)
#include <io.h>
#else
#include <unistd.h>
#include <limits.h>
#include <stdint.h>
#include <strings.h>
#endif

#ifndef ASYNC_DNS_MEMPOOL_H
#define ASYNC_DNS_MEMPOOL_H

#include <stdlib.h>
#include <string.h>
#include <sys/types.h>

#undef free
#undef calloc
#undef strdup

class AsyncDNSMemPool
{
private:
    struct PoolChunk {
        void * pool;
        size_t pos;
        size_t size;

        PoolChunk(size_t _size);
        ~PoolChunk();
    };
    PoolChunk ** chunks;
    size_t chunksCount;
    size_t defaultSize;

    size_t poolUsage;
    size_t poolUsageCounter;

    void addNewChunk(size_t size);

public:
    AsyncDNSMemPool(size_t _defaultSize = 4096);
    virtual ~AsyncDNSMemPool();

    int initialize();
    void free();
    void * alloc(size_t size);
    void * calloc(size_t size);
    char * strdup(const char *str);
};

#endif

class DOTCONFDocument;

class DOTCONFDocumentNode
{
friend class DOTCONFDocument;
private:
    DOTCONFDocumentNode * previousNode;
    DOTCONFDocumentNode * nextNode;
    DOTCONFDocumentNode * parentNode;
    DOTCONFDocumentNode * childNode;
    char ** values;
    int valuesCount;
    char * name;
    const DOTCONFDocument * document;
    int lineNum;
    char * fileName;
    bool closed;

    void pushValue(char * _value);

public:
    DOTCONFDocumentNode();
    ~DOTCONFDocumentNode();

    const char * getConfigurationFileName()const { return fileName; }
    int getConfigurationLineNumber() const { return lineNum; }

    const DOTCONFDocumentNode * getNextNode() const { return nextNode; }
    const DOTCONFDocumentNode * getPreviuosNode() const { return previousNode; }
    const DOTCONFDocumentNode * getParentNode() const { return parentNode; }
    const DOTCONFDocumentNode * getChildNode() const { return childNode; }
    const char* getValue(int index = 0) const;
    const char * getName() const { return name; }
    const DOTCONFDocument * getDocument() const { return document; }
};

#if PLATFORM == PLATFORM_WINDOWS
#  define ATTR_NORETURN
#  define ATTR_PRINTF(F,V)
#else
#  define ATTR_NORETURN __attribute__((noreturn))
#  define ATTR_PRINTF(F,V) __attribute__ ((format (printf, F, V)))
#endif

class DOTCONFDocument
{
public:
    enum CaseSensitive { CASESENSETIVE, CASEINSENSETIVE };
protected:
    AsyncDNSMemPool * mempool;
private:
    DOTCONFDocumentNode * curParent;
    DOTCONFDocumentNode * curPrev;
    int curLine;
    bool quoted;
    std::list<DOTCONFDocumentNode*> nodeTree;
    std::list<char*> requiredOptions;
    std::list<char*> processedFiles;
    FILE * file;
    char * fileName;
    std::list<char*> words;
    int (*cmp_func)(const char *, const char *);

    int checkRequiredOptions();
    int parseLine();
    int parseFile(DOTCONFDocumentNode * _parent = NULL);
    int checkConfig(const std::list<DOTCONFDocumentNode*>::iterator & from);
    int cleanupLine(char * line);
    char * getSubstitution(char * macro, int lineNum);
    int macroSubstitute(DOTCONFDocumentNode * tagNode, int valueIndex);

protected:
    virtual void error(int lineNum, const char * fileName, const char * fmt, ...) ATTR_PRINTF(4,5);

public:
    DOTCONFDocument(CaseSensitive caseSensitivity = CASESENSETIVE);
    virtual ~DOTCONFDocument();

    int setContent(const char * _fileName);

    void setRequiredOptionNames(const char ** requiredOptionNames);
    const DOTCONFDocumentNode * getFirstNode() const;
    const DOTCONFDocumentNode * findNode(const char * nodeName, const DOTCONFDocumentNode * parentNode = NULL, const DOTCONFDocumentNode * startNode = NULL) const;
};

#endif


AsyncDNSMemPool::PoolChunk::PoolChunk(size_t _size):
    pool(NULL), pos(0), size(_size)
{
    pool = ::malloc(size);
}

AsyncDNSMemPool::PoolChunk::~PoolChunk()
{
    ::free(pool);
}

AsyncDNSMemPool::AsyncDNSMemPool(size_t _defaultSize):
    chunks(NULL), chunksCount(0), defaultSize(_defaultSize),
    poolUsage(0), poolUsageCounter(0)
{
}

AsyncDNSMemPool::~AsyncDNSMemPool()
{
    for(size_t i = 0; i<chunksCount; i++){
        delete chunks[i];
    }
    ::free(chunks);
}

int AsyncDNSMemPool::initialize()
{
    chunksCount = 1;
    chunks = (PoolChunk**)::malloc(sizeof(PoolChunk*));
    if(chunks == NULL)
        return -1;

    chunks[chunksCount-1] = new PoolChunk(defaultSize);

    return 0;
}

void AsyncDNSMemPool::addNewChunk(size_t size)
{
    ++chunksCount;
    chunks = (PoolChunk**)::realloc(chunks, chunksCount*sizeof(PoolChunk*));
    if(size <= defaultSize)
        chunks[chunksCount-1] = new PoolChunk(defaultSize);
    else
        chunks[chunksCount-1] = new PoolChunk(size);
}

void * AsyncDNSMemPool::alloc(size_t size)
{
    PoolChunk * chunk = NULL;
    for(size_t i = 0; i<chunksCount; i++){
        chunk = chunks[i];
        if((chunk->size - chunk->pos) >= size){
            chunk->pos += size;
            return ((char*)chunk->pool) + chunk->pos - size;
        }
    }
    addNewChunk(size);
    chunks[chunksCount-1]->pos = size;
    return chunks[chunksCount-1]->pool;
}

void AsyncDNSMemPool::free()
{
    size_t pu = 0;
    size_t psz = 0;
    ++poolUsageCounter;

    for(size_t i = 0; i<chunksCount; i++){
        pu += chunks[i]->pos;
        psz += chunks[i]->size;
        chunks[i]->pos = 0;
    }
    poolUsage=(poolUsage>pu)?poolUsage:pu;

    if(poolUsageCounter >= 10 && chunksCount > 1){
        psz -= chunks[chunksCount-1]->size;
        if(poolUsage < psz){
            --chunksCount;
            delete chunks[chunksCount];
        }
        poolUsage = 0;
        poolUsageCounter = 0;
    }
}

void * AsyncDNSMemPool::calloc(size_t size)
{
    return ::memset(this->alloc(size), 0, size);
}

char * AsyncDNSMemPool::strdup(const char *str)
{
    return ::strcpy((char*)this->alloc(strlen(str)+1), str);
}


#if !defined(R_OK)
#define R_OK 04
#endif

DOTCONFDocumentNode::DOTCONFDocumentNode():previousNode(NULL), nextNode(NULL), parentNode(NULL), childNode(NULL),
    values(NULL), valuesCount(0),
    name(NULL), lineNum(0), fileName(NULL), closed(true)
{
}

DOTCONFDocumentNode::~DOTCONFDocumentNode()
{
    free(name);
    if(values != NULL){
        for(int i = 0 ; i < valuesCount; i++){
            free(values[i]);
        }
        free(values);
    }
}

void DOTCONFDocumentNode::pushValue(char * _value)
{
    ++valuesCount;
    values = (char**)realloc(values, valuesCount*sizeof(char*));
    values[valuesCount-1] = strdup(_value);
}

const char* DOTCONFDocumentNode::getValue(int index) const
{
    if(index >= valuesCount){
        return NULL;
    }
    return values[index];
}

DOTCONFDocument::DOTCONFDocument(DOTCONFDocument::CaseSensitive caseSensitivity):
    mempool(NULL),
    curParent(NULL), curPrev(NULL), curLine(0), file(NULL), fileName(NULL)
{
    if(caseSensitivity == CASESENSETIVE){
        cmp_func = strcmp;
    } else {
        cmp_func = strcasecmp;
    }

    mempool = new AsyncDNSMemPool(1024);
    mempool->initialize();
}

DOTCONFDocument::~DOTCONFDocument()
{
    for(std::list<DOTCONFDocumentNode*>::iterator i = nodeTree.begin(); i != nodeTree.end(); i++){
        delete(*i);
    }
    for(std::list<char*>::iterator i = requiredOptions.begin(); i != requiredOptions.end(); i++){
        free(*i);
    }
    for(std::list<char*>::iterator i = processedFiles.begin(); i != processedFiles.end(); i++){
        free(*i);
    }
    free(fileName);
    delete mempool;
}

int DOTCONFDocument::cleanupLine(char * line)
{
    char * start = line;
    char * bg = line;
    bool multiline = false;
    bool concat = false;
    char * word = NULL;

    if(!words.empty() && quoted)
        concat = true;

    while(*line){
        if((*line == '#' || *line == ';') && !quoted){
            *bg = 0;
            if(strlen(start)){

                if(concat){
                    word = (char*)mempool->alloc(strlen(words.back())+strlen(start)+1);
                    strcpy(word, words.back());
                    strcat(word, start);
                    words.pop_back();
                    concat = false;
                } else {
                    word = mempool->strdup(start);
                }
                words.push_back(word);
            }
            break;
        }
        if(*line == '=' && !quoted){
            *line = ' ';continue;
        }

        // Allowing \" in there causes problems with directory paths
        // like "C:\MaNGOS\"
        //if(*line == '\\' && (*(line+1) == '"' || *(line+1) == '\'')){
        if(*line == '\\' && (*(line+1) == '\'')) {
            *bg++ = *(line+1);
            line+=2; continue;
        }
        if(*line == '\\' && *(line+1) == 'n'){
            *bg++ = '\n';
            line+=2; continue;
        }
        if(*line == '\\' && *(line+1) == 'r'){
            *bg++ = '\r';
            line+=2; continue;
        }
        if(*line == '\\' && (*(line+1) == '\n' || *(line+1) == '\r')){
            *bg = 0;
            if(strlen(start)){

                if(concat){
                    word = (char*)mempool->alloc(strlen(words.back())+strlen(start)+1);
                    strcpy(word, words.back());
                    strcat(word, start);
                    words.pop_back();
                    concat = false;
                } else {
                    word = mempool->strdup(start);
                }
                words.push_back(word);
            }
            multiline = true;
            break;
        }
        if(*line == '"' || *line == '\''){
            quoted = !quoted;
            ++line; continue;
        }
        if(isspace(*line) && !quoted){
            *bg++ = 0;
            if(strlen(start)){

                if(concat){
                    word = (char*)mempool->alloc(strlen(words.back())+strlen(start)+1);
                    strcpy(word, words.back());
                    strcat(word, start);
                    words.pop_back();
                    concat = false;
                } else {
                    word = mempool->strdup(start);
                }
                words.push_back(word);
            }
            start = bg;
            while(isspace(*++line));
            continue;
        }
        *bg++ = *line++;
    }

    if(quoted && !multiline){
        error(curLine, fileName, "unterminated quote");
        return -1;
    }

    return multiline?1:0;
}

int DOTCONFDocument::parseLine()
{
    char * word = NULL;
    char * nodeName = NULL;
    char * nodeValue = NULL;
    DOTCONFDocumentNode * tagNode = NULL;
    bool newNode = false;

    for(std::list<char*>::iterator i = words.begin(); i != words.end(); i++) {
        word = *i;

        if(*word == '<'){
            newNode = true;
        }

        if(newNode){
            nodeValue = NULL;
            nodeName = NULL;
            newNode = false;
        }

        size_t wordLen = strlen(word);
        if(word[wordLen-1] == '>'){
            word[wordLen-1] = 0;
            newNode = true;
        }

        if(nodeName == NULL){
            nodeName = word;
            bool closed = true;
            if(*nodeName == '<'){
                if(*(nodeName+1) != '/'){
                    ++nodeName;
                    closed = false;
                } else {
                    nodeName+=2;
                    std::list<DOTCONFDocumentNode*>::reverse_iterator itr=nodeTree.rbegin();
                    for(; itr!=nodeTree.rend(); ++itr){
                        if(!cmp_func(nodeName, (*itr)->name) && !(*itr)->closed){
                            (*itr)->closed = true;
                            curParent = (*itr)->parentNode;
                            curPrev = *itr;
                            break;
                        }
                    }
                    if(itr==nodeTree.rend()){
                        error(curLine, fileName, "not matched closing tag </%s>", nodeName);
                        return -1;
                    }
                    continue;
                }
            }
            tagNode = new DOTCONFDocumentNode;
            tagNode->name = strdup(nodeName);
            tagNode->document = this;
            tagNode->fileName = processedFiles.back();
            tagNode->lineNum = curLine;
            tagNode->closed = closed;
            if(!nodeTree.empty()){
                DOTCONFDocumentNode * prev = nodeTree.back();
                if(prev->closed){

                    curPrev->nextNode = tagNode;
                    tagNode->previousNode = curPrev;
                    tagNode->parentNode = curParent;

                } else {
                    prev->childNode = tagNode;
                    tagNode->parentNode = prev;
                    curParent = prev;
                }
            }
            nodeTree.push_back(tagNode);
            curPrev = tagNode;
        } else {
            nodeValue = word;
            tagNode->pushValue(nodeValue);
        }
    }

    return 0;
}
int DOTCONFDocument::parseFile(DOTCONFDocumentNode * _parent)
{
    char str[512];
    int ret = 0;
    curLine = 0;
    curParent = _parent;

    quoted = false;
    size_t slen = 0;

    while(fgets(str, 511, file)){
        ++curLine;
    slen = strlen(str);
        if( slen >= 510 ){
            error(curLine, fileName, "warning: line too long");
        }
    if(str[slen-1] != '\n'){
        str[slen] = '\n';
        str[slen+1] = 0;
    }
        if((ret = cleanupLine(str)) == -1){
            break;
        }
        if(ret == 0){
            if(!words.empty()){
                ret = parseLine();
                mempool->free();
                words.clear();
                if(ret == -1){
                    break;
                }
            }
        }
    }

    return ret;
}

int DOTCONFDocument::checkConfig(const std::list<DOTCONFDocumentNode*>::iterator & from)
{
    int ret = 0;

    DOTCONFDocumentNode * tagNode = NULL;
    int vi = 0;
    for(std::list<DOTCONFDocumentNode*>::iterator i = from; i != nodeTree.end(); i++){
        tagNode = *i;
        if(!tagNode->closed){
            error(tagNode->lineNum, tagNode->fileName, "unclosed tag %s", tagNode->name);
            ret = -1;
            break;
        }
        vi = 0;
        while( vi < tagNode->valuesCount ){

            if(strstr(tagNode->values[vi], "${") && strchr(tagNode->values[vi], '}') ){
                ret = macroSubstitute(tagNode, vi );
                mempool->free();
                if(ret == -1){
                    break;
                }
            }
            ++vi;
        }
        if(ret == -1){
            break;
        }
    }

    return ret;
}

int DOTCONFDocument::setContent(const char * _fileName)
{
    int ret = 0;
    char realpathBuf[PATH_MAX];

    if(realpath(_fileName, realpathBuf) == NULL){
        error(0, NULL, "realpath(%s) failed: %s", _fileName, strerror(errno));
        return -1;
    }

    fileName = strdup(realpathBuf);

    processedFiles.push_back(strdup(realpathBuf));

    if(( file = fopen(fileName, "r")) == NULL){
        error(0, NULL, "failed to open file '%s': %s", fileName, strerror(errno));
        return -1;
    }

    ret = parseFile();

    (void) fclose(file);

    if(!ret){

        if( (ret = checkConfig(nodeTree.begin())) == -1){
            return -1;
        }

        std::list<DOTCONFDocumentNode*>::iterator from;
        DOTCONFDocumentNode * tagNode = NULL;
        int vi = 0;
        for(std::list<DOTCONFDocumentNode*>::iterator i = nodeTree.begin(); i!=nodeTree.end(); i++){
            tagNode = *i;
            if(!cmp_func("DOTCONFPPIncludeFile", tagNode->name)){
                vi = 0;
                while( vi < tagNode->valuesCount ){
                    if(access(tagNode->values[vi], R_OK) == -1){
                        error(tagNode->lineNum, tagNode->fileName, "%s: %s", tagNode->values[vi], strerror(errno));
                        return -1;
                    }
                    if(realpath(tagNode->values[vi], realpathBuf) == NULL){
                        error(tagNode->lineNum, tagNode->fileName, "realpath(%s) failed: %s", tagNode->values[vi], strerror(errno));
                        return -1;
                    }

                    bool processed = false;
                    for(std::list<char*>::const_iterator itInode = processedFiles.begin(); itInode != processedFiles.end(); itInode++){
                        if(!strcmp(*itInode, realpathBuf)){
                            processed = true;
                            break;
                        }
                    }
                    if(processed){
                        break;
                    }

                    processedFiles.push_back(strdup(realpathBuf));

                    file = fopen(tagNode->values[vi], "r");
                    if(file == NULL){
                        error(tagNode->lineNum, fileName, "failed to open file '%s': %s", tagNode->values[vi], strerror(errno));
                        return -1;
                    }

                    fileName = strdup(realpathBuf);
                    from = nodeTree.end(); --from;

                    ret = parseFile();
                    (void) fclose(file);
                    if(ret == -1)
                        return -1;
                    if(checkConfig(++from) == -1){
                        return -1;
                    }
                    ++vi;
                }
            }
        }


        if(!requiredOptions.empty())
            ret = checkRequiredOptions();
    }

    return ret;
}

int DOTCONFDocument::checkRequiredOptions()
{
    for(std::list<char*>::const_iterator ci = requiredOptions.begin(); ci != requiredOptions.end(); ci++){
        bool matched = false;
        for(std::list<DOTCONFDocumentNode*>::iterator i = nodeTree.begin(); i!=nodeTree.end(); i++){
            if(!cmp_func((*i)->name, *ci)){
                matched = true;
                break;
            }
        }
        if(!matched){
            error(0, NULL, "required option '%s' not specified", *ci);
            return -1;
        }
    }
    return 0;
}

void DOTCONFDocument::error(int lineNum, const char * fileName_, const char * fmt, ...)
{
    va_list args;
    va_start(args, fmt);

    size_t len = (lineNum!=0?strlen(fileName_):0) + strlen(fmt) + 50;
    char * buf = (char*)mempool->alloc(len);

    if(lineNum)
        (void) snprintf(buf, len, "DOTCONF++: file '%s', line %d: %s\n", fileName_, lineNum, fmt);
    else
        (void) snprintf(buf, len, "DOTCONF++: %s\n", fmt);

    (void) vfprintf(stderr, buf, args);

    va_end(args);
}

char * DOTCONFDocument::getSubstitution(char * macro, int lineNum)
{
    char * buf = NULL;
    char * variable = macro+2;

    char * endBr = strchr(macro, '}');

    if(!endBr){
        error(lineNum, fileName, "unterminated '{'");
        return NULL;
    }
    *endBr = 0;

    char * defaultValue = strchr(variable, ':');

    if(defaultValue){
        *defaultValue++ = 0;
        if(*defaultValue != '-'){
            error(lineNum, fileName, "incorrect macro substitution syntax");
            return NULL;
        }
        ++defaultValue;
        if(*defaultValue == '"' || *defaultValue == '\''){
            ++defaultValue;
            defaultValue[strlen(defaultValue)-1] = 0;
        }
    } else {
        defaultValue = NULL;
    }

    char * subs = getenv(variable);
    if( subs ){
        buf = mempool->strdup(subs);
    } else {
        std::list<DOTCONFDocumentNode*>::iterator i = nodeTree.begin();
        DOTCONFDocumentNode * tagNode = NULL;
        for(; i!=nodeTree.end(); i++){
            tagNode = *i;
            if(!cmp_func(tagNode->name, variable)){
                if(tagNode->valuesCount != 0){
                    buf = mempool->strdup(tagNode->values[0]);
                    break;
                }
            }
        }
        if( i == nodeTree.end() ){
            if( defaultValue ){
                buf = mempool->strdup(defaultValue);
            } else {
                error(lineNum, fileName, "substitution not found and default value not given");
                return NULL;
            }
        }
    }
    return buf;
}

int DOTCONFDocument::macroSubstitute(DOTCONFDocumentNode * tagNode, int valueIndex)
{
    int ret = 0;
    char * macro = tagNode->values[valueIndex];
    size_t valueLen = strlen(tagNode->values[valueIndex])+1;
    char * value = (char*)mempool->alloc(valueLen);
    char * v = value;
    char * subs = NULL;

    while(*macro){
        if(*macro == '$' && *(macro+1) == '{'){
            char * m = strchr(macro, '}');
            subs = getSubstitution(macro, tagNode->lineNum);
            if(subs == NULL){
                ret = -1;
                break;
            }
            macro = m + 1;
            *v = 0;
            v = (char*)mempool->alloc(strlen(value)+strlen(subs)+valueLen);
            strcpy(v, value);
            value = strcat(v, subs);
            v = value + strlen(value);
            continue;
        }
        *v++ = *macro++;
    }
    *v = 0;

    free(tagNode->values[valueIndex]);
    tagNode->values[valueIndex] = strdup(value);
    return ret;
}

const DOTCONFDocumentNode * DOTCONFDocument::getFirstNode() const
{
    if ( !nodeTree.empty() ) {
        return *nodeTree.begin();
    } else {
        return NULL;
    }
}

const DOTCONFDocumentNode * DOTCONFDocument::findNode(const char * nodeName, const DOTCONFDocumentNode * parentNode, const DOTCONFDocumentNode * startNode) const
{


    std::list<DOTCONFDocumentNode*>::const_iterator i = nodeTree.begin();

    if(startNode == NULL)
        startNode = parentNode;

    if(startNode != NULL){
        while( i != nodeTree.end() && (*i) != startNode ){
            ++i;
        }
        if( i != nodeTree.end() ) ++i;
    }

    for(; i!=nodeTree.end(); i++){

    if((*i)->parentNode != parentNode){
            continue;
        }
        if(!cmp_func(nodeName, (*i)->name)){
            return *i;
        }
    }
    return NULL;
}

void DOTCONFDocument::setRequiredOptionNames(const char ** requiredOptionNames)
{
    while(*requiredOptionNames){
        requiredOptions.push_back(strdup( *requiredOptionNames ));
        ++requiredOptionNames;
    }
}


Config::Config() : mConf(0)
{
}


Config::~Config()
{
    if (mConf)
        delete mConf;
}


bool Config::SetSource(const char *file, bool ignorecase)
{
    mConf = new DOTCONFDocument(ignorecase ?
        DOTCONFDocument::CASEINSENSETIVE :
    DOTCONFDocument::CASESENSETIVE);

    if (mConf->setContent(file) == -1)
    {
        delete mConf;
        mConf = 0;
        return false;
    }

    return true;
}


bool Config::GetString(const char* name, std::string *value)
{
    if(!mConf)
        return false;

    DOTCONFDocumentNode const *node = mConf->findNode(name);
    if(!node || !node->getValue())
        return false;

    *value = node->getValue();

    return true;
}

bool Config::GetString(const char* name, char const **value)
{
    if(!mConf)
        return false;

    DOTCONFDocumentNode const *node = mConf->findNode(name);
    if(!node || !node->getValue())
        return false;

    *value = node->getValue();

    return true;
}


std::string Config::GetStringDefault(const char* name, const char* def)
{
    if(!mConf)
        return std::string(def);

    DOTCONFDocumentNode const *node = mConf->findNode(name);
    if(!node || !node->getValue())
        return std::string(def);

    return std::string(node->getValue());
}


bool Config::GetBool(const char* name, bool *value)
{
    if(!mConf)
        return false;

    DOTCONFDocumentNode const *node = mConf->findNode(name);
    if(!node || !node->getValue())
        return false;

    const char* str = node->getValue();
    if(strcmp(str, "true") == 0 || strcmp(str, "TRUE") == 0 ||
        strcmp(str, "yes") == 0 || strcmp(str, "YES") == 0 ||
        strcmp(str, "1") == 0)
    {
        *value = true;
    }
    else
        *value = false;

    return true;
}


bool Config::GetBoolDefault(const char* name, const bool def)
{
    bool val;
    return GetBool(name, &val) ? val : def;
}


bool Config::GetInt(const char* name, int *value)
{
    if(!mConf)
        return false;

    DOTCONFDocumentNode const *node = mConf->findNode(name);
    if(!node || !node->getValue())
        return false;

    *value = atoi(node->getValue());

    return true;
}


bool Config::GetFloat(const char* name, float *value)
{
    if(!mConf)
        return false;

    DOTCONFDocumentNode const *node = mConf->findNode(name);
    if(!node || !node->getValue())
        return false;

    *value = atof(node->getValue());

    return true;
}


int Config::GetIntDefault(const char* name, const int def)
{
    int val;
    return GetInt(name, &val) ? val : def;
}


float Config::GetFloatDefault(const char* name, const float def)
{
    float val;
    return (GetFloat(name, &val) ? val : def);
}
