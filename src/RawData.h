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

#include <fstream>
#include <vector>

#include "utils.h"

class RawData
{
  public:
    explicit RawData(const std::string &filename);
    ~RawData();

    uint getValuesRange(uint index) const;
    std::vector<uint> getValuesRangeArray() const;
    uint getDataSize() const;
    uint getFeaturesSize() const;

    // Return a span or vector reference rather than raw pointer
    std::vector<t_data> getFeature(int index) const;

  private:
    void calculateVR();
    void calculateDSandFS();
    void loadData();

    std::vector<t_data> data_;
    uint features_size_;
    uint data_size_;
    std::vector<uint> values_range_;
    std::ifstream data_file_;
};
