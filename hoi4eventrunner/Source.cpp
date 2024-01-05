#include <stdio.h>
#include <iostream>
#include <fstream>
#include <filesystem>
#include <vector>
#include <map>
#include <queue>
#include <stack>
#include <algorithm>
#include <set>
#include <regex>

const std::string modfolder = R"(C:\Users\sahil\Documents\Paradox Interactive\Hearts of Iron IV\mod\git-days-of-europe)";
const std::map<std::string, std::string> country = {
    {"Britain","BRI"}
};

class Event {
private:
    std::string name;
    int date;

public:

    Event() : name(""), date(0) {}
    
    Event(const std::string& eventName, const int& eventDate) : name(eventName), date(eventDate) {}

    void setName(std::string name) {
        this->name = name;
    }
    void setDate(int date) {
        this->date = date;
    }

    std::string getName() const {
        return name;
    }

    int getDate() const {
        return date;
    }
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
struct CompareEvents {
    bool operator()(const Event& e1, const Event& e2) {
        return e1.getDate() < e2.getDate();
    }
};

struct EventInfo {
    std::string id;
};

std::vector<EventInfo> extractInfo(const std::string& filename) {
    std::ifstream file(filename);
    std::vector<EventInfo> extractedInfo;
    std::string line;
    std::string currentBlock;
    std::stack<char> braceStack;
    EventInfo currentEvent;

    if (file.is_open()) {
        while (std::getline(file, line)) {
            for (char c : line) {
                if (c == '{') {
                    braceStack.push(c);
                }
                else if (c == '}') {
                    if (!braceStack.empty()) {
                        braceStack.pop();
                    }
                }
            }

            currentBlock += line + '\n';

            if (braceStack.empty() && !currentBlock.empty()) {
                size_t idPos = currentBlock.find("id");
                size_t optionPos = currentBlock.find("option");
                if (idPos != std::string::npos) {
                    size_t equalsPos = currentBlock.find("=", idPos);
                    if (equalsPos != std::string::npos) {
                        currentEvent.id = currentBlock.substr(equalsPos + 1);
                    }
                }
                if (optionPos != std::string::npos) {
                    size_t equalsPos = currentBlock.find("=", optionPos);
                }
                extractedInfo.push_back(currentEvent);
                currentEvent = {}; // Clear currentEvent
                currentBlock.clear();
            }
        }

        file.close();
    }
    else {
        std::cerr << "Unable to open file." << std::endl;
    }

    return extractedInfo;
}
std::string ltrim(const std::string& s) {
    auto it = std::find_if(s.begin(), s.end(), [](char c) {
        return !std::isspace(static_cast<unsigned char>(c));
        });
    return std::string(it, s.end());
}

// Function to trim whitespace from the end of a string
std::string rtrim(const std::string& s) {
    auto it = std::find_if(s.rbegin(), s.rend(), [](char c) {
        return !std::isspace(static_cast<unsigned char>(c));
        });
    return std::string(s.begin(), it.base());
}

std::vector<std::string> extractOptionBlocks(const std::string& data) {
    std::vector<std::string> optionBlocks;
    size_t startPos = 0;

    while (true) {
        startPos = data.find("option", startPos);
        if (startPos == std::string::npos) {
            break; // No more 'option' blocks found
        }

        size_t optionEndPos = data.find('{', startPos);
        if (optionEndPos == std::string::npos) {
            break; // If opening brace not found, exit loop
        }

        // Ensure 'option' is at the start of the line or preceded by a space/tab
        if (startPos != 0 && !isspace(data[startPos - 1])) {
            startPos = optionEndPos + 1;
            continue; // Move to the next occurrence if 'option' is not correctly placed
        }

        size_t closeBracePos = optionEndPos;
        int braceCount = 1;

        for (size_t i = optionEndPos + 1; i < data.size(); ++i) {
            if (data[i] == '{') {
                braceCount++;
            }
            else if (data[i] == '}') {
                braceCount--;
                if (braceCount == 0) {
                    closeBracePos = i;
                    break;
                }
            }
        }

        if (braceCount == 0) {
            optionBlocks.push_back(data.substr(optionEndPos, closeBracePos - optionEndPos + 1));
        }

        startPos = closeBracePos + 1; // Move to the next position after the extracted block
    }

    return optionBlocks;
}

std::vector<Event> extractEvents(const std::string& filePath) {
    std::ifstream file(filePath);
    std::vector<Event> events;

    if (file.is_open()) {
        std::string line;
        std::string block;
        while (std::getline(file, line)) {
            block += line;

            std::smatch match;
            if (std::regex_search(block, match, std::regex(R"(id\s*=\s*([^\s}]+)\s*days\s*=\s*(\d+))"))) {
                events.emplace_back(match[1], std::stoi(match[2]));
                block.clear(); // Reset block for the next event
            }
        }

        file.close();
    }

    return events;
}

int main() {
    // first we get the on_action events
    std::string onActionPath = modfolder + R"(\common\on_actions)";
    std::string eventPath = modfolder + R"(\events)";

    std::vector<std::string> onActionsFiles;
    std::vector<std::string> eventFiles;

    findFilesWithSubstring(onActionPath, country, onActionsFiles);
    std::queue<Event> eventsToCheck;

    for (const auto& filePath : onActionsFiles) {
        std::vector<Event> tempvec = extractEvents(filePath);

        for (Event swagev : tempvec) {
            eventsToCheck.push(swagev);
        }

        
    }
    findFilesWithSubstring(eventPath, country, eventFiles);
    std::list<std::string> allEvList;
    for (const auto& filepath : eventFiles) {

        std::vector<EventInfo> test = extractInfo(filepath);

        for (const EventInfo& info : test) {
            if ((!info.id.empty())) {
                allEvList.push_back(info.id);
            }
        }

    }
    std::priority_queue<Event, std::vector<Event>, CompareEvents> allEvs;
    
    std::ofstream outputFile("output.txt");
    while (!eventsToCheck.empty()){
        Event ev = eventsToCheck.front();
        allEvs.push(ev);
        eventsToCheck.pop();

        for (std::string info : allEvList) {
            size_t newlinePos = info.find('\n');
            std::string swah = rtrim(ltrim(info.substr(0, newlinePos)));
            if (ev.getName() == swah) {
                std::vector<std::string> optionblks = extractOptionBlocks(info);
                for (std::string optionblk : optionblks) {
                    size_t swag = optionblk.find("country_event");
                    if (swag != std::string::npos) {
                        std::string country_ev = optionblk.substr(swag);
                        std::regex countryevpattern(R"id(\b(id|days)\s*=\s*([^}\s]+))id");
                        std::smatch match;

                        auto textBegin = country_ev.cbegin();
                        auto textEnd = country_ev.cend();
                        std::string tempName;
                        int tempDate = ev.getDate();
                        Event nextEvent;
                        while (std::regex_search(textBegin, textEnd, match, countryevpattern)) {
                            if (match[1] == "id") {
                                
                                nextEvent.setName(match[2]);
                                

                            }
                            else if (match[1] == "days") {
                                tempDate += std::stoi(match[2]);
                                nextEvent.setDate(tempDate);
                                eventsToCheck.push(nextEvent);
                            }
                            textBegin = match.suffix().first; // Move to the next match
                        }
                        
                        
                        
                        std::cout << allEvs.size() << std::endl;
                        
                        //std::cout << testtt << std::endl; 
                    }
                    
                }
                
            }
        }
        
    }
    
    while(!allEvs.empty()){
        Event testev = allEvs.top();
        outputFile << "ID: " << testev.getName() << std::endl;
        outputFile << "Days: " << testev.getDate() << std::endl;
        outputFile << "----------------" << std::endl;
        allEvs.pop();
    }

	
	return 0;
}