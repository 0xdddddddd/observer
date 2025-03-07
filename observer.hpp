#ifndef __OBSERVICE_H__
#define __OBSERVICE_H__

#if defined(_MSC_VER) && (_MSC_VER >= 1200)
#	pragma once
#endif

#include <iostream>
#include <vector>
#include <utility>

namespace test
{
    enum class bind_type : std::size_t
    {
        recv,
        send,
        connect,
        disconnect,
        accept,
        handshake,
        upgrade,
        init,
        start,
        stop,
        max
    };
    
    namespace details
    {
        template <typename T>
        struct is_lambda
        {
        private:
            template <typename U>
            static auto test(int) -> decltype(&U::operator(), std::true_type{});
    
            template <typename U>
            static std::false_type test(...) {};
    
        public:
            static constexpr bool value = std::is_class<T>::value && decltype(test<T>(0))::value;
        };
    
        template <typename T>
        inline constexpr bool is_lambda_v = is_lambda<T>::value;
    
        template <typename... Types>
        struct function_traits_type {};
    
        template <typename T>
        struct function_traits
        {
            using type = T;
        };
    
        template <typename T, typename... Types>
        struct function_traits<T(__cdecl)(Types...)>
        {
            using type = T(Types...);
        };
    
        template <typename T, typename... Types>
        struct function_traits<T(__stdcall)(Types...)>
        {
            using type = T(Types...);
        };
    
        template <typename T, typename... Types>
        struct function_traits<T(__fastcall)(Types...)>
        {
            using type = T(Types...);
        };
    
        template <typename T, typename... Types>
        struct function_traits<T(__vectorcall)(Types...)>
        {
            using type = T(Types...);
        };
    
        template <typename T, typename... Types>
        struct function_traits<T(__cdecl&)(Types...)>
        {
            using type = T(Types...);
        };
    
        template <typename T, typename... Types>
        struct function_traits<T(__stdcall&)(Types...)>
        {
            using type = T(Types...);
        };
    
        template <typename T, typename... Types>
        struct function_traits<T(__fastcall&)(Types...)>
        {
            using type = T(Types...);
        };
    
        template <typename T, typename... Types>
        struct function_traits<T(__vectorcall&)(Types...)>
        {
            using type = T(Types...);
        };
    
        template <typename T, typename C, typename... Types>
        struct function_traits<T(__thiscall C::*)(Types...)>
        {
            using type = T(Types...);
        };
    
        template <typename T, typename C, typename... Types>
        struct function_traits<T(__cdecl C::*)(Types...)>
        {
            using type = T(Types...);
        };
    
        template <typename T, typename C, typename... Types>
        struct function_traits<T(__stdcall C::*)(Types...)>
        {
            using type = T(Types...);
        };
    
        template <typename T, typename C, typename... Types>
        struct function_traits<T(__fastcall C::*)(Types...)>
        {
            using type = T(Types...);
        };
    
        template <typename T, typename C, typename... Types>
        struct function_traits<T(__vectorcall C::*)(Types...)>
        {
            using type = T(Types...);
        };
    
        template <typename T>
        struct function_traits_of
        {
            using type = typename function_traits<std::remove_pointer_t<T>>::type;
        };
    
        // lambda
        template <typename T>
        struct function_lambda_traits;
    
        template <typename R, typename... Args>
        struct function_lambda_traits<R(Args...)>
        {
            using type = R(Args...);
            using return_type = R;
            static constexpr std::size_t arity = sizeof...(Args);
    
            template <std::size_t I>
            struct arg
            {
                using type = typename std::tuple_element<I, std::tuple<Args...>>::type;
            };
        };
    
        template <typename C, typename R, typename... Args>
        struct function_lambda_traits<R(C::*)(Args...) const> : function_lambda_traits<R(Args...)> {};
    
        template <typename C, typename R, typename... Args>
        struct function_lambda_traits<R(C::*)(Args...)> : function_lambda_traits<R(Args...)> {};
    
        template <typename T>
        struct function_lambda_traits_of
        {
            using type = typename function_lambda_traits<decltype(&std::remove_reference_t<T>::operator())>::type;
        };
    
        template <typename T, typename E = void>
        struct function_lambda_traits_if
        {
            using type = T;
        };
    
