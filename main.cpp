#include "log_messages_storage.h"
#include <iostream>

using namespace log_messages;

void SimpleTest()
{
    std::string pathToParamsFile{ "params.txt" };
    std::string pathToTemplatesFile{ "templates.txt" };

    LogMessagesStorage storage;
    storage.LoadFromFile(pathToParamsFile, pathToTemplatesFile);

    const auto data = storage.GetData();
    const auto messages = GetLogMessagesFromStorageData(data);

    for (auto&& message : messages)
    {
        std::cout << message << std::endl;
    }
}

int main()
{
    SimpleTest();
    system("pause");
    return 0;
}
