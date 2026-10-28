#pragma once

#include <map>
#include <chrono>

using timepoint = std::chrono::high_resolution_clock::time_point;

struct timeoutItem
{
    timeoutItem() : index(0), timeoutMs(0) {}
    timeoutItem(int idx, int timeout) : index(idx), timeoutMs(timeout) {}

    int index; 
    int timeoutMs;
};

class BaseController
{
public:
    BaseController();

protected:
    /**
     * \brief Checks if the specified timeout has elapsed for the given index.
     * \param index The index to check the timeout for.
     * \param timeoutMs The timeout duration in milliseconds.
     * \return True if the timeout has elapsed, false otherwise.
     */
    bool checkTimeout(int index, int timeoutMs);

    /**
     * \brief Checks if the specified timeout has elapsed for the given timeout item.
     * \param item The timeout item containing the index and timeout duration.
     * \return True if the timeout has elapsed, false otherwise.
     */
    bool checkTimeout(timeoutItem item);

private:

    static BaseController* instance;
    std::map<int, timepoint> timestamps;
};