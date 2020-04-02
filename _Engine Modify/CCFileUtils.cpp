/****************************************************************************
Copyright (c) 2010-2013 cocos2d-x.org
Copyright (c) 2013-2017 Chukong Technologies Inc.

http://www.cocos2d-x.org

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in
all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
THE SOFTWARE.
****************************************************************************/

#include "platform/CCFileUtils.h"

#include <stack>

#include "base/CCData.h"
#include "base/ccMacros.h"
#include "base/CCDirector.h"
#include "platform/CCSAXParser.h"
//#include "base/ccUtils.h"

#include "tinyxml2/tinyxml2.h"
#include "tinydir/tinydir.h"
#ifdef MINIZIP_FROM_SYSTEM
#include <minizip/unzip.h>
#else // from our embedded sources
#include "unzip.h"
#endif
#include <sys/stat.h>

NS_CC_BEGIN

// Implement DictMaker

#if (CC_TARGET_PLATFORM != CC_PLATFORM_IOS) && (CC_TARGET_PLATFORM != CC_PLATFORM_MAC)

typedef enum
{
    SAX_NONE = 0,
    SAX_KEY,
    SAX_DICT,
    SAX_INT,
    SAX_REAL,
    SAX_STRING,
    SAX_ARRAY
}SAXState;

typedef enum
{
    SAX_RESULT_NONE = 0,
    SAX_RESULT_DICT,
    SAX_RESULT_ARRAY
}SAXResult;

class DictMaker : public SAXDelegator
{
public:
    SAXResult _resultType;
    ValueMap _rootDict;
    ValueVector _rootArray;

    std::string _curKey;   ///< parsed key
    std::string _curValue; // parsed value
    SAXState _state;

    ValueMap*  _curDict;
    ValueVector* _curArray;

    std::stack<ValueMap*> _dictStack;
    std::stack<ValueVector*> _arrayStack;
    std::stack<SAXState>  _stateStack;

public:
    DictMaker()
        : _resultType(SAX_RESULT_NONE)
    {
    }

    ~DictMaker()
    {
    }

    ValueMap dictionaryWithContentsOfFile(const std::string& fileName)
    {
        _resultType = SAX_RESULT_DICT;
        SAXParser parser;

        CCASSERT(parser.init("UTF-8"), "The file format isn't UTF-8");
        parser.setDelegator(this);

        parser.parse(fileName);
        return _rootDict;
    }

    ValueMap dictionaryWithDataOfFile(const char* filedata, int filesize)
    {
        _resultType = SAX_RESULT_DICT;
        SAXParser parser;

        CCASSERT(parser.init("UTF-8"), "The file format isn't UTF-8");
        parser.setDelegator(this);

        parser.parse(filedata, filesize);
        return _rootDict;
    }

    ValueVector arrayWithContentsOfFile(const std::string& fileName)
    {
        _resultType = SAX_RESULT_ARRAY;
        SAXParser parser;

        CCASSERT(parser.init("UTF-8"), "The file format isn't UTF-8");
        parser.setDelegator(this);

        parser.parse(fileName);
        return _rootArray;
    }

    void startElement(void *ctx, const char *name, const char **atts)
    {
        const std::string sName(name);
        if( sName == "dict" )
        {
            if(_resultType == SAX_RESULT_DICT && _rootDict.empty())
            {
                _curDict = &_rootDict;
            }

            _state = SAX_DICT;

            SAXState preState = SAX_NONE;
            if (! _stateStack.empty())
            {
                preState = _stateStack.top();
            }

            if (SAX_ARRAY == preState)
            {
                // add a new dictionary into the array
                _curArray->push_back(Value(ValueMap()));
                _curDict = &(_curArray->rbegin())->asValueMap();
            }
            else if (SAX_DICT == preState)
            {
                // add a new dictionary into the pre dictionary
                CCASSERT(! _dictStack.empty(), "The state is wrong!");
                ValueMap* preDict = _dictStack.top();
                (*preDict)[_curKey] = Value(ValueMap());
                _curDict = &(*preDict)[_curKey].asValueMap();
            }

            // record the dict state
            _stateStack.push(_state);
            _dictStack.push(_curDict);
        }
        else if(sName == "key")
        {
            _state = SAX_KEY;
        }
        else if(sName == "integer")
        {
            _state = SAX_INT;
        }
        else if(sName == "real")
        {
            _state = SAX_REAL;
        }
        else if(sName == "string")
        {
            _state = SAX_STRING;
        }
        else if (sName == "array")
        {
            _state = SAX_ARRAY;

            if (_resultType == SAX_RESULT_ARRAY && _rootArray.empty())
            {
                _curArray = &_rootArray;
            }
            SAXState preState = SAX_NONE;
            if (! _stateStack.empty())
            {
                preState = _stateStack.top();
            }

            if (preState == SAX_DICT)
            {
                (*_curDict)[_curKey] = Value(ValueVector());
                _curArray = &(*_curDict)[_curKey].asValueVector();
            }
            else if (preState == SAX_ARRAY)
            {
                CCASSERT(! _arrayStack.empty(), "The state is wrong!");
                ValueVector* preArray = _arrayStack.top();
                preArray->push_back(Value(ValueVector()));
                _curArray = &(_curArray->rbegin())->asValueVector();
            }
            // record the array state
            _stateStack.push(_state);
            _arrayStack.push(_curArray);
        }
        else
        {
            _state = SAX_NONE;
        }
    }

    void endElement(void *ctx, const char *name)
    {
        SAXState curState = _stateStack.empty() ? SAX_DICT : _stateStack.top();
        const std::string sName((char*)name);
        if( sName == "dict" )
        {
            _stateStack.pop();
            _dictStack.pop();
            if ( !_dictStack.empty())
            {
                _curDict = _dictStack.top();
            }
        }
        else if (sName == "array")
        {
            _stateStack.pop();
            _arrayStack.pop();
            if (! _arrayStack.empty())
            {
                _curArray = _arrayStack.top();
            }
        }
        else if (sName == "true")
        {
            if (SAX_ARRAY == curState)
            {
                _curArray->push_back(Value(true));
            }
            else if (SAX_DICT == curState)
            {
                (*_curDict)[_curKey] = Value(true);
            }
        }
        else if (sName == "false")
        {
            if (SAX_ARRAY == curState)
            {
                _curArray->push_back(Value(false));
            }
            else if (SAX_DICT == curState)
            {
                (*_curDict)[_curKey] = Value(false);
            }
        }
        else if (sName == "string" || sName == "integer" || sName == "real")
        {
            if (SAX_ARRAY == curState)
            {
                if (sName == "string")
                    _curArray->push_back(Value(_curValue));
                else if (sName == "integer")
                    _curArray->push_back(Value(atoi(_curValue.c_str())));
                else
                    _curArray->push_back(Value(std::atof(_curValue.c_str())));
            }
            else if (SAX_DICT == curState)
            {
                if (sName == "string")
                    (*_curDict)[_curKey] = Value(_curValue);
                else if (sName == "integer")
                    (*_curDict)[_curKey] = Value(atoi(_curValue.c_str()));
                else
                    (*_curDict)[_curKey] = Value(std::atof(_curValue.c_str()));
            }

            _curValue.clear();
        }

        _state = SAX_NONE;
    }

