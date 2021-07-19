// Copyright 2021 The WebNN-native Authors
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//     http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.

#include "examples/SampleUtils.h"

#include "src/tests/WebnnTest.h"

class DeviceSelectionTests : public WebnnTest {};

TEST_F(DeviceSelectionTests, AddConstantAndInputOnGPU) {
    ml::ContextOptions options;
    options.devicePreference = ml::DevicePreference::Gpu;
    ml::Context mContext = CreateCppContext(&options);
    mContext.SetUncapturedErrorCallback(
        [](MLErrorType type, char const* message, void* userData) {
            if (type != MLErrorType_NoError) {
                dawn::ErrorLog() << "Error type is " << type << ", message is " << message;
            }
        },
        this);
    const ml::GraphBuilder builder = ml::CreateGraphBuilder(mContext);
    const ml::Operand a = utils::BuildInput(builder, "a", {3, 4, 5});
    const std::vector<float> bData = {
        -0.5781865,  -0.49248728, -0.2162451,  -0.13176449, -0.52118045, 1.9125274,   0.6508799,
        0.71873736,  -2.3154447,  0.8080079,   0.3022368,   0.21394566,  -0.6511544,  0.20001237,
        -0.08041809, 1.1127822,   -1.521739,   0.7249548,   -0.91961324, -0.83175105, -1.4569077,
        -0.5417681,  -1.6476909,  0.1223801,   2.220618,    -0.14914903, 0.7790501,   -0.18711103,
        -0.9941537,  -1.828552,   -1.36035,    0.5727087,   2.5213664,   -0.3267195,  0.8431539,
        0.12337407,  1.0018097,   -0.23469485, -0.4530751,  0.09238022,  0.7888511,   0.11107288,
        0.48171726,  0.34308678,  -0.90550417, 0.203841,    0.02521433,  -1.7966009,  -1.4287543,
        0.3222213,   1.0590587,   -1.7948701,  -1.7195907,  -0.9120889,  -0.9391962,  -0.2566791,
        -0.5464537,  1.4351872,   0.5705938,   -0.30327085};
    const ml::Operand b =
        utils::BuildConstant(builder, {3, 4, 5}, bData.data(), bData.size() * sizeof(float));
    const ml::Operand c = builder.Add(a, b);
    const ml::Graph graph = utils::Build(builder, {{"c", c}});
    ASSERT_TRUE(graph);
    const std::vector<float> dataA = {
        0.08939514,  -1.5887482,  0.8545348,   0.20523034, -0.41728342, 1.01752,     0.19677015,
        0.5398451,   0.56893295,  1.2511084,   2.0092728,  1.0606714,   0.4893267,   0.09536829,
        -2.3467007,  2.4527607,   0.61307395,  -1.0799897, -0.15071101, -0.48422927, -0.20479254,
        0.32798728,  -0.37435308, -1.7116562,  1.6952512,  -0.7479369,  -0.09019202, 0.14343949,
        1.6754607,   1.6427531,   0.9470988,   0.20872667, -1.9530525,  -0.21783416, 0.0309498,
        0.3008434,   1.1686599,   1.4920886,   0.06633294, 0.6674667,   0.60627925,  0.04302086,
        -0.03482966, -0.7343786,  -0.76851964, 0.9446942,  -0.35489243, 0.44452578,  0.00648887,
        -0.55656946, -0.735903,   0.22050636,  -0.5008282, -1.3132697,  1.6642882,   -0.48397836,
        0.20099205,  -0.28786168, 1.3315053,   -0.41619393};
    const std::vector<float> result(utils::SizeOfShape({3, 4, 5}));
    utils::Compute(graph, {{"a", dataA}}, {{"c", result}});
    const std::vector<float> expectedValue(
        {-0.48879138, -2.0812354,  0.6382897,   0.07346585,  -0.93846387, 2.9300475,   0.84765005,
         1.2585825,   -1.7465117,  2.0591164,   2.3115096,   1.2746171,   -0.16182771, 0.29538065,
         -2.4271188,  3.565543,    -0.90866506, -0.3550349,  -1.0703243,  -1.3159803,  -1.6617002,
         -0.21378079, -2.022044,   -1.5892761,  3.9158692,   -0.8970859,  0.6888581,   -0.04367155,
         0.681307,    -0.18579888, -0.41325122, 0.7814354,   0.56831384,  -0.54455364, 0.8741037,
         0.42421746,  2.1704698,   1.2573937,   -0.38674217, 0.7598469,   1.3951304,   0.15409374,
         0.4468876,   -0.3912918,  -1.6740239,  1.1485353,   -0.32967812, -1.3520751,  -1.4222654,
         -0.23434815, 0.32315564,  -1.5743638,  -2.220419,   -2.2253585,  0.72509193,  -0.74065745,
         -0.34546167, 1.1473255,   1.9020991,   -0.7194648});
    EXPECT_TRUE(utils::CheckValue(result, expectedValue));
}

