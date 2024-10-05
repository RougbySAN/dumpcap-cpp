#include <iostream>
#include <string>
#include <cstdio>
#include <cstdlib>
#include <windows.h>

/* Filter expression parameters*/
#define FILTER_LOCAL_IP     "192.168.1.15"
#define FILTER_LOCAL_PORT   6002
#define FILTER_TARGET_IP    "192.168.1.16"
#define FILTER_TARGET_PORT  6001

/* Adapter expression parameters */
#define NAME_ADAPTER        "Ethernet"


/* Generate filter expression "-f"filter-expression""*/
std::string generate_filter_expression(const std::string& local_ip, const uint16_t& local_port, \
                                         const std::string& target_ip, const uint16_t& target_port)
{
    std::string filterExpression = "-f \"host " + local_ip + " and port " + std::to_string(local_port)
                                 + " and host " + target_ip + " and port " + std::to_string(target_port) + "\"";
    return filterExpression;
}


/* Generate adapter expression "-i"adapter-name"" */
std::string generate_adapter_expression(const std::string& adapter_name)
{
    return "-i \"" + adapter_name + "\"";
}


/**
 * @brief Make sure to end the path string with "/" character
 * 
 *  Example path: C:/PATH_TO_FILE/
 *  Example file name: test1
 *
 *  Result  C:/PATH_TO_FILE/test1.pcapng 
 *
 * @param file_path 
 * @param log_file_name 
 * @return std::string 
 */
std::string generate_write_expression(const std::string& file_path, const std::string& log_file_name)
{
    return "-w \"" + file_path + log_file_name + ".pcapng" + "\"";
}


// Function to start dumpcap capture
void startCapture(const std::string &adapter_expression, const std::string &filter_expression, \
                const std::string &file_name, PROCESS_INFORMATION &pi)
{
    // Construct the full command string
    std::string command = "dumpcap " + adapter_expression + " " + filter_expression + " " + file_name;

    // Prepare STARTUPINFO and PROCESS_INFORMATION structures
    STARTUPINFO si = {0};
    si.cb = sizeof(si);
    ZeroMemory(&pi, sizeof(pi));

    // Create process to run dumpcap
    if (!CreateProcess(NULL, \
                        const_cast<LPSTR>(command.c_str()), \
                        NULL, \
                        NULL, \
                        FALSE, \
                        0, \
                        NULL, \
                        NULL, \
                        &si, \
                        &pi))
    {
        std::cerr << "Failed to start capture process." << std::endl;
    }
    else
    {
        std::cout << "Started capturing to " << file_name << std::endl;
    }
}

// Function to stop the capture process
void stopCapture(PROCESS_INFORMATION &pi)
{
    // Check if the process is valid
    if (pi.hProcess) {
        TerminateProcess(pi.hProcess, 0);
        WaitForSingleObject(pi.hProcess, INFINITE);
        CloseHandle(pi.hProcess);
        CloseHandle(pi.hThread);
        std::cout << "Capture stopped." << std::endl;
    }
}

int main()
{
    PROCESS_INFORMATION pi = {0};                              // For managing dumpcap process

    bool running = true;
    while (running)
    {
        std::cout << "Options:\n1. Start Capture\n2. Stop Capture\n3. Exit\nSelect an option: ";
        int choice;
        std::cin >> choice;

        switch (choice)
        {
            case 1:
            {
                if (!pi.hProcess) {
                    // Generate the dumpcap command components
                    std::string filterExpression = generate_filter_expression(FILTER_LOCAL_IP, FILTER_LOCAL_PORT, FILTER_TARGET_IP, FILTER_TARGET_PORT);
                    std::string adapterExpression = generate_adapter_expression(std::string(NAME_ADAPTER));
                    std::string logFileExpression = generate_write_expression(std::string(""), std::string("Test1"));
                    startCapture(adapterExpression, filterExpression, logFileExpression, pi);
                } else {
                    std::cout << "Capture is already running!" << std::endl;
                }
                break;
            }
            case 2:
            {
                stopCapture(pi);
                break;
            }
            case 3:
            {
                stopCapture(pi);
                running = false;
                break;
            }
            default:
            {
                std::cout << "Invalid option!" << std::endl;
            }
        }
    }

    return 0;
}
