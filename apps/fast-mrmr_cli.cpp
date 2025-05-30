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

#include <stdio.h>
#include <string.h>

#include <algorithm>
#include <chrono>
#include <cstdlib>
#include <iostream>
#include <limits>
#include <vector>

#include "MutualInfo.h"
#include "RawData.h"

// Returns the index of the higher value in the classRelevances Vector different
// from classIndex
std::uint32_t getMaxRelevance(std::vector<double> classRelevances, std::uint32_t classIndex)
{
    std::uint32_t i = 0;
    std::uint32_t newFeature = -1;
    double relevance = 0;
    for (i = 0; i < classRelevances.size(); ++i) {
        if (classRelevances[i] > relevance && i != classIndex) {
            relevance = classRelevances[i];
            newFeature = i;
        }
    }
    return newFeature;
}

typedef struct options {
    std::uint32_t classIndex;
    std::uint32_t selectedFeatures;
    std::string file;
} options;

options parseOptions(int argc, char *argv[])
{
    options opts;
    opts.classIndex = 0;
    opts.selectedFeatures = 10;
    opts.file = "../data.mrmr";

    if (argc > 1) {
        for (int i = 0; i < argc; ++i) {
            if (strcmp(argv[i], "-f") == 0) {
                opts.file = argv[i + 1];
            }
            if (strcmp(argv[i], "-a") == 0) {
                opts.selectedFeatures = atoi(argv[i + 1]) - 1;
            }
            if (strcmp(argv[i], "-c") == 0) {
                opts.classIndex = atoi(argv[i + 1]) - 1;
            }
            if (strcmp(argv[i], "-h") == 0) {
                printf(
                    "fast-mrmr:\nOptions:\n -f <inputfile>\t\tMRMR file generated "
                    "using mrmrReader (default: data.mrmr).\n-c "
                    "<classindex>\t\tIndicates the class index in the dataset "
                    "(default: 0).\n-a <nfeatures>\t Indicates the number of "
                    "features to select (default: 10).\n-h Prints this message");
                exit(0);
            }
        }
    }
    return opts;
}

int main(int argc, char *argv[])
{
    options opts;
    std::uint32_t i = 0;
    std::uint32_t j = 0;
    std::uint32_t newFeatureIndex = 0;
    std::uint32_t lastFeatureIndex = 0;
    double mrmr = 0;
    double best_score = 0;
    std::vector<double> relevances;
    std::vector<double> redundances;
    std::vector<int> selectedFeatures;

    opts = parseOptions(argc, argv);
    RawData rawData(opts.file);

    auto start_time = std::chrono::high_resolution_clock::now();

    ProbTable prob = ProbTable(rawData);
    MutualInfo mutualInfo = MutualInfo(rawData, prob);

    // Get relevances between all features and class.
    for (i = 0; i < rawData.getFeaturesSize(); ++i) {
        relevances.push_back(mutualInfo.fetch(opts.classIndex, i));
        redundances.push_back(0);
    }

    // Max relevance feature is added because no redundancy is possible.
    newFeatureIndex = getMaxRelevance(relevances, opts.classIndex);
    selectedFeatures.push_back(newFeatureIndex);
    lastFeatureIndex = newFeatureIndex;

    std::cout << newFeatureIndex << ",";
    // MRMR
    while (selectedFeatures.size() < rawData.getFeaturesSize() - 1  //-1 because class is discarded
           and selectedFeatures.size() < opts.selectedFeatures) {
        best_score = -std::numeric_limits<double>::infinity();
        for (j = 0; j < rawData.getFeaturesSize(); ++j) {
            // If feature not in selected selectedFeatures
            if (find(selectedFeatures.begin(), selectedFeatures.end(), j) == selectedFeatures.end()
                && j != opts.classIndex) {
                redundances[j] += mutualInfo.fetch(lastFeatureIndex, j);
                mrmr = relevances[j] - (redundances[j] / selectedFeatures.size());
                if (mrmr > best_score) {
                    best_score = mrmr;
                    newFeatureIndex = j;
                }
            }
        }
        // Last feature doesn't prints comma.
        if ((selectedFeatures.size() == (opts.selectedFeatures - 1))
            or (selectedFeatures.size() == (rawData.getFeaturesSize() - 2))) {
            std::cout << newFeatureIndex;
        } else {
            std::cout << newFeatureIndex << ",";
        }
        selectedFeatures.push_back(newFeatureIndex);
        lastFeatureIndex = newFeatureIndex;
    }

    // Calculate elapsed time
    auto end_time = std::chrono::high_resolution_clock::now();
    double elapsed_ms = std::chrono::duration<double, std::milli>(end_time - start_time).count();

    std::cout << "Elapsed time: " << elapsed_ms << " ms" << std::endl;

    return EXIT_SUCCESS;
}
