#ifndef PARSINGSTRUCTS_H
#define PARSINGSTRUCTS_H
/*
Все, что связано с консольным интерфейсом
*/
#include <regex>
#include <unordered_map>
#include "AVLTreeWordlist.h"

const std::wstring welcomeMessage = LR"(Поддерживаемые команды:
    insert <word> <слово>  // добавление элемента в словарь
    search <word>          // поиск слова в словаре
    remove <word>          // удаление слова из словаря
    print                  // вывод словаря в консоль
    *Ctrl + Z*             // завершение программы
)";

std::string wstringToString(const std::wstring& wstr) {
    std::wstring_convert<std::codecvt_utf8<wchar_t>> converter;
    std::string str = converter.to_bytes(wstr);
    std::transform(str.begin(), str.end(), str.begin(), ::tolower);
    return str;
}

enum class CommandType {
    INSERT, REMOVE, SEARCH, PRINT
};

struct Command {
    CommandType type_;

    std::string word_;
    bool has_word_ = false;

    std::wstring translation_;
    bool has_translation_ = false;
};

namespace patterns {
    /*
    Мой вариант подразумевает 3 типа команд по формату вызова:
    1) <type> (print)
    2) <type> <word> (search *word*, remove *word*)
    3) <type> <word> <translate> (insert *word* *слово*)
    */

    const std::wstring word = L"[a-zA-Z-]+";  // Только английские буквы
    const std::wstring ru_word = L"[А-Яа-яЁё-]+";  // Только русские буквы

    std::wregex commandLinePattern_1(L"^\\s*(\\w+)\\s*$");
    std::wregex commandLinePattern_2(L"^\\s*(\\w+)\\s+(" + word + L")\\s*$");
    std::wregex commandLinePattern_3(L"^\\s*(\\w+)\\s+(" + word + L")\\s+(" + ru_word + L")\\s*$");}

CommandType wstringToCommandType(const std::wstring& wstring) {
    static const std::unordered_map<std::wstring, CommandType> converter = {
        {L"insert", CommandType::INSERT},
        {L"remove", CommandType::REMOVE},
        {L"search", CommandType::SEARCH},
        {L"print", CommandType::PRINT}
    };

    std::wstring lowerWstring = wstring;
    std::transform(lowerWstring.begin(), lowerWstring.end(), lowerWstring.begin(), ::tolower);
    auto it = converter.find(lowerWstring);
    if (it == converter.end()) {
        throw std::invalid_argument("Input error: unidentified command type");
    }
    return it->second;
}

Command parseCommandLine(const std::wstring& wstr) {
    Command cmd;
    std::wsmatch matches;

    if (std::regex_match(wstr, matches, patterns::commandLinePattern_1)) {
        cmd.type_ = wstringToCommandType(matches[1].str());

    } else if (std::regex_match(wstr, matches, patterns::commandLinePattern_2)) {
        cmd.type_ = wstringToCommandType(matches[1].str());
        cmd.word_ = wstringToString(matches[2].str());
        cmd.has_word_ = true;

    } else if (std::regex_match(wstr, matches, patterns::commandLinePattern_3)) {
        cmd.type_ = wstringToCommandType(matches[1].str());
        cmd.word_ = wstringToString(matches[2].str());
        cmd.has_word_ = true;
        cmd.translation_ = matches[3].str();
        cmd.has_translation_ = true;

    } else {
        throw std::invalid_argument("Parsing error: invalid command format");
    }

    switch (cmd.type_) {
        case CommandType::PRINT:
            if (cmd.has_word_ || cmd.has_translation_) {
                throw std::invalid_argument("PRINT command accepts no arguments");
            }
            break;
        case CommandType::SEARCH:
        case CommandType::REMOVE:
            if (!cmd.has_word_ || cmd.has_translation_) {
                std::string type = cmd.type_ == CommandType::SEARCH ? "SEARCH" : "REMOVE"; 
                throw std::invalid_argument(type + " command requires exactly one argument");
            }
            break;
        case CommandType::INSERT:
            if (!cmd.has_word_ || !cmd.has_translation_) {
                throw std::invalid_argument("INSERT command requires two arguments");
            }
            break;
    }
    return cmd;
}

void CommandHandler(AVLTreeWordlist& wordList, const Command& cmd) {
    try {
        switch (cmd.type_) {
            case CommandType::INSERT: {
                wordList.insert(cmd.word_, cmd.translation_);
                break;
            }

            case CommandType::REMOVE: {
                wordList.remove(cmd.word_);
                break;
            }

            case CommandType::SEARCH: {
                wordList.search(cmd.word_);
                break;
            }

            case CommandType::PRINT: {
                wordList.printAsTree();
                break;
            }
        }
    } catch (const std::invalid_argument& ex) {
        throw ex;
    }
}

#endif // PARSINGSTRUCTS_H