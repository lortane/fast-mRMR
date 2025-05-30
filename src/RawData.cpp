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

#include "RawData.h"

#include <stdexcept>

/**
 * Constructor that creates a rawData object.
 *
 * @param filename Path to the data_ file
 */
RawData::RawData(const std::string &filename)
{
    // Open file with binary mode
    data_file_.open(filename, std::ios::binary);
    if (!data_file_) {
        throw std::runtime_error("Could not open file: " + filename);
    }

    calculateDSandFS();
    loadData();
    calculateVR();
}

RawData::~RawData()
{
    // No need to manually free memory - vectors handle cleanup automatically
    if (data_file_.is_open()) {
        data_file_.close();
    }
}

/**
 * Calculates DataSize: Number of patterns or samples
 * FeaturesSize: Number of features
 */
void RawData::calculateDSandFS()
{
    uint data_size_buffer = 0;
    uint features_size_buffer = 0;

    if (!data_file_.read(reinterpret_cast<char *>(&data_size_buffer), sizeof(uint))
        || !data_file_.read(reinterpret_cast<char *>(&features_size_buffer), sizeof(uint))) {
        throw std::runtime_error("Failed to read data dimensions from file");
    }

    data_size_ = data_size_buffer;
    features_size_ = features_size_buffer;
}

void RawData::loadData()
{
    t_data buffer;

    // Preallocate with the right size
    data_.resize(features_size_ * data_size_);

    // Seek to position after header (8 bytes)
    data_file_.seekg(8);

    for (uint i = 0; i < data_size_; i++) {
        for (uint j = 0; j < features_size_; j++) {
            if (!data_file_.read(reinterpret_cast<char *>(&buffer), sizeof(t_data))) {
                throw std::runtime_error("Failed to read data_ from file");
            }
            data_[j * data_size_ + i] = buffer;
        }
    }
}

/**
 * Calculates how many different values each feature has.
 */
void RawData::calculateVR()
{
    values_range_.resize(features_size_, 0);

    for (uint i = 0; i < features_size_; i++) {
        uint vr = 0;
        for (uint j = 0; j < data_size_; j++) {
            t_data dataRead = data_[i * data_size_ + j];
            if (dataRead > vr) {
                vr++;
            }
        }
        values_range_[i] = vr + 1;
    }
}

uint RawData::getDataSize() const
{
    return data_size_;
}

uint RawData::getFeaturesSize() const
{
    return features_size_;
}

/**
 * Returns how many values a feature has, FROM 1 to VALUES
 */
uint RawData::getValuesRange(uint index) const
{
    if (index >= values_range_.size()) {
        throw std::out_of_range("Feature index out of range");
    }
    return values_range_[index];
}

/**
 * Returns a vector with the number of possible values for each feature.
 */
std::vector<uint> RawData::getValuesRangeArray() const
{
    return values_range_;  // Return a copy of the vector
}

/**
 * Returns a vector containing a feature.
 */
std::vector<t_data> RawData::getFeature(int index) const
{
    if (index < 0 || static_cast<uint>(index) >= features_size_) {
        throw std::out_of_range("Feature index out of range");
    }

    std::vector<t_data> feature(data_size_);
    for (uint i = 0; i < data_size_; ++i) {
        feature[i] = data_[index * data_size_ + i];
    }
    return feature;
}