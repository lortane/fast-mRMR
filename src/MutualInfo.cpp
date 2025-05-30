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

#include "MutualInfo.h"

#include <cmath>

#include "JointProb.h"

MutualInfo::MutualInfo(RawData &rd, ProbTable &pt)
    : raw_data_(rd),
      prob_table_(pt)
{
}

// Calculates the mutual information between the given features.
double MutualInfo::fetch(std::uint32_t feature_index1, std::uint32_t feature_index2) const
{
    std::uint32_t range1 = raw_data_.getValuesRange(feature_index1);
    std::uint32_t range2 = raw_data_.getValuesRange(feature_index2);
    double mutual_info = 0;
    constexpr double epsilon = 1e-10;  // Small value to avoid division by zero

    JointProb joint_probability_table = JointProb(raw_data_, feature_index1, feature_index2);

    for (std::uint32_t i = 0; i < range1; i++) {
        for (std::uint32_t j = 0; j < range2; j++) {
            double joint_probability = joint_probability_table.fetchProbability(i, j);

            if (joint_probability > epsilon) {  // Avoid log(~0) which is undefined
                double marginalX = prob_table_.fetchProbability(feature_index1, i);
                double marginalY = prob_table_.fetchProbability(feature_index2, j);

                // Check for zero probabilities to avoid division by zero
                if (marginalX > 0 && marginalY > 0) {
                    double division = joint_probability / (marginalX * marginalY);
                    mutual_info += joint_probability * std::log2(division);
                }
            }
        }
    }

    return mutual_info;
}