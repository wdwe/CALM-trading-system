#include "yaml-cpp/yaml.h"
#include <limits>
#include <iostream>
#include <filesystem>
#include "fmt/format.h"



int main() {
//    std::vector<HistBarReq> res;

    auto cfg = YAML::LoadFile("/home/calm/hist_download.yaml");
    auto root_path = cfg["root_folder"].as<std::string>();
    auto start = cfg["start"].as<std::string>();
    auto end = cfg["end"].as<std::string>();
    auto timezone = cfg["timezone"].as<std::string>();
    auto type = cfg["type"].as<std::string>();
    for (auto it = cfg["downloads"].begin(); it != cfg["downloads"].end(); ++it) {
        std::string save_path = fmt::format("{}/{}-{}.csv", root_path, it->first.as<std::string>(), type);
        auto symbol = it->second.as<std::string>();
        fmt::print("symbol:{}, start:{}, end:{}, timezone:{}, type:{}, save_path:{}\n", symbol, start, end, timezone, type, save_path);
    }

    return 0;
}