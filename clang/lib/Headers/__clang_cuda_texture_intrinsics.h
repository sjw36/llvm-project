/*===--- __clang_cuda_texture_intrinsics.h - Device-side texture support ---===
 *
 * Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
 * See https://llvm.org/LICENSE.txt for license information.
 * SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
 *
 *===-----------------------------------------------------------------------===
 *
 * This header provides in-header implmentations for NVCC's built-in
 * __nv_tex_surf_handler() which is used by CUDA's texture-related headers.  The
 * built-in is unusual as it's actually a set of function overloads that use the
 * first string literal argument as one of the overload parameters.
 */
#ifndef __CLANG_CUDA_TEXTURE_INTRINSICS_H__
#define __CLANG_CUDA_TEXTURE_INTRINSICS_H__
#ifndef __CUDA__
#error "This file is for CUDA compilation only."
#endif

// __nv_tex_surf_handler() provided by this header as a macro.
#define __nv_tex_surf_handler(__op, __ptr, ...)                                \
  ::__cuda_tex::__tex_fetch<                                                   \
      ::__cuda_tex::__Tag<::__cuda_tex::__tex_op_hash(__op)>>(__ptr,           \
                                                              __VA_ARGS__)

#pragma push_macro("__ASM_OUT")
#pragma push_macro("__ASM_OUTP")
#pragma push_macro("__Args")
#pragma push_macro("__ID")
#pragma push_macro("__IDV")
#pragma push_macro("__OP_TYPE_SURFACE")
#pragma push_macro("__IMPL_2DGATHER")
#pragma push_macro("__IMPL_ALIAS")
#pragma push_macro("__IMPL_ALIASI")
#pragma push_macro("__IMPL_F1")
#pragma push_macro("__IMPL_F3")
#pragma push_macro("__IMPL_F3N")
#pragma push_macro("__IMPL_F3S")
#pragma push_macro("__IMPL_S")
#pragma push_macro("__IMPL_S3")
#pragma push_macro("__IMPL_S3I")
#pragma push_macro("__IMPL_S3N")
#pragma push_macro("__IMPL_S3NI")
#pragma push_macro("__IMPL_S3S")
#pragma push_macro("__IMPL_S3SI")
#pragma push_macro("__IMPL_SI")
#pragma push_macro("__L")
#pragma push_macro("__STRIP_PARENS")
#pragma push_macro("__SURF_WRITE_V2")
#pragma push_macro("__SW_ASM_ARGS")
#pragma push_macro("__SW_ASM_ARGS1")
#pragma push_macro("__SW_ASM_ARGS2")
#pragma push_macro("__SW_ASM_ARGS4")
#pragma push_macro("__SURF_WRITE_V2")
#pragma push_macro("__SURF_READ_V2")
#pragma push_macro("__SW_ASM_ARGS")
#pragma push_macro("__SW_ASM_ARGS1")
#pragma push_macro("__SW_ASM_ARGS2")
#pragma push_macro("__SW_ASM_ARGS4")
#pragma push_macro("__SURF_READ1D");
#pragma push_macro("__SURF_READ2D");
#pragma push_macro("__SURF_READ3D");
#pragma push_macro("__SURF_READ1DLAYERED");
#pragma push_macro("__SURF_READ2DLAYERED");
#pragma push_macro("__SURF_READCUBEMAP");
#pragma push_macro("__SURF_READCUBEMAPLAYERED");
#pragma push_macro("__1DV1");
#pragma push_macro("__1DV2");
#pragma push_macro("__1DV4");
#pragma push_macro("__2DV1");
#pragma push_macro("__2DV2");
#pragma push_macro("__2DV4");
#pragma push_macro("__1DLAYERV1");
#pragma push_macro("__1DLAYERV2");
#pragma push_macro("__1DLAYERV4");
#pragma push_macro("__3DV1");
#pragma push_macro("__3DV2");
#pragma push_macro("__3DV4");
#pragma push_macro("__2DLAYERV1");
#pragma push_macro("__2DLAYERV2");
#pragma push_macro("__2DLAYERV4");
#pragma push_macro("__CUBEMAPV1");
#pragma push_macro("__CUBEMAPV2");
#pragma push_macro("__CUBEMAPV4");
#pragma push_macro("__CUBEMAPLAYERV1");
#pragma push_macro("__CUBEMAPLAYERV2");
#pragma push_macro("__CUBEMAPLAYERV4");
#pragma push_macro("__SURF_READXD_ALL");
#pragma push_macro("__SURF_WRITE1D_V2");
#pragma push_macro("__SURF_WRITE1DLAYERED_V2");
#pragma push_macro("__SURF_WRITE2D_V2");
#pragma push_macro("__SURF_WRITE2DLAYERED_V2");
#pragma push_macro("__SURF_WRITE3D_V2");
#pragma push_macro("__SURF_CUBEMAPWRITE_V2");
#pragma push_macro("__SURF_CUBEMAPLAYEREDWRITE_V2");
#pragma push_macro("__SURF_WRITEXD_V2_ALL");
#pragma push_macro("__1DV1");
#pragma push_macro("__1DV2");
#pragma push_macro("__1DV4");
#pragma push_macro("__2DV1");
#pragma push_macro("__2DV2");
#pragma push_macro("__2DV4");
#pragma push_macro("__3DV1");
#pragma push_macro("__3DV2");
#pragma push_macro("__3DV4");

