#include <stdio.h>
#include <windows.h>
#include <tlhelp32.h>
#include <windef.h>
#include <stdlib.h>
#include <string.h>

// 当前要创建的守护进程的数量
#define GUARD_PROCESS_NUM 5
// 当前运行的进程名称
#define FILE_NAME 'win-keep-alive.exe'

// 根据进程的名当前进程的数量
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

// 获取当前支持命令的长度
int getCommondLength (char* binaryDirectoryFile) {
    char* start = "start";
    char* symbol = " \"";
    return strlen(start) + (strlen(symbol) * 2) + strlen(binaryDirectoryFile);
}

/**
 * 启动进程信息
 * @param  processNum 要启动进程数量
 * @param binaryDirectoryFile 启动的参数信息
 */
void start_process(int processNum, char* binaryDirectoryFile) {
  for (int i = 0; i < processNum; i++ ) {
        int length = getCommondLength(binaryDirectoryFile);
        char* command = malloc(sizeof(char) * length);
        snprintf(command, length, "start \"%s\"", binaryDirectoryFile);
        system(command);
        free(command);
    }
}

// 执行定时任务
void tasks (char* binaryDirectoryFile) {
    while (TRUE) {
        int processCount = CountProcess(binaryDirectoryFile);
        if (processCount == 0) {
            start_process(1, binaryDirectoryFile);
        }
        Sleep(800);
    }
}

// 守护进程，自己防止自己被结束进程 启动五个进程进行守护
void guard_win_keep_alive(char* binaryDirectoryFile) {
    for (int i = 0; i < GUARD_PROCESS_NUM; i++) {
        int length = getCommondLength(binaryDirectoryFile);
        char* command = malloc(sizeof(char) * length);
        snprintf(command, length, "start \"%s\"", binaryDirectoryFile);
        system(command);
        free(command);
    }
}

// 主方法
int main(int argc, char* argv[]) {
    // 二进制文件的目录信息
    char* binaryDirectoryFile = argv[1];
    tasks(binaryDirectoryFile);
    return 0;
}