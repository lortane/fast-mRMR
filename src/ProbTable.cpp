/*
 * Licensed to the Apache Software Foundation (ASF) under one or more
 * contributor license agreements.  See the NOTICE file distributed with
 * this work for additional information regarding copyright ownership.
 * The ASF licenses this file to You under the Apache License, Version 2.0
 * (the "License"); you may not use this file except in compliance with
 * the License.  You may obtain a copy of the License at
 *
 *    http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#include "ProbTable.h"

#include <stdexcept>

#include "Histogram.h"

ProbTable::ProbTable(RawData& rd)
    : raw_data_(rd)
{
    data_size_ = raw_data_.getDataSize();
    features_size_ = raw_data_.getFeaturesSize();
    values_range_ = raw_data_.getValuesRangeArray();

    // Initialize table with the right dimensions
    table_.resize(features_size_);

    // Calculate probabilities immediately
    calculate();
}

// Calculates the marginal probability table for each possible value in a feature.
// This table is cached in memory to avoid repeating calculations.
void ProbTable::calculate()
{
    Histogram histogram(raw_data_);

    for (std::uint32_t i = 0; i < features_size_; ++i) {
        // Get histogram for this feature
        std::vector<std::uint32_t> hist_data = histogram.getHistogram(i);

        // Resize the inner vector for this feature
        table_[i].resize(values_range_[i]);

        // Calculate and store probabilities
        for (std::uint32_t j = 0; j < values_range_[i]; ++j) {
            table_[i][j] = static_cast<double>(hist_data[j]) / static_cast<double>(data_size_);
        }
    }
}

/**
 * Get probability for a specific feature and value
 *
 * @param index The index of the feature
 * @param value The value to get the probability for
 * @return The probability value
 */
double ProbTable::fetchProbability(std::uint32_t index, std::uint8_t value) const
{
    // Add bounds checking
    if (index >= table_.size()) {
        throw std::out_of_range("Feature index out of range in fetchProbability");
    }

    if (value >= table_[index].size()) {
        throw std::out_of_range("Value out of range in fetchProbability");
    }

    return table_[index][value];
}