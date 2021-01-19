#pragma once

#include <vector>
#include <string>
#include <string_view>

namespace log_messages {

enum class OperationStatus
{
    Ok,
    Error
};

struct LogMessageParams
{
    size_t templateNumber{ 0 };
    std::vector<std::string> templateParamNames;
};

struct LogMessagesStorageData
{
    const std::vector<LogMessageParams>& params;
    const std::vector<std::vector<std::string>>& templates;
};

std::vector<std::string> GetLogMessagesFromStorageData(const LogMessagesStorageData& data);

class LogMessagesStorage
{
public:
    LogMessagesStorage() = default;
    //LogMessagesStorage(
    //    std::vector<std::string> logMessages,
    //    std::vector<std::vector<std::string>> templates);
    //LogMessagesStorage(
    //    std::vector<std::string> logMessages,
    //    std::wstring pathToTemplatesFile);
    LogMessagesStorage(
        std::vector<LogMessageParams> params,
        std::vector<std::vector<std::string>> templates) noexcept;

    OperationStatus LoadFromFile(const std::string& pathToParamsFile, const std::string& pathToTemplatesFile);
    OperationStatus SaveToFile(const std::string&  pathToParamsFile, const std::string& pathToTemplatesFile);

    LogMessagesStorageData GetData() const noexcept;

private:
    OperationStatus LoadTemplatesFromFile(const std::string& pathToTemplatesFile);
    OperationStatus LoadParamsFromFile(const std::string& pathToParamsFile);

private:
    std::vector<LogMessageParams> params_;
    std::vector<std::vector<std::string>> templates_;
};

} // namespace log_messages
