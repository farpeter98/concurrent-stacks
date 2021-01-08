#pragma once

#include <exception>

namespace concurrent
{
    //exception raised when a pop is attempted on an empty stack
    class EmptyException :
        public std::exception
    {
    public:
        const char* what() const noexcept
        {
            return "Stack is empty!";
        }
    };
}