TEST_F(DeviceSelectionTests, AddConstantAndInputOnCPU) {
    ml::ContextOptions options;
    options.devicePreference = ml::DevicePreference::Cpu;
    ml::Context mContext = CreateCppContext(&options);
    mContext.SetUncapturedErrorCallback(
        [](MLErrorType type, char const* message, void* userData) {
            if (type != MLErrorType_NoError) {
                dawn::ErrorLog() << "Error type is " << type << ", message is " << message;
            }
        },
        this);
    const ml::GraphBuilder builder = ml::CreateGraphBuilder(mContext);
    const ml::Operand a = utils::BuildInput(builder, "a", {3, 4, 5});
    const std::vector<float> bData = {
        -0.5781865,  -0.49248728, -0.2162451,  -0.13176449, -0.52118045, 1.9125274,   0.6508799,
        0.71873736,  -2.3154447,  0.8080079,   0.3022368,   0.21394566,  -0.6511544,  0.20001237,
        -0.08041809, 1.1127822,   -1.521739,   0.7249548,   -0.91961324, -0.83175105, -1.4569077,
        -0.5417681,  -1.6476909,  0.1223801,   2.220618,    -0.14914903, 0.7790501,   -0.18711103,
        -0.9941537,  -1.828552,   -1.36035,    0.5727087,   2.5213664,   -0.3267195,  0.8431539,
        0.12337407,  1.0018097,   -0.23469485, -0.4530751,  0.09238022,  0.7888511,   0.11107288,
        0.48171726,  0.34308678,  -0.90550417, 0.203841,    0.02521433,  -1.7966009,  -1.4287543,
        0.3222213,   1.0590587,   -1.7948701,  -1.7195907,  -0.9120889,  -0.9391962,  -0.2566791,
        -0.5464537,  1.4351872,   0.5705938,   -0.30327085};
    const ml::Operand b =
        utils::BuildConstant(builder, {3, 4, 5}, bData.data(), bData.size() * sizeof(float));
    const ml::Operand c = builder.Add(a, b);
    const ml::Graph graph = utils::Build(builder, {{"c", c}});
    ASSERT_TRUE(graph);
    const std::vector<float> dataA = {
        0.08939514,  -1.5887482,  0.8545348,   0.20523034, -0.41728342, 1.01752,     0.19677015,
        0.5398451,   0.56893295,  1.2511084,   2.0092728,  1.0606714,   0.4893267,   0.09536829,
        -2.3467007,  2.4527607,   0.61307395,  -1.0799897, -0.15071101, -0.48422927, -0.20479254,
        0.32798728,  -0.37435308, -1.7116562,  1.6952512,  -0.7479369,  -0.09019202, 0.14343949,
        1.6754607,   1.6427531,   0.9470988,   0.20872667, -1.9530525,  -0.21783416, 0.0309498,
        0.3008434,   1.1686599,   1.4920886,   0.06633294, 0.6674667,   0.60627925,  0.04302086,
        -0.03482966, -0.7343786,  -0.76851964, 0.9446942,  -0.35489243, 0.44452578,  0.00648887,
        -0.55656946, -0.735903,   0.22050636,  -0.5008282, -1.3132697,  1.6642882,   -0.48397836,
        0.20099205,  -0.28786168, 1.3315053,   -0.41619393};
    const std::vector<float> result(utils::SizeOfShape({3, 4, 5}));
    utils::Compute(graph, {{"a", dataA}}, {{"c", result}});
    const std::vector<float> expectedValue(
        {-0.48879138, -2.0812354,  0.6382897,   0.07346585,  -0.93846387, 2.9300475,   0.84765005,
         1.2585825,   -1.7465117,  2.0591164,   2.3115096,   1.2746171,   -0.16182771, 0.29538065,
         -2.4271188,  3.565543,    -0.90866506, -0.3550349,  -1.0703243,  -1.3159803,  -1.6617002,
         -0.21378079, -2.022044,   -1.5892761,  3.9158692,   -0.8970859,  0.6888581,   -0.04367155,
         0.681307,    -0.18579888, -0.41325122, 0.7814354,   0.56831384,  -0.54455364, 0.8741037,
         0.42421746,  2.1704698,   1.2573937,   -0.38674217, 0.7598469,   1.3951304,   0.15409374,
         0.4468876,   -0.3912918,  -1.6740239,  1.1485353,   -0.32967812, -1.3520751,  -1.4222654,
         -0.23434815, 0.32315564,  -1.5743638,  -2.220419,   -2.2253585,  0.72509193,  -0.74065745,
         -0.34546167, 1.1473255,   1.9020991,   -0.7194648});
    EXPECT_TRUE(utils::CheckValue(result, expectedValue));
}

