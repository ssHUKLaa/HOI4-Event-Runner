#include <stdio.h>
#include <iostream>
#include <fstream>
#include <filesystem>
#include <vector>
#include <map>



const std::string modfolder = R"(C:\Users\sahil\Documents\Paradox Interactive\Hearts of Iron IV\mod\git-days-of-europe)";
const std::map<std::string, std::string> country = {
    {"Mexico","MEX"}
};
void findFilesWithSubstring(const std::string& directoryPath, const std::map<std::string, std::string>& substrings, std::vector<std::string>& fileList) {
    std::string key = (substrings.begin())->first;
    std::string value = (substrings.begin())->second;
    for (const auto& entry : std::filesystem::recursive_directory_iterator(directoryPath)) {
        if (entry.is_regular_file()) {
            std::string filename = entry.path().filename().string();
            if ((filename.find(key) != std::string::npos) || (filename.find(value) != std::string::npos)) {
                fileList.push_back(entry.path().string());
            }
        }
    }
}

int main() {
    // first we get the on_action events
    std::string onActionPath = modfolder + R"(\common\on_actions)";

    std::cout << onActionPath << std::endl;
    std::vector<std::string> onActionsFiles;

    findFilesWithSubstring(onActionPath, country, onActionsFiles);

    for (const auto& filePath : onActionsFiles) {
        std::ifstream file(filePath);
        std::string line;
        std::getline(file, line);
        std::cout << line << std::endl;
    }
	
	return 0;
}