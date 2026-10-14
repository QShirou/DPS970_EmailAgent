#include "email.h"
#include "FilterAgent.h"

#include <cstdlib>
#include <fstream>
#include <iostream>
#include <string>
#include <vector>
#include <filesystem>
#include <sstream>


std::vector<std::string> readListFile(
    const std::string& filename
) {
    std::ifstream inputFile(filename);

    if (!inputFile.is_open()) {
        std::cerr << "Error: Could not open " << filename << '\n';
        std::exit(EXIT_FAILURE);
    }

    std::vector<std::string> items;
    std::string line;

    while (std::getline(inputFile, line)) {
        if (!line.empty()) {
            items.push_back(line);
        }
    }

    return items;
}
Email readEmailFile(
    const std::filesystem::path& filePath
) {
    std::ifstream inputFile(filePath);

    if (!inputFile.is_open()) {
        std::cerr << "Error: Could not open email file "
            << filePath << '\n';
        std::exit(EXIT_FAILURE);
    }

    std::string line;
    std::string fromHeader;
    std::string body;
    bool readingBody = false;

    while (std::getline(inputFile, line)) {
        if (!readingBody) {
            if (line.empty()) {
                readingBody = true;
                continue;
            }

            if (line.rfind("From:", 0) == 0) {
                fromHeader = line;
            }
        }
        else {
            body += line + '\n';
        }
    }

    std::size_t atPosition = fromHeader.find('@');
    std::size_t domainEnd = fromHeader.find('>', atPosition);

    if (fromHeader.empty() ||
        atPosition == std::string::npos) {
        std::cerr << "Error: Invalid or missing From header in "
            << filePath << '\n';
        std::exit(EXIT_FAILURE);
    }

    if (domainEnd == std::string::npos) {
        domainEnd = fromHeader.size();
    }

    std::string senderDomain = fromHeader.substr(
        atPosition + 1,
        domainEnd - atPosition - 1
    );

    return Email{
        filePath.filename().string(),
        senderDomain,
        body
    };
}

int main() {
    const std::vector<std::string> allowList =
        readListFile("data/allow_list.txt");

    const std::vector<std::string> restrictList =
        readListFile("data/restrict_list.txt");

    const std::vector<std::string> badWords =
        readListFile("data/bad_words.txt");

    FilterAgent agent(allowList, restrictList, badWords);

    const std::filesystem::path inboxPath = "data/inbox";
    const std::filesystem::path spamPath = "spam";
    const std::filesystem::path emailPath = "email";

    for (const auto& entry :
        std::filesystem::directory_iterator(inboxPath)) {
        if (!entry.is_regular_file() ||
            entry.path().extension() != ".eml") {
            continue;
        }

        Email email = readEmailFile(entry.path());

        Classification result = agent.classifyEmail(email);

        std::filesystem::path destination =
            result == Classification::Spam
            ? spamPath / entry.path().filename()
            : emailPath / entry.path().filename();

        std::filesystem::rename(
            entry.path(),
            destination
        );

        std::cout << email.filename << " -> "
            << (result == Classification::Spam
                ? "spam"
                : "email")
            << '\n';
    }

    return 0;
}