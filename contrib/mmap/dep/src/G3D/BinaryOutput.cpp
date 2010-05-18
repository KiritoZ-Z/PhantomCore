/**
 @file BinaryOutput.cpp
 
 @author Morgan McGuire, graphics3d.com
 Copyright 2002-2007, Morgan McGuire, All rights reserved.
 
 @created 2002-02-20
 @edited  2005-06-07
 */

#include "G3D/platform.h"
#include "G3D/BinaryOutput.h"
#include "G3D/fileutils.h"
#include "G3D/stringutils.h"
#include "G3D/Array.h"
#include <zlib.h>

#include <cstring>

// Largest memory buffer that the system will use for writing to
// disk.  After this (or if the system runs out of memory)
// chunks of the file will be dumped to disk.
//
// Currently 400 MB
#define MAX_BINARYOUTPUT_BUFFER_SIZE 400000000

namespace G3D {

void BinaryOutput::writeBool8(const std::vector<bool>& out, int n) {
    for (int i = 0; i < n; ++i) {
        writeBool8(out[i]);
    }
}


void BinaryOutput::writeBool8(const Array<bool>& out, int n) {
    writeBool8(out.getCArray(), n);
}

#define IMPLEMENT_WRITER(ucase, lcase)\
void BinaryOutput::write##ucase(const std::vector<lcase>& out, int n) {\
    write##ucase(&out[0], n);\
}\
\
\
void BinaryOutput::write##ucase(const Array<lcase>& out, int n) {\
    write##ucase(out.getCArray(), n);\
}


IMPLEMENT_WRITER(UInt8,   uint8)
IMPLEMENT_WRITER(Int8,    int8)
IMPLEMENT_WRITER(UInt16,  uint16)
IMPLEMENT_WRITER(Int16,   int16)
IMPLEMENT_WRITER(UInt32,  uint32)
IMPLEMENT_WRITER(Int32,   int32)
IMPLEMENT_WRITER(UInt64,  uint64)
IMPLEMENT_WRITER(Int64,   int64)
IMPLEMENT_WRITER(Float32, float32)
IMPLEMENT_WRITER(Float64, float64)    

#undef IMPLEMENT_WRITER

// Data structures that are one byte per element can be 
// directly copied, regardles of endian-ness.
#define IMPLEMENT_WRITER(ucase, lcase)\
void BinaryOutput::write##ucase(const lcase* out, int n) {\
    if (sizeof(lcase) == 1) {\
        writeBytes((void*)out, n);\
    } else {\
        for (int i = 0; i < n ; ++i) {\
            write##ucase(out[i]);\
        }\
    }\
}

IMPLEMENT_WRITER(Bool8,   bool)
IMPLEMENT_WRITER(UInt8,   uint8)
IMPLEMENT_WRITER(Int8,    int8)

#undef IMPLEMENT_WRITER


#define IMPLEMENT_WRITER(ucase, lcase)\
void BinaryOutput::write##ucase(const lcase* out, int n) {\
    if (swapBytes) {\
        for (int i = 0; i < n; ++i) {\
            write##ucase(out[i]);\
        }\
    } else {\
        writeBytes((const void*)out, sizeof(lcase) * n);\
    }\
}


IMPLEMENT_WRITER(UInt16,  uint16)
IMPLEMENT_WRITER(Int16,   int16)
IMPLEMENT_WRITER(UInt32,  uint32)
IMPLEMENT_WRITER(Int32,   int32)
IMPLEMENT_WRITER(UInt64,  uint64)
IMPLEMENT_WRITER(Int64,   int64)
IMPLEMENT_WRITER(Float32, float32)
IMPLEMENT_WRITER(Float64, float64)    

#undef IMPLEMENT_WRITER


void BinaryOutput::reallocBuffer(size_t bytes, size_t oldBufferLen) {
    //debugPrintf("reallocBuffer(%d, %d)\n", bytes, oldBufferLen);

    size_t newBufferLen = (int)(bufferLen * 1.5) + 100;
    uint8* newBuffer = NULL;

    if ((filename == "<memory>") || (newBufferLen < MAX_BINARYOUTPUT_BUFFER_SIZE)) {
        // We're either writing to memory (in which case we *have* to try and allocate)
        // or we've been asked to allocate a reasonable size buffer.

        //debugPrintf("  realloc(%d)\n", newBufferLen); 
        newBuffer = (uint8*)System::realloc(buffer, newBufferLen);
        if (newBuffer != NULL) {
            maxBufferLen = newBufferLen;
        }
    }

    if ((newBuffer == NULL) && (bytes > 0)) {
        // Realloc failed; we're probably out of memory.  Back out
        // the entire call and try to dump some data to disk.
        bufferLen = oldBufferLen;
        reserveBytesWhenOutOfMemory(bytes);
    } else {
        buffer = newBuffer;
        debugAssert(isValidHeapPointer(buffer));
    }
}