    void textHandler(void *ctx, const char *ch, size_t len) override
    {
        if (_state == SAX_NONE)
        {
            return;
        }

        SAXState curState = _stateStack.empty() ? SAX_DICT : _stateStack.top();
        const std::string text = std::string((char*)ch,len);

        switch(_state)
        {
        case SAX_KEY:
            _curKey = text;
            break;
        case SAX_INT:
        case SAX_REAL:
        case SAX_STRING:
            {
                if (curState == SAX_DICT)
                {
                    CCASSERT(!_curKey.empty(), "key not found : <integer/real>");
                }

                _curValue.append(text);
            }
            break;
        default:
            break;
        }
    }
};

ValueMap FileUtils::getValueMapFromFile(const std::string& filename)
{
    const std::string fullPath = fullPathForFilename(filename);
    DictMaker tMaker;
    return tMaker.dictionaryWithContentsOfFile(fullPath);
}

ValueMap FileUtils::getValueMapFromData(const char* filedata, int filesize)
{
    DictMaker tMaker;
    return tMaker.dictionaryWithDataOfFile(filedata, filesize);
}

ValueVector FileUtils::getValueVectorFromFile(const std::string& filename)
{
    const std::string fullPath = fullPathForFilename(filename);
    DictMaker tMaker;
    return tMaker.arrayWithContentsOfFile(fullPath);
}


/*
 * forward statement
 */
static tinyxml2::XMLElement* generateElementForArray(const ValueVector& array, tinyxml2::XMLDocument *doc);
static tinyxml2::XMLElement* generateElementForDict(const ValueMap& dict, tinyxml2::XMLDocument *doc);

/*
 * Use tinyxml2 to write plist files
 */
bool FileUtils::writeToFile(const ValueMap& dict, const std::string &fullPath)
{
    return writeValueMapToFile(dict, fullPath);
}

bool FileUtils::writeValueMapToFile(const ValueMap& dict, const std::string& fullPath)
{
    tinyxml2::XMLDocument *doc = new (std::nothrow)tinyxml2::XMLDocument();
    if (nullptr == doc)
        return false;

    tinyxml2::XMLDeclaration *declaration = doc->NewDeclaration("xml version=\"1.0\" encoding=\"UTF-8\"");
    if (nullptr == declaration)
    {
        delete doc;
        return false;
    }

    doc->LinkEndChild(declaration);
    tinyxml2::XMLElement *docType = doc->NewElement("!DOCTYPE plist PUBLIC \"-//Apple//DTD PLIST 1.0//EN\" \"http://www.apple.com/DTDs/PropertyList-1.0.dtd\"");
    doc->LinkEndChild(docType);

    tinyxml2::XMLElement *rootEle = doc->NewElement("plist");
    rootEle->SetAttribute("version", "1.0");
    if (nullptr == rootEle)
    {
        delete doc;
        return false;
    }
    doc->LinkEndChild(rootEle);

    tinyxml2::XMLElement *innerDict = generateElementForDict(dict, doc);
    if (nullptr == innerDict)
    {
        delete doc;
        return false;
    }
    rootEle->LinkEndChild(innerDict);

    bool ret = tinyxml2::XML_SUCCESS == doc->SaveFile(getSuitableFOpen(fullPath).c_str());

    delete doc;
    return ret;
}

bool FileUtils::writeValueVectorToFile(const ValueVector& vecData, const std::string& fullPath)
{
    tinyxml2::XMLDocument *doc = new (std::nothrow)tinyxml2::XMLDocument();
    if (nullptr == doc)
        return false;

    tinyxml2::XMLDeclaration *declaration = doc->NewDeclaration("xml version=\"1.0\" encoding=\"UTF-8\"");
    if (nullptr == declaration)
    {
        delete doc;
        return false;
    }

    doc->LinkEndChild(declaration);
    tinyxml2::XMLElement *docType = doc->NewElement("!DOCTYPE plist PUBLIC \"-//Apple//DTD PLIST 1.0//EN\" \"http://www.apple.com/DTDs/PropertyList-1.0.dtd\"");
    doc->LinkEndChild(docType);

    tinyxml2::XMLElement *rootEle = doc->NewElement("plist");
    rootEle->SetAttribute("version", "1.0");
    if (nullptr == rootEle)
    {
        delete doc;
        return false;
    }
    doc->LinkEndChild(rootEle);

    tinyxml2::XMLElement *innerDict = generateElementForArray(vecData, doc);
    if (nullptr == innerDict)
    {
        delete doc;
        return false;
    }
    rootEle->LinkEndChild(innerDict);

    bool ret = tinyxml2::XML_SUCCESS == doc->SaveFile(getSuitableFOpen(fullPath).c_str());

    delete doc;
    return ret;
}

/*
 * Generate tinyxml2::XMLElement for Object through a tinyxml2::XMLDocument
 */
static tinyxml2::XMLElement* generateElementForObject(const Value& value, tinyxml2::XMLDocument *doc)
{
    // object is String
    if (value.getType() == Value::Type::STRING)
    {
        tinyxml2::XMLElement* node = doc->NewElement("string");
        tinyxml2::XMLText* content = doc->NewText(value.asString().c_str());
        node->LinkEndChild(content);
        return node;
    }

    // object is integer
    if (value.getType() == Value::Type::INTEGER)
    {
        tinyxml2::XMLElement* node = doc->NewElement("integer");
        tinyxml2::XMLText* content = doc->NewText(value.asString().c_str());
        node->LinkEndChild(content);
        return node;
    }

    // object is real
    if (value.getType() == Value::Type::FLOAT || value.getType() == Value::Type::DOUBLE)
    {
        tinyxml2::XMLElement* node = doc->NewElement("real");
        tinyxml2::XMLText* content = doc->NewText(value.asString().c_str());
        node->LinkEndChild(content);
        return node;
    }

    //object is bool
    if (value.getType() == Value::Type::BOOLEAN) {
        tinyxml2::XMLElement* node = doc->NewElement(value.asString().c_str());
        return node;
    }

    // object is Array
    if (value.getType() == Value::Type::VECTOR)
        return generateElementForArray(value.asValueVector(), doc);

    // object is Dictionary
    if (value.getType() == Value::Type::MAP)
        return generateElementForDict(value.asValueMap(), doc);

    CCLOG("This type cannot appear in property list");
    return nullptr;
}

