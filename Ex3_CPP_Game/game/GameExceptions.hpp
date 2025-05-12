#pragma once

#include <stdexcept>
#include <string>


class CoinsError : public std::runtime_error {
public:
    explicit CoinsError(const std::string& message)
        : std::runtime_error(message) {}
};

class GatherError : public std::runtime_error {
public:
    explicit GatherError(const std::string& message)
        : std::runtime_error(message) {}
};


class TaxError : public std::runtime_error {
public:
    explicit TaxError(const std::string& message)
        : std::runtime_error(message) {}
};

class ArrestTwiceInRow : public std::runtime_error {
public:
    explicit ArrestTwiceInRow(const std::string& message)
        : std::runtime_error(message) {}
};


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


class CoupBlocked : public std::runtime_error {
public:
    explicit CoupBlocked(const std::string& message)
        : std::runtime_error(message) {}
};


class SelfError : public std::runtime_error {
public:
    explicit SelfError (const std::string& message)
        : std::runtime_error(message){}
};

class MerchantError : public std::runtime_error {
public:
    explicit MerchantError (const std::string& message)
        : std::runtime_error(message){}
};


