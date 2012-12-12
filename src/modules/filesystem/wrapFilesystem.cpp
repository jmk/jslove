#include "WrapUtils.h"

#include <common/types.h>
#include <filesystem/File.h>
#include <filesystem/FileData.h>
#include <filesystem/physfs/Filesystem.h>

using love::Exception;
using love::Type;
using love::filesystem::File;
using love::filesystem::FileData;
using love::filesystem::physfs::Filesystem;

DECLARE_TYPE(File,     love::FILESYSTEM_FILE_T);
DECLARE_TYPE(FileData, love::FILESYSTEM_FILE_DATA_T);

static Filesystem* _f = NULL;

WRAP_FUNCTION(init)
{
    if (argCount == 1) {
        std::string arg0 = JSLGetString(ctx, args[0]);
        try {
            _f->init(arg0.c_str());
        } catch (Exception & e) {
            printf("ERROR: Couldn't init filesystem module: %s\n", e.what());
        }
    }
    return JSValueMakeUndefined(ctx);
}

WRAP_FUNCTION(setRelease)
{
    if (argCount == 1) {
        bool release = JSLGetBoolean(ctx, args[0]);
        _f->setRelease(release);
    }
    return JSValueMakeUndefined(ctx);
}

WRAP_FUNCTION(setIdentity)
{
    if (argCount == 1) {
        std::string identity = JSLGetString(ctx, args[0]);

        bool result = _f->setIdentity(identity.c_str());
        return JSValueMakeBoolean(ctx, result);
    }
    return JSValueMakeUndefined(ctx);
}

WRAP_FUNCTION(setSource)
{
    if (argCount == 1) {
        std::string source = JSLGetString(ctx, args[0]);

        bool result = _f->setSource(source.c_str());

        if (not result) {
            printf("ERROR: Couldn't set source.\n");
        }

    printf("setSource! %s -> %d\n", source.c_str(), result);
        return JSValueMakeBoolean(ctx, result);
    }
    return JSValueMakeUndefined(ctx);
}

WRAP_FUNCTION(getWorkingDirectory)
{
    std::string dir(_f->getWorkingDirectory());
    return JSLMakeStringValue(ctx, dir);
}

WRAP_FUNCTION(exists)
{
    if (argCount == 1) {
        std::string path = JSLGetString(ctx, args[0]);

        bool result = _f->exists(path.c_str());
        return JSValueMakeBoolean(ctx, result);
    }
    return JSValueMakeUndefined(ctx);
}

WRAP_FUNCTION(newFileData)
{
    if (argCount == 3) {
        if (not JSValueIsString(ctx, args[0])) {
            printf("ERROR: Invalid contents");
            goto undefined;
        }
        if (not JSValueIsString(ctx, args[1])) {
            printf("ERROR: Invalid name");
            goto undefined;
        }

        std::string contents = JSLGetString(ctx, args[0]);
        std::string name     = JSLGetString(ctx, args[1]);
        std::string decoder  = JSLGetString(ctx, args[2]);

        // Create new FileData object.
        FileData::Decoder decoderType = FileData::FILE;

        if (not decoder.empty()) {
            // Note: This function overwrites 'decoderType'.
            FileData::getConstant(decoder.c_str(), decoderType);
        }

        FileData* fileData = NULL;

        switch (decoderType) {
            case FileData::FILE:
                fileData = _f->newFileData((void*) contents.c_str(),
                                           contents.length(),
                                           name.c_str());
                break;
            case FileData::BASE64:
                fileData = _f->newFileData(contents.c_str(),
                                           name.c_str());
                break;
            default:
                printf("ERROR: Unknown file decoder: '%s'", decoder.c_str());
                goto undefined;
        }

        if (fileData) {
            JSObjectRef obj = JSLCreateObject(ctx, fileData);
            return obj;
        }
    }

undefined:
    return JSValueMakeUndefined(ctx);
}

WRAP_MODULE(filesystem)
{
    printf("INIT filesystem\n");

    if (not _f) {
        _f = new Filesystem();
    } else {
        printf("ERROR: already initialized filesystem\n");
    }

    // XXX: Not strictly necessary without a private data object, but may be
    //      useful later.
    JSClassDefinition def = kJSClassDefinitionEmpty;
    JSClassRef cls = JSClassCreate(&def);

    JSObjectRef obj = JSObjectMake(ctx, cls, NULL);

    JSLAddFunction(ctx, obj, "init", init);
    JSLAddFunction(ctx, obj, "setRelease", setRelease);
    JSLAddFunction(ctx, obj, "setIdentity", setIdentity);
    JSLAddFunction(ctx, obj, "setSource", setSource);
//    newFile
    JSLAddFunction(ctx, obj, "getWorkingDirectory", getWorkingDirectory);
//    getUserDirectory
//    getAppdataDirectory
//    getSaveDirectory
    JSLAddFunction(ctx, obj, "exists", exists);
//    isDirectory
//    isFile
//    mkdir
//    remove
//    read
//    write
//    enumerate
//    lines
//    load
//    getLastModified
    JSLAddFunction(ctx, obj, "newFileData", newFileData);

    return obj;
}

namespace love
{
    namespace filesystem
    {
        namespace physfs
        {
            // XXX: Symbol required by File class.
            bool hack_setupWriteDirectory() {
                return false;
            }
        }
    }
}