/*
 * Generate tinyxml2::XMLElement for Dictionary through a tinyxml2::XMLDocument
 */
static tinyxml2::XMLElement* generateElementForDict(const ValueMap& dict, tinyxml2::XMLDocument *doc)
{
    tinyxml2::XMLElement* rootNode = doc->NewElement("dict");

    for (const auto &iter : dict)
    {
        tinyxml2::XMLElement* tmpNode = doc->NewElement("key");
        rootNode->LinkEndChild(tmpNode);
        tinyxml2::XMLText* content = doc->NewText(iter.first.c_str());
        tmpNode->LinkEndChild(content);

        tinyxml2::XMLElement *element = generateElementForObject(iter.second, doc);
        if (element)
            rootNode->LinkEndChild(element);
    }
    return rootNode;
}

/*
 * Generate tinyxml2::XMLElement for Array through a tinyxml2::XMLDocument
 */
static tinyxml2::XMLElement* generateElementForArray(const ValueVector& array, tinyxml2::XMLDocument *pDoc)
{
    tinyxml2::XMLElement* rootNode = pDoc->NewElement("array");

    for(const auto &value : array) {
        tinyxml2::XMLElement *element = generateElementForObject(value, pDoc);
        if (element)
            rootNode->LinkEndChild(element);
    }
    return rootNode;
}

#else

/* The subclass FileUtilsApple should override these two method. */
ValueMap FileUtils::getValueMapFromFile(const std::string& /*filename*/) {return ValueMap();}
ValueMap FileUtils::getValueMapFromData(const char* /*filedata*/, int /*filesize*/) {return ValueMap();}
ValueVector FileUtils::getValueVectorFromFile(const std::string& /*filename*/) {return ValueVector();}
bool FileUtils::writeToFile(const ValueMap& /*dict*/, const std::string &/*fullPath*/) {return false;}

#endif /* (CC_TARGET_PLATFORM != CC_PLATFORM_IOS) && (CC_TARGET_PLATFORM != CC_PLATFORM_MAC) */

// Implement FileUtils
FileUtils* FileUtils::s_sharedFileUtils = nullptr;

void FileUtils::destroyInstance()
{
    CC_SAFE_DELETE(s_sharedFileUtils);
}

void FileUtils::setDelegate(FileUtils *delegate)
{
    if (s_sharedFileUtils)
        delete s_sharedFileUtils;

    s_sharedFileUtils = delegate;
}

FileUtils::FileUtils()
    : _writablePath("")
{
}

FileUtils::~FileUtils()
{
}

bool FileUtils::writeStringToFile(const std::string& dataStr, const std::string& fullPath)
{
    Data data;
    data.fastSet((unsigned char*)dataStr.c_str(), dataStr.size());

    bool rv = writeDataToFile(data, fullPath);

    data.fastSet(nullptr, 0);
    return rv;
}

void FileUtils::writeStringToFile(std::string dataStr, const std::string& fullPath, std::function<void(bool)> callback)
{
    performOperationOffthread([fullPath](const std::string& dataStrIn) -> bool {
        return FileUtils::getInstance()->writeStringToFile(dataStrIn, fullPath);
    }, std::move(callback),std::move(dataStr));
}

bool FileUtils::writeDataToFile(const Data& data, const std::string& fullPath)
{
    size_t size = 0;
    const char* mode = "wb";

    CCASSERT(!fullPath.empty() && data.getSize() != 0, "Invalid parameters.");

    auto fileutils = FileUtils::getInstance();
    do
    {
        // Read the file from hardware
        FILE *fp = fopen(fileutils->getSuitableFOpen(fullPath).c_str(), mode);
        CC_BREAK_IF(!fp);
        size = data.getSize();

        fwrite(data.getBytes(), size, 1, fp);

        fclose(fp);

        return true;
    } while (0);

    return false;
}

void FileUtils::writeDataToFile(Data data, const std::string& fullPath, std::function<void(bool)> callback)
{
    performOperationOffthread([fullPath](const Data& dataIn) -> bool {
        return FileUtils::getInstance()->writeDataToFile(dataIn, fullPath);
    }, std::move(callback), std::move(data));
}

bool FileUtils::init()
{
    _searchPathArray.push_back(_defaultResRootPath);
    _searchResolutionsOrderArray.push_back("");
    return true;
}

void FileUtils::purgeCachedEntries()
{
    _fullPathCache.clear();
}

std::string FileUtils::getStringFromFile(const std::string& filename)
{
    std::string s;
    getContents(filename, &s);
    return s;
}

void FileUtils::getStringFromFile(const std::string &path, std::function<void (std::string)> callback)
{
    // Get the full path on the main thread, to avoid the issue that FileUtil's is not
    // thread safe, and accessing the fullPath cache and searching the search paths is not thread safe
    auto fullPath = fullPathForFilename(path);
    performOperationOffthread([fullPath]() -> std::string {
        return FileUtils::getInstance()->getStringFromFile(fullPath);
    }, std::move(callback));
}

Data FileUtils::getDataFromFile(const std::string& filename)
{
    Data d;
    getContents(filename, &d);
	int nd = d.getSize();
	int np = m_password.size();
	if (nd > np)
	{
		for (int i = 0; i < np; ++i)
		{
			if (m_password.at(i) != d.getBytes()[i])
				return d;
		}

		int nmd = nd - np;
		char* buffer = new char[nmd + 1];
		for (int i = 0; i < nmd; ++i)
		{
			buffer[i] = d.getBytes()[i + np];
		}
		buffer[nmd] = '\0';
		
		//����buffer
		UnEncrypt(buffer, nmd);

		d.clear();
		d.fastSet((unsigned char*)buffer, nmd);
	}
    return d;
}

void FileUtils::getDataFromFile(const std::string& filename, std::function<void(Data)> callback)
{
    auto fullPath = fullPathForFilename(filename);
    performOperationOffthread([fullPath]() -> Data {
        return FileUtils::getInstance()->getDataFromFile(fullPath);
    }, std::move(callback));
}

