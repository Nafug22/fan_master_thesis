#ifndef COMPILER_H
#define COMPILER_H

/**
 * This code is converted from https://github.com/butterfly0923/PTX2Kernel
 * by Zhang Shuai
 */

#include <fstream>
#include <cstdio>
#include <iostream>
#include <unordered_map>
#include <vector>
#include <regex>
#include <string>

/**
 * @class CUFuncProto
 * @brief Resolves all the prototypes in a ptx file (that will be used as module).
 * 
 * The prototype will be stored in a `unordered_map<string, vector<string>>`
 * in the <func_name, params> manner.
 */
class CUFuncProto{
  public:
    /**
     * @param ptx_file the correct file directory of the ptx file
     */
    CUFuncProto(){};
    CUFuncProto(std::string file_name){
        std::ifstream ptx_file;
        ptx_file.open(file_name);
        if(!ptx_file.is_open()){
            std::cerr << "Error opening the file!" << std::endl;
        }
        initialize(ptx_file);
        ptx_file.close();
    };
    ~CUFuncProto(){};

    std::vector<std::string> &get_params(const std::string func_name){
        if(!hashmap.count(func_name)) printf("not finding the func_name %s\n", func_name.c_str());
        return hashmap[func_name];
    }

    void add_module(std::ifstream &ptx_file) { initialize(ptx_file); }

    /**
     * @brief tests the result
     */
    void test(){
        if(hashmap.empty()) printf("No CUfunction found!\n");
        for(auto &[key, val] : hashmap){
            printf("%s: ", key.c_str());
            for(auto &param : val) printf("%s ", param.c_str());

            printf("\n");
        }
    }

  private:
    std::unordered_map<std::string, std::vector<std::string>> hashmap;  /* stores the function parameters types of each function. */

    /**
     * @brief convert each entry for the hashmap.
     */
    void convert(std::ifstream &ptx_file, const std::string& entry){
        std::regex rgx("\\.entry (\\w+)\\(");
        std::smatch match;
        
        std::regex_search(entry, match, rgx);
        std::string kernel_name = match[1];

        std::string line;
        while(line != ")"){
          std::getline(ptx_file, line);
          std::regex rgx(R"(\.param\s+(\.\w+))");
          if (std::regex_search(line, match, rgx)) {
              // The first capture group contains the type
              std::string type = match[1];
              hashmap[kernel_name].push_back(type);
          }
        }
    }

    /**
     * @brief initializes the hashmap that stores the func prototype info.
     */
    void initialize(std::ifstream &ptx_file){
        std::string line;
        while(std::getline(ptx_file, line)) {
            if(line.find(".entry") != std::string::npos) convert(ptx_file, line);
        }
    }
};

#endif