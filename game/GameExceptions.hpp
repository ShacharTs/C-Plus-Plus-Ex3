#pragma once

#include <stdexcept>
#include <string>

class ArrestError : public std::runtime_error {
public:
    explicit ArrestError(const std::string& message)
        : std::runtime_error(message) {}
};

class BribeError : public std::runtime_error {
public:
    explicit BribeError(const std::string& message)
        : std::runtime_error(message) {}
};

class JudgeBlockBribeError : public std::runtime_error {
public:
    explicit JudgeBlockBribeError(const std::string& message)
        : std::runtime_error(message) {}
};

class CoupError : public std::runtime_error {
public:
    explicit CoupError(const std::string& message)
        : std::runtime_error(message) {}
};


class SelfError : public std::runtime_error {
public:
    explicit SelfError (const std::string& message)
        : std::runtime_error(message){}
};


