#pragma once

#include <variant>
#include <cassert>
#include <utility>

namespace util 
{
template <typename T, typename E>
    class Expected 
    {
    public:
        // --- Constructors ---
        // Success
        Expected(const T& value)
            : data_(value) {}

        Expected(T&& value)
            : data_(std::move(value)) {}

        // Error
        Expected(const E& error)
            : data_(error) {}

        Expected(E&& error) 
            : data_(std::move(error)) {}

        // --- Observers ---
        bool has_value() const noexcept 
        {
            return std::holds_alternative<T>(data_);;
        }

        explicit operator bool() const noexcept 
        {
            return has_value();
        }

        // --- Accessors ---
        T& value() 
        {
            assert(has_value());
            return std::get<T>(data_);
        }

        const T& value() const 
        {
            assert(has_value());
            return std::get<T>(data_);
        }

        E& error() 
        {
            assert(!has_value());
            return std::get<E>(data_);
        }

        const E& error() const 
        {
            assert(!has_value());
            return std::get<E>(data_);
        }

    private:
        std::variant<T, E> data_;
    };

// ===============================
// Partial specialisation for void
// ===============================

template <typename E>
    class Expected<void, E> {
    public:
        // Success
        Expected() : has_value_(true) {}

        // Error
        Expected(const E& error)
            : has_value_(false), error_(error) {}

        Expected(E&& error)
            : has_value_(false), error_(std::move(error)) {}

        bool has_value() const noexcept {
            return has_value_;
        }

        explicit operator bool() const noexcept {
            return has_value_;
        }

        void value() const {
            assert(has_value_);
        }

        E& error() {
            assert(!has_value_);
            return error_;
        }

        const E& error() const {
            assert(!has_value_);
            return error_;
        }
        
    private:
        bool has_value_;
        E error_;
}