// Put all functions into anonymous namespace so they have internal linkage.
// The device-only function here must be internal in order to avoid ODR
// violations in case they are used from the files compiled with
// -fgpu-rdc. E.g. a library and an app using it may be built with a different
// version of this header file.
namespace {

// Put the implmentation into its own namespace so we don't pollute the TU.
namespace __cuda_tex {

// First, we need a perfect hash function and a few constexpr helper functions
// for converting a string literal into a numeric value which can be used to
// parametrize a template. We can not use string literals for that as that would
// require C++20.
//
// The hash function was generated with 'gperf' and then manually converted into
// its constexpr equivalent.
//
// NOTE: the perfect hashing scheme comes with inherent self-test. If the hash
// function has a collision for any of the texture operations, the compilation
// will fail due to an attempt to redefine a tag with the same value. If the
// header compiles, then the hash function is good enough for the job.

constexpr int __tex_len(const char *s) {
  return (s[0] == 0)    ? 0
         : (s[1] == 0)  ? 1
         : (s[2] == 0)  ? 2
         : (s[3] == 0)  ? 3
         : (s[4] == 0)  ? 4
         : (s[5] == 0)  ? 5
         : (s[6] == 0)  ? 6
         : (s[7] == 0)  ? 7
         : (s[8] == 0)  ? 8
         : (s[9] == 0)  ? 9
         : (s[10] == 0) ? 10
         : (s[11] == 0) ? 11
         : (s[12] == 0) ? 12
         : (s[13] == 0) ? 13
         : (s[14] == 0) ? 14
         : (s[15] == 0) ? 15
         : (s[16] == 0) ? 16
         : (s[17] == 0) ? 17
         : (s[18] == 0) ? 18
         : (s[19] == 0) ? 19
         : (s[20] == 0) ? 20
         : (s[21] == 0) ? 21
         : (s[22] == 0) ? 22
         : (s[23] == 0) ? 23
         : (s[24] == 0) ? 24
         : (s[25] == 0) ? 25
         : (s[26] == 0) ? 26
         : (s[27] == 0) ? 27
         : (s[28] == 0) ? 28
         : (s[29] == 0) ? 29
         : (s[30] == 0) ? 30
         : (s[31] == 0) ? 31
                        : 32;
}

constexpr int __tex_hash_map(int c) {
  return (c == 49)    ? 10
         : (c == 50)  ? 0
         : (c == 51)  ? 100
         : (c == 52)  ? 30
         : (c == 67)  ? 10
         : (c == 68)  ? 0
         : (c == 69)  ? 25
         : (c == 72)  ? 70
         : (c == 77)  ? 0
         : (c == 96)  ? 44
         : (c == 99)  ? 10
         : (c == 100) ? 5
         : (c == 101) ? 60
         : (c == 102) ? 40
         : (c == 103) ? 70
         : (c == 104) ? 25
         : (c == 112) ? 0
         : (c == 114) ? 45
         : (c == 117) ? 5
         : (c == 118) ? 85
         : (c == 120) ? 20
                      : 225;
}

constexpr int __tex_op_hash(const char *str) {
  return __tex_len(str) + __tex_hash_map(str[7] + 1) + __tex_hash_map(str[6]) +
         __tex_hash_map(str[5]) + __tex_hash_map(str[__tex_len(str) - 1]);
}

// Tag type to identify particular texture operation.
template <int N> struct __Tag;
#define __ID(__op) __Tag<__tex_op_hash(__op)>
// Tags for variants of particular operation. E.g. tex2Dgather can translate
// into 4 different instructions.
#define __IDV(__op, __variant)                                                 \
  __Tag<10000 + __tex_op_hash(__op) * 100 + __variant>

// Helper classes for figuring out key data types for derived types.
// E.g. char2 has __base_t = char, __fetch_t = char4
template <class> struct __TypeInfoT;
// Type info for the fundamental types.
template <> struct __TypeInfoT<float> {
  using __base_t = float;
  using __fetch_t = float4;
};
template <> struct __TypeInfoT<char> {
  using __base_t = char;
  using __fetch_t = int4;
};
template <> struct __TypeInfoT<signed char> {
  using __base_t = signed char;
  using __fetch_t = int4;
};
template <> struct __TypeInfoT<unsigned char> {
  using __base_t = unsigned char;
  using __fetch_t = uint4;
};
template <> struct __TypeInfoT<short> {
  using __base_t = short;
  using __fetch_t = int4;
};
template <> struct __TypeInfoT<unsigned short> {
  using __base_t = unsigned short;
  using __fetch_t = uint4;
};
template <> struct __TypeInfoT<int> {
  using __base_t = int;
  using __fetch_t = int4;
};
template <> struct __TypeInfoT<unsigned int> {
  using __base_t = unsigned int;
  using __fetch_t = uint4;
};

// Derived base/fetch types for N-element vectors.
template <class __T> struct __TypeInfoT {
  using __base_t = decltype(__T::x);
  using __fetch_t = typename __TypeInfoT<__base_t>::__fetch_t;
};

// Tag structs to distinguish operation types
struct __texture_op_tag {};
struct __surface_op_tag {};

// Template specialization to determine operation type based on tag value
template <class __op> struct __op_type_traits {
  using type = __texture_op_tag;
};

// Specialize for known surface operation tags
#define __OP_TYPE_SURFACE(__op)                                                \
  template <> struct __op_type_traits<__op> {                                  \
    using type = __surface_op_tag;                                             \
  }

// Classes that implement specific texture ops.
template <class __op> struct __tex_fetch_v4;

// Helper macros to strip parens from a macro argument.
#define __Args(...) __VA_ARGS__
#define __STRIP_PARENS(__X) __X
#define __L(__X) __STRIP_PARENS(__Args __X)

// Construct inline assembly output args.
// Results are stored in a temp var __r.
// isResident bool is pointed to by __ir
// Asm args for return values. It's a 4-element vector
#define __ASM_OUT(__t)                                                         \
  ("=" __t(__r.x), "=" __t(__r.y), "=" __t(__r.z), "=" __t(__r.w))
// .. possibly combined with a predicate.
#define __ASM_OUTP(__t) (__L(__ASM_OUT(__t)), "=h"(*__ir))

// Implements a single variant of texture fetch instruction.
#define __IMPL_F1(__rt, __dt, __args, __asm_op, __asm_outs, __asm_args)        \
  template <>                                                                  \
  __device__ __rt __run<__dt>(cudaTextureObject_t __obj, __L(__args)) {        \
    __rt __r;                                                                  \
    asm(__asm_op : __L(__asm_outs) : "l"(__obj), __L(__asm_args));             \
    return __r;                                                                \
  }

// Implements texture fetch instructions for int4/uint4/float4 data types.
#define __IMPL_F3(__args, __asm_op, __ctype, __asm_op_args, __asm_args)        \
  __IMPL_F1(int4, int4, __args, __asm_op ".s32." __ctype "\t" __asm_op_args,   \
            __ASM_OUT("r"), __asm_args)                                        \
  __IMPL_F1(uint4, uint4, __args, __asm_op ".u32." __ctype "\t" __asm_op_args, \
            __ASM_OUT("r"), __asm_args)                                        \
  __IMPL_F1(float4, float4, __args,                                            \
            __asm_op ".f32." __ctype "\t" __asm_op_args, __ASM_OUT("f"),       \
            __asm_args)
// Implements 'sparse' texture fetch instructions for int4/uint4/float4 data
// types. Similar to above, but returns a boolean 'isPresent' value in addition
// to texture data,
#define __IMPL_F3S(__args, __asm_op, __ctype, __asm_op_args, __asm_args)       \
  __IMPL_F1(int4, int4, __args, __asm_op ".s32." __ctype "\t" __asm_op_args,   \
            __ASM_OUTP("r"), __asm_args)                                       \
  __IMPL_F1(uint4, uint4, __args, __asm_op ".u32." __ctype "\t" __asm_op_args, \
            __ASM_OUTP("r"), __asm_args)                                       \
  __IMPL_F1(float4, float4, __args,                                            \
            __asm_op ".f32." __ctype "\t" __asm_op_args, __ASM_OUTP("f"),      \
            __asm_args)

// Similar to F3, but for integer data which is returned as normalized floats.
// Only instantiates fetch functions for int4/uint4.
#define __IMPL_F3N(__args, __asm_op, __ctype, __asm_op_args, __asm_args)       \
  __IMPL_F1(float4, int4, __args, __asm_op ".s32." __ctype "\t" __asm_op_args, \
            __ASM_OUT("r"), __asm_args)                                        \
  __IMPL_F1(float4, uint4, __args,                                             \
            __asm_op ".u32." __ctype "\t" __asm_op_args, __ASM_OUT("r"),       \
            __asm_args)

// Instantiates __tex_fetch_v4 with regular fetch functions.
#define __IMPL_S3I(__op, __args, __asm_op, __ctype, __asm_op_args, __asm_args) \
  template <> struct __tex_fetch_v4<__op> {                                    \
    template <class T>                                                         \
    __device__ static T __run(cudaTextureObject_t __obj, __L(__args));         \
    __IMPL_F3(__args, __asm_op, __ctype, __asm_op_args, __asm_args)            \
  }

// Same, but for sparse ops. Only available on sm_60+
#if !defined(__CUDA_ARCH__) || (__CUDA_ARCH__ >= 600)
#define __IMPL_S3SI(__op, __args, __asm_op, __ctype, __asm_op_args,            \
                    __asm_args)                                                \
  template <> struct __tex_fetch_v4<__op> {                                    \
    template <class T>                                                         \
    __device__ static T __run(cudaTextureObject_t __obj, __L(__args));         \
    __IMPL_F3S(__args, __asm_op, __ctype, __asm_op_args, __asm_args)           \
  }
#else
#define __IMPL_S3SI(__op, __args, __asm_op, __ctype, __asm_op_args, __asm_args)
#endif

// Same, but for normalized float ops.
#define __IMPL_S3NI(__op, __args, __asm_op, __ctype, __asm_op_args,            \
                    __asm_args)                                                \
  template <> struct __tex_fetch_v4<__op> {                                    \
    template <class T>                                                         \
    __device__ static float4 __run(cudaTextureObject_t __obj, __L(__args));    \
    __IMPL_F3N(__args, __asm_op, __ctype, __asm_op_args, __asm_args)           \
  }

// Regular and normalized float ops share a lot of similarities.  This macro
// instantiates both variants -- normal for __op and normalized for __opn.
#define __IMPL_SI(__op, __opn, __args, __asm_op, __ctype, __asm_op_args,       \
                  __asm_args)                                                  \
  __IMPL_S3I(__op, __args, __asm_op, __ctype, __asm_op_args, __asm_args);      \
  __IMPL_S3NI(__opn, __args, __asm_op, __ctype, __asm_op_args, __asm_args)

// Convenience macros which converts string literal __op into a __Tag,
#define __IMPL_S3(__op, __args, __asm_op, __ctype, __asm_op_args, __asm_args)  \
  __IMPL_S3I(__ID(__op), __args, __asm_op, __ctype, __asm_op_args, __asm_args)
#define __IMPL_S3S(__op, __args, __asm_op, __ctype, __asm_op_args, __asm_args) \
  __IMPL_S3SI(__ID(__op), __args, __asm_op, __ctype, __asm_op_args, __asm_args)
#define __IMPL_S3N(__op, __args, __asm_op, __ctype, __asm_op_args, __asm_args) \
  __IMPL_S3NI(__ID(__op), __args, __asm_op, __ctype, __asm_op_args, __asm_args)
#define __IMPL_S(__op, __opn, __args, __asm_op, __ctype, __asm_op_args,        \
                 __asm_args)                                                   \
  __IMPL_SI(__ID(__op), __ID(__opn), __args, __asm_op, __ctype, __asm_op_args, \
            __asm_args)

// CUDA headers have some 'legacy' texture oprerations that duplicate
// functionality. So, we just inherit it, instead of refining a copy.
#define __IMPL_ALIASI(__op, __opn)                                             \
  template <> struct __tex_fetch_v4<__op> : __tex_fetch_v4<__opn> {}
#define __IMPL_ALIAS(__op, __opn) __IMPL_ALIASI(__ID(__op), __ID(__opn))