FileUtils::Status FileUtils::getContents(const std::string& filename, ResizableBuffer* buffer)
{
    if (filename.empty())
        return Status::NotExists;

    auto fs = FileUtils::getInstance();

    std::string fullPath = fs->fullPathForFilename(filename);
    if (fullPath.empty())
        return Status::NotExists;

    FILE *fp = fopen(fs->getSuitableFOpen(fullPath).c_str(), "rb");
    if (!fp)
        return Status::OpenFailed;

#if defined(_MSC_VER)
    auto descriptor = _fileno(fp);
#else
    auto descriptor = fileno(fp);
#endif
    struct stat statBuf;
    if (fstat(descriptor, &statBuf) == -1) {
        fclose(fp);
        return Status::ReadFailed;
    }
    size_t size = statBuf.st_size;

    buffer->resize(size);
    size_t readsize = fread(buffer->buffer(), 1, size, fp);
    fclose(fp);

    if (readsize < size) {
        buffer->resize(readsize);
        return Status::ReadFailed;
    }

    return Status::OK;
}

unsigned char* FileUtils::getFileData(const std::string& filename, const char* mode, ssize_t *size)
{
    CCASSERT(!filename.empty() && size != nullptr && mode != nullptr, "Invalid parameters.");
    (void)(mode); // mode is unused, as we do not support text mode any more...

    Data d;
    if (getContents(filename, &d) != Status::OK) {
        *size = 0;
        return nullptr;
    }

    return d.takeBuffer(size);
}

unsigned char* FileUtils::getFileDataFromZip(const std::string& zipFilePath, const std::string& filename, ssize_t *size)
{
    unsigned char * buffer = nullptr;
    unzFile file = nullptr;
    *size = 0;

    do
    {
        CC_BREAK_IF(zipFilePath.empty());

        file = unzOpen(FileUtils::getInstance()->getSuitableFOpen(zipFilePath).c_str());
        CC_BREAK_IF(!file);

        // FIXME: Other platforms should use upstream minizip like mingw-w64
#ifdef MINIZIP_FROM_SYSTEM
        int ret = unzLocateFile(file, filename.c_str(), NULL);
#else
        int ret = unzLocateFile(file, filename.c_str(), 1);
#endif
        CC_BREAK_IF(UNZ_OK != ret);

        char filePathA[260];
        unz_file_info fileInfo;
        ret = unzGetCurrentFileInfo(file, &fileInfo, filePathA, sizeof(filePathA), nullptr, 0, nullptr, 0);
        CC_BREAK_IF(UNZ_OK != ret);

        ret = unzOpenCurrentFile(file);
        CC_BREAK_IF(UNZ_OK != ret);

        buffer = (unsigned char*)malloc(fileInfo.uncompressed_size);
        int CC_UNUSED readedSize = unzReadCurrentFile(file, buffer, static_cast<unsigned>(fileInfo.uncompressed_size));
        CCASSERT(readedSize == 0 || readedSize == (int)fileInfo.uncompressed_size, "the file size is wrong");

        *size = fileInfo.uncompressed_size;
        unzCloseCurrentFile(file);
    } while (0);

    if (file)
    {
        unzClose(file);
    }

    return buffer;
}

void FileUtils::writeValueMapToFile(ValueMap dict, const std::string& fullPath, std::function<void(bool)> callback)
{
    
    performOperationOffthread([fullPath](const ValueMap& dictIn) -> bool {
        return FileUtils::getInstance()->writeValueMapToFile(dictIn, fullPath);
    }, std::move(callback), std::move(dict));
}

void FileUtils::writeValueVectorToFile(ValueVector vecData, const std::string& fullPath, std::function<void(bool)> callback)
{
    performOperationOffthread([fullPath] (const ValueVector& vecDataIn) -> bool {
        return FileUtils::getInstance()->writeValueVectorToFile(vecDataIn, fullPath);
    }, std::move(callback), std::move(vecData));
}

std::string FileUtils::getNewFilename(const std::string &filename) const
{
    std::string newFileName;

    // in Lookup Filename dictionary ?
    auto iter = _filenameLookupDict.find(filename);

    if (iter == _filenameLookupDict.end())
    {
        newFileName = filename;
    }
    else
    {
        newFileName = iter->second.asString();
    }
    return newFileName;
}

std::string FileUtils::getPathForFilename(const std::string& filename, const std::string& resolutionDirectory, const std::string& searchPath) const
{
    std::string file = filename;
    std::string file_path = "";
    size_t pos = filename.find_last_of("/");
    if (pos != std::string::npos)
    {
        file_path = filename.substr(0, pos+1);
        file = filename.substr(pos+1);
    }

    // searchPath + file_path + resourceDirectory
    std::string path = searchPath;
    path += file_path;
    path += resolutionDirectory;

    path = getFullPathForDirectoryAndFilename(path, file);

    return path;
}

std::string FileUtils::fullPathForFilename(const std::string &filename) const
{
    if (filename.empty())
    {
        return "";
    }

    if (isAbsolutePath(filename))
    {
        return filename;
    }

    // Already Cached ?
    auto cacheIter = _fullPathCache.find(filename);
    if(cacheIter != _fullPathCache.end())
    {
        return cacheIter->second;
    }

    // Get the new file name.
    const std::string newFilename( getNewFilename(filename) );

    std::string fullpath;

    for (const auto& searchIt : _searchPathArray)
    {
        for (const auto& resolutionIt : _searchResolutionsOrderArray)
        {
            fullpath = this->getPathForFilename(newFilename, resolutionIt, searchIt);

            if (!fullpath.empty())
            {
                // Using the filename passed in as key.
                _fullPathCache.emplace(filename, fullpath);
                return fullpath;
            }

        }
    }

    if(isPopupNotify()){
		if (fullpath.empty())
		{
			fullpath = ResourcesDecode::getInstance()->findPathWithZip(filename);
			if (fullpath.size() == 0)
			{
				return "";
			}
			return fullpath;
		}
		CCLOG("cocos2d: fullPathForFilename: No file found at %s. Possible missing file.", filename.c_str());
    }

    // The file wasn't found, return empty string.
    return "";
}

std::string FileUtils::fullPathFromRelativeFile(const std::string &filename, const std::string &relativeFile)
{
    return relativeFile.substr(0, relativeFile.rfind('/')+1) + getNewFilename(filename);
}

void FileUtils::setSearchResolutionsOrder(const std::vector<std::string>& searchResolutionsOrder)
{
    if (_searchResolutionsOrderArray == searchResolutionsOrder)
    {
        return;
    }

    bool existDefault = false;

    _fullPathCache.clear();
    _searchResolutionsOrderArray.clear();
    for(const auto& iter : searchResolutionsOrder)
    {
        std::string resolutionDirectory = iter;
        if (!existDefault && resolutionDirectory == "")
        {
            existDefault = true;
        }

        if (resolutionDirectory.length() > 0 && resolutionDirectory[resolutionDirectory.length()-1] != '/')
        {
            resolutionDirectory += "/";
        }

        _searchResolutionsOrderArray.push_back(resolutionDirectory);
    }

    if (!existDefault)
    {
        _searchResolutionsOrderArray.push_back("");
    }
}

