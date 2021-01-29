#include <stdio.h>
#include <windows.h>
#include <tlhelp32.h>
#include <windef.h>
#include <stdlib.h>
#include <string.h>

// 根据进程的名称获取当前进程的数量
int CountProcess(const char* processName) {
    int exists = 0;
    PROCESSENTRY32 entry;
    entry.dwSize = sizeof(PROCESSENTRY32);
    HANDLE snapshot = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
    if (Process32First(snapshot, &entry)) {
        while (Process32Next(snapshot, &entry)) {
            if (strstr(processName, entry.szExeFile)) {
                exists += 1;
            }
        }
    }
    CloseHandle(snapshot);
    return exists;
}

// 执行定时任务
void tasks (char* binaryDirectoryFile) {
    while (TRUE) {
        int processCount = CountProcess(binaryDirectoryFile);
        if (processCount == 0) {
            char* start = "start";
            char* symbol = " \"";
            int lenth = strlen(start) + (strlen(symbol) * 2) + strlen(binaryDirectoryFile);
            char* command = malloc(sizeof(char) * lenth);
            snprintf(command, lenth, "start \"%s\"", binaryDirectoryFile);
            system(command);
        }
        Sleep(800);
    }
}

int main(int argc, char* argv[]) {
    // 二进制文件的目录信息
    char* binaryDirectoryFile = argv[1];
    tasks(binaryDirectoryFile);
    return 0;
}