// Now we can instantiate everything we need for each specific texture fetch
// variant.
__IMPL_S("__tex1D_v2", "__tex1D_rmnf_v2", (float __x), "tex.1d.v4", "f32",
         "{%0, %1, %2, %3}, [%4, {%5}];", ("f"(__x)));
__IMPL_S("__tex1Dfetch_v2", "__tex1Dfetch_rmnf_v2", (int __x), "tex.1d.v4",
         "s32", "{%0, %1, %2, %3}, [%4, {%5}];", ("r"(__x)));
__IMPL_ALIAS("__itex1D", "__tex1D_v2");
__IMPL_ALIAS("__itex1Dfetch", "__tex1Dfetch_v2");

__IMPL_S("__tex1DGrad_v2", "__tex1DGrad_rmnf_v2",
         (float __x, float __dPdx, float __dPdy), "tex.grad.1d.v4", "f32",
         "{%0, %1, %2, %3}, [%4, {%5}], {%6}, {%7};",
         ("f"(__x), "f"(__dPdx), "f"(__dPdy)));
__IMPL_ALIAS("__itex1DGrad", "__tex1DGrad_v2");

__IMPL_S("__tex1DLayered_v2", "__tex1DLayered_rmnf_v2",
         (float __x, int __layer), "tex.a1d.v4", "f32",
         "{%0, %1, %2, %3}, [%4, {%5, %6}];", ("r"(__layer), "f"(__x)));
__IMPL_ALIAS("__itex1DLayered", "__tex1DLayered_v2");

__IMPL_S("__tex1DLayeredGrad_v2", "__tex1DLayeredGrad_rmnf_v2",
         (float __x, int __layer, float __dPdx, float __dPdy),
         "tex.grad.a1d.v4", "f32",
         "{%0, %1, %2, %3}, [%4, {%5, %6}], {%7}, {%8};",
         ("r"(__layer), "f"(__x), "f"(__dPdx), "f"(__dPdy)));
__IMPL_ALIAS("__itex1DLayeredGrad", "__tex1DLayeredGrad_v2");

__IMPL_S("__tex1DLayeredLod_v2", "__tex1DLayeredLod_rmnf_v2",
         (float __x, int __layer, float __level), "tex.level.a1d.v4", "f32",
         "{%0, %1, %2, %3}, [%4, {%5, %6}], %7;",
         ("r"(__layer), "f"(__x), "f"(__level)));
__IMPL_ALIAS("__itex1DLayeredLod", "__tex1DLayeredLod_v2");

__IMPL_S("__tex1DLod_v2", "__tex1DLod_rmnf_v2", (float __x, float __level),
         "tex.level.1d.v4", "f32", "{%0, %1, %2, %3}, [%4, {%5}], %6;",
         ("f"(__x), "f"(__level)));
__IMPL_ALIAS("__itex1DLod", "__tex1DLod_v2");

// 2D
__IMPL_S("__tex2D_v2", "__tex2D_rmnf_v2", (float __x, float __y), "tex.2d.v4",
         "f32", "{%0, %1, %2, %3}, [%4, {%5, %6}];", ("f"(__x), "f"(__y)));
__IMPL_ALIAS("__itex2D", "__tex2D_v2");

__IMPL_S3S("__itex2D_sparse", (float __x, float __y, unsigned char *__ir),
           "{.reg .pred %%p0;\n\t"
           "tex.2d.v4",
           "f32",
           "{%0, %1, %2, %3}|%%p0, [%5, {%6, %7}];\n\t"
           " selp.u16 %4, 1, 0, %%p0; }",
           ("f"(__x), "f"(__y)));

__IMPL_S("__tex2DGrad_v2", "__tex2DGrad_rmnf_v2",
         (float __x, float __y, const float2 *__dPdx, const float2 *__dPdy),
         "tex.grad.2d.v4", "f32",
         "{%0, %1, %2, %3}, [%4, {%5, %6}], {%7, %8}, {%9, %10};",
         ("f"(__x), "f"(__y), "f"(__dPdx->x), "f"(__dPdx->y), "f"(__dPdy->x),
          "f"(__dPdy->y)));
__IMPL_ALIAS("__itex2DGrad_v2", "__tex2DGrad_v2");

__IMPL_S3S("__itex2DGrad_sparse",
           (float __x, float __y, const float2 *__dPdx, const float2 *__dPdy,
            unsigned char *__ir),
           "{.reg .pred %%p0;\n\t"
           "tex.grad.2d.v4",
           "f32",
           "{%0, %1, %2, %3}|%%p0, [%5, {%6, %7}], {%8, %9}, {%10, %11};\n\t"
           "selp.u16 %4, 1, 0, %%p0; }",
           ("f"(__x), "f"(__y), "f"(__dPdx->x), "f"(__dPdx->y), "f"(__dPdy->x),
            "f"(__dPdy->y)));

__IMPL_S("__tex2DLayered_v2", "__tex2DLayered_rmnf_v2",
         (float __x, float __y, int __layer), "tex.a2d.v4", "f32",
         "{%0, %1, %2, %3}, [%4, {%5, %6, %7, %7}];",
         ("r"(__layer), "f"(__x), "f"(__y)));
__IMPL_ALIAS("__itex2DLayered", "__tex2DLayered_v2");

__IMPL_S3S("__itex2DLayered_sparse",
           (float __x, float __y, int __layer, unsigned char *__ir),
           "{.reg .pred %%p0;\n\t"
           "tex.a2d.v4",
           "f32",
           "{%0, %1, %2, %3}|%%p0, [%5, {%6, %7, %8, %8}];\n\t"
           "selp.u16 %4, 1, 0, %%p0; }",
           ("r"(__layer), "f"(__x), "f"(__y)));

__IMPL_S("__tex2DLayeredGrad_v2", "__tex2DLayeredGrad_rmnf_v2",
         (float __x, float __y, int __layer, const float2 *__dPdx,
          const float2 *__dPdy),
         "tex.grad.a2d.v4", "f32",
         "{%0, %1, %2, %3}, [%4, {%5, %6, %7, %7}], {%8, %9}, {%10, %11};",
         ("r"(__layer), "f"(__x), "f"(__y), "f"(__dPdx->x), "f"(__dPdx->y),
          "f"(__dPdy->x), "f"(__dPdy->y)));
__IMPL_ALIAS("__itex2DLayeredGrad_v2", "__tex2DLayeredGrad_v2");

__IMPL_S3S(
    "__itex2DLayeredGrad_sparse",
    (float __x, float __y, int __layer, const float2 *__dPdx,
     const float2 *__dPdy, unsigned char *__ir),
    "{.reg .pred %%p0;\n\t"
    "tex.grad.a2d.v4",
    "f32",
    "{%0, %1, %2, %3}|%%p0, [%5, {%6, %7, %8, %8}], {%9, %10}, {%11, %12};\n\t"
    "selp.u16 %4, 1, 0, %%p0; }",
    ("r"(__layer), "f"(__x), "f"(__y), "f"(__dPdx->x), "f"(__dPdx->y),
     "f"(__dPdy->x), "f"(__dPdy->y)));

__IMPL_S("__tex2DLayeredLod_v2", "__tex2DLayeredLod_rmnf_v2",
         (float __x, float __y, int __layer, float __level), "tex.level.a2d.v4",
         "f32", "{%0, %1, %2, %3}, [%4, {%5, %6, %7, %7}], %8;",
         ("r"(__layer), "f"(__x), "f"(__y), "f"(__level)));
__IMPL_ALIAS("__itex2DLayeredLod", "__tex2DLayeredLod_v2");

__IMPL_S3S("__itex2DLayeredLod_sparse",
           (float __x, float __y, int __layer, float __level,
            unsigned char *__ir),
           "{.reg .pred %%p0;\n\t"
           "tex.level.a2d.v4",
           "f32",
           "{%0, %1, %2, %3}|%%p0, [%5, {%6, %7, %8, %8}], %9;\n\t"
           "selp.u16 %4, 1, 0, %%p0; }",
           ("r"(__layer), "f"(__x), "f"(__y), "f"(__level)));

__IMPL_S("__tex2DLod_v2", "__tex2DLod_rmnf_v2",
         (float __x, float __y, float __level), "tex.level.2d.v4", "f32",
         "{%0, %1, %2, %3}, [%4, {%5, %6}], %7;",
         ("f"(__x), "f"(__y), "f"(__level)));
__IMPL_ALIAS("__itex2DLod", "__tex2DLod_v2");

__IMPL_S3S("__itex2DLod_sparse",
           (float __x, float __y, float __level, unsigned char *__ir),
           "{.reg .pred %%p0;\n\t"
           "tex.level.2d.v4",
           "f32",
           "{%0, %1, %2, %3}|%%p0, [%5, {%6, %7}], %8;\n\t"
           "selp.u16 %4, 1, 0, %%p0; }",
           ("f"(__x), "f"(__y), "f"(__level)));

