#pragma once

#include <exception>

namespace concurrent {

    //exception to signal timing out in the elimination exchanger
    class TimeoutException :
        public std::exception
    {
    public:
        const char* what() const noexcept
        {
            return "Exchange timed out";
        }
    };
}


