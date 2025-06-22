#include "AVLTreeWordlist.h"
#include "ParsingStructs.h"
#include <iostream>
#include <windows.h>
#include <io.h>
#include <fcntl.h>
#include <limits>
/*
Словарь не чуствителен к регистру ключей, все ключи переводятся в нижний

Попытка удалить узел по несуществующему ключу
не является исключительной ситуацией, просто игнорируется
*/

int main() {
    _setmode(_fileno(stdin), _O_U16TEXT);
    _setmode(_fileno(stdout), _O_U16TEXT);

    std::wcout << welcomeMessage << L"> ";

    AVLTreeWordlist wl;
    std::wstring line;
    bool lastLineWasEmpty = false;
    while (std::getline(std::wcin, line)) {
        if (line.empty() && !lastLineWasEmpty) {
            lastLineWasEmpty = true;
            continue;
        }

        lastLineWasEmpty = false;
        try {
            Command cmd = parseCommandLine(line);
            CommandHandler(wl, cmd);
        } catch (const std::invalid_argument& ex) {
            std::wcout << ex.what() << std::endl;
        }
        std::wcout << L"> ";
    }

    return 0;
}