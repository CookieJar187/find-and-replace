#include <string>
#include <filesystem>
#include <vector>
#include <map>

struct FileOccurrence {
    std::filesystem::path filePath;
    std::map<int, std::string> lineOccurrences;
};

class Logic
{
private:
	bool valid = false;

	bool matchCase;
	bool wholeWord;

	std::filesystem::path projectRootDirectory;
	std::string searchTerm;

	std::vector<FileOccurrence> occurrences;

	std::vector<std::filesystem::path> getAllFiles();
	void filterOutBySearchTerm(
		std::vector<std::filesystem::path>& files,
		const std::string& searchTerm
	);
	std::map<int, std::string> getLineOccurrencesFromFile(
		const std::filesystem::path& filePath,
		const std::string& searchTerm
	);
	void findOccurances();

public:
	void setMatchCase(bool value) { matchCase = value; }
	void setWholeWord(bool value) { wholeWord = value; }
	
	void trySetProjectRootDirectory(
		std::filesystem::path rootDirectory
	);
	void trySetSearchTerm(
		const std::string& searchTerm
	);

	std::vector<FileOccurrence>& getOccurrences();

	void tryFindOccurances();
	void replaceOccurances();
	bool isValid() const;
};
