#include <fstream>
#include <sstream>
#include <string>
#include <windows.h>

std::string ws2s(std::wstring_view s);
bool WriteRegistryDWORD(const std::wstring &subKey, const std::wstring &valueName, DWORD data);
bool WriteRegistryString(
        const std::wstring &subKey, const std::wstring &valueName, const std::wstring &data);

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, PSTR lpCmdLine, int nCmdShow) {
    std::ofstream out;
    out.open("out.log", std::ios::app);
    if (! out.is_open()) {
        return 1;
    }

    int argc = 0;
    LPWSTR *argv = CommandLineToArgvW(GetCommandLineW(), &argc);

    out << "hello" << std::endl;
    out << ws2s(argv[0]) << std::endl;

    std::wstringstream converter(argv[1]);
    DWORD dwordValue;
    converter >> dwordValue;

    WriteRegistryDWORD(L"SOFTWARE\\Arskom\\updsvc", L"PERIOD", dwordValue);
    WriteRegistryString(L"SOFTWARE\\Arskom\\updsvc", L"REL_CHAN", argv[2]);
    out << ws2s(argv[1]) << std::endl;
    out << ws2s(argv[2]) << std::endl;
    out << argc << std::endl;
    out.close();
    return 0;
}

std::string ws2s(std::wstring_view s) {
    int len;
    int slength = (int)s.length();
    len = WideCharToMultiByte(CP_UTF8, 0, s.data(), slength, 0, 0, 0, 0);
    std::string buf;
    buf.resize(len);
    WideCharToMultiByte(CP_UTF8, 0, s.data(), slength, const_cast<char *>(buf.data()), len, 0, 0);
    return buf;
}

bool WriteRegistryDWORD(const std::wstring &subKey, const std::wstring &valueName, DWORD data) {
    HKEY hSubKey;
    if (RegCreateKeyExW(HKEY_LOCAL_MACHINE, subKey.c_str(), 0, nullptr, REG_OPTION_NON_VOLATILE,
                KEY_SET_VALUE, nullptr, &hSubKey, nullptr)
            != ERROR_SUCCESS) {
        // error handling
        return false;
    }

    if (RegSetValueExW(hSubKey, valueName.c_str(), 0, REG_DWORD,
                reinterpret_cast<const BYTE *>(&data), sizeof(data))
            != ERROR_SUCCESS) {
        RegCloseKey(hSubKey);
        // error handling
        return false;
    }

    RegCloseKey(hSubKey);
    return true;
}

bool WriteRegistryString(
        const std::wstring &subKey, const std::wstring &valueName, const std::wstring &data) {
    HKEY hSubKey;
    if (RegCreateKeyExW(HKEY_LOCAL_MACHINE, subKey.c_str(), 0, nullptr, REG_OPTION_NON_VOLATILE,
                KEY_SET_VALUE, nullptr, &hSubKey, nullptr)
            != ERROR_SUCCESS) {
        return false;
    }

    if (RegSetValueExW(hSubKey, valueName.c_str(), 0, REG_SZ,
                reinterpret_cast<const BYTE *>(data.c_str()),
                static_cast<DWORD>((data.length() + 1) * sizeof(wchar_t)))
            != ERROR_SUCCESS) {
        RegCloseKey(hSubKey);
        return false;
    }

    RegCloseKey(hSubKey);
    return true;
}

/*DWORD ws2dw(std::wstring s) {
    std::wstringstream converter(s);
    DWORD dwordValue;
    converter >> dwordValue;
    return s;
}*/