#ifndef FILE_ENUMS_H
#define FILE_ENUMS_H

// Define some basic bitwise operators for all enum classes within the File namespace.
namespace File
{

template <typename Enum>
Enum operator&(Enum lhs, Enum rhs)
{
  typedef typename std::underlying_type<Enum>::type underlying;

  return static_cast<Enum>(
      static_cast<underlying>(lhs) &
      static_cast<underlying>(rhs));
}

template <typename Enum>
Enum operator|(Enum lhs, Enum rhs)
{
  typedef typename std::underlying_type<Enum>::type underlying;

  return static_cast<Enum>(
      static_cast<underlying>(lhs) |
      static_cast<underlying>(rhs));
}

template <typename Enum>
Enum operator^(Enum lhs, Enum rhs)
{
  typedef typename std::underlying_type<Enum>::type underlying;

  return static_cast<Enum>(
      static_cast<underlying>(lhs) ^
      static_cast<underlying>(rhs));
}

template <typename Enum>
Enum operator~(Enum rhs)
{
  typedef typename std::underlying_type<Enum>::type underlying;

  return static_cast<Enum>(~static_cast<underlying>(rhs));
}

template <typename Enum>
Enum &operator|=(Enum &lhs, Enum rhs)
{
  typedef typename std::underlying_type<Enum>::type underlying;

  return lhs = static_cast<Enum>(
             static_cast<underlying>(lhs) |
             static_cast<underlying>(rhs));
}

template <typename Enum>
Enum &operator&=(Enum &lhs, Enum rhs)
{
  typedef typename std::underlying_type<Enum>::type underlying;

  return lhs = static_cast<Enum>(
             static_cast<underlying>(lhs) & static_cast<underlying>(rhs));
}

template <typename Enum>
Enum &operator^=(Enum &lhs, Enum rhs)
{
  typedef typename std::underlying_type<Enum>::type underlying;

  return lhs = static_cast<Enum>(
             static_cast<underlying>(lhs) ^ static_cast<underlying>(rhs));
}

} // End namespace

#endif