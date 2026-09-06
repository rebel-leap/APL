#include "apl/lexer.h"
#include "apl/parser.h"
#include "apl/interpreter.h"
#include "apl/compiler.h"
#include <iostream>
#include <fstream>
#include <sstream>
#include <filesystem>
#include <string>
#include <vector>
#include <algorithm>

#ifdef _WIN32
#include <windows.h>
#include <shlobj.h>
#include <comdef.h>
#include <shellapi.h>
#endif

namespace apl {

using String = std::string;

String readFile(const String& path) {
    std::ifstream file(path);
    if (!file.is_open()) throw std::runtime_error("Cannot open file: " + path);
    std::stringstream buffer;
    buffer << file.rdbuf();
    return buffer.str();
}

void writeFile(const String& path, const String& content) {
    std::ofstream file(path);
    file << content;
}

void runFile(const String& path) {
    String source = readFile(path);
    Lexer lexer(source);
    auto tokens = lexer.tokenize();
    Parser parser(tokens);
    auto statements = parser.parse();
    Interpreter interpreter;
    interpreter.execute(statements);
}

void compileFile(const String& path, const String& output) {
    String source = readFile(path);
    Lexer lexer(source);
    auto tokens = lexer.tokenize();
    Parser parser(tokens);
    auto statements = parser.parse();
    Compiler compiler;
    String cpp = compiler.compile(statements);
    writeFile(output, cpp);
    std::cout << "✅ تم الترجمة إلى: " << output << std::endl;
}

#ifdef _WIN32
String openFileDialog() {
    OPENFILENAMEW ofn = {0};
    wchar_t fileName[MAX_PATH] = {0};
    wchar_t initialDir[MAX_PATH] = {0};

    if (SUCCEEDED(SHGetFolderPathW(NULL, CSIDL_DESKTOP, NULL, 0, initialDir))) {
    }

    ofn.lStructSize = sizeof(ofn);
    ofn.hwndOwner = GetConsoleWindow();
    ofn.lpstrFilter = L"APL Files\0*.ع\0Module Files\0*.م\0All Files\0*.*\0";
    ofn.lpstrFile = fileName;
    ofn.nMaxFile = MAX_PATH;
    ofn.lpstrInitialDir = initialDir;
    ofn.Flags = OFN_PATHMUSTEXIST | OFN_FILEMUSTEXIST;

    if (GetOpenFileNameW(&ofn)) {
        int len = WideCharToMultiByte(CP_UTF8, 0, fileName, -1, NULL, 0, NULL, NULL);
        std::string path(len, 0);
        WideCharToMultiByte(CP_UTF8, 0, fileName, -1, &path[0], len, NULL, NULL);
        return path.c_str();
    }
    return "";
}

String saveFileDialog(const String& defaultName) {
    OPENFILENAMEW ofn = {0};
    wchar_t fileName[MAX_PATH] = {0};
    wchar_t initialDir[MAX_PATH] = {0};

    if (SUCCEEDED(SHGetFolderPathW(NULL, CSIDL_DESKTOP, NULL, 0, initialDir))) {
    }

    std::wstring wideDefault = std::wstring(defaultName.begin(), defaultName.end());
    wcscpy(fileName, wideDefault.c_str());

    ofn.lStructSize = sizeof(ofn);
    ofn.hwndOwner = GetConsoleWindow();
    ofn.lpstrFilter = L"APL Files\0*.ع\0Module Files\0*.م\0C++ Files\0*.cpp\0All Files\0*.*\0";
    ofn.lpstrFile = fileName;
    ofn.nMaxFile = MAX_PATH;
    ofn.lpstrInitialDir = initialDir;
    ofn.lpstrDefExt = L"ع";
    ofn.Flags = OFN_PATHMUSTEXIST | OFN_OVERWRITEPROMPT;

    if (GetSaveFileNameW(&ofn)) {
        int len = WideCharToMultiByte(CP_UTF8, 0, fileName, -1, NULL, 0, NULL, NULL);
        std::string path(len, 0);
        WideCharToMultiByte(CP_UTF8, 0, fileName, -1, &path[0], len, NULL, NULL);
        return path.c_str();
    }
    return "";
}
#endif

void printBanner() {
    std::cout << "\n";
    std::cout << "    _    ____  _       \n";
    std::cout << "   / \\  |  _ \\| |      \n";
    std::cout << "  / _ \\ | |_) | |      \n";
    std::cout << " / ___ \\|  __/| |___   \n";
    std::cout << "/_/   \\_\\_|   |_____|  \n";
    std::cout << "\nAPL - Arabic Programming Language v1.0\n";
    std::cout << "----------------------------------------\n\n";
}

void printMenu() {
    std::cout << "┌────────────────────────────────────────┐\n";
    std::cout << "│  1. تشغيل ملف APL                     │\n";
    std::cout << "│  2. ترجمة ملف APL إلى C++            │\n";
    std::cout << "│  3. الوضع التفاعلي (REPL)            │\n";
    std::cout << "│  4. عرض الأمثلة                      │\n";
    std::cout << "│  5. الخروج                           │\n";
    std::cout << "└────────────────────────────────────────┘\n";
    std::cout << "\nاختر رقم: ";
}

void printSuccess(const String& message) {
    std::cout << "\n✅ " << message << "\n\n";
}

void printError(const String& message) {
    std::cout << "\n❌ " << message << "\n\n";
}

void runInteractive() {
    printBanner();
    std::cout << "الوضع التفاعلي. اكتب 'exit' للخروج.\n\n";
    Interpreter interpreter;
    while (true) {
        std::cout << ">>> ";
        std::string line;
        if (!std::getline(std::cin, line)) break;
        if (line == "exit" || line == "خروج") break;
        if (line.empty()) continue;
        try {
            Lexer lexer(line);
            auto tokens = lexer.tokenize();
            Parser parser(tokens);
            auto statements = parser.parse();
            interpreter.execute(statements);
        } catch (const std::exception& e) {
            std::cerr << "خطأ: " << e.what() << std::endl;
        }
    }
}

void showExamples() {
    std::cout << "\n📚 أمثلة برامج APL:\n";
    std::cout << "─────────────────────\n\n";

    std::cout << "1️⃣ مرحباً بالعالم:\n";
    std::cout << "   اطبع_سطر(\"مرحباً بالعالم!\")\n\n";

    std::cout << "2️⃣ المتغيرات:\n";
    std::cout << "   متغير اسم = \"أحمد\"\n";
    std::cout << "   متغير عمر = 25\n";
    std::cout << "   اطبع_سطر(\"الاسم: \" + اسم)\n\n";

    std::cout << "3️⃣ الدوال:\n";
    std::cout << "   دالة جمع(أ, ب):\n";
    std::cout << "       أرجع أ + ب\n";
    std::cout << "   نهاية\n";
    std::cout << "   متغير ناتج = جمع(10, 20)\n";
    std::cout << "   اطبع_سطر(ناتج)\n\n";

    std::cout << "4️⃣ الحلقات:\n";
    std::cout << "   لكل i في مجال(1, 6):\n";
    std::cout << "       اطبع_سطر(i)\n";
    std::cout << "   نهاية\n\n";

    std::cout << "5️⃣ الشروط:\n";
    std::cout << "   إذا درجة >= 90:\n";
    std::cout << "       اطبع_سطر(\"ممتاز\")\n";
    std::cout << "   وإلا إذا درجة >= 80:\n";
    std::cout << "       اطبع_سطر(\"جيد جداً\")\n";
    std::cout << "   نهاية\n\n";
}

void runFileWithDialog() {
#ifdef _WIN32
    String path = openFileDialog();
    if (path.empty()) {
        printError("لم يتم اختيار ملف");
        return;
    }
    
    if (!std::filesystem::exists(path)) {
        printError("الملف غير موجود: " + path);
        return;
    }
    
    try {
        std::cout << "📂 تشغيل: " << path << "\n\n";
        runFile(path);
        printSuccess("تم التشغيل بنجاح");
    } catch (const std::exception& e) {
        printError(String("خطأ أثناء التشغيل: ") + e.what());
    }
#else
    std::cout << "يرجى إدخال مسار الملف: ";
    String path;
    std::getline(std::cin, path);
    if (path.empty()) {
        printError("لم يتم إدخال مسار");
        return;
    }
    try {
        runFile(path);
        printSuccess("تم التشغيل بنجاح");
    } catch (const std::exception& e) {
        printError(String("خطأ أثناء التشغيل: ") + e.what());
    }
#endif
}

void compileFileWithDialog() {
#ifdef _WIN32
    String path = openFileDialog();
    if (path.empty()) {
        printError("لم يتم اختيار ملف");
        return;
    }
    
    String defaultName = std::filesystem::path(path).stem().string() + ".cpp";
    String output = saveFileDialog(defaultName);
    if (output.empty()) {
        printError("لم يتم اختيار مسار الحفظ");
        return;
    }
    
    try {
        std::cout << "📂 ترجمة: " << path << "\n";
        std::cout << "📁 إلى: " << output << "\n\n";
        compileFile(path, output);
        printSuccess("تمت الترجمة بنجاح");
    } catch (const std::exception& e) {
        printError(String("خطأ أثناء الترجمة: ") + e.what());
    }
#else
    std::cout << "يرجى إدخال مسار ملف APL: ";
    String path;
    std::getline(std::cin, path);
    if (path.empty()) {
        printError("لم يتم إدخال مسار");
        return;
    }
    std::cout << "يرجى إدخال مسار ملف الإخراج: ";
    String output;
    std::getline(std::cin, output);
    if (output.empty()) {
        printError("لم يتم إدخال مسار الإخراج");
        return;
    }
    try {
        compileFile(path, output);
        printSuccess("تمت الترجمة بنجاح");
    } catch (const std::exception& e) {
        printError(String("خطأ أثناء الترجمة: ") + e.what());
    }
#endif
}

} // namespace apl

