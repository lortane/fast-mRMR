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

#pragma once

#include <stdfloat>
#include <vector>

#include "RawData.h"

class JointProb
{
  public:
    JointProb(RawData &rd, std::uint32_t index1, std::uint32_t index2);

    double fetchProbability(std::uint8_t value_feature1, std::uint8_t value_feature2) const;

  private:
    RawData &raw_data_;
    std::uint32_t index1_;
    std::uint32_t index2_;
    std::vector<std::uint32_t> data_;
    std::uint32_t values_range1_;
    std::uint32_t values_range2_;
    std::uint32_t data_size_;

    void calculate();
};