void BinaryOutput::reserveBytesWhenOutOfMemory(size_t bytes) {
    if (filename == "<memory>") {
        throw "Out of memory while writing to memory in BinaryOutput (no RAM left).";
    } else if ((int)bytes > (int)maxBufferLen) {
        throw "Out of memory while writing to disk in BinaryOutput (could not create a large enough buffer).";
    } else {

        // Dump the contents to disk.  In order to enable seeking backwards, 
        // we keep the last 10 MB in memory.
        int writeBytes = bufferLen - 10 * 1024 * 1024;

        if (writeBytes < bufferLen / 3) {
            // We're going to write less than 1/3 of the file;
            // give up and just write the whole thing.
            writeBytes = bufferLen;
        }
        debugAssert(writeBytes > 0);

        //debugPrintf("Writing %d bytes to disk\n", writeBytes);

        const char* mode = (alreadyWritten > 0) ? "ab" : "wb";
        FILE* file = fopen(filename.c_str(), mode);
        debugAssert(file);

        size_t count = fwrite(buffer, 1, writeBytes, file);
        debugAssert((int)count == writeBytes); (void)count;

        fclose(file);
        file = NULL;

        // Record that we saved this data.
        alreadyWritten += writeBytes;
        bufferLen -= writeBytes;
        pos -= writeBytes;

        debugAssert(bufferLen < maxBufferLen);
        debugAssert(bufferLen >= 0);
        debugAssert(pos >= 0);
        debugAssert(pos <= bufferLen);

        // Shift the unwritten data back appropriately in the buffer.
        debugAssert(isValidHeapPointer(buffer));
        System::memcpy(buffer, buffer + writeBytes, bufferLen);
        debugAssert(isValidHeapPointer(buffer));

        // *now* we allocate bytes (there should presumably be enough 
        // space in the buffer; if not, we'll come back through this 
        // code and dump the last 10MB to disk as well.  Note that the 
        // bytes > maxBufferLen case above would already have triggered
        // if this call couldn't succeed. 
        reserveBytes(bytes);
    }
}


BinaryOutput::BinaryOutput() {
    alreadyWritten = 0;
    swapBytes = false;
    pos       = 0;
    filename  = "<memory>";
    buffer = NULL;
    bufferLen = 0;
    maxBufferLen = 0;
    beginEndBits = 0;
    bitString = 0;
    bitPos = 0;
    committed = false;
}


BinaryOutput::BinaryOutput(
    const std::string&  filename,
    G3DEndian           fileEndian) {

    pos = 0;
    alreadyWritten = 0;
    setEndian(fileEndian);
    this->filename = filename;
    buffer = NULL;
    bufferLen = 0;
    maxBufferLen = 0;
    beginEndBits = 0;
    bitString = 0;
    bitPos = 0;
    committed = false;
}


void BinaryOutput::reset() {
    debugAssert(beginEndBits == 0);
    alwaysAssertM(filename == "<memory>", 
        "Can only reset a BinaryOutput that writes to memory.");

    // Do not reallocate, just clear the size of the buffer.
    pos = 0;
    alreadyWritten = 0;
    bufferLen = 0;
    beginEndBits = 0;
    bitString = 0;
    bitPos = 0;
    committed = false;
}


BinaryOutput::~BinaryOutput() {
    debugAssert((buffer == NULL) || isValidHeapPointer(buffer));
    System::free(buffer);
    buffer = NULL;
    bufferLen = 0;
    maxBufferLen = 0;
}


void BinaryOutput::setEndian(G3DEndian fileEndian) {
    swapBytes = (fileEndian != System::machineEndian());
}


void BinaryOutput::compress(int level) {
    if (alreadyWritten > 0) {
        throw "Cannot compress huge files (part of this file has already been written to disk).";
    }

    // Old buffer size
    int L = bufferLen;
    uint8* convert = (uint8*)&L;

    // Zlib requires the output buffer to be this big
    unsigned long newSize = iCeil(bufferLen * 1.01) + 12;
    uint8* temp = (uint8*)System::malloc(newSize);
    int result = compress2(temp, &newSize, buffer, bufferLen, level); 

    debugAssert(result == Z_OK); (void)result;

    // Write the header
    if (swapBytes) {
        buffer[0] = convert[3];
        buffer[1] = convert[2];
        buffer[2] = convert[1];
        buffer[3] = convert[0];
    } else {
        buffer[0] = convert[0];
        buffer[1] = convert[1];
        buffer[2] = convert[2];
        buffer[3] = convert[3];
    }

    // Write the data
    if ((int64)newSize + 4 > (int64)maxBufferLen) {
        maxBufferLen = newSize + 4;
        buffer = (uint8*)System::realloc(buffer, maxBufferLen);
    }
    bufferLen = newSize + 4;
    System::memcpy(buffer + 4, temp, newSize);
    pos = bufferLen;

    System::free(temp);
}


