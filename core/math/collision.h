/**
 * @file collision.h
 * @brief Collision detection utility functions
 *
 * Provides reusable collision detection functions for game entities.
 */

#ifndef ENGINE_CORE_MATH_COLLISION_H_
#define ENGINE_CORE_MATH_COLLISION_H_

#include <stdbool.h>

/**
 * @brief Axis-Aligned Bounding Box (AABB) collision detection
 * 
 * Tests if two rectangular bounding boxes overlap.
 *
 * @param x1 X position of first rectangle
 * @param y1 Y position of first rectangle  
 * @param w1 Width of first rectangle
 * @param h1 Height of first rectangle
 * @param x2 X position of second rectangle
 * @param y2 Y position of second rectangle
 * @param w2 Width of second rectangle
 * @param h2 Height of second rectangle
 * @return true if rectangles overlap, false otherwise
 */
bool aabb_collision(float x1, float y1, float w1, float h1,
                   float x2, float y2, float w2, float h2);

#endif  // ENGINE_CORE_MATH_COLLISION_H_