// 2D gather is special. Unlike other variants that translate into exactly one
// asm instruction, it uses one of the four different instructions selected by
// __comp.  We implement each instruction variant separately, and dispatch the
// right one from the manually implemented 'umbrella' fetch.
#define __IMPL_2DGATHER(variant, instr)                                        \
  __IMPL_SI(__IDV("__tex2Dgather_v2", variant),                                \
            __IDV("__tex2Dgather_rmnf_v2", variant),                           \
            (float __x, float __y, int __comp), instr, "f32",                  \
            "{%0, %1, %2, %3}, [%4, {%5, %6}];", ("f"(__x), "f"(__y)));        \
  __IMPL_ALIASI(__IDV("__itex2Dgather", variant),                              \
                __IDV("__tex2Dgather_v2", variant));                           \
  __IMPL_S3SI(__IDV("__itex2Dgather_sparse", variant),                         \
              (float __x, float __y, unsigned char *__ir, int __comp),         \
              "{.reg .pred %%p0;\n\t" instr, "f32",                            \
              "{%0, %1, %2, %3}|%%p0, [%5, {%6, %7}];\n\t"                     \
              "selp.u16 %4, 1, 0, %%p0; }",                                    \
              ("f"(__x), "f"(__y)));
__IMPL_2DGATHER(0, "tld4.r.2d.v4");
__IMPL_2DGATHER(1, "tld4.g.2d.v4");
__IMPL_2DGATHER(2, "tld4.b.2d.v4");
__IMPL_2DGATHER(3, "tld4.a.2d.v4");

// Umbrella dispatcher -- calls into specific 2Dgather variant.
template <> struct __tex_fetch_v4<__ID("__tex2Dgather_v2")> {
  template <class __T>
  __device__ static __T __run(cudaTextureObject_t __obj, float __x, float __y,
                              int __comp) {
    switch (__comp) {
    case 0:
      return __tex_fetch_v4<__IDV("__tex2Dgather_v2", 0)>::__run<__T>(
          __obj, __x, __y, __comp);
    case 1:
      return __tex_fetch_v4<__IDV("__tex2Dgather_v2", 1)>::__run<__T>(
          __obj, __x, __y, __comp);
    case 2:
      return __tex_fetch_v4<__IDV("__tex2Dgather_v2", 2)>::__run<__T>(
          __obj, __x, __y, __comp);
    case 3:
      return __tex_fetch_v4<__IDV("__tex2Dgather_v2", 3)>::__run<__T>(
          __obj, __x, __y, __comp);
    }
  }
};
__IMPL_ALIAS("__itex2Dgather", "__tex2Dgather_v2");

template <> struct __tex_fetch_v4<__ID("__tex2Dgather_rmnf_v2")> {
  template <class __T>
  __device__ static float4 __run(cudaTextureObject_t __obj, float __x,
                                 float __y, int __comp) {
    switch (__comp) {
    case 0:
      return __tex_fetch_v4<__IDV("__tex2Dgather_rmnf_v2", 0)>::__run<__T>(
          __obj, __x, __y, __comp);
    case 1:
      return __tex_fetch_v4<__IDV("__tex2Dgather_rmnf_v2", 1)>::__run<__T>(
          __obj, __x, __y, __comp);
    case 2:
      return __tex_fetch_v4<__IDV("__tex2Dgather_rmnf_v2", 2)>::__run<__T>(
          __obj, __x, __y, __comp);
    case 3:
      return __tex_fetch_v4<__IDV("__tex2Dgather_rmnf_v2", 3)>::__run<__T>(
          __obj, __x, __y, __comp);
    }
  }
};

#if !defined(__CUDA_ARCH__) || (__CUDA_ARCH__ >= 600)
template <> struct __tex_fetch_v4<__ID("__itex2Dgather_sparse")> {
  template <class __T>
  __device__ static __T __run(cudaTextureObject_t __obj, float __x, float __y,
                              unsigned char *__ir, int __comp) {
    switch (__comp) {
    case 0:
      return __tex_fetch_v4<__IDV("__itex2Dgather_sparse", 0)>::__run<__T>(
          __obj, __x, __y, __ir, __comp);
    case 1:
      return __tex_fetch_v4<__IDV("__itex2Dgather_sparse", 1)>::__run<__T>(
          __obj, __x, __y, __ir, __comp);
    case 2:
      return __tex_fetch_v4<__IDV("__itex2Dgather_sparse", 2)>::__run<__T>(
          __obj, __x, __y, __ir, __comp);
    case 3:
      return __tex_fetch_v4<__IDV("__itex2Dgather_sparse", 3)>::__run<__T>(
          __obj, __x, __y, __ir, __comp);
    }
  }
};
#endif

// 3D
__IMPL_S("__tex3D_v2", "__tex3D_rmnf_v2", (float __x, float __y, float __z),
         "tex.3d.v4", "f32", "{%0, %1, %2, %3}, [%4, {%5, %6, %7, %7}];",
         ("f"(__x), "f"(__y), "f"(__z)));
__IMPL_ALIAS("__itex3D", "__tex3D_v2");

__IMPL_S3S("__itex3D_sparse",
           (float __x, float __y, float __z, unsigned char *__ir),
           "{.reg .pred %%p0;\n\t"
           "tex.3d.v4",
           "f32",
           "{%0, %1, %2, %3}|%%p0, [%5, {%6, %7, %8, %8}];\n\t"
           "selp.u16 %4, 1, 0, %%p0; }",
           ("f"(__x), "f"(__y), "f"(__z)));

__IMPL_S("__tex3DGrad_v2", "__tex3DGrad_rmnf_v2",
         (float __x, float __y, float __z, const float4 *__dPdx,
          const float4 *__dPdy),
         "tex.grad.3d.v4", "f32",
         "{%0, %1, %2, %3}, [%4, {%5, %6, %7, %7}], "
         "{%8, %9, %10, %10}, {%11, %12, %13, %13};",
         ("f"(__x), "f"(__y), "f"(__z), "f"(__dPdx->x), "f"(__dPdx->y),
          "f"(__dPdx->z), "f"(__dPdy->x), "f"(__dPdy->y), "f"(__dPdy->z)));
__IMPL_ALIAS("__itex3DGrad_v2", "__tex3DGrad_v2");

__IMPL_S3S("__itex3DGrad_sparse",
           (float __x, float __y, float __z, const float4 *__dPdx,
            const float4 *__dPdy, unsigned char *__ir),
           "{.reg .pred %%p0;\n\t"
           "tex.grad.3d.v4",
           "f32",
           "{%0, %1, %2, %3}|%%p0, [%5, {%6, %7, %8, %8}], "
           "{%9, %10, %11, %11}, {%12, %13, %14, %14};\n\t"
           "selp.u16 %4, 1, 0, %%p0; }",
           ("f"(__x), "f"(__y), "f"(__z), "f"(__dPdx->x), "f"(__dPdx->y),
            "f"(__dPdx->z), "f"(__dPdy->x), "f"(__dPdy->y), "f"(__dPdy->z)));

__IMPL_S("__tex3DLod_v2", "__tex3DLod_rmnf_v2",
         (float __x, float __y, float __z, float __level), "tex.level.3d.v4",
         "f32", "{%0, %1, %2, %3}, [%4, {%5, %6, %7, %7}], %8;",
         ("f"(__x), "f"(__y), "f"(__z), "f"(__level)));
__IMPL_ALIAS("__itex3DLod", "__tex3DLod_v2");

__IMPL_S3S("__itex3DLod_sparse",
           (float __x, float __y, float __z, float __level,
            unsigned char *__ir),
           "{.reg .pred %%p0;\n\t"
           "tex.level.3d.v4",
           "f32",
           "{%0, %1, %2, %3}|%%p0, [%5, {%6, %7, %8, %8}], %9;\n\t"
           "selp.u16 %4, 1, 0, %%p0; }",
           ("f"(__x), "f"(__y), "f"(__z), "f"(__level)));

// Cubemap
__IMPL_S("__texCubemap_v2", "__texCubemap_rmnf_v2",
         (float __x, float __y, float __z), "tex.cube.v4", "f32",
         "{%0, %1, %2, %3}, [%4, {%5, %6, %7, %7}];",
         ("f"(__x), "f"(__y), "f"(__z)));
__IMPL_ALIAS("__itexCubemap", "__texCubemap_v2");

__IMPL_S3S("__itexCubemap_sparse",
           (float __x, float __y, float __z, unsigned char *__ir),
           "{.reg .pred %%p0;\n\t"
           "tex.cube.v4",
           "f32",
           "{%0, %1, %2, %3}|%%p0, [%5, {%6, %7, %8, %8}];\n\t"
           "selp.u16 %4, 1, 0, %%p0; }",
           ("f"(__x), "f"(__y), "f"(__z)));

