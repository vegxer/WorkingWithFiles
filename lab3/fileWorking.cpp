#include <iostream>
#include <windows.h>
#include <string>
#include <vector>

using namespace std;


bool isDirectory(wstring path);
vector<wstring> getItems(wstring dirName);
void printItems(vector<wstring> items);
wstring toRussian(wstring str);
double getItemSize(wstring path);
double getDirectorySize(wstring path);
bool createFile(wstring path);
bool copyFile(wstring pathFrom, wstring pathTo);
void deleteItem(wstring path);
bool moveDirectory(wstring pathFrom, wstring pathTo);
bool isHidden(wstring path);
wstring getFileName(wstring absPath);


int main()
{
    SetConsoleCP(1251);
    SetConsoleOutputCP(1251);
    setlocale(LC_CTYPE, "rus");

    bool active = true;
    while (active)
    {
        cout << "1 - Отображение списка файлов заданной директории\n";
        cout << "2 - Создание файла\n";
        cout << "3 - Копирование файла\n";
        cout << "4 - Удалить файл/директорию\n";
        cout << "5 - Переместить/переименовать директорию\n";
        cout << "6 - Установить атрибут \"Только для чтения\"\n";
        cout << "7 - Снять атрибут \"Только для чтения\"\n";
        cout << "8 - Проверить, является ли файл/директория скрытым\n";
        cout << "9 - Выйти из программы\n";
        int choice;
        cout << "Ваш выбор: ";
        cin >> choice;
        cin.get();
        string name, copyName;
        switch (choice)
        {
        case 1:
            cout << "Введите путь к директории: ";
            getline(cin, name, '\n');
            printItems(getItems(toRussian(wstring(name.begin(), name.end()))));
            break;
        case 2:
            cout << "Введите путь к новому файлу: ";
            getline(cin, name, '\n');
            if (createFile(toRussian(wstring(name.begin(), name.end()))))
                cout << "Файл успешно создан\n";
            else
                cout << "Не удалось создать файл\n";
            break;
        case 3:
            cout << "Введите путь копируемого файла: ";
            getline(cin, name, '\n');
            cout << "Введите путь для файла-копии: ";
            getline(cin, copyName, '\n');
            if (copyFile(toRussian(wstring(name.begin(), name.end())),
                toRussian(wstring(copyName.begin(), copyName.end()))))
                cout << "Копирование прошло успешно\n";
            else
                cout << "Копирование не удалось\n";
            break;
        case 4:
            cout << "Введите путь к файлу/директории: ";
            getline(cin, name, '\n');
            deleteItem(toRussian(wstring(name.begin(), name.end())));
            cout << "Удаление прошло успешно\n";
            break;
        case 5:
            cout << "Введите путь перемещаемой директории: ";
            getline(cin, name, '\n');
            cout << "Введите новый путь для директории: ";
            getline(cin, copyName, '\n');
            if (moveDirectory(toRussian(wstring(name.begin(), name.end())),
                toRussian(wstring(copyName.begin(), copyName.end()))))
                cout << "Перемещение директории прошло успешно\n";
            else
                cout << "Не удалось переместить директорию\n";
            break;
        case 6:
            cout << "Введите путь к файлу: ";
            getline(cin, name, '\n');
            if (SetFileAttributesW(toRussian(wstring(name.begin(), name.end())).c_str(),
                FILE_ATTRIBUTE_READONLY))
                cout << "Атрибут успешно установлен\n";
            else
                cout << "Не удалось установить атрибут\n";
            break;
        case 7:
            cout << "Введите путь к файлу: ";
            getline(cin, name, '\n');
            if (SetFileAttributesW(toRussian(wstring(name.begin(), name.end())).c_str(),
                FILE_ATTRIBUTE_NORMAL))
                cout << "Атрибут успешно снят\n";
            else
                cout << "Не удалось снять атрибут\n";
            break;
        case 8:
            cout << "Введите путь к файлу/директории: ";
            getline(cin, name, '\n');
            cout << "Файл/директория ";
            if (isHidden(toRussian(wstring(name.begin(), name.end()))))
                cout << "скрытый\n";
            else
                cout << "не скрытый\n";
            break;
        case 9:
            active = false;
            break;
        default:
            cout << "Неверное введено число\n";
            break;
        }
        cout << '\n';
    }
    return 0;
}

vector<wstring> getItems(wstring dirName)
{
    if (!isDirectory(dirName))
    {
        cout << "Нужен путь к папке, а не файлу\n";
        return {};
    }

    wstring pathEnd = L"*";
    if (dirName[dirName.length() - 1] != L'\\' && dirName[dirName.length() - 1] != L'/')
        pathEnd = L"\\*";
    dirName += pathEnd; 

    vector<wstring> items;
    WIN32_FIND_DATAW file;
    HANDLE hFind = FindFirstFileW(dirName.c_str(), &file);
    FindNextFileW(hFind, &file);
    dirName.erase(dirName.length() - 1, 1);

    if (hFind != INVALID_HANDLE_VALUE)
    {
        while (FindNextFileW(hFind, &file) != NULL)
            items.push_back(dirName + &file.cFileName[0]);

        FindClose(hFind);
    }
    else
        cout << "Ошибка чтения директории\n";

    return items;
}