        template <typename T>
        struct function_lambda_traits_if<T, std::enable_if_t<is_lambda_v<T>>>
        {
            using type = typename function_lambda_traits_of<T>::type;
        };
    
        // std::bind
        template <typename R, typename F, typename... Types>
        struct function_bind
        {
            using type = R;
            using func = F;
            using args = function_traits_type<Types ...>;
        };
    
        template <typename T>
        struct function_bind_traits;
    
        template <typename T>
        struct function_bind_traits
        {
            using type = T;
        };
    
        // std::bind ref
        // class std::_Binder<struct std::_Unforced,void (__cdecl&)(int,int),struct std::_Ph<1> const &,struct std::_Ph<2> const &>
        template <typename R, typename F>
        struct function_bind_traits<function_bind<R, F&>>
        {
            using type = F;
        };
    
        template <typename R, typename F>
        struct function_bind_traits<function_bind<R, F>>
        {
            using type = F;
        };
    
        // std::bind ref placeholders
        template <typename R, typename F, typename C, typename... Types>
        struct function_bind_traits<function_bind<R, F&, C, Types...>>
        {
            using type = F;
        };
    
        // std::bind placeholders
        template <typename R, typename F, typename C, typename... Types>
        struct function_bind_traits<function_bind<R, F, C, Types...>>
        {
            using type = F;
        };
    
        template <typename T>
        struct function_bind_traits_of
        {
            using type = typename std::remove_pointer_t<typename function_lambda_traits_if<typename function_bind_traits<T>>>::type;
        };
    
        template <typename T, typename E = void>
        struct function_traits_cvref
        {
            static_assert((std::is_function_v<std::remove_pointer_t<T>> || std::is_member_function_pointer_v<T>) ||
                          (!std::is_bind_expression_v<T> && is_lambda_v<T>) ||
                          (std::is_bind_expression_v<T> && !is_lambda_v<T>),
                          "Unsupported type: T must be a function, member function pointer, lambda, or bind expression.");
        };
    
        template <typename T>
        struct function_traits_cvref<T, std::enable_if_t<std::is_function_v<std::remove_pointer_t<T>> || std::is_member_function_pointer_v<T>>>
        {
            using type = typename function_traits_of<T>::type;
        };
    
        template <typename T>
        struct function_traits_cvref<T, std::enable_if_t<!std::is_bind_expression_v<T>&& is_lambda_v<T>>>
        {
            using type = typename function_lambda_traits_of<T>::type;
        };
    
        template <typename T>
        struct function_traits_cvref<T, std::enable_if_t<std::is_bind_expression_v<T> && !is_lambda_v<T>>>
        {
            using type = typename function_bind_traits_of<T>::type;
        };
    
        template <typename T>
        using function_traits_cvref_t = typename function_traits_cvref<T>::type;
    
    
        template <typename T, template <typename, typename...> typename ImplType>
        struct function_analysis {};
    
        // Function pointer
        template <typename T, typename... Types, template <typename, typename...> typename ImplType>
        struct function_analysis<T(__cdecl)(Types...), ImplType>
        {
            using type = ImplType<T, Types...>;
        };
    
        template <typename T, typename... Types, template <typename, typename...> typename ImplType>
        struct function_analysis<T(__fastcall)(Types...), ImplType>
        {
            using type = ImplType<T, Types...>;
        };
    
        template <typename T, typename... Types, template <typename, typename...> typename ImplType>
        struct function_analysis<T(__stdcall)(Types...), ImplType>
        {
            using type = ImplType<T, Types...>;
        };
    
        template <typename T, typename... Types, template <typename, typename...> typename ImplType>
        struct function_analysis<T(__vectorcall)(Types...), ImplType>
        {
            using type = ImplType<T, Types...>;
        };
    
        // Member function pointer
        template <typename T, typename C, typename... Types, template <typename, typename...> typename ImplType>
        struct function_analysis<T(__thiscall C::*)(Types...), ImplType>
        {
            using type = ImplType<T, Types...>;
        };
    
        template <typename T, typename C, typename... Types, template <typename, typename...> typename ImplType>
        struct function_analysis<T(__cdecl C::*)(Types...), ImplType>
        {
            using type = ImplType<T, Types...>;
        };
    
