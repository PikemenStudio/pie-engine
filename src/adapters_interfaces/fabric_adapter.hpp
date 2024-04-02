//
// Created by FullHat on 02/04/2024.
//

#ifndef ENGINE_SRC_ADAPTERS_INTERFACES_FABRIC_ADAPTER_HPP
#define ENGINE_SRC_ADAPTERS_INTERFACES_FABRIC_ADAPTER_HPP

#include <memory>

template <typename T, typename InitParams> class Adapter {
public:
  // Functions

protected:
  std::unique_ptr<T> Obj;
};

#endif // ENGINE_SRC_ADAPTERS_INTERFACES_FABRIC_ADAPTER_HPP
