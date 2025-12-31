/**
 * @file inline.h
 * @brief Compiler-specific inline optimization macros
 *
 * Defines the ALWAYS_INLINE macro to force function inlining for
 * performance-critical code paths. Uses compiler-specific attributes
 * to ensure aggressive inlining optimization.
 */

#ifndef CORE_UTILS_INLINE_H_
#define CORE_UTILS_INLINE_H_

#define ALWAYS_INLINE inline __attribute__((always_inline))

#endif  // CORE_UTILS_INLINE_H_
