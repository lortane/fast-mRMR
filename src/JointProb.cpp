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


JointProb::JointProb(RawData &rd, uint index1, uint index2)
    : rawData(rd)
{
    this->index1 = index1;
    this->index2 = index2;
    this->valuesRange1 = rawData.getValuesRange(index1);
    this->valuesRange2 = rawData.getValuesRange(index2);
    this->datasize = rawData.getDataSize();

    // Initialize vector with zeros (equivalent to calloc)
    this->data.resize(valuesRange1 * valuesRange2, 0);

    calculate();
}

// Calculates the joint probability between the given features.
void JointProb::calculate()
{
    // Get feature vectors - these are now std::vector<t_data>, not raw pointers
    std::vector<t_data> h_vector1 = rawData.getFeature(index1);
    std::vector<t_data> h_vector2 = rawData.getFeature(index2);

    // Calculate histogram in CPU
    for (uint i = 0; i < datasize; i++) {
        // Access vectors with bounds checking
        if (i < h_vector1.size() && i < h_vector2.size()) {
            uint index = h_vector1[i] * valuesRange2 + h_vector2[i];
            if (index < data.size()) {
                data[index]++;
            }
        }
    }
}

t_prob JointProb::getProb(t_data valueFeature1, t_data valueFeature2)
{
    uint index = valueFeature1 * valuesRange2 + valueFeature2;

    // Add bounds checking for safety
    if (index >= data.size()) {
        throw std::out_of_range("Index out of range in JointProb::getProb");
    }

    return static_cast<t_prob>(data[index]) / static_cast<t_prob>(datasize);
}