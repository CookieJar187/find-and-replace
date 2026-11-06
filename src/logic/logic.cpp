#include <iostream>
#include <fstream>
#include <logic.hpp>
#include <system_error>

std::vector<std::filesystem::path> Logic::getAllFiles()
{
    std::vector<std::filesystem::path> foundFiles;

    for (const auto& entry : std::filesystem::recursive_directory_iterator(projectRootDirectory))
    {
        if (entry.is_regular_file())
        {
            foundFiles.push_back(entry.path());
        }
    }
    return foundFiles;
}

void Logic::filterOutBySearchTerm(
    std::vector<std::filesystem::path>& files,
    const std::string& searchTerm
)
{
    for (int i = files.size() - 1; i >= 0; --i)
    {
        const auto& filePath = files[i];
        std::ifstream file(filePath);
        if (!file.is_open())
        {
            std::cerr << "Failed to open file: " << filePath << std::endl;
            continue;
        }

        std::string line;
        bool found = false;
        while (std::getline(file, line))
        {
            if ((!matchCase && line.find(searchTerm) != std::string::npos) ||
                (matchCase && line.find(searchTerm) != std::string::npos)) // Adjust this condition for case sensitivity
            {
                found = true;
                break;
            }
        }

        if (!found)
        {
            files.erase(files.begin() + i);
        }
    }
}

std::map<int, std::string> Logic::getLineOccurrencesFromFile(const std::filesystem::path& filePath, const std::string& searchTerm)
{
    std::map<int, std::string> lineOccurrences;
    std::ifstream file(filePath);
    if (!file.is_open())
    {
        std::cerr << "Failed to open file: " << filePath << std::endl;
        return lineOccurrences;
    }

    std::string line;
    int lineNumber = 0;
    while (std::getline(file, line))
    {
        ++lineNumber;
        if (line.find(searchTerm) != std::string::npos)
        {
            lineOccurrences[lineNumber] = line;
        }
    }

    return lineOccurrences;
}

void Logic::trySetProjectRootDirectory(std::filesystem::path rootDirectory)
{
    std::error_code ec;

    if (
        std::filesystem::exists(rootDirectory, ec) && !ec &&
        std::filesystem::is_directory(rootDirectory)
    )
    {
        projectRootDirectory = rootDirectory;
        valid = true;
    }
    else
    {
        valid = false;
    }
}

void Logic::trySetSearchTerm(const std::string& searchTerm)
{
    if (!searchTerm.empty())
    {
        this->searchTerm = searchTerm;
    }
    else
    {
        this->searchTerm.clear();
    }
}

void Logic::findOccurances()
{
    occurrences.clear();

    std::vector<std::filesystem::path> files = getAllFiles();
    filterOutBySearchTerm(files, searchTerm);

    //std::cout << "Found " << files.size() << " occurrences of '" << searchTerm << "' in the project directory." << std::endl;

    for (const auto& file : files)
    {
        FileOccurrence occurrence;
        occurrence.filePath = file;
        occurrence.lineOccurrences = getLineOccurrencesFromFile(file, searchTerm);

        //std::cout << file.filename() << std::endl;
        //std::cout << "Line occurrences:" << std::endl;
        //for (const auto& [lineNumber, lineContent] : occurrence.lineOccurrences)
        //{
            //std::cout << "Line " << lineNumber << ": " << lineContent << std::endl;
        //}
        //std::cout << std::endl;
        occurrences.push_back(occurrence);
    }

}

void Logic::tryFindOccurances()
{
	if (isValid() && searchTerm.length() > 0)
	{
		findOccurances();
	}
    else
    {
        occurrences.clear();
    }
}

std::vector<FileOccurrence>& Logic::getOccurrences()
{
    return occurrences;
}

void Logic::replaceOccurances()
{
    // Implementation for replacing occurrences
}

bool Logic::isValid() const
{
    return valid;
}