void FileUtils::addSearchResolutionsOrder(const std::string &order,const bool front)
{
    std::string resOrder = order;
    if (!resOrder.empty() && resOrder[resOrder.length()-1] != '/')
        resOrder.append("/");

    if (front) {
        _searchResolutionsOrderArray.insert(_searchResolutionsOrderArray.begin(), resOrder);
    } else {
        _searchResolutionsOrderArray.push_back(resOrder);
    }
}

const std::vector<std::string>& FileUtils::getSearchResolutionsOrder() const
{
    return _searchResolutionsOrderArray;
}

const std::vector<std::string>& FileUtils::getSearchPaths() const
{
    return _searchPathArray;
}

const std::vector<std::string>& FileUtils::getOriginalSearchPaths() const
{
    return _originalSearchPaths;
}

void FileUtils::setWritablePath(const std::string& writablePath)
{
    _writablePath = writablePath;
}

const std::string& FileUtils::getDefaultResourceRootPath() const
{
    return _defaultResRootPath;
}

void FileUtils::setDefaultResourceRootPath(const std::string& path)
{
    if (_defaultResRootPath != path)
    {
        _fullPathCache.clear();
        _defaultResRootPath = path;
        if (!_defaultResRootPath.empty() && _defaultResRootPath[_defaultResRootPath.length()-1] != '/')
        {
            _defaultResRootPath += '/';
        }

        // Updates search paths
        setSearchPaths(_originalSearchPaths);
    }
}

void FileUtils::setSearchPaths(const std::vector<std::string>& searchPaths)
{
    bool existDefaultRootPath = false;
    _originalSearchPaths = searchPaths;

    _fullPathCache.clear();
    _searchPathArray.clear();

    for (const auto& path : _originalSearchPaths)
    {
        std::string prefix;
        std::string fullPath;

        if (!isAbsolutePath(path))
        { // Not an absolute path
            prefix = _defaultResRootPath;
        }
        fullPath = prefix + path;
        if (!path.empty() && path[path.length()-1] != '/')
        {
            fullPath += "/";
        }
        if (!existDefaultRootPath && path == _defaultResRootPath)
        {
            existDefaultRootPath = true;
        }
        _searchPathArray.push_back(fullPath);
    }

    if (!existDefaultRootPath)
    {
        //CCLOG("Default root path doesn't exist, adding it.");
        _searchPathArray.push_back(_defaultResRootPath);
    }
}

void FileUtils::addSearchPath(const std::string &searchpath,const bool front)
{
    std::string prefix;
    if (!isAbsolutePath(searchpath))
        prefix = _defaultResRootPath;

    std::string path = prefix + searchpath;
    if (!path.empty() && path[path.length()-1] != '/')
    {
        path += "/";
    }

    if (front) {
        _originalSearchPaths.insert(_originalSearchPaths.begin(), searchpath);
        _searchPathArray.insert(_searchPathArray.begin(), path);
    } else {
        _originalSearchPaths.push_back(searchpath);

        if (!_searchPathArray.empty() && _searchPathArray[_searchPathArray.size()-1] == _defaultResRootPath)
        {
            _searchPathArray.insert(_searchPathArray.begin() + _searchPathArray.size() -1, path);
        }
        else
        {
            _searchPathArray.push_back(path);
        }
    }
}

void FileUtils::setFilenameLookupDictionary(const ValueMap& filenameLookupDict)
{
    _fullPathCache.clear();
    _filenameLookupDict = filenameLookupDict;
}

void FileUtils::loadFilenameLookupDictionaryFromFile(const std::string &filename)
{
    const std::string fullPath = fullPathForFilename(filename);
    if (!fullPath.empty())
    {
        ValueMap dict = FileUtils::getInstance()->getValueMapFromFile(fullPath);
        if (!dict.empty())
        {
            ValueMap& metadata =  dict["metadata"].asValueMap();
            int version = metadata["version"].asInt();
            if (version != 1)
            {
                CCLOG("cocos2d: ERROR: Invalid filenameLookup dictionary version: %d. Filename: %s", version, filename.c_str());
                return;
            }
            setFilenameLookupDictionary( dict["filenames"].asValueMap());
        }
    }
}

std::string FileUtils::getFullPathForDirectoryAndFilename(const std::string& directory, const std::string& filename) const
{
    // get directory+filename, safely adding '/' as necessary
    std::string ret = directory;
    if (directory.size() && directory[directory.size()-1] != '/'){
        ret += '/';
    }
    ret += filename;

    // if the file doesn't exist, return an empty string
    if (!isFileExistInternal(ret)) {
        ret = "";
    }
    return ret;
}

bool FileUtils::isFileExist(const std::string& filename) const
{
    if (isAbsolutePath(filename))
    {
        return isFileExistInternal(filename);
    }
    else
    {
        std::string fullpath = fullPathForFilename(filename);
        if (fullpath.empty())
            return false;
        else
            return true;
    }
}

void FileUtils::isFileExist(const std::string& filename, std::function<void(bool)> callback)
{
    auto fullPath = fullPathForFilename(filename);
    performOperationOffthread([fullPath]() -> bool {
        return FileUtils::getInstance()->isFileExist(fullPath);
    }, std::move(callback));
}

bool FileUtils::isAbsolutePath(const std::string& path) const
{
    return (path[0] == '/');
}

bool FileUtils::isDirectoryExist(const std::string& dirPath) const
{
    CCASSERT(!dirPath.empty(), "Invalid path");

    if (isAbsolutePath(dirPath))
    {
        return isDirectoryExistInternal(dirPath);
    }

    // Already Cached ?
    auto cacheIter = _fullPathCache.find(dirPath);
    if( cacheIter != _fullPathCache.end() )
    {
        return isDirectoryExistInternal(cacheIter->second);
    }

    std::string fullpath;
    for (const auto& searchIt : _searchPathArray)
    {
        for (const auto& resolutionIt : _searchResolutionsOrderArray)
        {
            // searchPath + file_path + resourceDirectory
            fullpath = fullPathForFilename(searchIt + dirPath + resolutionIt);
            if (isDirectoryExistInternal(fullpath))
            {
                _fullPathCache.emplace(dirPath, fullpath);
                return true;
            }
        }
    }
    return false;
}