void BinaryOutput::commit(bool flush) {
    debugAssertM(! committed, "Cannot commit twice");
    committed = true;
    debugAssertM(beginEndBits == 0, "Missing endBits before commit");

    // Make sure the directory exists.
    std::string root, base, ext, path;
    Array<std::string> pathArray;
    parseFilename(filename, root, pathArray, base, ext); 

    path = root + stringJoin(pathArray, '/');
    if (! fileExists(path, false)) {
        createDirectory(path);
    }

    const char* mode = (alreadyWritten > 0) ? "ab" : "wb";

    FILE* file = fopen(filename.c_str(), mode);
    debugAssertM(file, 
                 std::string("Could not open '") + filename + "'");

    alreadyWritten += bufferLen;

    fwrite(buffer, bufferLen, 1, file);
    if (flush) {
        fflush(file);
    }
    fclose(file);
    file = NULL;
}


void BinaryOutput::commit(
    uint8*                  out) {
    debugAssertM(! committed, "Cannot commit twice");
    committed = true;

    System::memcpy(out, buffer, bufferLen);
}


void BinaryOutput::writeUInt16(uint16 u) {
    reserveBytes(2);

    uint8* convert = (uint8*)&u;

    if (swapBytes) {
        buffer[pos]     = convert[1];
        buffer[pos + 1] = convert[0];
    } else {
        *(uint16*)(buffer + pos) = u;
    }

    pos += 2;
}


void BinaryOutput::writeUInt32(uint32 u) {
    reserveBytes(4);

    uint8* convert = (uint8*)&u;

    debugAssert(beginEndBits == 0);

    if (swapBytes) {
        buffer[pos]     = convert[3];
        buffer[pos + 1] = convert[2];
        buffer[pos + 2] = convert[1];
        buffer[pos + 3] = convert[0];
    } else {
        *(uint32*)(buffer + pos) = u;
    }

    pos += 4;
}


void BinaryOutput::writeUInt64(uint64 u) {
    reserveBytes(8);

    uint8* convert = (uint8*)&u;

    if (swapBytes) {
        buffer[pos]     = convert[7];
        buffer[pos + 1] = convert[6];
        buffer[pos + 2] = convert[5];
        buffer[pos + 3] = convert[4];
        buffer[pos + 4] = convert[3];
        buffer[pos + 5] = convert[2];
        buffer[pos + 6] = convert[1];
        buffer[pos + 7] = convert[0];
    } else {
        *(uint64*)(buffer + pos) = u;
    }

    pos += 8;
}


void BinaryOutput::writeString(const char* s) {
    // +1 is because strlen doesn't count the null
    int len = strlen(s) + 1;

    debugAssert(beginEndBits == 0);
    reserveBytes(len);
    System::memcpy(buffer + pos, s, len);
    pos += len;
}


void BinaryOutput::writeStringEven(const char* s) {
    // +1 is because strlen doesn't count the null
    int len = strlen(s) + 1;

    reserveBytes(len);
    System::memcpy(buffer + pos, s, len);
    pos += len;

    // Pad with another NULL
    if ((len % 2) == 1) {
        writeUInt8(0);
    }
}


void BinaryOutput::writeString32(const char* s) {
    writeUInt32(strlen(s) + 1);
    writeString(s);
}


void BinaryOutput::writeVector4(const Vector4& v) {
    writeFloat32(v.x);
    writeFloat32(v.y);
    writeFloat32(v.z);
    writeFloat32(v.w);
}


void BinaryOutput::writeVector3(const Vector3& v) {
    writeFloat32(v.x);
    writeFloat32(v.y);
    writeFloat32(v.z);
}


void BinaryOutput::writeVector2(const Vector2& v) {
    writeFloat32(v.x);
    writeFloat32(v.y);
}


void BinaryOutput::writeColor4(const Color4& v) {
    writeFloat32(v.r);
    writeFloat32(v.g);
    writeFloat32(v.b);
    writeFloat32(v.a);
}


void BinaryOutput::writeColor3(const Color3& v) {
    writeFloat32(v.r);
    writeFloat32(v.g);
    writeFloat32(v.b);
}


void BinaryOutput::beginBits() {
    debugAssertM(beginEndBits == 0, "Already in beginBits...endBits");
    bitString = 0x00;
    bitPos = 0;
    beginEndBits = 1;
}


void BinaryOutput::writeBits(uint32 value, int numBits) {

    while (numBits > 0) {
        // Extract the current bit of value and
        // insert it into the current byte
        bitString |= (value & 1) << bitPos;
        ++bitPos;
        value = value >> 1;
        --numBits;

        if (bitPos > 7) {
            // We've reached the end of this byte
            writeUInt8(bitString);
            bitString = 0x00;
            bitPos = 0;
        }
    }
}


void BinaryOutput::endBits() {
    debugAssertM(beginEndBits == 1, "Not in beginBits...endBits");
    if (bitPos > 0) {
        writeUInt8(bitString);
    }
    bitString = 0;
    bitPos = 0;
    beginEndBits = 0;
}

}