void printItems(vector<wstring> items)
{
    for (vector<wstring>::iterator it = items.begin(); it < items.end(); ++it)
    {
        wcout << getFileName(*it);
        cout << " (" << getItemSize(*it) << " КБ)\n";
    }
}

double getItemSize(wstring path)
{
    if (isDirectory(path))
        return getDirectorySize(path);
    HANDLE file = CreateFileW(path.c_str(),
        GENERIC_READ, FILE_SHARE_READ | FILE_SHARE_WRITE, nullptr,
        OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, nullptr);
    LARGE_INTEGER fileSize = { 0 };
    GetFileSizeEx(file, &fileSize);
    CloseHandle(file);
    return fileSize.QuadPart / (double)1024;
}

bool isDirectory(wstring path)
{
    return GetFileAttributesW(path.c_str()) != INVALID_FILE_ATTRIBUTES && 
        GetFileAttributesW(path.c_str()) & FILE_ATTRIBUTE_DIRECTORY;
}

double getDirectorySize(wstring dirName)
{
    double size = 0;
    WIN32_FIND_DATAW file;

    dirName += L"\\*";
    HANDLE hFind = FindFirstFileW(dirName.c_str(), &file);
    FindNextFileW(hFind, &file);
    dirName.erase(dirName.length() - 1, 1);

    if (hFind != INVALID_HANDLE_VALUE)
    {
        while (FindNextFileW(hFind, &file) != NULL)
            size += getItemSize(dirName + &file.cFileName[0]);

        FindClose(hFind);
    }

    return size;
}

wstring toRussian(wstring str)
{
    for (int i = 0; i < str.length(); ++i)
    {
        if (str[i] > 255)
            str[i] -= 64432;
    }

    return str;
}

bool createFile(wstring path)
{
    HANDLE file = CreateFileW(path.c_str(), GENERIC_READ |
        GENERIC_WRITE, FILE_SHARE_WRITE | FILE_SHARE_READ, NULL, CREATE_NEW,
        FILE_ATTRIBUTE_NORMAL | FILE_FLAG_OVERLAPPED, 0);
    CloseHandle(file);
    return file != INVALID_HANDLE_VALUE;
}

bool copyFile(wstring pathFrom, wstring pathTo)
{
    if (isDirectory(pathFrom))
        return false;

    HANDLE file = CreateFileW(pathTo.c_str(),
        GENERIC_READ, FILE_SHARE_READ | FILE_SHARE_WRITE, nullptr,
        OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, nullptr);
    if (file != INVALID_HANDLE_VALUE)
    {
        char rewrite;
        cout << "Файл-копия уже существует. Перезаписать его? (y/n): ";
        cin >> rewrite;
        if (rewrite == 'n')
            return false;
    }
    CloseHandle(file);
    return CopyFileW(pathFrom.c_str(), pathTo.c_str(), false);
}

void deleteItem(wstring path)
{
    if (!isDirectory(path))
    {
        DeleteFileW(path.c_str());
        return;
    }

    vector<wstring> items = getItems(path);
    for (vector<wstring>::iterator it = items.begin(); it < items.end(); ++it)
    {
        if (isDirectory(*it))
            deleteItem(*it);
        else
        {
            if (!DeleteFileW((*it).c_str()))
            {
                cout << "Не удаётся удалить файл ";
                wcout << *it;
                cout << '\n';
            }
        }
    }

    if (!RemoveDirectoryW((path).c_str()))
    {
        cout << "Не удаётся удалить директорию ";
        wcout << path;
        cout << '\n';
    }
}

bool moveDirectory(wstring pathFrom, wstring pathTo)
{
    if (!isDirectory(pathFrom))
        return false;

    return MoveFileExW(pathFrom.c_str(), pathTo.c_str(),
        MOVEFILE_COPY_ALLOWED | MOVEFILE_WRITE_THROUGH);
}

bool isHidden(wstring path)
{
    return GetFileAttributesW(path.c_str()) & FILE_ATTRIBUTE_HIDDEN;
}

wstring getFileName(wstring absPath)
{  
    int lastSeparatorIndex = max((int)absPath.rfind('\\'), (int)absPath.rfind('/'));
    if (lastSeparatorIndex >= 0)
    {
        absPath.erase(0, lastSeparatorIndex + 1);
        return absPath;
    }
    else
    {
        cout << "Неверный ввод пути файла\n";
        return L"";
    }
}
