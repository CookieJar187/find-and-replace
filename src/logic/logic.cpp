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

std::multimap<int, std::string> Logic::getLineOccurrencesFromFile(const std::filesystem::path& filePath, const std::string& searchTerm)
{
    std::multimap<int, std::string> lineOccurrences;
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
        if (!searchTerm.empty())
        {
            std::string::size_type position = 0;

            while ((position = line.find(searchTerm, position)) != std::string::npos)
            {
                lineOccurrences.emplace(lineNumber, line);
                position += searchTerm.length();
            }
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

    for (const auto& file : files)
    {
        FileOccurrence occurrence;
        occurrence.filePath = file;
        occurrence.lineOccurrences = getLineOccurrencesFromFile(file, searchTerm);

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
    for (const auto& occurrence : occurrences)
    {
        std::ifstream file(occurrence.filePath);
        if (!file.is_open())
        {
            std::cerr << "Failed to open file for reading: " << occurrence.filePath << std::endl;
            continue;
        }

        std::string content((std::istreambuf_iterator<char>(file)), std::istreambuf_iterator<char>());
        file.close();

        for (const auto& [lineNumber, lineContent] : occurrence.lineOccurrences)
        {
            size_t pos = 0;
            while ((pos = content.find(searchTerm, pos)) != std::string::npos)
            {
                content.replace(pos, searchTerm.length(), replaceTerm); // Replace with the desired replacement
                pos += replaceTerm.length(); // Move past the replaced term
            }
        }

        std::ofstream outFile(occurrence.filePath);
        if (!outFile.is_open())
        {
            std::cerr << "Failed to open file for writing: " << occurrence.filePath << std::endl;
            continue;
        }
        outFile << content;
    }

    tryFindOccurances(); // Refresh occurrences after replacement
}

bool Logic::isValid() const
{
    return valid;
}