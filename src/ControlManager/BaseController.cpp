#include "BaseController.h"
#include <iostream>

BaseController* BaseController::instance = nullptr;

BaseController::BaseController() {
    timestamps = std::map<int, timepoint>();
}

bool BaseController::checkTimeout(int index, int timeoutMs) {
    bool result{false};

    if (index < 0) {
        result = false;
    }
    else if (timestamps.find(index) == timestamps.end())
    {
        // First time the timeout is computed, so just record the current time
        timestamps[index] = std::chrono::high_resolution_clock::now();
        result = true;
    }
    else
    {
        auto now = std::chrono::high_resolution_clock::now();
        auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(now - timestamps[index]).count();
        if (duration > timeoutMs) {
            timestamps[index] = now;
            result = true;
        }
    }

    
    return result;
}

bool BaseController::checkTimeout(timeoutItem item) {
    return checkTimeout(item.index, item.timeoutMs);
}
