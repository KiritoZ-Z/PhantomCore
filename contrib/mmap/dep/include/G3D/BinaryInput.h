/**
 @file BinaryInput.h
 
 @maintainer Morgan McGuire, graphics3d.com
 
 @created 2001-08-09
 @edited  2006-07-19

 Copyright 2000-2007, Morgan McGuire.
 All rights reserved.
 */

#ifndef G3D_BINARYINPUT_H
#define G3D_BINARYINPUT_H

#ifdef _MSC_VER
// Disable conditional expression is constant, which occurs incorrectly on inlined functions
#   pragma  warning(push)
#   pragma warning( disable : 4127 )
#endif

#include <assert.h>
#include <string>
#include <vector>
#include <sys/stat.h>
#include <sys/types.h>
#include <stdio.h>
#include "G3D/platform.h"
#include "G3D/Array.h"
#include "G3D/Color4.h"
#include "G3D/Color3.h"
#include "G3D/Vector4.h"
#include "G3D/Vector3.h"
#include "G3D/Vector2.h"
#include "G3D/g3dmath.h"
#include "G3D/debug.h"
#include "G3D/System.h"


namespace G3D {

#if defined(G3D_WIN32) || defined(G3D_LINUX)
    // Allow writing of integers to non-word aligned locations.
    // This is legal on x86, but not on other platforms.
    #define G3D_ALLOW_UNALIGNED_WRITES
#endif

/**
 Sequential or random access byte-order independent binary file access.
 Files compressed with zlib and beginning with an unsigned 32-bit int
 size are transparently decompressed when the compressed = true flag is
 specified to the constructor.

 For every readX method there are also versions that operate on a whole
 Array, std::vector, or C-array.  e.g. readFloat32(Array<float32>& array, n)
 These methods resize the array or std::vector to the appropriate size
 before reading.  For a C-array, they require the pointer to reference
 a memory block at least large enough to hold <I>n</I> elements.

 Most classes define serialize/deserialize methods that use BinaryInput,
 BinaryOutput, TextInput, and TextOutput.  There are text serializer 
 functions for primitive types (e.g. int, std::string, float, double) but not 
 binary serializers-- you <B>must</b> call the BinaryInput::readInt32 or
 other appropriate function.  This is because it would be very hard to 
 debug the error sequence: <CODE>serialize(1.0, bo); ... float f; deserialize(f, bi);</CODE>
 in which a double is serialized and then deserialized as a float. 
 */
class BinaryInput {
private:

    // The initial buffer will be no larger than this, but 
    // may grow if a large memory read occurs.  50 MB
    enum {INITIAL_BUFFER_LENGTH = 50000000};

    /**
     is the file big or little endian
     */
    G3DEndian       fileEndian;
    std::string     filename;

    bool            swapBytes;

    /** Next position to read from in bitString during readBits. */
    int             bitPos;

    /** Bits currently being read by readBits.  
        Contains at most 8 (low) bits.  Note that
        beginBits/readBits actually consumes one extra byte, which
        will be restored by writeBits.*/
    uint32          bitString;

    /** 1 when between beginBits and endBits, 0 otherwise. */
    int             beginEndBits;

    /** When operating on huge files, we cannot load the whole file into memory.
        This is the file position to which buffer[0] corresponds.
        */
    int64           alreadyRead;

    /**
     Length of the entire file, in bytes.  
     For the length of the buffer, see bufferLength
     */
    int64           length;

    /** Length of the array referenced by buffer. May go past the end of the file!*/
    int64           bufferLength;
    uint8*          buffer;

    /**
     Next byte in file, relative to buffer.
     */
    int64           pos;

    /**
     When true, the buffer is freed in the destructor.
     */
    bool            freeBuffer;

    /** Ensures that we are able to read at least minLength from startPosition (relative
        to start of file). */
    void loadIntoMemory(int64 startPosition, int64 minLength = 0);

