//
// Created by FullHat on 02/04/2024.
//

#ifndef ENGINE_SRC_ADAPTERS_INTERFACES_FABRIC_ADAPTER_HPP
#define ENGINE_SRC_ADAPTERS_INTERFACES_FABRIC_ADAPTER_HPP

#include <memory>

#define IMPL_FUNC(return_type, func_name, ...)                                 \
  return_type func_name(__VA_ARGS__) { Adapter<T, InitParams>::Obj->func_name(__VA_ARGS__); }

template <typename T, typename InitParams> class Adapter {
public:
  Adapter(InitParams Params) : Obj(std::make_unique<T>(Params)) {}

  // Functions

protected:
  std::unique_ptr<T> Obj;
};

#endif // ENGINE_SRC_ADAPTERS_INTERFACES_FABRIC_ADAPTER_HPP
