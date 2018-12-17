#ifndef OPTIONAL_OPTIONAL_H
#define OPTIONAL_OPTIONAL_H

#include <exception>
#include <type_traits>
#include <utility>

#if (defined(__GNUC__) && __GNUC__ == 4 && !defined(__clang__))
// GCC versions < 5 had a bug for const && ref qualifiers overloads.
#define CM_NO_CONSTREFREF
#endif

namespace cm {

static struct nullopt_t {
} nullopt;

class bad_optional_access : public std::exception {
public:
  bad_optional_access() = default;
  const char *what() const noexcept { return "Optional is empty"; }
};

namespace detail {
template <class T> struct add_const { using type = const T; };
template <class T> struct add_const<const T> { using type = const T; };
template <class T> struct add_const<T &> { using type = const T &; };
template <class T> struct add_const<const T &> { using type = const T &; };

template <class T> using constify = typename add_const<T>::type;

template <class T> class optional_base_storage_no_dtor {
protected:
  void m_destroy() {
    if (m_is_engaged())
      m_get_value().~T();
    m_engaged = false;
  }

  template <class... Args> void m_inplace_construct(Args &&... args) {
    m_engaged = true;
    new (&m_storage) T(std::forward<Args>(args)...);
  }

  template <class U> void m_assign_or_construct(U &&value) {
    if (m_is_engaged())
      m_get_value() = std::forward<U>(value);
    else
      m_inplace_construct(std::forward<U>(value));
  }

  bool m_is_engaged() const { return m_engaged; }

  T &m_get_value() & { return reinterpret_cast<T &>(m_storage); }
  const T &m_get_value() const & {
    return reinterpret_cast<const T &>(m_storage);
  }
  T &&m_get_value() && { return reinterpret_cast<T &&>(m_storage); }
  const T &&m_get_value() const && {
    return reinterpret_cast<const T &&>(m_storage);
  }

private:
  typename std::aligned_storage<sizeof(T), alignof(T)>::type m_storage;
  bool m_engaged = false;
};

template <class T,
          bool TriviallyDistructible = std::is_trivially_destructible<T>::value>
struct optional_base_storage_with_dtor : optional_base_storage_no_dtor<T> {};

template <class T>
struct optional_base_storage_with_dtor<T, false>
    : optional_base_storage_no_dtor<T> {
  using base = optional_base_storage_no_dtor<T>;
  ~optional_base_storage_with_dtor() { base::m_destroy(); }
};

template <class T>
class optional_base_storage : public optional_base_storage_with_dtor<T> {};

template <class T> class optional_base_storage<T &> {
protected:
  void m_inplace_construct(T &value) { m_storage = std::addressof(value); }

  void m_assign_or_construct(T &value) { m_inplace_construct(value); }
  void m_assign_or_construct(const T &value) {
    m_inplace_construct(const_cast<T &>(value));
  }

  void m_destroy() { m_storage = nullptr; }

  T &m_get_value() { return *m_storage; }
  T &m_get_value() const { return const_cast<T &>(*m_storage); }

  bool m_is_engaged() const { return m_storage != nullptr; }

private:
  T *m_storage = nullptr;
};
} // namespace detail

template <class T> class optional : public detail::optional_base_storage<T> {
  using base = detail::optional_base_storage<T>;

public:
  using value_type = typename std::remove_reference<T>::type;

  optional() noexcept {}
  optional(nullopt_t) noexcept {}
  optional(const optional &other) {
    if (other.has_value())
      base::m_inplace_construct(other.base::m_get_value());
  }
  template <class = typename std::enable_if<
                std::is_move_constructible<T>::value>::type>
  optional(optional &&other) noexcept(
      std::is_nothrow_move_constructible<T>::value) {
    if (other.has_value())
      base::m_inplace_construct(std::move(other).value());
  }
  template <class U> optional(const optional<U> &other) {
    if (other.has_value())
      base::m_inplace_construct(other.value());
  }
  template <class U,
            class = typename std::enable_if<
                std::is_constructible<T, U &&>::value &&
                !std::is_constructible<T, optional<U> &>::value &&
                !std::is_constructible<T, const optional<U> &>::value &&
                !std::is_constructible<T, optional<U> &&>::value &&
                !std::is_constructible<T, const optional<U> &&>::value &&
                !std::is_convertible<optional<U> &, T>::value &&
                !std::is_convertible<const optional<U> &, T>::value &&
                !std::is_convertible<optional<U> &&, T>::value &&
                !std::is_convertible<const optional<U> &&, T>::value>::type>
  optional(optional<U> &&other) {
    if (other.has_value())
      base::m_inplace_construct(std::move(other).value());
  }
  template <class U = T, class = typename std::enable_if<
                             std::is_constructible<T, U &&>::value &&
                             !std::is_same<typename std::decay<U>::type,
                                           optional<T>>::value>::type>
  optional(U &&value) {
    base::m_inplace_construct(std::forward<U>(value));
  }

  optional &operator=(nullopt_t) noexcept { base::m_destroy(); }
  optional &operator=(const optional &other) {
    if (other.has_value())
      base::m_assign_or_construct(other.value());
    else
      base::m_destroy();
    return *this;
  }
  optional &operator=(optional &&other) noexcept(
      std::is_nothrow_move_assignable<T>::value
          &&std::is_nothrow_move_constructible<T>::value) {
    if (other.has_value())
      base::m_assign_or_construct(std::move(other).value());
    else
      base::m_destroy();
    return *this;
  }
  template <
      class U = T,
      class = typename std::enable_if<
          std::is_constructible<T, U>::value &&
          std::is_assignable<T &, U>::value &&
          !std::is_same<typename std::decay<U>::type, optional<T>>::value &&
          !(std::is_scalar<T>::value &&
            std::is_same<T, typename std::decay<U>::type>::value)>::type>
  optional &operator=(U &&other) {
    base::m_assign_or_construct(std::forward<U>(other));
  }
  template <class U> optional &operator=(const optional<U> &other) {
    if (other.has_value()) {
      base::m_assign_or_construct(other.value());
    } else
      base::m_destroy();
  }
  template <class U> optional &operator=(optional<U> &&other) {
    if (other.has_value())
      base::m_assign_or_construct(std::move(other).value());
    else
      base::m_destroy();
  }

  bool has_value() const noexcept { return base::m_is_engaged(); }
  explicit operator bool() const noexcept { return has_value(); }

  value_type &value() & {
    m_check_access();
    return base::m_get_value();
  }
  const value_type &value() const & {
    m_check_access();
    return base::m_get_value();
  }
  auto value() && -> decltype(std::move(*this).base::m_get_value()) {
    m_check_access();
    return std::move(*this).base::m_get_value();
  }
#ifndef CM_NO_CONSTREFREF
  auto value() const
      && -> detail::constify<decltype(std::move(*this).base::m_get_value())> {
    m_check_access();
    return std::move(*this).base::m_get_value();
  }
#endif

  value_type &operator*() & { return value(); }
  const value_type &operator*() const & { return value(); }
  auto operator*() && -> decltype(std::move(*this).value()) {
    return std::move(*this).value();
  }
#ifndef CM_NO_CONSTREFREF
  auto operator*() const && -> decltype(std::move(*this).value()) {
    return std::move(*this).value();
  }
#endif

  value_type *operator->() { return &value(); }
  const value_type *operator->() const { return &value(); }

  template <class U> T value_or(U &&default_value) const & {
    return has_value() ? base::m_get_value()
                       : static_cast<T>(std::forward<U>(default_value));
  }
  template <class U> T value_or(U &&default_value) && {
    return has_value() ? std::move(*this).base::m_get_value()
                       : static_cast<T>(std::forward<U>(default_value));
  }

  template <class... Args> value_type &emplace(Args &&... args) {
    if (has_value()) {
      base::m_destroy();
    }
    base::m_inplace_construct(std::forward<Args>(args)...);
  }

private:
  void m_check_access() const {
    if (!has_value())
      throw bad_optional_access();
  }
};
} // namespace cm

#endif // OPTIONAL_OPTIONAL_H
