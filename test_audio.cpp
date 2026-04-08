#include <Windows.h>
#include <mmsystem.h>
#include <iostream>
#include <string>
#pragma comment(lib, "winmm.lib")

int main() {
    // 测试音频文件路径
    std::wstring musicPath = L"Tiles\\bg.wav";
    std::wcout << L"Testing audio file: " << musicPath << std::endl;
    
    // 检查文件是否存在
    DWORD attributes = GetFileAttributes(musicPath.c_str());
    if (attributes == INVALID_FILE_ATTRIBUTES) {
        std::cout << "File does not exist!" << std::endl;
        return 1;
    }
    std::cout << "File exists." << std::endl;
    
    // 尝试使用PlaySound
    std::cout << "Trying PlaySound..." << std::endl;
    BOOL result = PlaySound(musicPath.c_str(), NULL, SND_FILENAME | SND_SYNC);
    std::cout << "PlaySound result: " << result << std::endl;
    if (!result) {
        std::cout << "PlaySound error: " << GetLastError() << std::endl;
    }
    
    // 尝试使用MCI
    std::cout << "Trying MCI..." << std::endl;
    std::wstring openCommand = L"open \"" + musicPath + L"\" type waveaudio alias testmusic";
    MCIERROR error = mciSendString(openCommand.c_str(), NULL, 0, NULL);
    std::cout << "MCI open error: " << error << std::endl;
    
    if (error == 0) {
        error = mciSendString(L"play testmusic", NULL, 0, NULL);
        std::cout << "MCI play error: " << error << std::endl;
        
        // 等待播放完成
        Sleep(5000);
        
        mciSendString(L"close testmusic", NULL, 0, NULL);
    }
    
    std::cout << "Test completed. Press Enter to exit..." << std::endl;
    std::cin.get();
    return 0;
}