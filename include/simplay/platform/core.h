#pragma once

#ifdef _WIN32
#define SIM_WINDOWS
#else
#error "Unsupported OS"
#endif

#ifdef _M_X64
#define SIM_X86_64
#else
#error "Unsupported architecture"
#endif

#ifdef SIM_X86_64
#define SIM_64_BITS
#else
#error "Unsupported pointer width"
#endif

#if defined(SIM_WINDOWS) && defined(SIM_64_BITS)
#define SIM_LLP64
#define SIM_WINDOWS_64
#else
#error "Unsupported data model"
#endif

namespace sim {
#ifdef SIM_LLP64
  typedef signed char i8;
  typedef unsigned char u8;
  typedef short i16;
  typedef unsigned short u16;
  typedef int i32;
  typedef unsigned int u32;
  typedef long long i64;
  typedef unsigned long long u64;
#else
#error "Missing core integer types"
#endif
  
#ifdef SIM_WINDOWS_64
  typedef float f32;
  typedef double f64;
#else
#error "Missing core floating-point types"
#endif
  
#ifdef SIM_64_BITS
  typedef i64 isize;
  typedef u64 usize;
#else
#error "Missing core size types"
#endif
}