__IMPL_S("__texCubemapGrad_v2", "__texCubemapGrad_rmnf_v2",
         (float __x, float __y, float __z, const float4 *__dPdx,
          const float4 *__dPdy),
         "tex.grad.cube.v4", "f32",
         "{%0, %1, %2, %3}, [%4, {%5, %6, %7, %7}], "
         "{%8, %9, %10, %10}, {%11, %12, %13, %13};",
         ("f"(__x), "f"(__y), "f"(__z), "f"(__dPdx->x), "f"(__dPdx->y),
          "f"(__dPdx->z), "f"(__dPdy->x), "f"(__dPdy->y), "f"(__dPdy->z)));
__IMPL_ALIAS("__itexCubemapGrad_v2", "__texCubemapGrad_v2");

__IMPL_S("__texCubemapLayered_v2", "__texCubemapLayered_rmnf_v2",
         (float __x, float __y, float __z, int __layer), "tex.acube.v4", "f32",
         "{%0, %1, %2, %3}, [%4, {%5, %6, %7, %8}];",
         ("r"(__layer), "f"(__x), "f"(__y), "f"(__z)));
__IMPL_ALIAS("__itexCubemapLayered", "__texCubemapLayered_v2");

__IMPL_S("__texCubemapLayeredGrad_v2", "__texCubemapLayeredGrad_rmnf_v2",
         (float __x, float __y, float __z, int __layer, const float4 *__dPdx,
          const float4 *__dPdy),
         "tex.grad.acube.v4", "f32",
         "{%0, %1, %2, %3}, [%4, {%5, %6, %7, %8}], "
         "{%9, %10, %11, %11}, {%12, %13, %14, %14};",
         ("r"(__layer), "f"(__x), "f"(__y), "f"(__z), "f"(__dPdx->x),
          "f"(__dPdx->y), "f"(__dPdx->z), "f"(__dPdy->x), "f"(__dPdy->y),
          "f"(__dPdy->z)));
__IMPL_ALIAS("__itexCubemapLayeredGrad_v2", "__texCubemapLayeredGrad_v2");

__IMPL_S("__texCubemapLayeredLod_v2", "__texCubemapLayeredLod_rmnf_v2",
         (float __x, float __y, float __z, int __layer, float __level),
         "tex.level.acube.v4", "f32",
         "{%0, %1, %2, %3}, [%4, {%5, %6, %7, %8}], %9;",
         ("r"(__layer), "f"(__x), "f"(__y), "f"(__z), "f"(__level)));
__IMPL_ALIAS("__itexCubemapLayeredLod", "__texCubemapLayeredLod_v2");

__IMPL_S("__texCubemapLod_v2", "__texCubemapLod_rmnf_v2",
         (float __x, float __y, float __z, float __level), "tex.level.cube.v4",
         "f32", "{%0, %1, %2, %3}, [%4, {%5, %6, %7, %7}], %8;",
         ("f"(__x), "f"(__y), "f"(__z), "f"(__level)));
__IMPL_ALIAS("__itexCubemapLod", "__texCubemapLod_v2");

// Helper class for extracting slice of data from V4 fetch results.
template <class __DestT, class __SrcT> struct __convert {
  template <int __NElements = sizeof(__DestT) /
                              sizeof(typename __TypeInfoT<__DestT>::__base_t)>
  __device__ static __DestT __run(__SrcT __v);
  template <> __device__ static __DestT __run<1>(__SrcT __v) { return {__v.x}; }
  template <> __device__ static __DestT __run<2>(__SrcT __v) {
    return {__v.x, __v.y};
  }
  template <> __device__ static __DestT __run<3>(__SrcT __v) {
    return {__v.x, __v.y, __v.z};
  }
  template <> __device__ static __DestT __run<4>(__SrcT __v) {
    return {__v.x, __v.y, __v.z, __v.w};
  }
};

// There are a couple of layers here.  First, __op_type_traits is used to
// dispatch to either surface write calls, or to the texture read calls.
//
// Then, that dispatches to __tex_fetch_impl below, which dispatches by both tag
// and datatype to the appropriate
// __surf_read_write_v2.
// TODO(austin): Do the reads too.

// Mark which of the ids we should be dispatching to surface write calls.
__OP_TYPE_SURFACE(__ID("__isurf1Dread"));
__OP_TYPE_SURFACE(__ID("__isurf2Dread"));
__OP_TYPE_SURFACE(__ID("__isurf3Dread"));
__OP_TYPE_SURFACE(__ID("__isurf1DLayeredread"));
__OP_TYPE_SURFACE(__ID("__isurf2DLayeredread"));
__OP_TYPE_SURFACE(__ID("__isurfCubemapread"));
__OP_TYPE_SURFACE(__ID("__isurfCubemapLayeredread"));
__OP_TYPE_SURFACE(__ID("__isurf1Dwrite_v2"));
__OP_TYPE_SURFACE(__ID("__isurf2Dwrite_v2"));
__OP_TYPE_SURFACE(__ID("__isurf3Dwrite_v2"));
__OP_TYPE_SURFACE(__ID("__isurf1DLayeredwrite_v2"));
__OP_TYPE_SURFACE(__ID("__isurf2DLayeredwrite_v2"));
__OP_TYPE_SURFACE(__ID("__isurfCubemapwrite_v2"));
__OP_TYPE_SURFACE(__ID("__isurfCubemapLayeredwrite_v2"));

template <class __op, typename __type> struct __surf_read_write_v2;

// For the various write calls, we need to be able to generate variations with
// different IDs, different numbers of arguments, and different numbers of
// outputs.

#define __SURF_WRITE_V2(__op, __asm_dim, __asmtype, __type, __index_op_args,   \
                        __index_args, __index_asm_args, __asm_op_args,         \
                        __asm_args)                                            \
  template <> struct __surf_read_write_v2<__op, __type> {                      \
    static __device__ void __run(__type *__ptr, cudaSurfaceObject_t obj,       \
                                 __L(__index_args),                            \
                                 cudaSurfaceBoundaryMode mode) {               \
      switch (mode) {                                                          \
      case cudaBoundaryModeZero:                                               \
        asm volatile("sust.b." __asm_dim "." __asmtype                         \
                     ".zero [%0, " __index_op_args "], " __asm_op_args ";"     \
                     :                                                         \
                     : "l"(obj), __L(__index_asm_args), __L(__asm_args));      \
        break;                                                                 \
      case cudaBoundaryModeClamp:                                              \
        asm volatile("sust.b." __asm_dim "." __asmtype                         \
                     ".clamp [%0, " __index_op_args "], " __asm_op_args ";"    \
                     :                                                         \
                     : "l"(obj), __L(__index_asm_args), __L(__asm_args));      \
        break;                                                                 \
      case cudaBoundaryModeTrap:                                               \
        asm volatile("sust.b." __asm_dim "." __asmtype                         \
                     ".trap [%0, " __index_op_args "], " __asm_op_args ";"     \
                     :                                                         \
                     : "l"(obj), __L(__index_asm_args), __L(__asm_args));      \
        break;                                                                 \
      }                                                                        \
    }                                                                          \
  }

#define __SURF_READ_V2(__op, __asm_dim, __asmtype, __type, __asm_op_args,      \
                       __asm_args, __index_args, __index_asm_args)             \
  template <> struct __surf_read_write_v2<__op, __type> {                      \
    static __device__ void __run(__type *__ptr, cudaSurfaceObject_t obj,       \
                                 __L(__index_args),                            \
                                 cudaSurfaceBoundaryMode mode) {               \
      switch (mode) {                                                          \
      case cudaBoundaryModeZero:                                               \
        asm("suld.b." __asm_dim "." __asmtype ".zero " __asm_op_args ";"       \
            : __L(__asm_args)                                                  \
            : "l"(obj), __L(__index_asm_args));                                \
        break;                                                                 \
      case cudaBoundaryModeClamp:                                              \
        asm("suld.b." __asm_dim "." __asmtype ".clamp " __asm_op_args ";"      \
            : __L(__asm_args)                                                  \
            : "l"(obj), __L(__index_asm_args));                                \
        break;                                                                 \
      case cudaBoundaryModeTrap:                                               \
        asm("suld.b." __asm_dim "." __asmtype ".trap " __asm_op_args ";"       \
            : __L(__asm_args)                                                  \
            : "l"(obj), __L(__index_asm_args));                                \
        break;                                                                 \
      }                                                                        \
    }                                                                          \
  }

// Amazing, the read side should follow the same flow, I just need to change the
// generated assembly calls, and the rest should fall in line.

#define __SW_ASM_ARGS(__type) (__type(*__ptr))
#define __SW_ASM_ARGS1(__type) (__type(__ptr->x))
#define __SW_ASM_ARGS2(__type) (__type(__ptr->x), __type(__ptr->y))
#define __SW_ASM_ARGS4(__type)                                                 \
  (__type(__ptr->x), __type(__ptr->y), __type(__ptr->z), __type(__ptr->w))

