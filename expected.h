#include <type_traits>
#include <utility>
#include <variant>

template <typename T, typename E>
class Expected : std::variant<T, E> {
 public:
  using std::variant<T, E>::variant;

  bool ok() noexcept {
    return !std::holds_alternative<E>(*this) || std::get<E>(*this) == _ok;
  }

  const E &error() noexcept { return ok() ? _ok : std::get<E>(*this); }

  // crash if std::bad_variant_access
  T &get() noexcept { return std::get<T>(*this); }

  // crash if std::bad_variant_access
  const T &get() const noexcept { return std::get<T>(*this); }

 private:
  static inline const std::decay_t<E> _ok{};
};

template <typename E>
class Expected<void, E> {
 public:
  Expected() noexcept = default;
  Expected(E err) noexcept : _err(err) {}

  bool ok() noexcept { return _err == _ok; }

  const E &error() noexcept { return _err; }

  // should not access
  void get() noexcept {}

  // should not access
  void get() const noexcept {}

 private:
  static inline const std::decay_t<E> _ok{};
  std::decay_t<E> _err{};
};

template <typename T, typename E,
          std::enable_if_t<!std::is_same_v<T, void>> * = nullptr,
          typename... Args>
Expected<T, E> UnExpected(Args &&...args) noexcept {
  return Expected<T, E>{std::in_place_index_t<1u>{},
                        std::forward<Args>(args)...};
}

template <typename T, typename E, typename... Args,
          std::enable_if_t<std::is_same_v<T, void>> * = nullptr>
Expected<T, E> UnExpected(Args &&...args) noexcept {
  return Expected<T, E>{std::forward<Args>(args)...};
}
