/* ==================================== JUCER_BINARY_RESOURCE ====================================

   This is an auto-generated file: Any edits you make may be overwritten!

*/

#include <cstring>

namespace BinaryData
{

//================== CMakeLists.txt ==================
static const unsigned char temp_binary_data_0[] =
"cmake_minimum_required(VERSION 3.2)\n"
"\n"
"project(cbmp)\n"
"\n"
"set(CMAKE_EXPORT_COMPILE_COMMANDS ON)\n"
"\n"
"set(CMAKE_BUILD_TYPE DEBUG)\n"
"\n"
"set(CMAKE_C_FLAGS_DEBUG \"${CMAKE_C_FLAGS_DEBUG} -Wall -Wextra\")\n"
"set(CMAKE_CXX_FLAGS_DEBUG \"${CMAKE_CXX_FLAGS_DEBUG} -Wall -Wextra\")\n"
"\n"
"add_subdirectory(lib/googletest)\n"
"\n"
"include_directories(\n"
"  ${GTEST_ROOT}/include\n"
")\n"
"\n"
"add_executable(\n"
"  tests\n"
"  tests.cc\n"
"  cbmp.c\n"
")\n"
"\n"
"target_link_libraries(\n"
"  tests\n"
"  gtest_main\n"
")\n"
"\n"
"enable_testing()\n"
"add_test(CBMPTests tests)\n";

const char* CMakeLists_txt = (const char*) temp_binary_data_0;


const char* getNamedResource (const char* resourceNameUTF8, int& numBytes);
const char* getNamedResource (const char* resourceNameUTF8, int& numBytes)
{
    unsigned int hash = 0;

    if (resourceNameUTF8 != nullptr)
        while (*resourceNameUTF8 != 0)
            hash = 31 * hash + (unsigned int) *resourceNameUTF8++;

    switch (hash)
    {
        case 0x90e15cf5:  numBytes = 472; return CMakeLists_txt;
        default: break;
    }

    numBytes = 0;
    return nullptr;
}

const char* namedResourceList[] =
{
    "CMakeLists_txt"
};

const char* originalFilenames[] =
{
    "CMakeLists.txt"
};

const char* getNamedResourceOriginalFilename (const char* resourceNameUTF8);
const char* getNamedResourceOriginalFilename (const char* resourceNameUTF8)
{
    for (unsigned int i = 0; i < (sizeof (namedResourceList) / sizeof (namedResourceList[0])); ++i)
        if (strcmp (namedResourceList[i], resourceNameUTF8) == 0)
            return originalFilenames[i];

    return nullptr;
}

}
