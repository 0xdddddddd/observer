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
    enum class binder_type : std::size_t
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

   template <typename T>
   struct function_traits
   {
       using type = T;
   };

   template <typename T, typename... _Types>
   struct function_traits<T(__cdecl)(_Types...)>
   {
       using type = T(_Types...);
   };

   template <typename T, typename... _Types>
   struct function_traits<T(__stdcall)(_Types...)>
   {
       using type = T(_Types...);
   };

   template <typename T, typename... _Types>
   struct function_traits<T(__fastcall)(_Types...)>
   {
       using type = T(_Types...);
   };

   template <typename T, typename... _Types>
   struct function_traits<T(__vectorcall)(_Types...)>
   {
       using type = T(_Types...);
   };

   template <typename T, typename... _Types>
   struct function_traits<T(__cdecl&)(_Types...)>
   {
       using type = T(_Types...);
   };

   template <typename T, typename... _Types>
   struct function_traits<T(__stdcall&)(_Types...)>
   {
       using type = T(_Types...);
   };

   template <typename T, typename... _Types>
   struct function_traits<T(__fastcall&)(_Types...)>
   {
       using type = T(_Types...);
   };

   template <typename T, typename... _Types>
   struct function_traits<T(__vectorcall&)(_Types...)>
   {
       using type = T(_Types...);
   };

   template <typename T, typename C, typename... _Types>
   struct function_traits<T(__thiscall C::*)(_Types...)>
   {
       using type = T(_Types...);
   };

   template <typename T, typename C, typename... _Types>
   struct function_traits<T(__cdecl C::*)(_Types...)>
   {
       using type = T(_Types...);
   };

   template <typename T, typename C, typename... _Types>
   struct function_traits<T(__stdcall C::*)(_Types...)>
   {
       using type = T(_Types...);
   };

   template <typename T, typename C, typename... _Types>
   struct function_traits<T(__fastcall C::*)(_Types...)>
   {
       using type = T(_Types...);
   };

   template <typename T, typename C, typename... _Types>
   struct function_traits<T(__vectorcall C::*)(_Types...)>
   {
       using type = T(_Types...);
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
   struct function_lambda_traits_is
   {
       using type = T;
   };

   template <typename T>
   struct function_lambda_traits_is<T, std::enable_if_t<is_lambda_v<T>>>
   {
       using type = typename function_lambda_traits_of<T>::type;
   };

   //template <typename C, typename R, typename... Args>
   //struct function_lambda_traits_of<R(C::*)(Args...)>
   //{
   //    using type = R(Args...);
   //};

   //template <typename C, typename R, typename... Args>
   //struct function_lambda_traits_of<R(C::*)(Args...) const>
   //{
   //    using type = R(Args...);
   //};

   //template <typename C, typename R, typename... Args>
   //struct function_lambda_traits_of<R(C::*&)(Args...)>
   //{
   //    using type = R(Args...);
   //};

   //template <typename C, typename R, typename... Args>
   //struct function_lambda_traits_of<R(C::*&)(Args...) const>
   //{
   //    using type = R(Args...);
   //};

   //template <typename R, typename... Args>
   //struct function_lambda_traits_of<R(*)(Args...)>
   //{
   //    using type = R(Args...);
   //};

   //template <typename R, typename... Args>
   //struct function_lambda_traits_of<R(&)(Args...)>
   //{
   //    using type = R(Args...);
   //};


   // std::bind
   template <typename T>
   struct function_bind_traits;

   template <typename T>
   struct function_bind_traits
   {
       using type = T;
   };

   // std::bind ref
   template <typename Callable, typename Func>
   struct function_bind_traits<std::_Binder<Callable, Func&>>
   {
       using type = Func;
   };

   template <typename Callable, typename Func>
   struct function_bind_traits<std::_Binder<Callable, Func>>
   {
       using type = Func;
       /*std::conditional_t
           <
           is_lambda_v<Func>,
           typename function_lambda_traits_of<Func>::type,
           Func
           >;*/
   };

   // std::bind ref placeholders
   template <typename Callable, typename Func, typename ClassType, typename... Placeholders>
   struct function_bind_traits<std::_Binder<Callable, Func&, ClassType, Placeholders...>>
   {
       using type = Func;
   };

   // std::bind placeholders
   template <typename Callable, typename Func, typename ClassType, typename... Placeholders>
   struct function_bind_traits<std::_Binder<Callable, Func, ClassType, Placeholders...>>
   {
       using type = Func;
   };

   template <typename T>
   struct function_bind_traits_of
   {
       using type = typename std::remove_pointer_t<typename function_lambda_traits_is<typename function_bind_traits<T>::type>>::type;
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


   template <typename... _Types>
   struct type_list {};

   template <typename T, typename... _Types>
   struct apply_result
   {
       using type = T;
       using args = type_list<_Types...>;
   };

   template <typename T, typename... _Types>
   struct apply_result<T __cdecl(_Types...)>
   {
       using type = T;
       using args = type_list<_Types...>;
   };

   template <typename T, typename... _Types>
   struct apply_result<T __fastcall(_Types...)>
   {
       using type = T;
       using args = type_list<_Types...>;
   };

   template <typename T, typename... _Types>
   struct apply_result<T __stdcall(_Types...)>
   {
       using type = T;
       using args = type_list<_Types...>;
   };

   template <typename T, typename... _Types>
   struct apply_result<T __vectorcall(_Types...)>
   {
       using type = T;
       using args = type_list<_Types...>;
   };

   template <typename T>
   struct apply_result_of
   {
       using type = apply_result<T>::type;
       using args = apply_result<T>::args;
   };

   template <typename T, template <typename, typename...> typename ImplType>
   struct determine_type {};

   // Function pointer
   template <typename T, typename... _Types, template <typename, typename...> typename ImplType>
   struct determine_type<T(__cdecl)(_Types...), ImplType>
   {
       using type = ImplType<T, _Types...>;
   };

   template <typename T, typename... _Types, template <typename, typename...> typename ImplType>
   struct determine_type<T(__fastcall)(_Types...), ImplType>
   {
       using type = ImplType<T, _Types...>;
   };

   template <typename T, typename... _Types, template <typename, typename...> typename ImplType>
   struct determine_type<T(__stdcall)(_Types...), ImplType>
   {
       using type = ImplType<T, _Types...>;
   };

   template <typename T, typename... _Types, template <typename, typename...> typename ImplType>
   struct determine_type<T(__vectorcall)(_Types...), ImplType>
   {
       using type = ImplType<T, _Types...>;
   };

   // Member function pointer
   template <typename T, typename C, typename... _Types, template <typename, typename...> typename ImplType>
   struct determine_type<T(__thiscall C::*)(_Types...), ImplType>
   {
       using type = ImplType<T, _Types...>;
   };

   template <typename T, typename C, typename... _Types, template <typename, typename...> typename ImplType>
   struct determine_type<T(__cdecl C::*)(_Types...), ImplType>
   {
       using type = ImplType<T, _Types...>;
   };

   template <typename T, typename C, typename... _Types, template <typename, typename...> typename ImplType>
   struct determine_type<T(__stdcall C::*)(_Types...), ImplType>
   {
       using type = ImplType<T, _Types...>;
   };

   template <typename T, typename C, typename... _Types, template <typename, typename...> typename ImplType>
   struct determine_type<T(__fastcall C::*)(_Types...), ImplType>
   {
       using type = ImplType<T, _Types...>;
   };

   template <typename T, typename C, typename... _Types, template <typename, typename...> typename ImplType>
   struct determine_type<T(__vectorcall C::*)(_Types...), ImplType>
   {
       using type = ImplType<T, _Types...>;
   };

   template <typename R, typename... _Types>
   class observer_wrapper_callable_base
   {
   public:
       using return_type = R;
   public:
       virtual return_type do_call(_Types&&...) = 0;
   };

   template <typename T, typename R, typename... _Types>
   class observer_wrapper_callable : public observer_wrapper_callable_base<R, _Types...>
   {
   public:
       using return_type = R;
   public:
       template <typename _Other>
       explicit observer_wrapper_callable(_Other&& val) : callable(std::forward<_Other>(val)) {}
   public:
       return_type do_call(_Types&&... args) override
       {
           if constexpr (std::is_void_v<return_type>)
           {
               return std::invoke(callable, std::forward<_Types>(args)...);
           }
           else
           {
               return std::invoke(callable, std::forward<_Types>(args)...);
           }
       }
   private:
       T callable;
   };

   class observer_base
   {
   public:
       virtual ~observer_base() = default;
   };

   template <typename R, typename... _Types>
   class observer_impl : public observer_base
   {
   public:
       using callable_type = observer_wrapper_callable_base<R, _Types...>;
       using argument_types = std::tuple<_Types...>;
       using return_type = R;
   public:
       virtual ~observer_impl()
       {
           if (ptr) { delete ptr; }
       }
   public:
       return_type operator()(_Types... args) const
       {
           return static_cast<callable_type*>(ptr)->do_call(std::forward<_Types>(args)...);
       }

       return_type call(_Types... args) const
       {
           return static_cast<callable_type*>(ptr)->do_call(std::forward<_Types>(args)...);
       }
   public:
       template <class _Fx>
       void reset(_Fx&& val)
       {
           ptr = new observer_wrapper_callable<std::decay_t<_Fx>, return_type, _Types...>(std::forward<_Fx>(val));
       }
   private:
       void* ptr;
   };

   template <typename T>
   class observer : public determine_type<T, observer_impl>::type
   {
   public:
       observer() noexcept {}

       template <typename _Fx>
       observer(_Fx&& _Func)
       {
           this->reset(std::forward<_Fx>(_Func));
       }

       template <typename... _Types>
       constexpr auto do_call(_Types&&... _Args)
       {
           return this->call(std::forward<_Types>(_Args)...);
       }
   };

   class binder
   {
   public:
       using bind_type = binder_type;
   public:
       explicit binder() = default;
       template <typename _Fx>
       constexpr explicit binder(_Fx&& _Func) = delete;
       virtual ~binder() = default;
   public:
       template <typename _Fx>
       constexpr void bind(bind_type&& e, _Fx&& _Func)
       {
           observers_[static_cast<std::size_t>(e)] = std::make_unique<observer<typename function_traits_cvref<std::decay_t<_Fx>>::type>>(std::forward<_Fx>(_Func));
       }

       template <typename R = void, typename... _Types>
       constexpr auto notify(bind_type&& e, _Types&&... _Args) const
       {
           using return_type = R;
           using func_type = observer_impl<return_type, _Types... >;
           func_type* ptr = /*dynamic_cast*/ static_cast<func_type*>(observers_[static_cast<std::size_t>(e)].get());
           if (ptr)
           {
               return ptr->call(std::forward<_Types>(_Args)...);
           }

           return return_type{};
       }
   private:
       std::array<std::unique_ptr<observer_base>, static_cast<std::size_t>(bind_type::max)> observers_;
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

    test.bind(binder_type::recv, std::bind(&tttt::add1, &t, std::placeholders::_1, std::placeholders::_2));
    test.notify(binder_type::recv, 1, 2);
    test.bind(binder_type::recv, std::bind([] (int a, int b) {
        printf("%d\n", a + b);
    }, std::placeholders::_1, std::placeholders::_2));
    test.notify(binder_type::recv, 1, 2);
    test.bind(binder_type::recv, std::bind(t_add, std::placeholders::_1, std::placeholders::_2));
    test.notify(binder_type::recv, 1, 2);
    test.bind(binder_type::recv, std::bind(&t_add, std::placeholders::_1, std::placeholders::_2));
    test.notify(binder_type::recv, 1, 2);
    test.bind(binder_type::recv, t_add);
    test.notify(binder_type::recv, 1, 2);
    test.bind(binder_type::recv, &t_add);
    test.notify(binder_type::recv, 1, 2);
    return 0;
}
#endif //
