/**
 @file RegistryUtil.cpp

 @created 2006-04-06
 @edited  2006-04-24

 Copyright 2000-2006, Morgan McGuire.
 All rights reserved.
*/

#include "G3D/platform.h"

// This file is only used on Windows
#ifdef G3D_WIN32

#include "G3D/RegistryUtil.h"
#include "G3D/System.h"

// declare HKEY constants as needed for VC6
#if !defined(HKEY_PERFORMANCE_DATA)
    #define HKEY_PERFORMANCE_DATA       (( HKEY ) ((LONG)0x80000004) )
#endif
#if !defined(HKEY_PERFORMANCE_TEXT)
    #define HKEY_PERFORMANCE_TEXT       (( HKEY ) ((LONG)0x80000050) )
#endif
#if !defined(HKEY_PERFORMANCE_NLSTEXT)
    #define HKEY_PERFORMANCE_NLSTEXT    (( HKEY ) ((LONG)0x80000060) )
#endif

namespace G3D {

// static helpers
static HKEY getKeyFromString(const char* str, size_t length);


bool RegistryUtil::keyExists(const std::string& key) {
    size_t pos = key.find('\\', 0);
    if ( pos == std::string::npos ) {
        return false;
    }

    HKEY hkey = getKeyFromString(key.c_str(), pos);

    if ( hkey == NULL ) {
        return false;
    }

    debugAssert(key.size() > (pos + 1));
    HKEY openKey;
    int32 result = RegOpenKeyExA(hkey, (key.c_str() + pos + 1), 0, KEY_ALL_ACCESS, &openKey);

    if ( result == ERROR_SUCCESS ) {
        RegCloseKey(openKey);
        return true;
    } else {
        return false;
    }
}

bool RegistryUtil::readInt32(const std::string& key, int32& valueData) {

    size_t pos = key.find('\\', 0);
    if ( pos == std::string::npos ) {
        return false;
    }

    HKEY hkey = getKeyFromString(key.c_str(), pos);

    if ( hkey == NULL ) {
        return false;
    }

    debugAssert(key.size() > (pos + 1));

    size_t valuePos = key.rfind('\\');
    
    if ( valuePos != std::string::npos ) {
        std::string subKey = key.substr(pos + 1, valuePos - pos - 1);
        std::string value = key.substr(valuePos + 1, key.size() - valuePos);

        HKEY openKey;
        int32 result = RegOpenKeyExA(hkey, subKey.c_str(), 0, KEY_ALL_ACCESS, &openKey);

        if ( result == ERROR_SUCCESS ) {
            uint32 dataSize = sizeof(int32);
            result = RegQueryValueExA(openKey, value.c_str(), NULL, NULL, reinterpret_cast<LPBYTE>(&valueData), reinterpret_cast<LPDWORD>(&dataSize));

            debugAssertM(result == ERROR_SUCCESS, "Could not read registry key value.");

            RegCloseKey(openKey);
            return (result == ERROR_SUCCESS);
        }
    }
    return false;
}

bool RegistryUtil::readBytes(const std::string& key, uint8* valueData, uint32& dataSize) {
    size_t pos = key.find('\\', 0);
    if ( pos == std::string::npos ) {
        return false;
    }

    HKEY hkey = getKeyFromString(key.c_str(), pos);

    if ( hkey == NULL ) {
        return false;
    }

    debugAssert(key.size() > (pos + 1));

    size_t valuePos = key.rfind('\\');
    
    if ( valuePos != std::string::npos ) {
        std::string subKey = key.substr(pos + 1, valuePos - pos - 1);
        std::string value = key.substr(valuePos + 1, key.size() - valuePos);

        HKEY openKey;
        int32 result = RegOpenKeyExA(hkey, subKey.c_str(), 0, KEY_ALL_ACCESS, &openKey);

        if ( result == ERROR_SUCCESS ) {

            if ( valueData == NULL ) {
                result = RegQueryValueExA(openKey, value.c_str(), NULL, NULL, NULL, reinterpret_cast<LPDWORD>(&dataSize));
            } else {
                result = RegQueryValueExA(openKey, value.c_str(), NULL, NULL, reinterpret_cast<LPBYTE>(&valueData), reinterpret_cast<LPDWORD>(&dataSize));
            }

            debugAssertM(result == ERROR_SUCCESS, "Could not read registry key value.");

            RegCloseKey(openKey);
            return (result == ERROR_SUCCESS);
        }
    }

    return false;
}

bool RegistryUtil::readString(const std::string& key, std::string& valueData) {
    size_t pos = key.find('\\', 0);
    if ( pos == std::string::npos ) {
        return false;
    }

    HKEY hkey = getKeyFromString(key.c_str(), pos);

    if ( hkey == NULL ) {
        return false;
    }

    debugAssert(key.size() > (pos + 1));

    size_t valuePos = key.rfind('\\');
    
    if ( valuePos != std::string::npos ) {
        std::string subKey = key.substr(pos + 1, valuePos - pos - 1);
        std::string value = key.substr(valuePos + 1, key.size() - valuePos);

        HKEY openKey;
        int32 result = RegOpenKeyExA(hkey, subKey.c_str(), 0, KEY_ALL_ACCESS, &openKey);

        if ( result == ERROR_SUCCESS ) {
            uint32 dataSize = 0;

            result = RegQueryValueExA(openKey, value.c_str(), NULL, NULL, NULL, reinterpret_cast<LPDWORD>(&dataSize));

            if ( result == ERROR_SUCCESS ) {
                char* tmpStr = reinterpret_cast<char*>(System::malloc(dataSize));
                System::memset(tmpStr, 0, dataSize);

                result = RegQueryValueExA(openKey, value.c_str(), NULL, NULL, reinterpret_cast<LPBYTE>(tmpStr), reinterpret_cast<LPDWORD>(&dataSize));
                
                if ( result == ERROR_SUCCESS ) {
                    valueData = tmpStr;
                }
            }
            debugAssertM(result == ERROR_SUCCESS, "Could not read registry key value.");

            RegCloseKey(openKey);
            return (result == ERROR_SUCCESS);
        }
    }

    return false;
}

bool RegistryUtil::writeInt32(const std::string& key, int32 valueData) {

    size_t pos = key.find('\\', 0);
    if ( pos == std::string::npos ) {
        return false;
    }

    HKEY hkey = getKeyFromString(key.c_str(), pos);

    if ( hkey == NULL ) {
        return false;
    }

    debugAssert(key.size() > (pos + 1));

    size_t valuePos = key.rfind('\\');
    
    if ( valuePos != std::string::npos ) {
        std::string subKey = key.substr(pos + 1, valuePos - pos - 1);
        std::string value = key.substr(valuePos + 1, key.size() - valuePos);

        HKEY openKey;
        int32 result = RegOpenKeyExA(hkey, subKey.c_str(), 0, KEY_ALL_ACCESS, &openKey);

        if ( result == ERROR_SUCCESS ) {
            result = RegSetValueExA(openKey, value.c_str(), NULL, REG_DWORD, reinterpret_cast<const BYTE*>(&valueData), sizeof(int32));

            debugAssertM(result == ERROR_SUCCESS, "Could not write registry key value.");

            RegCloseKey(openKey);
            return (result == ERROR_SUCCESS);
        }
    }
    return false;
}

bool RegistryUtil::writeBytes(const std::string& key, const uint8* valueData, uint32 dataSize) {
    debugAssert(valueData);

    size_t pos = key.find('\\', 0);
    if ( pos == std::string::npos ) {
        return false;
    }

    HKEY hkey = getKeyFromString(key.c_str(), pos);

    if ( hkey == NULL ) {
        return false;
    }

    debugAssert(key.size() > (pos + 1));

    size_t valuePos = key.rfind('\\');
    
    if ( valuePos != std::string::npos ) {
        std::string subKey = key.substr(pos + 1, valuePos - pos - 1);
        std::string value = key.substr(valuePos + 1, key.size() - valuePos);

        HKEY openKey;
        int32 result = RegOpenKeyExA(hkey, subKey.c_str(), 0, KEY_ALL_ACCESS, &openKey);

        if ( result == ERROR_SUCCESS ) {

            if (valueData) {
                result = RegSetValueExA(openKey, value.c_str(), NULL, REG_BINARY, reinterpret_cast<const BYTE*>(valueData), dataSize);
            }

            debugAssertM(result == ERROR_SUCCESS, "Could not write registry key value.");

            RegCloseKey(openKey);
            return (result == ERROR_SUCCESS);
        }
    }

    return false;
}

bool RegistryUtil::writeString(const std::string& key, const std::string& valueData) {
    size_t pos = key.find('\\', 0);
    if ( pos == std::string::npos ) {
        return false;
    }

    HKEY hkey = getKeyFromString(key.c_str(), pos);

    if ( hkey == NULL ) {
        return false;
    }

    debugAssert(key.size() > (pos + 1));

    size_t valuePos = key.rfind('\\');
    
    if ( valuePos != std::string::npos ) {
        std::string subKey = key.substr(pos + 1, valuePos - pos - 1);
        std::string value = key.substr(valuePos + 1, key.size() - valuePos);

        HKEY openKey;
        int32 result = RegOpenKeyExA(hkey, subKey.c_str(), 0, KEY_ALL_ACCESS, &openKey);

        if ( result == ERROR_SUCCESS ) {
            result = RegSetValueExA(openKey, value.c_str(), NULL, REG_SZ, reinterpret_cast<const BYTE*>(valueData.c_str()), (valueData.size() + 1));                
            debugAssertM(result == ERROR_SUCCESS, "Could not write registry key value.");

            RegCloseKey(openKey);
            return (result == ERROR_SUCCESS);
        }
    }

    return false;
}


// static helpers
static HKEY getKeyFromString(const char* str, uint32 length) {
    debugAssert(str);

    if (str) {
        if ( strncmp(str, "HKEY_CLASSES_ROOT", length) == 0 ) {
            return HKEY_CLASSES_ROOT;
        } else if  ( strncmp(str, "HKEY_CURRENT_CONFIG", length) == 0 ) {
            return HKEY_CURRENT_CONFIG;
        } else if  ( strncmp(str, "HKEY_CURRENT_USER", length) == 0 ) {
            return HKEY_CURRENT_USER;
        } else if  ( strncmp(str, "HKEY_LOCAL_MACHINE", length) == 0 ) {
            return HKEY_LOCAL_MACHINE;
        } else if  ( strncmp(str, "HKEY_PERFORMANCE_DATA", length) == 0 ) {
            return HKEY_PERFORMANCE_DATA;
        } else if  ( strncmp(str, "HKEY_PERFORMANCE_NLSTEXT", length) == 0 ) {
            return HKEY_PERFORMANCE_NLSTEXT;
        } else if  ( strncmp(str, "HKEY_PERFORMANCE_TEXT", length) == 0 ) {
            return HKEY_PERFORMANCE_TEXT;
        } else if  ( strncmp(str, "HKEY_CLASSES_ROOT", length) == 0 ) {
            return HKEY_CLASSES_ROOT;
        } else {
            return NULL;
        }
    } else {
        return NULL;
    }
}

} // namespace G3D

#endif // G3D_WIN32
