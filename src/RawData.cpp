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
 * @param filename Path to the data file
 */
RawData::RawData(const std::string &filename)
{
    // Open file with binary mode
    dataFile.open(filename, std::ios::binary);
    if (!dataFile) {
        throw std::runtime_error("Could not open file: " + filename);
    }

    calculateDSandFS();
    loadData();
    calculateVR();
}

RawData::~RawData()
{
    // No need to manually free memory - vectors handle cleanup automatically
    if (dataFile.is_open()) {
        dataFile.close();
    }
}

/**
 * Calculates DataSize: Number of patterns or samples
 * FeaturesSize: Number of features
 */
void RawData::calculateDSandFS()
{
    uint datasizeBuffer = 0;
    uint featuresSizeBuffer = 0;

    if (!dataFile.read(reinterpret_cast<char *>(&datasizeBuffer), sizeof(uint))
        || !dataFile.read(reinterpret_cast<char *>(&featuresSizeBuffer), sizeof(uint))) {
        throw std::runtime_error("Failed to read data dimensions from file");
    }

    datasize = datasizeBuffer;
    featuresSize = featuresSizeBuffer;
}

void RawData::loadData()
{
    t_data buffer;

    // Preallocate with the right size
    data.resize(featuresSize * datasize);

    // Seek to position after header (8 bytes)
    dataFile.seekg(8);

    for (uint i = 0; i < datasize; i++) {
        for (uint j = 0; j < featuresSize; j++) {
            if (!dataFile.read(reinterpret_cast<char *>(&buffer), sizeof(t_data))) {
                throw std::runtime_error("Failed to read data from file");
            }
            data[j * datasize + i] = buffer;
        }
    }
}

/**
 * Calculates how many different values each feature has.
 */
void RawData::calculateVR()
{
    valuesRange.resize(featuresSize, 0);

    for (uint i = 0; i < featuresSize; i++) {
        uint vr = 0;
        for (uint j = 0; j < datasize; j++) {
            t_data dataRead = data[i * datasize + j];
            if (dataRead > vr) {
                vr++;
            }
        }
        valuesRange[i] = vr + 1;
    }
}

uint RawData::getDataSize() const
{
    return datasize;
}

uint RawData::getFeaturesSize() const
{
    return featuresSize;
}

/**
 * Returns how many values a feature has, FROM 1 to VALUES
 */
uint RawData::getValuesRange(uint index) const
{
    if (index >= valuesRange.size()) {
        throw std::out_of_range("Feature index out of range");
    }
    return valuesRange[index];
}

/**
 * Returns a vector with the number of possible values for each feature.
 */
std::vector<uint> RawData::getValuesRangeArray() const
{
    return valuesRange;  // Return a copy of the vector
}

/**
 * Returns a vector containing a feature.
 */
std::vector<t_data> RawData::getFeature(int index) const
{
    if (index < 0 || static_cast<uint>(index) >= featuresSize) {
        throw std::out_of_range("Feature index out of range");
    }

    std::vector<t_data> feature(datasize);
    for (uint i = 0; i < datasize; ++i) {
        feature[i] = data[index * datasize + i];
    }
    return feature;
}