int main(int argc, char* argv[]) {
#ifdef _WIN32
    SetConsoleOutputCP(CP_UTF8);
    SetConsoleTitleA("APL - Arabic Programming Language v1.0");
#endif

    using namespace apl;

    if (argc > 1) {
        String arg = argv[1];
        if (arg == "-h" || arg == "--help") {
            printBanner();
            std::cout << "الاستخدام:\n";
            std::cout << "  " << argv[0] << " [خيار] [ملف]\n\n";
            std::cout << "الخيارات:\n";
            std::cout << "  (بدون خيارات)  القائمة التفاعلية\n";
            std::cout << "  -i             الوضع التفاعلي (REPL)\n";
            std::cout << "  -c <ملف>       ترجمة إلى C++\n";
            std::cout << "  -o <اسم>       اسم ملف الإخراج (مع -c)\n";
            std::cout << "  -h             عرض هذه المساعدة\n";
            std::cout << "\nأمثلة:\n";
            std::cout << "  " << argv[0] << "               القائمة التفاعلية\n";
            std::cout << "  " << argv[0] << " -i            الوضع التفاعلي\n";
            std::cout << "  " << argv[0] << " -c program.ع -o output.cpp\n";
            return 0;
        }
        if (arg == "-i") {
            runInteractive();
            return 0;
        }
        if (arg == "-c" && argc >= 3) {
            String output = (argc >= 5 && String(argv[3]) == "-o") ? argv[4] : "output.cpp";
            try {
                compileFile(argv[2], output);
                printSuccess("تمت الترجمة بنجاح");
            } catch (const std::exception& e) {
                printError(String("خطأ: ") + e.what());
                return 1;
            }
            return 0;
        }
        if (std::filesystem::exists(argv[1])) {
            try {
                runFile(argv[1]);
                return 0;
            } catch (const std::exception& e) {
                printError(String("خطأ: ") + e.what());
                return 1;
            }
        }
    }

    printBanner();

    while (true) {
        printMenu();
        String choice;
        std::getline(std::cin, choice);

        if (choice == "1") {
            runFileWithDialog();
        } else if (choice == "2") {
            compileFileWithDialog();
        } else if (choice == "3") {
            runInteractive();
        } else if (choice == "4") {
            showExamples();
        } else if (choice == "5") {
            std::cout << "\n👋 مع السلامة!\n\n";
            break;
        } else {
            printError("اختيار غير صحيح، حاول مرة أخرى");
        }
    }

    return 0;
}