#ifndef TYPE_DECL_H
#define TYPE_DECL_H

#include <tuple>
#include <type_traits>
#include <cuda.h>

#define SYMBOL_TO_STR(x) STRINGIFY(x)
#define STRINGIFY(x) #x

template<typename T>
struct FunctionTraits;

template <typename R, typename... Args>
struct FunctionTraits<R(Args...)> {
    // static constexpr size_t parameter_count = sizeof...(Args);
    using ParameterTuple = std::tuple<Args...>;
};

#define GEN_ARG_TUPLE(symbol) using symbol##_param_t = FunctionTraits<decltype(symbol)>::ParameterTuple;

GEN_ARG_TUPLE(cuMemcpyHtoD)

#endif