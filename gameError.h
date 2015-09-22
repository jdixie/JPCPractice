//game error handler

#ifndef _GAMEERROR_H            
#define _GAMEERROR_H            
#define WIN32_LEAN_AND_MEAN

#include <string>
#include <exception>

namespace gameErrorNS
{
    //negative numbers are fatal errors that may require the game to be shutdown.
    //positive numbers are warnings that do not require the game to be shutdown.
    const int FATAL_ERROR = -1;
    const int WARNING = 1;
}

//game error class. thrown when an error is detected by the game engine.
//inherits from std::exception
class GameError : public std::exception
{
private:
    int     errorCode;
    std::wstring message;
public:
    //default constructor
    GameError() throw() :errorCode(gameErrorNS::FATAL_ERROR), message(L"Undefined Error in game.") {}
    //copy constructor
    GameError(const GameError &e) throw(): std::exception(e), errorCode(e.errorCode), message(e.message) {}
    //constructor with args
    GameError(int code, const std::wstring &s) throw() :errorCode(code), message(s) {}
    //assignment operator
    GameError& operator= (const GameError &rhs) throw() 
    {
        std::exception::operator=(rhs);
        this->errorCode = rhs.errorCode;
        this->message = rhs.message;
    }
    //destructor
    virtual ~GameError() throw() {};

    //override what from base class
//   virtual const char* what() const throw() { return this->getMessage(); }

    const wchar_t* getMessage() const throw() { return message.c_str(); }
    int getErrorCode() const throw() { return errorCode; }
};

#endif