#define __SURF_READ1D(__asmtype, __type, __asm_op_args, __asm_args)            \
  __SURF_READ_V2(__ID("__isurf1Dread"), "1d", __asmtype, __type,               \
                 __asm_op_args, __asm_args, (int x), ("r"(x)))
#define __SURF_READ2D(__asmtype, __type, __asm_op_args, __asm_args)            \
  __SURF_READ_V2(__ID("__isurf2Dread"), "2d", __asmtype, __type,               \
                 __asm_op_args, __asm_args, (int x, int y), ("r"(x), "r"(y)))
#define __SURF_READ3D(__asmtype, __type, __asm_op_args, __asm_args)            \
  __SURF_READ_V2(__ID("__isurf3Dread"), "3d", __asmtype, __type,               \
                 __asm_op_args, __asm_args, (int x, int y, int z),             \
                 ("r"(x), "r"(y), "r"(z)))

#define __SURF_READ1DLAYERED(__asmtype, __type, __asm_op_args, __asm_args)     \
  __SURF_READ_V2(__ID("__isurf1DLayeredread"), "a1d", __asmtype, __type,       \
                 __asm_op_args, __asm_args, (int x, int layer),                \
                 ("r"(x), "r"(layer)))
#define __SURF_READ2DLAYERED(__asmtype, __type, __asm_op_args, __asm_args)     \
  __SURF_READ_V2(__ID("__isurf2DLayeredread"), "a2d", __asmtype, __type,       \
                 __asm_op_args, __asm_args, (int x, int y, int layer),         \
                 ("r"(x), "r"(y), "r"(layer)))
#define __SURF_READCUBEMAP(__asmtype, __type, __asm_op_args, __asm_args)       \
  __SURF_READ_V2(__ID("__isurfCubemapread"), "a2d", __asmtype, __type,         \
                 __asm_op_args, __asm_args, (int x, int y, int face),          \
                 ("r"(x), "r"(y), "r"(face)))
#define __SURF_READCUBEMAPLAYERED(__asmtype, __type, __asm_op_args,            \
                                  __asm_args)                                  \
  __SURF_READ_V2(__ID("__isurfCubemapLayeredread"), "a2d", __asmtype, __type,  \
                 __asm_op_args, __asm_args, (int x, int y, int layerface),     \
                 ("r"(x), "r"(y), "r"(layerface)))

#define __1DV1 "{%0}, [%1, {%2}]"
#define __1DV2 "{%0, %1}, [%2, {%3}]"
#define __1DV4 "{%0, %1, %2, %3}, [%4, {%5}]"

#define __2DV1 "{%0}, [%1, {%2, %3}]"
#define __2DV2 "{%0, %1}, [%2, {%3, %4}]"
#define __2DV4 "{%0, %1, %2, %3}, [%4, {%5, %6}]"

#define __1DLAYERV1 "{%0}, [%1, {%3, %2}]"
#define __1DLAYERV2 "{%0, %1}, [%2, {%4, %3}]"
#define __1DLAYERV4 "{%0, %1, %2, %3}, [%4, {%6, %5}]"

#define __3DV1 "{%0}, [%1, {%2, %3, %4, %4}]"
#define __3DV2 "{%0, %1}, [%2, {%3, %4, %5, %5}]"
#define __3DV4 "{%0, %1, %2, %3}, [%4, {%5, %6, %7, %7}]"

#define __2DLAYERV1 "{%0}, [%1, {%4, %2, %3, %3}]"
#define __2DLAYERV2 "{%0, %1}, [%2, {%5, %3, %4, %4}]"
#define __2DLAYERV4 "{%0, %1, %2, %3}, [%4, {%7, %5, %6, %6}]"

#define __CUBEMAPV1 "{%0}, [%1, {%4, %2, %3, %3}]"
#define __CUBEMAPV2 "{%0, %1}, [%2, {%5, %3, %4, %4}]"
#define __CUBEMAPV4 "{%0, %1, %2, %3}, [%4, {%7, %5, %6, %6}]"

#define __CUBEMAPLAYERV1 "{%0}, [%1, {%4, %2, %3, %3}]"
#define __CUBEMAPLAYERV2 "{%0, %1}, [%2, {%5, %3, %4, %4}]"
#define __CUBEMAPLAYERV4 "{%0, %1, %2, %3}, [%4, {%7, %5, %6, %6}]"

#define __SURF_READXD_ALL(__xdv1, __xdv2, __xdv4, __surf_readxd_v2)            \
  __surf_readxd_v2("b8", char, __xdv1, __SW_ASM_ARGS("=h"));                   \
  __surf_readxd_v2("b8", signed char, __xdv1, __SW_ASM_ARGS("=h"));            \
  __surf_readxd_v2("b8", char1, __xdv1, __SW_ASM_ARGS1("=h"));                 \
  __surf_readxd_v2("b8", unsigned char, __xdv1, __SW_ASM_ARGS("=h"));          \
  __surf_readxd_v2("b8", uchar1, __xdv1, __SW_ASM_ARGS1("=h"));                \
  __surf_readxd_v2("b16", short, __xdv1, __SW_ASM_ARGS("=h"));                 \
  __surf_readxd_v2("b16", short1, __xdv1, __SW_ASM_ARGS1("=h"));               \
  __surf_readxd_v2("b16", unsigned short, __xdv1, __SW_ASM_ARGS("=h"));        \
  __surf_readxd_v2("b16", ushort1, __xdv1, __SW_ASM_ARGS1("=h"));              \
  __surf_readxd_v2("b32", int, __xdv1, __SW_ASM_ARGS("=r"));                   \
  __surf_readxd_v2("b32", int1, __xdv1, __SW_ASM_ARGS1("=r"));                 \
  __surf_readxd_v2("b32", unsigned int, __xdv1, __SW_ASM_ARGS("=r"));          \
  __surf_readxd_v2("b32", uint1, __xdv1, __SW_ASM_ARGS1("=r"));                \
  __surf_readxd_v2("b64", long long, __xdv1, __SW_ASM_ARGS("=l"));             \
  __surf_readxd_v2("b64", longlong1, __xdv1, __SW_ASM_ARGS1("=l"));            \
  __surf_readxd_v2("b64", unsigned long long, __xdv1, __SW_ASM_ARGS("=l"));    \
  __surf_readxd_v2("b64", ulonglong1, __xdv1, __SW_ASM_ARGS1("=l"));           \
  __surf_readxd_v2("b32", float, __xdv1, __SW_ASM_ARGS("=r"));                 \
  __surf_readxd_v2("b32", float1, __xdv1, __SW_ASM_ARGS1("=r"));               \
                                                                               \
  __surf_readxd_v2("v2.b8", char2, __xdv2, __SW_ASM_ARGS2("=h"));              \
  __surf_readxd_v2("v2.b8", uchar2, __xdv2, __SW_ASM_ARGS2("=h"));             \
  __surf_readxd_v2("v2.b16", short2, __xdv2, __SW_ASM_ARGS2("=h"));            \
  __surf_readxd_v2("v2.b16", ushort2, __xdv2, __SW_ASM_ARGS2("=h"));           \
  __surf_readxd_v2("v2.b32", int2, __xdv2, __SW_ASM_ARGS2("=r"));              \
  __surf_readxd_v2("v2.b32", uint2, __xdv2, __SW_ASM_ARGS2("=r"));             \
  __surf_readxd_v2("v2.b64", longlong2, __xdv2, __SW_ASM_ARGS2("=l"));         \
  __surf_readxd_v2("v2.b64", ulonglong2, __xdv2, __SW_ASM_ARGS2("=l"));        \
  __surf_readxd_v2("v2.b32", float2, __xdv2, __SW_ASM_ARGS2("=r"));            \
                                                                               \
  __surf_readxd_v2("v4.b8", char4, __xdv4, __SW_ASM_ARGS4("=h"));              \
  __surf_readxd_v2("v4.b8", uchar4, __xdv4, __SW_ASM_ARGS4("=h"));             \
  __surf_readxd_v2("v4.b16", short4, __xdv4, __SW_ASM_ARGS4("=h"));            \
  __surf_readxd_v2("v4.b16", ushort4, __xdv4, __SW_ASM_ARGS4("=h"));           \
  __surf_readxd_v2("v4.b32", int4, __xdv4, __SW_ASM_ARGS4("=r"));              \
  __surf_readxd_v2("v4.b32", uint4, __xdv4, __SW_ASM_ARGS4("=r"));             \
  __surf_readxd_v2("v4.b32", float4, __xdv4, __SW_ASM_ARGS4("=r"))