    /** Verifies that at least this number of bytes can be read.*/
    inline void prepareToRead(int64 nbytes) {
        debugAssertM((int64)pos + (int64)nbytes + (int64)alreadyRead <= (int64)length, "Read past end of file.");

        if ((int64)pos + (int64)nbytes > (int64)bufferLength) {
            loadIntoMemory((int64)pos + (int64)alreadyRead, (int64)nbytes);    
        }
    }

    // Not implemented on purpose, don't use
    BinaryInput(const BinaryInput&);
    BinaryInput& operator=(const BinaryInput&);
    bool operator==(const BinaryInput&);

    /** Buffer is compressed; replace it with a decompressed version */
    void decompress();
public:

    /** false, constant to use with the copyMemory option */
    static const bool       NO_COPY;

    /**
       If the file cannot be opened, a zero length buffer is presented.
       Automatically opens files that are inside zipfiles.

       @param compressed Set to true if and only if the file was
       compressed using BinaryOutput's zlib compression.  This has
       nothing to do with whether the input is in a zipfile.
    */
    BinaryInput(
        const std::string&  filename,
        G3DEndian           fileEndian,
        bool                compressed = false);

    /**
     Creates input stream from an in memory source.
     Unless you specify copyMemory = false, the data is copied
     from the pointer, so you may deallocate it as soon as the
     object is constructed.  It is an error to specify copyMemory = false
     and compressed = true.

     To decompress part of a file, you can follow the following paradigm:

     <PRE>
        BinaryInput master(...);

        // read from master to point where compressed data exists.

        BinaryInput subset(master.getCArray() + master.getPosition(), 
                           master.length() - master.getPosition(),
                           true, true);

        // Now read from subset (it is ok for master to go out of scope)
     </PRE>
     */
    BinaryInput(
        const uint8*        data,
        int64               dataLen,
        G3DEndian           dataEndian,
        bool                compressed = false,
        bool                copyMemory = true);

    virtual ~BinaryInput();


    std::string getFilename() const {
        return filename;
    }

    /**
     Returns a pointer to the internal memory buffer.
     May throw an exception for huge files.
     */
    const uint8* getCArray() const {
        if (alreadyRead > 0) {
            throw "Cannot getCArray for a huge file";
        }
        return buffer;
    }

    /**
     Performs bounds checks in debug mode.  [] are relative to
     the start of the file, not the current position.
     Seeks to the new position before reading (and leaves 
     that as the current position)
     */
    inline uint8 operator[](int64 n) {
        setPosition(n);
        return readUInt8();
    }

    /**
     Returns the length of the file in bytes.
     */
    inline int64 getLength() const {
        return length;
    }

    inline int64 size() const {
        return getLength();
    }

    /**
     Returns the current byte position in the file,
     where 0 is the beginning and getLength() - 1 is the end.
     */
    inline int64 getPosition() const {
        return (int64)pos + (int64)alreadyRead;
    }

    /**
     Sets the position.  Cannot set past length.
     May throw a char* when seeking backwards more than 10 MB on a huge file.
     */
    inline void setPosition(int64 p) {
        debugAssertM(p <= length, "Read past end of file");
        pos = (int64)p - (int64)alreadyRead;
        if ((pos < 0) || (pos > bufferLength)) {
            loadIntoMemory((int64)pos + (int64)alreadyRead);
        }
    }

    /**
     Goes back to the beginning of the file.
     */
    inline void reset() {
        setPosition(0);
    }

    inline int8 readInt8() {
        prepareToRead(1);
        return buffer[pos++];
    }

    inline bool readBool8() {
        return (readInt8() != 0);
    }

    inline uint8 readUInt8() {
        prepareToRead(1);
        return ((uint8*)buffer)[pos++];
    }

    uint16 inline readUInt16() {
        prepareToRead(2);

        pos += 2;
        if (swapBytes) {
            uint8 out[2];
            out[0] = buffer[pos - 1];
            out[1] = buffer[pos - 2];
            return *(uint16*)out;
        } else {
            #ifdef G3D_ALLOW_UNALIGNED_WRITES
                return *(uint16*)(&buffer[pos - 2]);
            #else
                uint8 out[2];
                out[0] = buffer[pos - 2];
                out[1] = buffer[pos - 1];
                return *(uint16*)out;
            #endif
        }

    }

