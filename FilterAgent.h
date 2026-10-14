#include "email.h"
#include <string>
#include <vector>

enum class Classification {
	Spam,
	NonSpam
};

class FilterAgent {
public:
	FilterAgent(const std::vector<std::string>& allowedList,
		const std::vector<std::string>& restrictedList,
		const std::vector<std::string>& badWordList
	);

	Classification classifyEmail(const Email& email) const;

private:
	std::vector<std::string> allowedList;
	std::vector<std::string> restrictedList;
	std::vector<std::string> badWordList;

	std::string trim(const std::string& word) const;

	bool isInList(const std::string& value, const std::vector<std::string>& list) const;

	int countBadWordMatches(const std::string& body) const;
};
