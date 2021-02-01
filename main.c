#include <stdio.h>
#include <windows.h>
#include <tlhelp32.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

// 共享内存的大小
#define SHARE_BUF_SIZE 1024

char* SHARE_FILE_NAME = "WIN-KEEP-ALIVE";

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
    char* symbol = " \"";
    return (strlen(symbol) * 2) + strlen(binaryDirectoryFile);
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
        snprintf(command, length, "\"%s\"", binaryDirectoryFile);
        system(command);
        free(command);
    }
}

// 复制数据到共享内存
void cpoy_share_memory (PVOID share_data) {
    int process_id = GetCurrentProcessId();
    time_t current_time = time(NULL);
    char* memory_msg = malloc(SHARE_BUF_SIZE);
    snprintf(memory_msg, SHARE_BUF_SIZE, "%d|%Ld", process_id, current_time); 
    CopyMemory(share_data, memory_msg, SHARE_BUF_SIZE);
    free(memory_msg);
}

// 执行定时任务
void tasks (char* binaryDirectoryFile) {
    char* share_data = OpenFileMapping(
        FILE_MAP_ALL_ACCESS,
        FALSE,
        SHARE_FILE_NAME);
    if (share_data == NULL) {
        // 创建共享内存
        HANDLE hmap = CreateFileMappingA(INVALID_HANDLE_VALUE,
            NULL,
            PAGE_READWRITE | SEC_COMMIT,
            0,
            SHARE_BUF_SIZE,
            SHARE_FILE_NAME);
        share_data = MapViewOfFile(hmap, FILE_MAP_READ | FILE_MAP_WRITE, 0, 0, SHARE_BUF_SIZE);
    }
    while (TRUE) {
        // 如果没有写入数据，则直接写入当前程序
        if (strlen(share_data) == 0 ) {
            cpoy_share_memory((PVOID)share_data);
        }
        int process_id = atoi(strtok(share_data, "|"));
        long old_time = atoi(strtok(share_data, "|"));
        time_t current_time = time(NULL);

        // 等于当前线程, 则执行master的任务
        if (process_id == GetCurrentProcessId()) {
            int processCount = CountProcess(binaryDirectoryFile);
            if (processCount == 0) {
                start_process(1, binaryDirectoryFile);
            }
        // 如果不等于当前线程，并且时间已经超过1.5秒，则选举为master
        } else if (process_id != GetCurrentProcessId() && current_time - old_time > 1.5) {
            cpoy_share_memory((PVOID)share_data);
            int processCount = CountProcess(binaryDirectoryFile);
            if (processCount == 0) {
                start_process(1, binaryDirectoryFile);
            }
        }
        Sleep(800);
    }
}

// 主方法
int main(int argc, char* argv[]) {
    setbuf(stdout, NULL);
    if (argc < 2) {
        printf("No valid parameters.");
        return 0;
    } else {
        // 二进制文件的目录信息
        char* binaryDirectoryFile = argv[1];
        tasks(binaryDirectoryFile);
        return 0;
    }
}