    inline int16 readInt16() {
        uint16 a = readUInt16();
        return *(int16*)&a;
    }

    inline uint32 readUInt32() {
        prepareToRead(4);

        pos += 4;
        if (swapBytes) {
            uint8 out[4];
            out[0] = buffer[pos - 1];
            out[1] = buffer[pos - 2];
            out[2] = buffer[pos - 3];
            out[3] = buffer[pos - 4];
            return *(uint32*)out;
        } else {
            #ifdef G3D_ALLOW_UNALIGNED_WRITES
                return *(uint32*)(&buffer[pos - 4]);
            #else
                uint8 out[4];
                out[0] = buffer[pos - 4];
                out[1] = buffer[pos - 3];
                out[2] = buffer[pos - 2];
                out[3] = buffer[pos - 1];
                return *(uint32*)out;
            #endif
        }
    }


    inline int32 readInt32() {
        uint32 a = readUInt32();
        return *(int32*)&a;
    }

    uint64 readUInt64();

    inline int64 readInt64() {
        uint64 a = readUInt64();
        return *(int64*)&a;
    }

    inline float32 readFloat32() {
        uint32 a = readUInt32();
        return *(float32*)&a;
    }    

    inline float64 readFloat64() {
        uint64 a = readUInt64();
        return *(float64*)&a;
    }

    void readBytes(void* bytes, int64 n);

    /**
     Reads an n character string.  The string is not
     required to end in NULL in the file but will
     always be a proper std::string when returned.
     */
    std::string readString(int64 n);

    /**
     Reads until NULL or the end of the file is encountered.
     */
    std::string readString();

    /**
     Reads until NULL or the end of the file is encountered.
     If the string has odd length (including NULL), reads 
     another byte.
     */
    std::string readStringEven();


    std::string readString32();

    Vector4 readVector4();
    Vector3 readVector3();
    Vector2 readVector2();

    Color4 readColor4();
    Color3 readColor3();

    /**
     Skips ahead n bytes.
     */
    inline void skip(int64 n) {
        setPosition((int64)pos + (int64)alreadyRead + n);
    }

    /**
      Returns true if the position is not at the end of the file
    */
    inline bool hasMore() const {
	return (int64)pos + (int64)alreadyRead < (int64)length;
    }

    /** Prepares for bit reading via readBits.  Only readBits can be
        called between beginBits and endBits without corrupting the
        data stream. */
    void beginBits();

    /** Can only be called between beginBits and endBits */
    uint32 readBits(int numBits);

    /** Ends bit-reading. */
    void endBits();

    /**
     Given a 32-bit integer, returns the integer with the bytes in the opposite order.
     */
    static uint32 flipEndian32(const uint32 x) {
        return (x << 24) | ((x & 0xFF00) << 8) | 
               ((x & 0xFF0000) >> 8) | ((x & 0xFF000000) >> 24);
    }

    /**
     Given a 16-bit integer, returns the integer with the bytes in the opposite order.
     */
    static uint16 flipEndian16(const uint16 x) {
        return (x << 8) | ((x & 0xFF00) >> 8);
    }

#   define DECLARE_READER(ucase, lcase)\
    void read##ucase(lcase* out, int64 n);\
    void read##ucase(std::vector<lcase>& out, int64 n);\
    void read##ucase(Array<lcase>& out, int64 n);

    DECLARE_READER(Bool8,   bool)
    DECLARE_READER(UInt8,   uint8)
    DECLARE_READER(Int8,    int8)
    DECLARE_READER(UInt16,  uint16)
    DECLARE_READER(Int16,   int16)
    DECLARE_READER(UInt32,  uint32)
    DECLARE_READER(Int32,   int32)
    DECLARE_READER(UInt64,  uint64)
    DECLARE_READER(Int64,   int64)
    DECLARE_READER(Float32, float32)
    DECLARE_READER(Float64, float64)    
#   undef DECLARE_READER
};


}

#ifdef _MSC_VER
#   pragma  warning(pop)
#endif

#endif
