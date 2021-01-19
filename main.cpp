#include "log_messages_storage.h"
#include <algorithm>
#include <iostream>
#include <fstream>

using namespace std::literals;
using namespace log_messages;

namespace {

constexpr auto SerializeModeOption = "-s"sv;
constexpr auto DeserializeModeOption = "-d"sv;
constexpr auto InputFileOption = "--i="sv;
constexpr auto OutputFileOption = "--o="sv;
constexpr auto TemplateFileOption = "--t="sv;

enum class WorkingMode
{
    Serialization,
    Deserialization,
    Error
};

struct CommandArguments
{
    WorkingMode mode;
    std::string inFile;
    std::string outFile;
    std::string templateFile;
};

std::vector<std::string> ReadLinesFromFile(const std::string& filePath)
{
    std::vector<std::string> lines;

    std::ifstream file{ filePath };
    if (!file.is_open())
    {
        std::cout << "Error during opening input file: " << filePath << std::endl;
        return lines;
    }

    std::string line;

    while (std::getline(file, line)) 
    {
        if (line.empty())
        {
            continue;
        }

        lines.push_back(std::move(line));
    }

    file.close();
    return lines;
}

void WriteLinesToFile(const std::vector<std::string>& messages, const std::string& filePath)
{
    std::ofstream file{ filePath };
    if (!file.is_open())
    {
        std::cout << "Error during opening input file: " << filePath << std::endl;
        return;
    }

    for (auto&& message : messages)
    {
        file << message << std::endl;
    }

    file.close();    
}

void Serialize(
    const std::string& inFile, 
    const std::string& outFile, 
    const std::string& templateFile = "")
{
    LogMessagesStorage storage{ ReadLinesFromFile(inFile), templateFile };
    storage.SaveToFile(outFile, templateFile);
}

void Deserialize(
    const std::string& inFile, 
    const std::string& outFile,
    const std::string& templateFile = "")
{
    LogMessagesStorage storage;
    storage.LoadFromFile(inFile, templateFile);
    const auto data{ storage.GetData() };
    const auto messages{ GetLogMessagesFromStorageData(data) };
    WriteLinesToFile(messages, outFile);
}

CommandArguments ParseArguments(int argc, char* argv[])
{
    CommandArguments arguments;
    arguments.mode = WorkingMode::Error;

    if (argc < 4 || argc > 5)
    {
        return arguments;
    }

    for (int i = 1; i < argc; ++i)
    {
        std::string_view arg{ argv[i] };
        
        if (arg.rfind(SerializeModeOption, 0) == 0)
        {
            if (arguments.mode == WorkingMode::Deserialization)
            {
                arguments.mode = WorkingMode::Error;
                return arguments;
            }
            arguments.mode = WorkingMode::Serialization;
        }

        if (arg.rfind(DeserializeModeOption, 0) == 0)
        {
            if (arguments.mode == WorkingMode::Serialization)
            {
                arguments.mode = WorkingMode::Error;
                return arguments;
            }
            arguments.mode = WorkingMode::Deserialization;
        }

        if (arg.rfind(InputFileOption, 0) == 0)
        {
            arg.remove_prefix(InputFileOption.size());
            arguments.inFile = arg;
        }

        if (arg.rfind(OutputFileOption, 0) == 0)
        {
            arg.remove_prefix(OutputFileOption.size());
            arguments.outFile = arg;
        }
        
        if (arg.rfind(TemplateFileOption, 0) == 0)
        {
            arg.remove_prefix(TemplateFileOption.size());
            arguments.templateFile = arg;
        }
    }

    if (arguments.inFile.empty() || arguments.outFile.empty())
    {
        std::cout << "One of the given file is empty" << std::endl;
        arguments.mode = WorkingMode::Error;
    }

    return arguments;
}

// test without any assertion, but useful to check visually functionality of Storage Class
void SimpleTest()
{
    std::string pathToParamsFile{ "params.txt" };
    std::string pathToTemplatesFile{ "templates.txt" };

    LogMessagesStorage storage;
    storage.LoadFromFile(pathToParamsFile, pathToTemplatesFile);

    const auto data = storage.GetData();
    auto messages = GetLogMessagesFromStorageData(data);

    for (auto&& message : messages)
    {
        std::cout << message << std::endl;
    }
    std::cout << std::endl;

    storage.SaveToFile("params1.txt", "templates1.txt");

    LogMessagesStorage storage1;
    storage1.LoadFromFile("params1.txt", "templates1.txt");

    const auto data1 = storage1.GetData();
    messages = GetLogMessagesFromStorageData(data1);

    for (auto&& message : messages)
    {
        std::cout << message << std::endl;
    }
    std::cout << std::endl;

    const LogMessagesStorage storage2{ messages, "templates1.txt" };

    const auto data2 = storage2.GetData();
    messages = GetLogMessagesFromStorageData(data1);
    
    for (auto&& message : messages)
    {
        std::cout << message << std::endl;
    }
}

} // namespace

int main(int argc, char* argv[])
{
    //SimpleTest();

    const auto commandArguments = ParseArguments(argc, argv);

    if (commandArguments.mode == WorkingMode::Serialization)
    {
        Serialize(commandArguments.inFile, commandArguments.outFile, commandArguments.templateFile);
        std::cout << "Successfully serialized in " << commandArguments.outFile << std::endl;
    }
    else if (commandArguments.mode == WorkingMode::Deserialization)
    {
        Deserialize(commandArguments.inFile, commandArguments.outFile, commandArguments.templateFile);
        std::cout << "Successfully deserialized in " << commandArguments.outFile << std::endl;
    }
    else
    {
        std::cout << "Parameters of program are incorrect, please see readme.md again" << std::endl;
    }

    return 0;
}
