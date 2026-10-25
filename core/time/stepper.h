/**
 * @file stepper.h
 * @brief Fixed-step simulation planning
 *
 * Splits a frame's elapsed time into equal simulation steps no longer than a
 * maximum, and caps a stalled frame, so physics stays stable at any display
 * rate.
 */

#ifndef CORE_TIME_STEPPER_H_
#define CORE_TIME_STEPPER_H_

/**
 * @brief Plan the simulation steps for one frame
 * @param frame_seconds Time since the last frame; negative or NaN counts as 0
 * @param max_step Longest allowed step, in seconds
 * @param max_frame The frame is clamped to this many seconds first
 * @param step Receives the length of each step
 * @return The number of steps to run (at least 1)
 */
int stepper_plan(double frame_seconds, double max_step, double max_frame,
                 double* step);

#endif  // CORE_TIME_STEPPER_H_
