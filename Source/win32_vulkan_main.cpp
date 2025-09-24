#define VK_USE_PLATFORM_WIN32_KHR
#include "vulkan_main.h"

#include "baseTypes.h"
#include "fileio.h"
#include <vulkan/vulkan_win32.h>
#include <windows.h>

DebugReadFileResult DebugReadFile(const char *FileName) {
    DebugReadFileResult Result{};
    HANDLE FileHandle = CreateFileA(FileName, GENERIC_READ, FILE_SHARE_READ, 0, OPEN_EXISTING, 0, 0);
    LARGE_INTEGER FileSize;
    if (!GetFileSizeEx(FileHandle, &FileSize)) {
        return Result;
    }

    uint32 FileSize32 = (uint32)FileSize.QuadPart;
    Result.Size = FileSize32;

    Result.Data = VirtualAlloc(0, Result.Size, MEM_COMMIT | MEM_RESERVE, PAGE_READWRITE);
    if (!Result.Data) {
        return Result;
    }
    DWORD BytesRead;

    if (ReadFile(FileHandle, Result.Data, FileSize32, &BytesRead, 0) && FileSize32 == BytesRead) {
        CloseHandle(FileHandle);
        return Result;
    }

    return Result;
}

void DebugLog(const wchar_t *log) {
    OutputDebugStringW(log);
}

LRESULT CALLBACK WindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam) {
    if (uMsg == WM_DESTROY) {
        PostQuitMessage(0);
        return 0;
    }
    return DefWindowProcW(hwnd, uMsg, wParam, lParam);
}

bool MakeSurface(VulkanState &vkState) {
    HWND window = GetActiveWindow();
    VkWin32SurfaceCreateInfoKHR win32SurfaceCreateInfo{};
    win32SurfaceCreateInfo.sType = VK_STRUCTURE_TYPE_WIN32_SURFACE_CREATE_INFO_KHR;
    win32SurfaceCreateInfo.hwnd = window;
    win32SurfaceCreateInfo.hinstance = GetModuleHandle(nullptr);

    if (vkCreateWin32SurfaceKHR(vkState.instance, &win32SurfaceCreateInfo, nullptr, &vkState.surface) != VK_SUCCESS) {
        OutputDebugStringW(L"Unable to create vulkan surface");
        return false;
    }
    return true;
}

VkExtent2D GetClientExtents() {

    VkExtent2D extent{};
    HWND window = GetActiveWindow();
    RECT rect;
    GetClientRect(window, &rect);
    extent.height = rect.bottom - rect.top;
    extent.width = rect.right - rect.left;
    return extent;
}

int WinMain(HINSTANCE hInstance, HINSTANCE hPrev, LPSTR szCmdLine, int sw) {

    constexpr wchar_t CLASS_NAME[] = L"WindowClass";

    WNDCLASSEXW wc = {};
    wc.cbSize = sizeof(WNDCLASSEXW);
    wc.lpfnWndProc = WindowProc;
    wc.hInstance = hInstance;
    wc.lpszClassName = CLASS_NAME;

    RegisterClassExW(&wc);

    HWND hwnd = CreateWindowExW(0, CLASS_NAME, L"Engine",
                                WS_OVERLAPPEDWINDOW,
                                CW_USEDEFAULT, CW_USEDEFAULT,
                                CW_USEDEFAULT, CW_USEDEFAULT,
                                NULL, NULL, hInstance, NULL);

    if (hwnd == NULL) {
        return 0;
    }
    ShowWindow(hwnd, sw);
    UpdateWindow(hwnd);

#pragma region Input

#pragma endregion
#pragma region Vulkan

    VulkanState vkState{};
    InnitVulkan(&vkState);

#pragma endregion
    uint32 currentFrame = 0;
    bool Running = true;

    while (Running) {
        MSG message;
        while (PeekMessageW(&message, 0, 0, 0, PM_REMOVE)) {
            switch (message.message) {
            case WM_CLOSE:
            case WM_QUIT: {
                Running = false;
            } break;
            case WM_DESTROY: {
                PostQuitMessage(0);
            } break;
            case WM_SIZE: { // todo: resize is not getting called
                RECT rect;
                GetClientRect(hwnd, &rect);
                VkExtent2D extent{};
                extent.height = rect.bottom - rect.top;
                extent.width = rect.right - rect.left;

            } break;

            case WM_SYSKEYUP:
            case WM_SYSKEYDOWN:
            case WM_KEYUP:
            case WM_KEYDOWN: {
                uint32 VCode = (uint32)message.wParam;
                bool32 WasDown = (message.lParam & 1 << 30) != 0;
                bool32 IsDown = (message.lParam & 1 << 31) == 0;
                if (WasDown != IsDown) {
                    switch (VCode) {
                    case VK_F4:
                        Running = false;
                        break;
                    }
                }
            } break;
            }
            TranslateMessage(&message);
            DispatchMessageW(&message);
            if (Running != false) {
                render(&vkState, currentFrame);
            }
        }

#pragma region Vulkan
#pragma endregion
    }
    CleanupVulkan(&vkState);

    return 0;
}

void *Allocate(uint64 Size) {
    return VirtualAlloc(0, Size, MEM_COMMIT | MEM_RESERVE, PAGE_READWRITE);
}