void FileUtils::isDirectoryExist(const std::string& fullPath, std::function<void(bool)> callback)
{
    CCASSERT(isAbsolutePath(fullPath), "Async isDirectoryExist only accepts absolute file paths");
    performOperationOffthread([fullPath]() -> bool {
        return FileUtils::getInstance()->isDirectoryExist(fullPath);
    }, std::move(callback));
}

void FileUtils::createDirectory(const std::string& dirPath, std::function<void(bool)> callback)
{
    performOperationOffthread([dirPath]() -> bool {
        return FileUtils::getInstance()->createDirectory(dirPath);
    }, std::move(callback));
}

void FileUtils::removeDirectory(const std::string& dirPath, std::function<void(bool)> callback)
{
    performOperationOffthread([dirPath]() -> bool {
        return FileUtils::getInstance()->removeDirectory(dirPath);
    }, std::move(callback));
}

void FileUtils::removeFile(const std::string &filepath, std::function<void (bool)> callback)
{
    auto fullPath = fullPathForFilename(filepath);
    performOperationOffthread([fullPath]() -> bool {
        return FileUtils::getInstance()->removeFile(fullPath);
    }, std::move(callback));
}

void FileUtils::renameFile(const std::string &path, const std::string &oldname, const std::string &name, std::function<void(bool)> callback)
{
    performOperationOffthread([path, oldname, name]() -> bool {
        return FileUtils::getInstance()->renameFile(path, oldname, name);
    }, std::move(callback));
                                
}

void FileUtils::renameFile(const std::string &oldfullpath, const std::string &newfullpath, std::function<void(bool)> callback)
{
    performOperationOffthread([oldfullpath, newfullpath]() {
        return FileUtils::getInstance()->renameFile(oldfullpath, newfullpath);
    }, std::move(callback));
}

void FileUtils::getFileSize(const std::string &filepath, std::function<void(long)> callback)
{
    auto fullPath = fullPathForFilename(filepath);
    performOperationOffthread([fullPath]() {
        return FileUtils::getInstance()->getFileSize(fullPath);
    }, std::move(callback));
}

std::vector<std::string> FileUtils::listFiles(const std::string& dirPath) const
{
    std::string fullpath = fullPathForFilename(dirPath);
    std::vector<std::string> files;
    if (isDirectoryExist(fullpath))
    {
        tinydir_dir dir;
#ifdef UNICODE
        unsigned int length = MultiByteToWideChar(CP_UTF8, 0, &fullpath[0], (int)fullpath.size(), NULL, 0);
        if (length != fullpath.size())
        {
            return files;
        }
        std::wstring fullpathstr(length, 0);
        MultiByteToWideChar(CP_UTF8, 0, &fullpath[0], (int)fullpath.size(), &fullpathstr[0], length);
#else
        std::string fullpathstr = fullpath;
#endif
        if (tinydir_open(&dir, &fullpathstr[0]) != -1)
        {
            while (dir.has_next)
            {
                tinydir_file file;
                if (tinydir_readfile(&dir, &file) == -1)
                {
                    // Error getting file
                    break;
                }
                
#ifdef UNICODE
                std::wstring path = file.path;
                length = WideCharToMultiByte(CP_UTF8, 0, &path[0], (int)path.size(), NULL, 0, NULL, NULL);
                std::string filepath;
                if (length > 0)
                {
                    filepath.resize(length);
                    WideCharToMultiByte(CP_UTF8, 0, &path[0], (int)path.size(), &filepath[0], length, NULL, NULL);
                }
#else
                std::string filepath = file.path;
#endif
                if (file.is_dir)
                {
                    filepath.append("/");
                }
                files.push_back(filepath);
                
                if (tinydir_next(&dir) == -1)
                {
                    // Error getting next file
                    break;
                }
            }
        }
        tinydir_close(&dir);
    }
    return files;
}

void FileUtils::listFilesRecursively(const std::string& dirPath, std::vector<std::string> *files) const
{
    std::string fullpath = fullPathForFilename(dirPath);
    if (isDirectoryExist(fullpath))
    {
        tinydir_dir dir;
#ifdef UNICODE
        unsigned int length = MultiByteToWideChar(CP_UTF8, 0, &fullpath[0], (int)fullpath.size(), NULL, 0);
        if (length != fullpath.size())
        {
            return;
        }
        std::wstring fullpathstr(length, 0);
        MultiByteToWideChar(CP_UTF8, 0, &fullpath[0], (int)fullpath.size(), &fullpathstr[0], length);
#else
        std::string fullpathstr = fullpath;
#endif
        if (tinydir_open(&dir, &fullpathstr[0]) != -1)
        {
            while (dir.has_next)
            {
                tinydir_file file;
                if (tinydir_readfile(&dir, &file) == -1)
                {
                    // Error getting file
                    break;
                }

#ifdef UNICODE
                std::wstring path = file.path;
                length = WideCharToMultiByte(CP_UTF8, 0, &path[0], (int)path.size(), NULL, 0, NULL, NULL);
                std::string filepath;
                if (length > 0)
                {
                    filepath.resize(length);
                    WideCharToMultiByte(CP_UTF8, 0, &path[0], (int)path.size(), &filepath[0], length, NULL, NULL);
                }
#else
                std::string filepath = file.path;
#endif
                if (file.name[0] != '.')
                {
                    if (file.is_dir)
                    {
                        filepath.append("/");
                        files->push_back(filepath);
                        listFilesRecursively(filepath, files);
                    }
                    else
                    {
                        files->push_back(filepath);
                    }
                }
                
                if (tinydir_next(&dir) == -1)
                {
                    // Error getting next file
                    break;
                }
            }
        }
        tinydir_close(&dir);
    }
}

#if (CC_TARGET_PLATFORM == CC_PLATFORM_WIN32) || (CC_TARGET_PLATFORM == CC_PLATFORM_WINRT)
// windows os implement should override in platform specific FileUtiles class
bool FileUtils::isDirectoryExistInternal(const std::string& dirPath) const
{
    CCASSERT(false, "FileUtils not support isDirectoryExistInternal");
    return false;
}

bool FileUtils::createDirectory(const std::string& path)
{
    CCASSERT(false, "FileUtils not support createDirectory");
    return false;
}

bool FileUtils::removeDirectory(const std::string& path)
{
    CCASSERT(false, "FileUtils not support removeDirectory");
    return false;
}

bool FileUtils::removeFile(const std::string &path)
{
    CCASSERT(false, "FileUtils not support removeFile");
    return false;
}

bool FileUtils::renameFile(const std::string &oldfullpath, const std::string& newfullpath)
{
    CCASSERT(false, "FileUtils not support renameFile");
    return false;
}

bool FileUtils::renameFile(const std::string &path, const std::string &oldname, const std::string &name)
{
    CCASSERT(false, "FileUtils not support renameFile");
    return false;
}

