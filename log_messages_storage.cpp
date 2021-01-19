#include "log_messages_storage.h"
#include <fstream>
#include <iostream>
#include <sstream>
#include <string_view>

using namespace std::literals;

namespace log_messages { 
namespace {

constexpr auto Separator = "#"sv;
constexpr auto WrongSize = static_cast<size_t>(-1);

size_t StringToSizeT(const std::string& str)
{
    size_t result{ 0 };

    try
    {
        std::stringstream stream(str);
        stream >> result;
    }
    catch (...)
    {
        result = WrongSize;
        std::cout << "Error during converting number: " << str << std::endl;
    }
    return result;
}

std::vector<std::string> ParseSerializedLine(std::string_view line)
{
    std::vector<std::string> parsedLine;

    auto separatorPosition = line.find(Separator);
    if (separatorPosition == std::string_view::npos)
    {
        std::cout << "Line is missing separator, invalid line serialization: " << line << std::endl;
        return parsedLine;
    }

    const auto paramsCount = StringToSizeT({ line.begin(), line.begin() + separatorPosition });
    if (paramsCount == WrongSize)
    {
        std::cout << "Zero params with separators in line: " << line << std::endl;
        return parsedLine;
    }

    parsedLine.reserve(paramsCount);
    line.remove_prefix(separatorPosition + Separator.size());

    while (!line.empty())
    {
        separatorPosition = line.find(Separator);
        if (separatorPosition == std::string_view::npos)
        {
            std::cout << "Line is missing separator, invalid line serialization: " << line << std::endl;
            parsedLine.clear();
            break;
        }
        parsedLine.emplace_back(line.begin(), line.begin() + separatorPosition);
        line.remove_prefix(separatorPosition + Separator.size());
    }

    if (parsedLine.size() != paramsCount)
    {
        std::cout << "Wrong params count in line: " << line << ", should be: " << paramsCount << std::endl;
        parsedLine.clear();
    }

    return parsedLine;
}

} // namespace

std::vector<std::string> GetLogMessagesFromStorageData(const LogMessagesStorageData& data)
{
    std::vector<std::string> messages;
    messages.reserve(data.params.size());

    for (auto&& param : data.params)
    {
        std::string message;

        const auto& matchingTemplate = data.templates[param.templateNumber];

        if (param.templateParamNames.size() + 1 != matchingTemplate.size())
        {
            continue;
        }

        for (size_t i = 0; i < param.templateParamNames.size(); ++i)
        {
            message.append(matchingTemplate[i]);
            message.append(param.templateParamNames[i]);
        }
        message.append(matchingTemplate.back());

        messages.push_back(std::move(message));

    }

    return messages;
}

LogMessagesStorage::LogMessagesStorage(
    std::vector<LogMessageParams> params, 
    std::vector<std::vector<std::string>> templates) noexcept
        : params_(std::move(params))
        , templates_(std::move(templates))
{    
}

OperationStatus LogMessagesStorage::LoadFromFile(const std::string& pathToParamsFile, const std::string& pathToTemplatesFile)
{
    return LoadTemplatesFromFile(pathToTemplatesFile) == OperationStatus::Ok &&
        LoadParamsFromFile(pathToParamsFile) == OperationStatus::Ok
            ? OperationStatus::Ok
            : OperationStatus::Error;    
}

OperationStatus LogMessagesStorage::SaveToFile(const std::string& pathToParamsFile, const std::string& pathToTemplatesFile)
{
    std::ofstream paramsFile{ pathToParamsFile };
    std::ofstream templatesFile{ pathToTemplatesFile };

    if (!paramsFile.is_open())
    {
        std::cout << "Error during opening params file: " << pathToParamsFile << std::endl;
        return OperationStatus::Error;
    }
    if (!templatesFile.is_open())
    {
        std::cout << "Error during opening templates file: " << pathToTemplatesFile << std::endl;
        return OperationStatus::Error;
    }

    paramsFile << std::to_string(params_.size()) << std::endl;
    for (auto&& params : params_)
    {
        paramsFile << std::to_string(params.templateParamNames.size() + 1) << Separator;
        paramsFile << std::to_string(params.templateNumber) << Separator;
        for (auto&& templateParamName : params.templateParamNames)
        {
            paramsFile << templateParamName << Separator;
        }
        paramsFile << std::endl;
    }

    templatesFile << std::to_string(templates_.size()) << std::endl;
    for (auto&& templateWords : templates_)
    {
        templatesFile << std::to_string(templateWords.size()) << Separator;
        for (auto&& templateWord : templateWords)
        {
            templatesFile << templateWord << Separator;
        }
        templatesFile << std::endl;
    }

    paramsFile.close();
    templatesFile.close();
    return OperationStatus::Ok;
}

LogMessagesStorageData LogMessagesStorage::GetData() const noexcept
{
    return { params_, templates_ };
}

OperationStatus LogMessagesStorage::LoadTemplatesFromFile(const std::string& pathToTemplatesFile)
{    
    std::ifstream templatesFile{ pathToTemplatesFile };
    if (!templatesFile.is_open())
    {
        std::cout << "Error during opening templates file: " << pathToTemplatesFile << std::endl;
        return OperationStatus::Error;
    }

    std::string line;
    std::getline(templatesFile, line);

    if (line.empty())
    {
        std::cout << "Empty templates file: " << pathToTemplatesFile << std::endl;
        return OperationStatus::Error;
    }
        
    const auto templatesSize = StringToSizeT(line);
    if (templatesSize == WrongSize)
    {
        std::cout << "Zero templates in file: " << pathToTemplatesFile << std::endl;
        return OperationStatus::Error;
    }
    templates_.reserve(templatesSize);

    while (std::getline(templatesFile, line)) 
    {
        auto words = ParseSerializedLine(line);
        if (words.empty())
        {
            continue;
        }
        templates_.push_back(std::move(words));
    }

    if (templates_.size() != templatesSize)
    {
        std::cout << "Wrong templates count in file: " << pathToTemplatesFile << ", should be: " << templatesSize << std::endl;
        templates_.clear();
        return OperationStatus::Error;
    }

    templatesFile.close();
    return OperationStatus::Ok;
}

OperationStatus LogMessagesStorage::LoadParamsFromFile(const std::string& pathToParamsFile)
{
    std::ifstream paramsFile{ pathToParamsFile };

    if (!paramsFile.is_open())
    {
        std::cout << "Error during opening params file: " << pathToParamsFile << std::endl;
        return OperationStatus::Error;
    }

    std::string line;
    std::getline(paramsFile, line);

    if (line.empty())
    {
        std::cout << "Empty params file: " << pathToParamsFile << std::endl;
        return OperationStatus::Error;
    }
        
    const auto paramsSize = StringToSizeT(line);
    if (paramsSize == WrongSize)
    {
        std::cout << "Zero params in file: " << pathToParamsFile << std::endl;
        return OperationStatus::Error;
    }
    params_.reserve(paramsSize);

    while (std::getline(paramsFile, line)) 
    {
        auto words = ParseSerializedLine(line);
        if (words.size() < 2)
        {
            continue;
        }

        params_.push_back({ 0 /* templateNumber */, { } /* templateParamNames */ });
        params_.back().templateNumber = StringToSizeT(words[0]);

        if (params_.back().templateNumber == WrongSize || params_.back().templateNumber >= templates_.size())
        {
            params_.pop_back();
            continue;
        }

        std::move(words.begin() + 1, words.end(), std::back_inserter(params_.back().templateParamNames));
    }
    
    if (params_.size() != paramsSize)
    {
        std::cout << "Wrong params count in file: " << pathToParamsFile << ", should be: " << paramsSize << std::endl;
        templates_.clear();
        params_.clear();
        return OperationStatus::Error;
    }

    paramsFile.close();
    return OperationStatus::Ok;    
}

} // namespace log_messages
