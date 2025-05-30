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

#include "JointProb.h"

#include <stdexcept>

JointProb::JointProb(RawData &raw_data, uint index1, uint index2)
    : raw_data_(raw_data)
{
    index1_ = index1;
    index2_ = index2;
    values_range1_ = raw_data_.getValuesRange(index1);
    values_range2_ = raw_data_.getValuesRange(index2);
    data_size_ = raw_data_.getDataSize();

    // Initialize vector with zeros (equivalent to calloc)
    data_.resize(values_range1_ * values_range2_, 0);

    calculate();
}

// Calculates the joint probability between the given features.
void JointProb::calculate()
{
    // Get feature vectors - these are now std::vector<t_data>, not raw pointers
    std::vector<t_data> h_vector1 = raw_data_.getFeature(index1_);
    std::vector<t_data> h_vector2 = raw_data_.getFeature(index2_);

    // Calculate histogram in CPU
    for (uint i = 0; i < data_size_; i++) {
        // Access vectors with bounds checking
        if (i < h_vector1.size() && i < h_vector2.size()) {
            uint index = h_vector1[i] * values_range2_ + h_vector2[i];
            if (index < data_.size()) {
                data_[index]++;
            }
        }
    }
}

t_prob JointProb::getProb(t_data value_feature1, t_data value_feature2)
{
    uint index = value_feature1 * values_range2_ + value_feature2;

    // Add bounds checking for safety
    if (index >= data_.size()) {
        throw std::out_of_range("Index out of range in JointProb::getProb");
    }

    return static_cast<t_prob>(data_[index]) / static_cast<t_prob>(data_size_);
}