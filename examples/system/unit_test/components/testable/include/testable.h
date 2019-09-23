/* testable.h: Implementation of a testable component.

   This example code is in the Public Domain (or CC0 licensed, at your option.)

   Unless required by applicable law or agreed to in writing, this
   software is distributed on an "AS IS" BASIS, WITHOUT WARRANTIES OR
   CONDITIONS OF ANY KIND, either express or implied.
*/

#pragma once

/**
 * @brief Calculate arithmetic mean of integer values
 * @param values  array of values
 * @param count   number of elements in the array
 * @return arithmetic mean of values, or zero count is zero
 */
int testable_mean(const int* values, int count);
