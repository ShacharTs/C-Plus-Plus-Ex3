#pragma once

/**
 * @file GameExceptions.hpp
 * @brief Custom exceptions for game-specific error conditions.
 */

#include <stdexcept>
#include <string>

/**
 * @class CoinsError
 * @brief Thrown when a player attempts an action without sufficient coins.
 */
class CoinsError : public std::runtime_error {
public:
    /**
     * @brief Construct a CoinsError with a descriptive message.
     * @param message Explanation of the coin error.
     */
    explicit CoinsError(const std::string& message)
        : std::runtime_error(message) {}
};

/**
 * @class GatherError
 * @brief Thrown when a player attempts to gather coins illegally or out of turn.
 */
class GatherError : public std::runtime_error {
public:
    /**
     * @brief Construct a GatherError with a descriptive message.
     * @param message Explanation of the gather error.
     */
    explicit GatherError(const std::string& message)
        : std::runtime_error(message) {}
};

/**
 * @class TaxError
 * @brief Thrown when a player attempts a tax action that is not allowed.
 */
class TaxError : public std::runtime_error {
public:
    /**
     * @brief Construct a TaxError with a descriptive message.
     * @param message Explanation of the tax error.
     */
    explicit TaxError(const std::string& message)
        : std::runtime_error(message) {}
};

/**
 * @class ArrestTwiceInRow
 * @brief Thrown when a player tries to arrest the same target twice consecutively.
 */
class ArrestTwiceInRow : public std::runtime_error {
public:
    /**
     * @brief Construct an ArrestTwiceInRow error with a descriptive message.
     * @param message Explanation of the consecutive arrest violation.
     */
    explicit ArrestTwiceInRow(const std::string& message)
        : std::runtime_error(message) {}
};

/**
 * @class ArrestError
 * @brief Thrown when an arrest action fails (e.g., blocked by Spy).
 */
class ArrestError : public std::runtime_error {
public:
    /**
     * @brief Construct an ArrestError with a descriptive message.
     * @param message Explanation of why the arrest failed.
     */
    explicit ArrestError(const std::string& message)
        : std::runtime_error(message) {}
};

/**
 * @class BribeError
 * @brief Thrown when a bribe action fails or is improperly performed.
 */
class BribeError : public std::runtime_error {
public:
    /**
     * @brief Construct a BribeError with a descriptive message.
     * @param message Explanation of the bribe failure.
     */
    explicit BribeError(const std::string& message)
        : std::runtime_error(message) {}
};

/**
 * @class JudgeBlockBribeError
 * @brief Thrown when a Judge role blocks a bribe action.
 */
class JudgeBlockBribeError : public std::runtime_error {
public:
    /**
     * @brief Construct a JudgeBlockBribeError with a descriptive message.
     * @param message Explanation of the judge block.
     */
    explicit JudgeBlockBribeError(const std::string& message)
        : std::runtime_error(message) {}
};

/**
 * @class CoupBlocked
 * @brief Thrown when a coup is blocked by a player's shield or defense.
 */
class CoupBlocked : public std::runtime_error {
public:
    /**
     * @brief Construct a CoupBlocked with a descriptive message.
     * @param message Explanation of why the coup was blocked.
     */
    explicit CoupBlocked(const std::string& message)
        : std::runtime_error(message) {}
};

/**
 * @class SelfError
 * @brief Thrown when a player attempts an invalid self-targeted action (e.g., coup self).
 */
class SelfError : public std::runtime_error {
public:
    /**
     * @brief Construct a SelfError with a descriptive message.
     * @param message Explanation of the self-action violation.
     */
    explicit SelfError(const std::string& message)
        : std::runtime_error(message) {}
};

/**
 * @class MerchantError
 * @brief Thrown when a Merchant-specific action fails or violates rules.
 */
class MerchantError : public std::runtime_error {
public:
    /**
     * @brief Construct a MerchantError with a descriptive message.
     * @param message Explanation of the merchant action failure.
     */
    explicit MerchantError(const std::string& message)
        : std::runtime_error(message) {}
};