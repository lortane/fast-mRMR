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
double MutualInfo::get(uint feature_index1, uint feature_index2)
{
    uint range1 = raw_data_.getValuesRange(feature_index1);
    uint range2 = raw_data_.getValuesRange(feature_index2);
    t_prob mInfo = 0;

    JointProb jointProbTable = JointProb(raw_data_, feature_index1, feature_index2);

    for (uint i = 0; i < range1; i++) {
        for (uint j = 0; j < range2; j++) {
            t_prob jointProb = jointProbTable.getProb(i, j);

            if (jointProb > 0) {  // Avoid log(0) which is undefined
                t_prob marginalX = prob_table_.getProbability(feature_index1, i);
                t_prob marginalY = prob_table_.getProbability(feature_index2, j);

                // Check for zero probabilities to avoid division by zero
                if (marginalX > 0 && marginalY > 0) {
                    t_prob division = jointProb / (marginalX * marginalY);
                    mInfo += jointProb * std::log2(division);
                }
            }
        }
    }

    return mInfo;
}