__SURF_READXD_ALL(__1DV1, __1DV2, __1DV4, __SURF_READ1D);
__SURF_READXD_ALL(__2DV1, __2DV2, __2DV4, __SURF_READ2D);
__SURF_READXD_ALL(__3DV1, __3DV2, __3DV4, __SURF_READ3D);
__SURF_READXD_ALL(__1DLAYERV1, __1DLAYERV2, __1DLAYERV4, __SURF_READ1DLAYERED);
__SURF_READXD_ALL(__2DLAYERV1, __2DLAYERV2, __2DLAYERV4, __SURF_READ2DLAYERED);
__SURF_READXD_ALL(__CUBEMAPV1, __CUBEMAPV2, __CUBEMAPV4, __SURF_READCUBEMAP);
__SURF_READXD_ALL(__CUBEMAPLAYERV1, __CUBEMAPLAYERV2, __CUBEMAPLAYERV4,
                  __SURF_READCUBEMAPLAYERED);

#define __SURF_WRITE1D_V2(__asmtype, __type, __asm_op_args, __asm_args)        \
  __SURF_WRITE_V2(__ID("__isurf1Dwrite_v2"), "1d", __asmtype, __type, "{%1}",  \
                  (int x), ("r"(x)), __asm_op_args, __asm_args)
#define __SURF_WRITE1DLAYERED_V2(__asmtype, __type, __asm_op_args, __asm_args) \
  __SURF_WRITE_V2(__ID("__isurf1DLayeredwrite_v2"), "a1d", __asmtype, __type,  \
                  "{%2, %1}", (int x, int layer), ("r"(x), "r"(layer)),        \
                  __asm_op_args, __asm_args)
#define __SURF_WRITE2D_V2(__asmtype, __type, __asm_op_args, __asm_args)        \
  __SURF_WRITE_V2(__ID("__isurf2Dwrite_v2"), "2d", __asmtype, __type,          \
                  "{%1, %2}", (int x, int y), ("r"(x), "r"(y)), __asm_op_args, \
                  __asm_args)
#define __SURF_WRITE2DLAYERED_V2(__asmtype, __type, __asm_op_args, __asm_args) \
  __SURF_WRITE_V2(__ID("__isurf2DLayeredwrite_v2"), "a2d", __asmtype, __type,  \
                  "{%3, %1, %2, %2}", (int x, int y, int layer),               \
                  ("r"(x), "r"(y), "r"(layer)), __asm_op_args, __asm_args)
#define __SURF_WRITE3D_V2(__asmtype, __type, __asm_op_args, __asm_args)        \
  __SURF_WRITE_V2(__ID("__isurf3Dwrite_v2"), "3d", __asmtype, __type,          \
                  "{%1, %2, %3, %3}", (int x, int y, int z),                   \
                  ("r"(x), "r"(y), "r"(z)), __asm_op_args, __asm_args)

#define __SURF_CUBEMAPWRITE_V2(__asmtype, __type, __asm_op_args, __asm_args)   \
  __SURF_WRITE_V2(__ID("__isurfCubemapwrite_v2"), "a2d", __asmtype, __type,    \
                  "{%3, %1, %2, %2}", (int x, int y, int face),                \
                  ("r"(x), "r"(y), "r"(face)), __asm_op_args, __asm_args)
#define __SURF_CUBEMAPLAYEREDWRITE_V2(__asmtype, __type, __asm_op_args,        \
                                      __asm_args)                              \
  __SURF_WRITE_V2(__ID("__isurfCubemapLayeredwrite_v2"), "a2d", __asmtype,     \
                  __type, "{%3, %1, %2, %2}", (int x, int y, int layerface),   \
                  ("r"(x), "r"(y), "r"(layerface)), __asm_op_args, __asm_args)

#define __SURF_WRITEXD_V2_ALL(__xdv1, __xdv2, __xdv4, __surf_writexd_v2)       \
  __surf_writexd_v2("b8", char, __xdv1, __SW_ASM_ARGS("h"));                   \
  __surf_writexd_v2("b8", signed char, __xdv1, __SW_ASM_ARGS("h"));            \
  __surf_writexd_v2("b8", char1, __xdv1, __SW_ASM_ARGS1("h"));                 \
  __surf_writexd_v2("b8", unsigned char, __xdv1, __SW_ASM_ARGS("h"));          \
  __surf_writexd_v2("b8", uchar1, __xdv1, __SW_ASM_ARGS1("h"));                \
  __surf_writexd_v2("b16", short, __xdv1, __SW_ASM_ARGS("h"));                 \
  __surf_writexd_v2("b16", short1, __xdv1, __SW_ASM_ARGS1("h"));               \
  __surf_writexd_v2("b16", unsigned short, __xdv1, __SW_ASM_ARGS("h"));        \
  __surf_writexd_v2("b16", ushort1, __xdv1, __SW_ASM_ARGS1("h"));              \
  __surf_writexd_v2("b32", int, __xdv1, __SW_ASM_ARGS("r"));                   \
  __surf_writexd_v2("b32", int1, __xdv1, __SW_ASM_ARGS1("r"));                 \
  __surf_writexd_v2("b32", unsigned int, __xdv1, __SW_ASM_ARGS("r"));          \
  __surf_writexd_v2("b32", uint1, __xdv1, __SW_ASM_ARGS1("r"));                \
  __surf_writexd_v2("b64", long long, __xdv1, __SW_ASM_ARGS("l"));             \
  __surf_writexd_v2("b64", longlong1, __xdv1, __SW_ASM_ARGS1("l"));            \
  __surf_writexd_v2("b64", unsigned long long, __xdv1, __SW_ASM_ARGS("l"));    \
  __surf_writexd_v2("b64", ulonglong1, __xdv1, __SW_ASM_ARGS1("l"));           \
  __surf_writexd_v2("b32", float, __xdv1, __SW_ASM_ARGS("r"));                 \
  __surf_writexd_v2("b32", float1, __xdv1, __SW_ASM_ARGS1("r"));               \
                                                                               \
  __surf_writexd_v2("v2.b8", char2, __xdv2, __SW_ASM_ARGS2("h"));              \
  __surf_writexd_v2("v2.b8", uchar2, __xdv2, __SW_ASM_ARGS2("h"));             \
  __surf_writexd_v2("v2.b16", short2, __xdv2, __SW_ASM_ARGS2("h"));            \
  __surf_writexd_v2("v2.b16", ushort2, __xdv2, __SW_ASM_ARGS2("h"));           \
  __surf_writexd_v2("v2.b32", int2, __xdv2, __SW_ASM_ARGS2("r"));              \
  __surf_writexd_v2("v2.b32", uint2, __xdv2, __SW_ASM_ARGS2("r"));             \
  __surf_writexd_v2("v2.b64", longlong2, __xdv2, __SW_ASM_ARGS2("l"));         \
  __surf_writexd_v2("v2.b64", ulonglong2, __xdv2, __SW_ASM_ARGS2("l"));        \
  __surf_writexd_v2("v2.b32", float2, __xdv2, __SW_ASM_ARGS2("r"));            \
                                                                               \
  __surf_writexd_v2("v4.b8", char4, __xdv4, __SW_ASM_ARGS4("h"));              \
  __surf_writexd_v2("v4.b8", uchar4, __xdv4, __SW_ASM_ARGS4("h"));             \
  __surf_writexd_v2("v4.b16", short4, __xdv4, __SW_ASM_ARGS4("h"));            \
  __surf_writexd_v2("v4.b16", ushort4, __xdv4, __SW_ASM_ARGS4("h"));           \
  __surf_writexd_v2("v4.b32", int4, __xdv4, __SW_ASM_ARGS4("r"));              \
  __surf_writexd_v2("v4.b32", uint4, __xdv4, __SW_ASM_ARGS4("r"));             \
  __surf_writexd_v2("v4.b32", float4, __xdv4, __SW_ASM_ARGS4("r"))

#define __1DV1 "{%2}"
#define __1DV2 "{%2, %3}"
#define __1DV4 "{%2, %3, %4, %5}"

#define __2DV1 "{%3}"
#define __2DV2 "{%3, %4}"
#define __2DV4 "{%3, %4, %5, %6}"

#define __3DV1 "{%4}"
#define __3DV2 "{%4, %5}"
#define __3DV4 "{%4, %5, %6, %7}"

__SURF_WRITEXD_V2_ALL(__1DV1, __1DV2, __1DV4, __SURF_WRITE1D_V2);
__SURF_WRITEXD_V2_ALL(__2DV1, __2DV2, __2DV4, __SURF_WRITE2D_V2);
__SURF_WRITEXD_V2_ALL(__3DV1, __3DV2, __3DV4, __SURF_WRITE3D_V2);
__SURF_WRITEXD_V2_ALL(__2DV1, __2DV2, __2DV4, __SURF_WRITE1DLAYERED_V2);
__SURF_WRITEXD_V2_ALL(__3DV1, __3DV2, __3DV4, __SURF_WRITE2DLAYERED_V2);
__SURF_WRITEXD_V2_ALL(__3DV1, __3DV2, __3DV4, __SURF_CUBEMAPWRITE_V2);
__SURF_WRITEXD_V2_ALL(__3DV1, __3DV2, __3DV4, __SURF_CUBEMAPLAYEREDWRITE_V2);

