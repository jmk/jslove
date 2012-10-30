#include "WrapUtils.h"

#include <common/types.h>
#include <filesystem/FileData.h>
#include <filesystem/physfs/Filesystem.h>

using love::filesystem::FileData;
using love::filesystem::physfs::Filesystem;
using love::Type;

static Filesystem* _f = NULL;

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

        FileData* data = NULL;

        switch (decoderType) {
            case FileData::FILE:
                data = _f->newFileData((void*) contents.c_str(),
                                       contents.length(),
                                       name.c_str());
                break;
            case FileData::BASE64:
                data = _f->newFileData(contents.c_str(),
                                       name.c_str());
                break;
            default:
                printf("ERROR: Unknown file decoder: '%s'", decoder.c_str());
                goto undefined;
        }

        if (data) {
            JSObjectRef obj = JSLCreateObject(ctx, data,
                                              love::FILESYSTEM_FILE_DATA_T);
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