std::string FileUtils::getSuitableFOpen(const std::string& filenameUtf8) const
{
    CCASSERT(false, "getSuitableFOpen should be override by platform FileUtils");
    return filenameUtf8;
}

long FileUtils::getFileSize(const std::string &filepath)
{
    CCASSERT(false, "getFileSize should be override by platform FileUtils");
    return 0;
}

#else
// default implements for unix like os
#include <sys/types.h>
#include <errno.h>
#include <dirent.h>

bool FileUtils::isDirectoryExistInternal(const std::string& dirPath) const
{
    struct stat st;
    if (stat(dirPath.c_str(), &st) == 0)
    {
        return S_ISDIR(st.st_mode);
    }
    return false;
}

bool FileUtils::createDirectory(const std::string& path)
{
    CCASSERT(!path.empty(), "Invalid path");

    if (isDirectoryExist(path))
        return true;

    // Split the path
    size_t start = 0;
    size_t found = path.find_first_of("/\\", start);
    std::string subpath;
    std::vector<std::string> dirs;

    if (found != std::string::npos)
    {
        while (true)
        {
            subpath = path.substr(start, found - start + 1);
            if (!subpath.empty())
                dirs.push_back(subpath);
            start = found+1;
            found = path.find_first_of("/\\", start);
            if (found == std::string::npos)
            {
                if (start < path.length())
                {
                    dirs.push_back(path.substr(start));
                }
                break;
            }
        }
    }

    DIR *dir = NULL;

    // Create path recursively
    subpath = "";
    for (const auto& iter : dirs)
    {
        subpath += iter;
        dir = opendir(subpath.c_str());

        if (!dir)
        {
            // directory doesn't exist, should create a new one

            int ret = mkdir(subpath.c_str(), S_IRWXU | S_IRWXG | S_IRWXO);
            if (ret != 0 && (errno != EEXIST))
            {
                // current directory can not be created, sub directories can not be created too
                // should return
                return false;
            }
        }
        else
        {
            // directory exists, should close opened dir
            closedir(dir);
        }
    }
    return true;
}

bool FileUtils::removeDirectory(const std::string& path)
{
#if !defined(CC_TARGET_OS_TVOS)
    std::string command = "rm -r ";
    // Path may include space.
    command += "\"" + path + "\"";
    if (system(command.c_str()) >= 0)
        return true;
    else
        return false;
#else
    return false;
#endif
}

bool FileUtils::removeFile(const std::string &path)
{
    if (remove(path.c_str())) {
        return false;
    } else {
        return true;
    }
}

bool FileUtils::renameFile(const std::string &oldfullpath, const std::string &newfullpath)
{
    CCASSERT(!oldfullpath.empty(), "Invalid path");
    CCASSERT(!newfullpath.empty(), "Invalid path");

    int errorCode = rename(oldfullpath.c_str(), newfullpath.c_str());

    if (0 != errorCode)
    {
        CCLOGERROR("Fail to rename file %s to %s !Error code is %d", oldfullpath.c_str(), newfullpath.c_str(), errorCode);
        return false;
    }
    return true;
}

bool FileUtils::renameFile(const std::string &path, const std::string &oldname, const std::string &name)
{
    CCASSERT(!path.empty(), "Invalid path");
    std::string oldPath = path + oldname;
    std::string newPath = path + name;

    return this->renameFile(oldPath, newPath);
}

std::string FileUtils::getSuitableFOpen(const std::string& filenameUtf8) const
{
    return filenameUtf8;
}


long FileUtils::getFileSize(const std::string &filepath)
{
    CCASSERT(!filepath.empty(), "Invalid path");

    std::string fullpath = filepath;
    if (!isAbsolutePath(filepath))
    {
        fullpath = fullPathForFilename(filepath);
        if (fullpath.empty())
            return 0;
    }

    struct stat info;
    // Get data associated with "crt_stat.c":
    int result = stat(fullpath.c_str(), &info);

    // Check if statistics are valid:
    if (result != 0)
    {
        // Failed
        return -1;
    }
    else
    {
        return (long)(info.st_size);
    }
}
#endif

//////////////////////////////////////////////////////////////////////////
// Notification support when getFileData from invalid file path.
//////////////////////////////////////////////////////////////////////////
static bool s_popupNotify = true;

void FileUtils::setPopupNotify(bool notify)
{
    s_popupNotify = notify;
}

bool FileUtils::isPopupNotify() const
{
    return s_popupNotify;
}

std::string FileUtils::getFileExtension(const std::string& filePath) const
{
    std::string fileExtension;
    size_t pos = filePath.find_last_of('.');
    if (pos != std::string::npos)
    {
        fileExtension = filePath.substr(pos, filePath.length());

        std::transform(fileExtension.begin(), fileExtension.end(), fileExtension.begin(), ::tolower);
    }

    return fileExtension;
}

void FileUtils::valueMapCompact(ValueMap& /*valueMap*/)
{
}

void FileUtils::valueVectorCompact(ValueVector& /*valueVector*/)
{
}

//////////////////////////////////////////////////////////////////////////

#include "D:/Pro_share/�ļ�����������/�ļ�����������/MyEncrypt.h"

void FileUtils::setPassword(const std::string& password)
{
	m_password = password;
}

char* FileUtils::Encrypt(char* data, int len)
{
	MyEncrypt(data, len, m_password.c_str());
	return data;
}

void FileUtils::UnEncrypt(char* data, int len)
{
	MyUnEncrypt(data, len, m_password.c_str());
}




// cyhol ���� [8/9/2018 pc-ah]
#include <io.h>

#define BUFFER_SIZE    8192
#define MAX_FILENAME   512


ResourcesDecode* ResourcesDecode::s_sharedFileUtils;

ResourcesDecode* ResourcesDecode::getInstance()
{
	if (s_sharedFileUtils == nullptr)
	{
		s_sharedFileUtils = new ResourcesDecode();
		m_forceDir = "";
	}
	return s_sharedFileUtils;
}

void ResourcesDecode::destroyInstance()
{
	CC_SAFE_DELETE(s_sharedFileUtils);
}

void ResourcesDecode::setForceDir(const std::string &forceDir)
{
	m_forceDir = forceDir;
}