template <class __op, class __DataT, class... __Args>
__device__ static void __tex_fetch_impl(__surface_op_tag, __DataT *__ptr,
                                        cudaSurfaceObject_t __handle,
                                        __Args... __args) {
  __surf_read_write_v2<__op, __DataT>::__run(__ptr, __handle, __args...);
}

// These are the top-level function overloads the __nv_tex_surf_handler expands
// to.  Each overload deals with one of the several ways __nv_tex_surf_handler
// is called by CUDA headers. In the end, each of the overloads does the same
// job -- it figures out which `__tex_fetch_v4::run` variant should be used to
// fetch texture data and which `__convert::run` is needed to convert it into
// appropriate return type.

// __nv_tex_surf_handler("__tex...", &ret, cudaTextureObject_t handle, args...);
//   Data type and return type are based on ret.
template <class __op, class __T, class... __Args>
__device__ static void __tex_fetch_impl(__texture_op_tag, __T *__ptr,
                                        cudaTextureObject_t __handle,
                                        __Args... __args) {
  using __FetchT = typename __TypeInfoT<__T>::__fetch_t;
  *__ptr = __convert<__T, __FetchT>::__run(
      __tex_fetch_v4<__op>::template __run<__FetchT>(__handle, __args...));
}

template <class __op, class __T, class... __Args>
__device__ static void __tex_fetch(__T *__ptr, cudaTextureObject_t __handle,
                                   __Args... __args) {
  using op_type = typename __op_type_traits<__op>::type;
  __tex_fetch_impl<__op>(op_type{}, __ptr, __handle, __args...);
}

#if CUDA_VERSION < 12000
// texture<> objects get magically converted into a texture reference.  However,
// there's no way to convert them to cudaTextureObject_t on C++ level. So, we
// cheat a bit and use inline assembly to do it. It costs us an extra register
// and a move, but that is easy for ptxas to optimize away.
template <class __T>
__device__ cudaTextureObject_t __tex_handle_to_obj(__T __handle) {
  cudaTextureObject_t __obj;
  asm("mov.b64 %0, %1; " : "=l"(__obj) : "l"(__handle));
  return __obj;
}

// __nv_tex_surf_handler ("__tex...", &ret, textureReference, args...);
//   Data type and return type is based on ret.
template <class __op, class __T, class __HandleT, class... __Args>
__device__ static void __tex_fetch(__T *__ptr, __HandleT __handle,
                                   __Args... __args) {
  using __FetchT = typename __TypeInfoT<__T>::__fetch_t;
  *__ptr = __convert<__T, __FetchT>::__run(
      __tex_fetch_v4<__op>::template __run<__FetchT>(
          __tex_handle_to_obj(__handle), __args...));
}

// __nv_tex_surf_handler ("__tex...", &type_dummy, &ret, texture<...>, args...);
// cudaReadModeNormalizedFloat fetches always return float4.
template <class __op, class __DataT, class __RetT, int __TexT, class... __Args>
__device__ static void
__tex_fetch(__DataT *, __RetT *__ptr,
            texture<__DataT, __TexT, cudaReadModeNormalizedFloat> __handle,
            __Args... __args) {
  using __FetchT = typename __TypeInfoT<__DataT>::__fetch_t;
  *__ptr = __convert<__RetT, float4>::__run(
      __tex_fetch_v4<__op>::template __run<__FetchT>(
          __tex_handle_to_obj(__handle), __args...));
}

// __nv_tex_surf_handler ("__tex...", &type_dummy, &ret, texture<...>, args...);
// For cudaReadModeElementType fetch return type is based on type_dummy.
template <class __op, class __DataT, class __RetT, int __TexT, class... __Args>
__device__ static void
__tex_fetch(__DataT *, __RetT *__ptr,
            texture<__DataT, __TexT, cudaReadModeElementType> __handle,
            __Args... __args) {
  using __FetchT = typename __TypeInfoT<__DataT>::__fetch_t;
  *__ptr = __convert<__RetT, __FetchT>::__run(
      __tex_fetch_v4<__op>::template __run<__FetchT>(
          __tex_handle_to_obj(__handle), __args...));
}
#endif // CUDA_VERSION
} // namespace __cuda_tex
} // namespace
#pragma pop_macro("__ASM_OUT")
#pragma pop_macro("__ASM_OUTP")
#pragma pop_macro("__Args")
#pragma pop_macro("__ID")
#pragma pop_macro("__IDV")
#pragma pop_macro("__OP_TYPE_SURFACE")
#pragma pop_macro("__IMPL_2DGATHER")
#pragma pop_macro("__IMPL_ALIAS")
#pragma pop_macro("__IMPL_ALIASI")
#pragma pop_macro("__IMPL_F1")
#pragma pop_macro("__IMPL_F3")
#pragma pop_macro("__IMPL_F3N")
#pragma pop_macro("__IMPL_F3S")
#pragma pop_macro("__IMPL_S")
#pragma pop_macro("__IMPL_S3")
#pragma pop_macro("__IMPL_S3I")
#pragma pop_macro("__IMPL_S3N")
#pragma pop_macro("__IMPL_S3NI")
#pragma pop_macro("__IMPL_S3S")
#pragma pop_macro("__IMPL_S3SI")
#pragma pop_macro("__IMPL_SI")
#pragma pop_macro("__L")
#pragma pop_macro("__STRIP_PARENS")
#pragma pop_macro("__SURF_WRITE_V2")
#pragma pop_macro("__SW_ASM_ARGS")
#pragma pop_macro("__SW_ASM_ARGS1")
#pragma pop_macro("__SW_ASM_ARGS2")
#pragma pop_macro("__SW_ASM_ARGS4")
#pragma pop_macro("__SURF_WRITE_V2")
#pragma pop_macro("__SURF_READ_V2")
#pragma pop_macro("__SW_ASM_ARGS")
#pragma pop_macro("__SW_ASM_ARGS1")
#pragma pop_macro("__SW_ASM_ARGS2")
#pragma pop_macro("__SW_ASM_ARGS4")
#pragma pop_macro("__SURF_READ1D");
#pragma pop_macro("__SURF_READ2D");
#pragma pop_macro("__SURF_READ3D");
#pragma pop_macro("__SURF_READ1DLAYERED");
#pragma pop_macro("__SURF_READ2DLAYERED");
#pragma pop_macro("__SURF_READCUBEMAP");
#pragma pop_macro("__SURF_READCUBEMAPLAYERED");
#pragma pop_macro("__1DV1");
#pragma pop_macro("__1DV2");
#pragma pop_macro("__1DV4");
#pragma pop_macro("__2DV1");
#pragma pop_macro("__2DV2");
#pragma pop_macro("__2DV4");
#pragma pop_macro("__1DLAYERV1");
#pragma pop_macro("__1DLAYERV2");
#pragma pop_macro("__1DLAYERV4");
#pragma pop_macro("__3DV1");
#pragma pop_macro("__3DV2");
#pragma pop_macro("__3DV4");
#pragma pop_macro("__2DLAYERV1");
#pragma pop_macro("__2DLAYERV2");
#pragma pop_macro("__2DLAYERV4");
#pragma pop_macro("__CUBEMAPV1");
#pragma pop_macro("__CUBEMAPV2");
#pragma pop_macro("__CUBEMAPV4");
#pragma pop_macro("__CUBEMAPLAYERV1");
#pragma pop_macro("__CUBEMAPLAYERV2");
#pragma pop_macro("__CUBEMAPLAYERV4");
#pragma pop_macro("__SURF_READXD_ALL");
#pragma pop_macro("__SURF_WRITE1D_V2");
#pragma pop_macro("__SURF_WRITE1DLAYERED_V2");
#pragma pop_macro("__SURF_WRITE2D_V2");
#pragma pop_macro("__SURF_WRITE2DLAYERED_V2");
#pragma pop_macro("__SURF_WRITE3D_V2");
#pragma pop_macro("__SURF_CUBEMAPWRITE_V2");
#pragma pop_macro("__SURF_CUBEMAPLAYEREDWRITE_V2");
#pragma pop_macro("__SURF_WRITEXD_V2_ALL");
#pragma pop_macro("__1DV1");
#pragma pop_macro("__1DV2");
#pragma pop_macro("__1DV4");
#pragma pop_macro("__2DV1");
#pragma pop_macro("__2DV2");
#pragma pop_macro("__2DV4");
#pragma pop_macro("__3DV1");
#pragma pop_macro("__3DV2");
#pragma pop_macro("__3DV4");
#endif // __CLANG_CUDA_TEXTURE_INTRINSICS_H__
