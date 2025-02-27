#include <cstdio>
#include <fstream>
#include <unordered_set>
#include <nlohmann/json.hpp>
#include <algorithm>
using json = nlohmann::json;

struct NidFuncTable {
    std::string m_id;
    std::string m_funcName;
    int m_libversion;
    int m_version_major;
    int m_version_minor;
};

void GenerateCodeFiles(
    const std::unordered_map<std::string, std::vector<NidFuncTable>>& libName2FuncTableMap,
    const std::string& moduleName) {
    // Generate Header
    std::string lowModName = moduleName;
    std::transform(lowModName.begin(), lowModName.end(), lowModName.begin(), ::tolower);
    std::string headerName(lowModName + ".h");
    std::string headerCode("#pragma once\n\n#include \"library_common.h\" \n\n");
    std::string trimmedName = moduleName;
    if (moduleName.find("libSce") != std::string::npos)
    {
        trimmedName = moduleName.substr(6, moduleName.size()-1);
    }
    else if (moduleName.find("lib") != std::string::npos) {
        trimmedName = moduleName.substr(3, moduleName.size() - 1);
        trimmedName[0] = toupper(trimmedName[0]);
    }
    headerCode += "namespace Libraries::" + trimmedName + "{\n\n";
    std::unordered_set<std::string> funcDeclares;
    for (const auto& lib : libName2FuncTableMap) {
        for (const auto& func : lib.second) {
            if (funcDeclares.find(func.m_funcName) == funcDeclares.end()) {
                const std::string funcDeclare("int PS4_SYSV_ABI " + func.m_funcName + "();\n");
                headerCode += funcDeclare;
                funcDeclares.insert(func.m_funcName);
            }
        }
    }

    headerCode += "\nvoid Register" + moduleName + "(Loader::SymbolsResolver * sym);\n";

    headerCode += "}";
    std::ofstream headerFile(MODULE_DIR + headerName);
    headerFile << headerCode;
    headerFile.close();

    std::string sourceName(lowModName + ".cpp");
    std::string sourceCode("//Generated By moduleGenerator\n#include \"" + headerName +
                           "\"\n#include \"common/log.h\"\n#include \"error_codes.h\"\n\n");

    sourceCode += "namespace Libraries::" + trimmedName + "{\n\n";

    // function implementation
    std::unordered_set<std::string> funcImplementation;
    for (const auto& lib : libName2FuncTableMap) {
        for (const auto& func : lib.second) {
            if (funcImplementation.find(func.m_funcName) == funcImplementation.end()) {
                const std::string funcDeclare(
                    "int PS4_SYSV_ABI " + func.m_funcName + "()\n" + "{\n" +
                    "	PRINT_UNIMPLEMENTED_FUNCTION_NAME();\n"
                    "	return ORBIS_OK;\n" +
                    "}\n\n");
                sourceCode += funcDeclare;
                funcImplementation.insert(func.m_funcName);
            }
        }
    }
    sourceCode += "void Register" + moduleName + "(Loader::SymbolsResolver * sym) {\n";
    for (const auto& lib : libName2FuncTableMap) {
        for (const auto& func : lib.second) {
            sourceCode += " LIB_FUNCTION(\"" + func.m_id + ", " + lib.first + " , " + std::to_string(func.m_libversion) + " , " + moduleName +
                ", " + std::to_string(func.m_version_major) + ", " +
                    std::to_string(func.m_version_minor) + " , " + func.m_funcName + " ),\n";
        }
    }

    sourceCode += "};\n\n";
    sourceCode += "}";
    std::ofstream sourceFile(MODULE_DIR + sourceName);
    sourceFile << sourceCode;
    sourceFile.close();
}

void GetSymbolsFromLibDoc(std::vector<std::string>& importModules) {
    for (uint32_t index = 0; index < importModules.size(); index++) {
        const std::string& moduleName = importModules[index] + ".sprx.json";

        std::ifstream file(LIBDOC_DIR + moduleName);
        if (std::filesystem::exists(LIBDOC_DIR + moduleName)) {
            printf("module found %s\n", moduleName.c_str());
            json m_json_data = json::parse(file);
            bool bFound = false;
            // parse "modules": [
            for (auto& modules : m_json_data["modules"]) {
                std::string subModuleName =
                    modules.at("name").get<std::string>(); // get the name of module
                if (subModuleName == importModules[index]) {
                    int m_version_major = modules.at("version_major").get<int>();
                    int m_version_minor = modules.at("version_minor").get<int>();
                    std::unordered_map<std::string, std::vector<NidFuncTable>> libName2FuncTableMap;
                    for (auto& libraries : modules["libraries"]) {
                        std::string libName = libraries["name"].get<std::string>();
                        if (libName2FuncTableMap.find(libName) == libName2FuncTableMap.end()) {
                            libName2FuncTableMap.emplace(
                                std::make_pair(libName, std::vector<NidFuncTable>()));
                        }
                        int libVersion = libraries["version"].get<int>();
                        for (auto& symbols : libraries["symbols"]) {
                            std::string encoded_id = symbols["encoded_id"].get<std::string>();
                            std::string symName;
                            if (symbols["name"] != nullptr) {
                                symName = symbols["name"].get<std::string>();
                            } else {
                                symName = "Func_" + encoded_id;
                            }

                            libName2FuncTableMap[libName].push_back(NidFuncTable{
                                encoded_id, symName, libVersion, m_version_major, m_version_minor});
                        }
                    }

                    GenerateCodeFiles(libName2FuncTableMap, subModuleName);
                    bFound = true;
                }
            }
            if (!bFound) {
                printf("module can't be found %s\n", moduleName.c_str());
            }
        } else {
            printf("module can't be found %s\n", moduleName.c_str());
        }
    }
}
int main(int argc, char* argv[]) {

    std::filesystem::path genFolder(MODULE_DIR);
    if (!std::filesystem::exists(genFolder)) {
        std::filesystem::create_directory(genFolder);
    }

    std::vector<std::string> modules_to_generate;
    modules_to_generate.push_back(std::string("libSceGnmDriver"));
    modules_to_generate.push_back(std::string("libScePad"));
    modules_to_generate.push_back(std::string("libSceVideoOut"));
    modules_to_generate.push_back(std::string("libkernel"));
    modules_to_generate.push_back(std::string("libSceSystemService"));
    modules_to_generate.push_back(std::string("libSceUserService"));
    GetSymbolsFromLibDoc(modules_to_generate);

    return 0;
}
