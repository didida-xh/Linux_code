#pragma once

#include <iostream>
#include <string>
#include <fstream>
#include <unordered_map>
#include "Logger.hpp"

static const std::string default_dict = "./Dict.txt";
static const std::string sep = ": ";
using namespace NS_LOG_MODULE;

class Dict
{
public:
    Dict(const std::string &dict_path = default_dict) : _dict_path(dict_path)
    {
        LoadDict();
    }
    ~Dict()
    {
    }
    void LoadDict()
    {
        std::ifstream in(_dict_path);
        if (!in.is_open())
        {
            LOG(LogLevel::FATAL) << " open " << _dict_path << " error";
            exit(1);
        }
        std::string line;
        while (std::getline(in, line))
        {
            LOG(LogLevel::FATAL) << "load: " << line << " success";
            auto pos = line.find(sep);
            if (pos == std::string::npos)
            {
                LOG(LogLevel::WARNING) << "Format: " << line << " error";
                continue;
            }

            std::string k = line.substr(0, pos);
            std::string v = line.substr(pos + sep.size());
            _dict.insert(std::make_pair(k, v));
        }
        in.close();
        LOG(LogLevel::INFO) << "load done....";
    }
    std::string Translate(std::string word)
    {
        auto iter = _dict.find(word);
        if (iter != _dict.end())
        {
            return iter->second;
        }
        else
        {
            return "None";
        }
    }

private:
    std::string _dict_path;
    std::unordered_map<std::string, std::string> _dict;
};