        template <typename T, typename C, typename... Types, template <typename, typename...> typename ImplType>
        struct function_analysis<T(__stdcall C::*)(Types...), ImplType>
        {
            using type = ImplType<T, Types...>;
        };
    
        template <typename T, typename C, typename... Types, template <typename, typename...> typename ImplType>
        struct function_analysis<T(__fastcall C::*)(Types...), ImplType>
        {
            using type = ImplType<T, Types...>;
        };
    
        template <typename T, typename C, typename... Types, template <typename, typename...> typename ImplType>
        struct function_analysis<T(__vectorcall C::*)(Types...), ImplType>
        {
            using type = ImplType<T, Types...>;
        };
    }
    
    template <typename R, typename... Types>
    struct observer_wrapper_callable_base
    {
        virtual R invoke(Types&&...) = 0;
    };
    
    template <typename T, typename R, typename... Types>
    class observer_wrapper_callable : public observer_wrapper_callable_base<R, Types...>
    {
    public:
        using return_type = R;
    public:
        template <typename F>
        explicit observer_wrapper_callable(F&& val) : callable(std::forward<F>(val)) {}
    public:
        return_type virtual invoke(Types&&... args) override
        {
            if constexpr (std::is_void_v<return_type>)
            {
                return std::invoke(callable, std::forward<Types>(args)...);
            }
            else
            {
                return std::invoke(callable, std::forward<Types>(args)...);
            }
        }
    private:
        T callable;
    };
    
    struct observer_base {};
    
    template <typename R, typename... Types>
    class observer_impl : public observer_base
    {
    public:
        using callable_type = observer_wrapper_callable_base<R, Types...>;
        using argument_types = std::tuple<Types...>;
        using return_type = R;
    public:
        template <class F>
        constexpr explicit observer_impl(F&& val)
        {
            ptr = new observer_wrapper_callable<std::decay_t<F>, return_type, Types...>(std::forward<F>(val));
        }
        virtual ~observer_impl() noexcept
        {
            if (ptr) { delete ptr; }
        }
    public:
        inline return_type operator()(Types&&... args) const
        {
            return this->call(std::forward<Types>(args)...);
        }
    
        inline return_type call(Types&&... args) const
        {
            if (ptr)
            {
                return static_cast<callable_type*>(ptr)->invoke(std::forward<Types>(args)...);
            }
            return return_type{};
        }
    private:
        void* ptr;
    };
    
    template <typename T>
    struct observer : public details::function_analysis<T, observer_impl>::type
    {
        template <typename F>
        explicit observer(F&& val)
            : details::function_analysis<T, observer_impl>::type(std::forward<F>(val))
        {
        }
    };
    
    class binder
    {
    public:
        explicit binder() = default;
        virtual ~binder() = default;
    public:
        template <typename F>
        inline void add(bind_type e, F&& val) noexcept
        {
            observers[static_cast<std::size_t>(e)] = std::make_unique<observer<typename details::function_traits_cvref<std::decay_t<F>>::type>>(std::forward<F>(val));
        }
    
        inline void del(bind_type e) noexcept
        {
            observers[static_cast<std::size_t>(e)] = {};
        }
    
        template <typename R = void, typename... Types>
        inline auto notify(bind_type e, Types&&... args) noexcept
        {
            /*dynamic_cast*/
            observer_impl<R, Types...>* ptr = static_cast<observer_impl<R, Types...>*>(observers[static_cast<std::size_t>(e)].get());
            if (ptr)
            {
                return ptr->call(std::forward<Types>(args)...);
            }
    
            return R{};
        }
    private:
        std::array<std::unique_ptr<observer_base>, static_cast<std::size_t>(bind_type::max)> observers;
    };
}

static int t_add(int a, int b)
{
    return a + b;
}

class tttt
{
public:
    int add1(int a, int b)
    {
        return a + b;
    }
};


int main()
{
    using namespace test;
    tttt t;
    binder test;

    test.add(bind_type::recv, std::bind(&tttt::add1, &t, std::placeholders::_1, std::placeholders::_2));
    test.notify(bind_type::recv, 1, 2);
    return 0;
}
#endif //
