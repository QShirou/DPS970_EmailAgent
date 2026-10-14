#include "FilterAgent.h"
#include <sstream>
#include <cctype>

FilterAgent::FilterAgent(
    const std::vector<std::string>& allowedList,
    const std::vector<std::string>& restrictList,
    const std::vector<std::string>& badWordList
)
    : allowedList(allowedList),
    restrictedList(restrictedList),
    badWordList(badWordList) {
}

bool FilterAgent::isInList(
    const std::string& value,
    const std::vector<std::string>& list
) const {
    for (const std::string& item : list) {
        if (value == item) {
            return true;
        }
    }

    return false;
}

std::string FilterAgent::trim (const std::string& word) const {
    std::string normalized;

    for (char character : word) {
        if (std::isalnum(
            static_cast<unsigned char>(character)
        )) {
            normalized += static_cast<char>(
                std::tolower(
                    static_cast<unsigned char>(character)
                )
                );
        }
    }

    return normalized;
}

int FilterAgent::countBadWordMatches(
    const std::string& body
) const {
    std::istringstream bodyStream(body);

    std::string word;
    int matches = 0;

    while (bodyStream >> word) {
        std::string normalizedWord = trim(word);

        if (isInList(normalizedWord, badWordList)) {
            ++matches;
        }
    }

    return matches;
}

Classification FilterAgent::classifyEmail(
    const Email& email
) const {
    if (isInList(email.e_senderDomain, allowedList)) {
        return Classification::NonSpam;
    }

    if (isInList(email.e_senderDomain, restrictedList)) {
        return Classification::Spam;
    }

    int badWordMatches = countBadWordMatches(email.e_body);

    if (badWordMatches > 5) {
        return Classification::Spam;
    }

    return Classification::NonSpam;
}