TEST_F(DeviceSelectionTests, AddConstantAndInputOnDefaultDevice) {
    const ml::GraphBuilder builder = ml::CreateGraphBuilder(GetContext());
    const ml::Operand a = utils::BuildInput(builder, "a", {3, 4, 5});
    const std::vector<float> bData = {
        -0.5781865,  -0.49248728, -0.2162451,  -0.13176449, -0.52118045, 1.9125274,   0.6508799,
        0.71873736,  -2.3154447,  0.8080079,   0.3022368,   0.21394566,  -0.6511544,  0.20001237,
        -0.08041809, 1.1127822,   -1.521739,   0.7249548,   -0.91961324, -0.83175105, -1.4569077,
        -0.5417681,  -1.6476909,  0.1223801,   2.220618,    -0.14914903, 0.7790501,   -0.18711103,
        -0.9941537,  -1.828552,   -1.36035,    0.5727087,   2.5213664,   -0.3267195,  0.8431539,
        0.12337407,  1.0018097,   -0.23469485, -0.4530751,  0.09238022,  0.7888511,   0.11107288,
        0.48171726,  0.34308678,  -0.90550417, 0.203841,    0.02521433,  -1.7966009,  -1.4287543,
        0.3222213,   1.0590587,   -1.7948701,  -1.7195907,  -0.9120889,  -0.9391962,  -0.2566791,
        -0.5464537,  1.4351872,   0.5705938,   -0.30327085};
    const ml::Operand b =
        utils::BuildConstant(builder, {3, 4, 5}, bData.data(), bData.size() * sizeof(float));
    const ml::Operand c = builder.Add(a, b);
    const ml::Graph graph = utils::Build(builder, {{"c", c}});
    ASSERT_TRUE(graph);
    const std::vector<float> dataA = {
        0.08939514,  -1.5887482,  0.8545348,   0.20523034, -0.41728342, 1.01752,     0.19677015,
        0.5398451,   0.56893295,  1.2511084,   2.0092728,  1.0606714,   0.4893267,   0.09536829,
        -2.3467007,  2.4527607,   0.61307395,  -1.0799897, -0.15071101, -0.48422927, -0.20479254,
        0.32798728,  -0.37435308, -1.7116562,  1.6952512,  -0.7479369,  -0.09019202, 0.14343949,
        1.6754607,   1.6427531,   0.9470988,   0.20872667, -1.9530525,  -0.21783416, 0.0309498,
        0.3008434,   1.1686599,   1.4920886,   0.06633294, 0.6674667,   0.60627925,  0.04302086,
        -0.03482966, -0.7343786,  -0.76851964, 0.9446942,  -0.35489243, 0.44452578,  0.00648887,
        -0.55656946, -0.735903,   0.22050636,  -0.5008282, -1.3132697,  1.6642882,   -0.48397836,
        0.20099205,  -0.28786168, 1.3315053,   -0.41619393};
    const std::vector<float> result(utils::SizeOfShape({3, 4, 5}));
    utils::Compute(graph, {{"a", dataA}}, {{"c", result}});
    const std::vector<float> expectedValue(
        {-0.48879138, -2.0812354,  0.6382897,   0.07346585,  -0.93846387, 2.9300475,   0.84765005,
         1.2585825,   -1.7465117,  2.0591164,   2.3115096,   1.2746171,   -0.16182771, 0.29538065,
         -2.4271188,  3.565543,    -0.90866506, -0.3550349,  -1.0703243,  -1.3159803,  -1.6617002,
         -0.21378079, -2.022044,   -1.5892761,  3.9158692,   -0.8970859,  0.6888581,   -0.04367155,
         0.681307,    -0.18579888, -0.41325122, 0.7814354,   0.56831384,  -0.54455364, 0.8741037,
         0.42421746,  2.1704698,   1.2573937,   -0.38674217, 0.7598469,   1.3951304,   0.15409374,
         0.4468876,   -0.3912918,  -1.6740239,  1.1485353,   -0.32967812, -1.3520751,  -1.4222654,
         -0.23434815, 0.32315564,  -1.5743638,  -2.220419,   -2.2253585,  0.72509193,  -0.74065745,
         -0.34546167, 1.1473255,   1.9020991,   -0.7194648});
    EXPECT_TRUE(utils::CheckValue(result, expectedValue));
}