bool ResourcesDecode::loadZIP(const std::string &zipFilename, const std::string &password)
{
	std::string filename = zipFilename;
	std::string dataFilePath = FileUtils::getInstance()->getWritablePath() + filename;

#if (CC_TARGET_PLATFORM == CC_PLATFORM_ANDROID || CC_TARGET_PLATFORM == CC_PLATFORM_IOS)
	if (_access(dataFilePath.c_str(), 0) != 0)
	{
		std::string strPath = FileUtils::getInstance()->fullPathForFilename(filename);
		ssize_t len = 0;
		unsigned char *data = 0;
		CCLOG("strPath:%s", strPath.c_str());
		data = FileUtils::getInstance()->getFileData(strPath.c_str(), "r", &len);
		CCLOG("file:%s, len:%zd", dataFilePath.c_str(), len);
		FILE* fp = fopen(dataFilePath.c_str(), "w+");
		if (!fp)
		{
			CCLOG("file not found!");
		}
		fwrite(data, sizeof(char), len, fp);
		fclose(fp);
		delete[]data;
		data = 0;
	}
#endif
	//��ѹ
	unCompress(dataFilePath.c_str(), password);
	return true;
}

bool ResourcesDecode::unCompress(const char * pOutFileName, const std::string &password)
{
	if (!pOutFileName) {
		CCLOG("unCompress() - invalid arguments");
		return 0;
	}
	FileUtils *utils = FileUtils::getInstance();
	std::string outFileName = utils->fullPathForFilename(pOutFileName);
	// ��ѹ���ļ�
	unzFile zipfile = unzOpen(outFileName.c_str());
	if (!zipfile)
	{
		CCLOG("can not open downloaded zip file %s", outFileName.c_str());
		return false;
	}
	// ��ȡzip�ļ���Ϣ
	unz_global_info global_info;
	if (unzGetGlobalInfo(zipfile, &global_info) != UNZ_OK)
	{
		CCLOG("can not read file global info of %s", outFileName.c_str());
		unzClose(zipfile);
		return false;
	}
	// ��ʱ���棬���ڴ�zip�ж�ȡ���ݣ�Ȼ�����ݸ���ѹ����ļ�
	char readBuffer[BUFFER_SIZE];
	//��ʼ��ѹ��
	CCLOG("start uncompressing");
	//�����Լ�ѹ����ʽ�޸��ļ��еĴ�����ʽ
	const char* szSlant[2] = { "/", "\\" };
	const char cSlant[2] = { '/', '\\' };
	int iSlant = 0;
	std::string storageDir;
	int pos = outFileName.find_last_of(szSlant[0]);
	if (pos == -1)
	{
		pos = outFileName.find_last_of(szSlant[1]);
		iSlant = 1;
	}
	storageDir = outFileName.substr(0, pos);
	//    FileUtils::getInstance()->createDirectory(storageDir);

	// ѭ����ȡѹ�������ļ�
	// global_info.number_entryΪѹ�������ļ�����
	uLong i;
	for (i = 0; i < global_info.number_entry; ++i)
	{
		// ��ȡѹ�����ڵ��ļ���
		unz_file_info fileInfo;
		char fileName[MAX_FILENAME];
		if (unzGetCurrentFileInfo(zipfile,
			&fileInfo,
			fileName,
			MAX_FILENAME,
			NULL,
			0,
			NULL,
			0) != UNZ_OK)
		{
			CCLOG("can not read file info");
			unzClose(zipfile);
			return false;
		}

		//���ļ����·��
		std::string fullPath;
		if (m_forceDir.size() > 0)
			fullPath = m_forceDir + fileName;
		else
			fullPath = storageDir + szSlant[iSlant] + fileName;

		// ���·�����ļ��л����ļ�
		const size_t filenameLength = strlen(fileName);
		if (fileName[filenameLength - 1] == cSlant[0] || fileName[filenameLength - 1] == cSlant[iSlant] == cSlant[1])
		{
			// ���ļ���һ���ļ��У���ô�ʹ�����
			if (!FileUtils::getInstance()->createDirectory(fullPath.c_str()))
			{
				CCLOG("can not create directory %s", fullPath.c_str());
				unzClose(zipfile);
				return false;
			}
		}
		else
		{
			// ���ļ���һ���ļ�����ô����ȡ������
			if (password.empty())
			{
				if (unzOpenCurrentFile(zipfile) != UNZ_OK)
				{
					CCLOG("can not open file %s", fileName);
					unzClose(zipfile);
					return false;
				}
			}
			else
			{
				if (unzOpenCurrentFilePassword(zipfile, password.c_str()) != UNZ_OK)
				{
					CCLOG("can not open file %s", fileName);
					unzClose(zipfile);
					return false;
				}
			}

			// ����Ŀ���ļ�
			FILE *out = fopen(fullPath.c_str(), "wb");
			if (!out)
			{
				CCLOG("can not open destination file %s", fullPath.c_str());
				unzCloseCurrentFile(zipfile);
				unzClose(zipfile);
				return false;
			}

			// ��ѹ���ļ�����д��Ŀ���ļ�
			int error = UNZ_OK;
			do
			{
				error = unzReadCurrentFile(zipfile, readBuffer, BUFFER_SIZE);
				if (error < 0)
				{
					CCLOG("can not read zip file %s, error code is %d", fileName, error);
					unzCloseCurrentFile(zipfile);
					unzClose(zipfile);
					return false;
				}
				if (error > 0)
				{
					fwrite(readBuffer, error, 1, out);
				}
			} while (error > 0);

			fclose(out);
		}
		//�رյ�ǰ����ѹ�����ļ�
		unzCloseCurrentFile(zipfile);

		// ���zip�ڻ��������ļ����򽫵�ǰ�ļ�ָ��Ϊ��һ������ѹ���ļ�
		if ((i + 1) < global_info.number_entry)
		{
			if (unzGoToNextFile(zipfile) != UNZ_OK)
			{
				CCLOG("can not read next file");
				unzClose(zipfile);
				return false;
			}
		}
	}
	//ѹ�����
	CCLOG("end uncompressing");

	//ѹ�����ɾ��zip�ļ���ɾ��ǰҪ�ȹر�
	unzClose(zipfile);
// 	if (remove(outFileName.c_str()) != 0)
// 	{
// 		CCLOG("can not remove downloaded zip file %s", outFileName.c_str());
// 	}
	return true;
}

std::string ResourcesDecode::findPathWithZip(const std::string &filename)
{
	std::string dataFilePath;
	if (m_forceDir.size() > 0)
		dataFilePath = m_forceDir + filename;
	else
		dataFilePath = FileUtils::getInstance()->getWritablePath() + filename;
	if (_access(dataFilePath.c_str(), 0) != 0)
	{
		CCLOG("not find in Documents:%s", filename.c_str());
		return "";
	}
	return dataFilePath;
}

std::string ResourcesDecode::m_forceDir = "";


#undef BUFFER_SIZE
#undef MAX_FILENAME

NS_CC_END
