#include "imgui.h"
#include <vector>
#include "gui.h"
#include <windows.h>
#include <iostream>
#include <filesystem>
#include <fstream>

struct Folder {
    std::string path;
    bool selected = false;
};

std::vector<Folder> folders;

std::wstring StringToWString(const std::string& str) {
    int size_needed = MultiByteToWideChar(CP_UTF8, 0, str.c_str(), (int)str.size(), NULL, 0);
    std::wstring wstr(size_needed, 0);
    MultiByteToWideChar(CP_UTF8, 0, str.c_str(), (int)str.size(), &wstr[0], size_needed);
    return wstr;
}

bool SetReadOnly(const std::string& path, bool readOnly) {
    std::wstring wPath = StringToWString(path);
    DWORD attrs = GetFileAttributesW(wPath.c_str());
    if (attrs == INVALID_FILE_ATTRIBUTES) {
        DWORD error = GetLastError();
        std::cerr << "Error getting attributes for " << path << ": " << error << std::endl;
        return false;
    }

    DWORD newAttrs = readOnly 
        ? (attrs | FILE_ATTRIBUTE_READONLY) 
        : (attrs & ~FILE_ATTRIBUTE_READONLY);

    if (!SetFileAttributesW(wPath.c_str(), newAttrs)) {
        DWORD error = GetLastError();
        std::cerr << "Error setting attributes for " << path << ": " << error << std::endl;
        return false;
    }

    DWORD checkAttrs = GetFileAttributesW(wPath.c_str());
    if (checkAttrs == INVALID_FILE_ATTRIBUTES) {
        std::cerr << "Error checking attributes after setting for " << path << ": " << GetLastError() << std::endl;
        return false;
    }

    if (readOnly && !(checkAttrs & FILE_ATTRIBUTE_READONLY)) {
        std::cerr << "Failed to set Read-Only for " << path << std::endl;
        return false;
    } else if (!readOnly && (checkAttrs & FILE_ATTRIBUTE_READONLY)) {
        std::cerr << "Failed to remove Read-Only for " << path << std::endl;
        return false;
    }

    return true;
}

void SetReadOnlyRecursively(const std::string& path, bool readOnly) {
    for (const auto& entry : std::filesystem::directory_iterator(path)) {
        if (entry.is_regular_file()) {
            if (!SetReadOnly(entry.path().string(), readOnly)) {
                std::cerr << "Failed to set Read-Only for file: " << entry.path().string() << std::endl;
            }
        }
    }
}

void SaveFolders(const std::string& filename) {
    std::ofstream file(filename);
    if (file.is_open()) {
        for (const auto& folder : folders) {
            file << folder.path << std::endl;
        }
        file.close();
    } else {
        std::cerr << "Unable to open file for saving: " << filename << std::endl;
    }
}

void LoadFolders(const std::string& filename) {
    std::ifstream file(filename);
    if (file.is_open()) {
        std::string line;
        while (std::getline(file, line)) {
            if (!line.empty()) {
                folders.push_back({line, false});
            }
        }
        file.close();
    } else {
        std::cerr << "Unable to open file for loading: " << filename << std::endl;
    }
}

void RenderUI() {
    ImGui::Begin("Folder Manager");

    static char pathBuffer[256] = "";
    ImGui::InputText("Path", pathBuffer, IM_ARRAYSIZE(pathBuffer));
    if (ImGui::Button("Add Folder")) {
        DWORD attrs = GetFileAttributesA(pathBuffer);
        if (attrs != INVALID_FILE_ATTRIBUTES && (attrs & FILE_ATTRIBUTE_DIRECTORY)) {
            folders.push_back({pathBuffer, false});
        } else {
            std::cerr << "Invalid folder path: " << pathBuffer << std::endl;
        }
    }

    for (size_t i = 0; i < folders.size(); ++i) {
        ImGui::Checkbox(folders[i].path.c_str(), &folders[i].selected);
        ImGui::SameLine();
        
        if (ImGui::Button(("Set Read-Only##" + std::to_string(i)).c_str())) {
            if (folders[i].selected) {
                SetReadOnlyRecursively(folders[i].path, true);
                std::cout << "Set Read-Only for: " << folders[i].path << std::endl;
            }
        }
        ImGui::SameLine();
        
        if (ImGui::Button(("Remove Read-Only##" + std::to_string(i)).c_str())) {
            if (folders[i].selected) {
                SetReadOnlyRecursively(folders[i].path, false);
                std::cout << "Removed Read-Only for: " << folders[i].path << std::endl;
            }
        }
        ImGui::SameLine();
        
        if (ImGui::Button(("Remove Folder##" + std::to_string(i)).c_str())) {
            folders.erase(folders.begin() + i);
            --i;
        }
    }

    ImGui::End();
}