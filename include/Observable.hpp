#ifndef OBSERVABLE_HPP
#define OBSERVABLE_HPP

#include <functional>
#include <vector>
#include <utility>
#include <algorithm>


template<typename T>
class Observable {
public:
    using Listener = std::function<void(const T &)>;

    void addListener(Listener listener) const { listeners.push_back(listener); }

    void removeListener(Listener listener) const {
        listeners.erase(std::ranges::remove(listeners, listener), listeners.end());
    }

    void operator+=(Listener listener) const { addListener(std::move(listener)); }

    void operator-=(Listener listener) const { removeListener(std::move(listener)); }

    [[nodiscard]] const T &get() const { return value; }

    void set(const T &value) {
        this->value = value;
        callListeners();
    }

    void set(T &&value) {
        this->value = std::move(value);
        callListeners();
    }

    virtual Observable &operator=(const T &value) {
        set(value);
        return *this;
    }

    Observable &operator=(T &&value) {
        set(std::move(value));
        return *this;
    }

    explicit operator const T &() const { return value; }

    explicit operator T &() { return value; }

    Observable() = default;
    virtual ~Observable() = default;

    explicit Observable(const T &value) : value(value) {}

    explicit Observable(T &&value) : value(std::move(value)) {}

    friend bool operator==(const Observable &lhs, const Observable &rhs) { return lhs.value == rhs.value; }

    friend bool operator<=>(const Observable &lhs, const Observable &rhs) { return lhs.value <=> rhs.value; }

    friend bool operator==(const Observable &lhs, const T &rhs) { return lhs.value == rhs; }

    friend bool operator==(const T &lhs, const Observable &rhs) { return lhs == rhs.value; }

    Observable(const Observable &) = delete;
    Observable(Observable &&) = delete;
    Observable &operator=(const Observable &) = delete;
    Observable &operator=(Observable &&) = delete;

private:
    T value{};
    mutable std::vector<Listener> listeners{};

    void callListeners() const { for (const auto &listener: listeners) listener(value); }
};


#endif //OBSERVABLE_HPP
