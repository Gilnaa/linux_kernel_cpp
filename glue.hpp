// Copyright (c) 2018 Gilad Naaman
//
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files (the "Software"), to deal
// in the Software without restriction, including without limitation the rights
// to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
// copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:
//
// The above copyright notice and this permission notice shall be included in all
// copies or substantial portions of the Software.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
// SOFTWARE.

/**
 * This file supplies glue between C++ and kernel headers.
 *
 * This header enables including basic kernel definitions, like numeric typedefs
 * and `printk`.
 *
 * Using this won't enable you to write full-fledged kernel modules, but it will let
 * you embed simple C++ libraries into your kernel.
 *
 * Included headers:
 * - <linux/stddef.h>
 * - <linux/types.h>
 * - <linux/printk.h>
 * - <linux/string.h>
 *
 * @author  Gilad "Salmon" Naaman
 * @since   08/02/2018
 */

#ifndef __LINUX_CPP_GLUE
#define __LINUX_CPP_GLUE


// Fix for:
// ```
// enum {
//     false  = 0,
//     true   = 1,
// };
// ```
#define false FALSE
#define true TRUE
#	include <linux/stddef.h>
#undef false
#undef true

// Fix for:
// ```
// typedef _Bool bool;
// ```
typedef bool _Bool;
#define bool BOOL;
#	include <linux/types.h>
#undef bool

// asmlinkage is defined to be `extern "C"`, but in one place `printk.h` uses `extern asmlinkage`.
// This declaration ends up as `extern extern "C"`, which does not compile.
// It's fine to undef this, because we wrap the whole thing in `extern "C" {}` anyway.
extern "C" {
	// We pre-include linkage so we'll be able to undef asmlinkage before printk.h would be able to use it.
	#include <linux/linkage.h>
	#undef asmlinkage
	#define asmlinkage
	#include <linux/printk.h>
}

// The following declarations are a compensation for the lack of `__builtin_types_compatible_p` in C++.
// All the template magic is copied from <type_traits>
namespace cpp_glue {
	template<typename, typename>
	struct is_same { static const bool value = false; };

	template<typename T>
	struct is_same<T, T> { static const bool value = true; };

	// Remove the const qualifier
	template<typename T>
	struct remove_const { typedef T type; };
	template<typename T>
	struct remove_const<T const> { typedef T type; };

	// Remove the volatile qualifier
	template<typename T>
	struct remove_volatile { typedef T type; };
	template<typename T>
	struct remove_volatile<T volatile> { typedef T type; };

	// Remove both const and volatile
	template<typename T>
	struct remove_cv
	{
	    typedef typename
	    remove_const<typename remove_volatile<T>::type>::type type;
	};
}

#define __builtin_types_compatible_p(a, b) cpp_glue::is_same<typename cpp_glue::remove_cv<a>::type, typename cpp_glue::remove_cv<b>::type>::value

// This could probably be replaced with template magic to avoid using C++11,
// but for now there's no point.
#define typeof decltype

// We hide `new` because of function's parameter in this name.
#define new _new

extern "C" {
	#include <linux/string.h>
}

#undef __builtin_types_compatible_p
#undef typeof
#undef new

#endif /* __LINUX_CPP_GLUE */