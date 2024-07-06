#ifndef __TESTDFN_H
#define __TESTDFN_H

#include <gtest/gtest.h>
#include "Fractures.hpp"
#include "Eigen/Eigen"
#include "Utils.hpp"
#include <iostream>

using namespace Eigen;
using namespace std;

namespace FractureLibrary
{
    TEST(FRACTURESTEST, TestImportFractures)
    {
        Fractures fractures;
        string filepath = "DFN/";
        string filename = "FR3_data.txt";
        bool flag=ImportFractures(filepath+"/FR3_data.txt", fractures);
        EXPECT_TRUE(flag);

        EXPECT_EQ(fractures.NumberFractures, 3);
        EXPECT_EQ(fractures.FracturesVertices[0].cols(), 4);
        EXPECT_EQ(fractures.FracturesVertices[1].cols(), 4);
        EXPECT_EQ(fractures.FracturesVertices[2].cols(), 4);
        EXPECT_EQ(fractures.FracturesVertices[0](0, 0), 0.0);
        EXPECT_EQ(fractures.FracturesVertices[0](1, 0), 0.0);
        EXPECT_EQ(fractures.FracturesVertices[0](2, 0), 0.0);
    }

    TEST(FRACTURESTEST, TestCheckIntersections)
    {
        Fractures fractures;
        string filepath = "DFN/";
        string filename = "FR3_data.txt";
        bool flag=ImportFractures(filepath+"/FR3_data.txt", fractures);
        EXPECT_TRUE(flag);

        map<int, vector<int>> intersections;
        checkIntersections(fractures, intersections);

        ASSERT_EQ(intersections.size(), 3);

        ASSERT_EQ(intersections[0].size(), 2);
        ASSERT_EQ(intersections[1].size(), 1);
        ASSERT_EQ(intersections[2].size(), 1);

        EXPECT_EQ(intersections[0][0], 1);
        EXPECT_EQ(intersections[0][1], 2);
        EXPECT_EQ(intersections[1][0], 0);
        EXPECT_EQ(intersections[2][0], 0);

        // Verify the traces
        ASSERT_EQ(fractures.Traces.size(), 2);
        Trace trace1 = fractures.Traces[0];
        Trace trace2 = fractures.Traces[1];

        EXPECT_EQ(trace1.fractureId1, 0);
        EXPECT_TRUE((trace1.fractureId2 == 1) || (trace1.fractureId2 == 2));

        EXPECT_EQ(trace2.fractureId1, 0);
        EXPECT_TRUE((trace2.fractureId2 == 1) || (trace2.fractureId2 == 2));
        EXPECT_NE(trace1.fractureId2, trace2.fractureId2);
    }

    TEST(FRACTURETEST, TestCalculateTraces)
    {
        Eigen::Matrix3Xd P(3,4);
        P<< 0, 2, 2, 0,
            0, 0, 1, 1,
            0, 0, 0, 0;

        Eigen::Matrix3Xd Q(3, 4);
        Q<< 1, 3, 3, 1,
            -1, -1, 2, 2,
            0, 0, 0, 0;

        int id1 = 1;
        int id2 = 2;
        int traceId = 1;

        Trace result = calculateTrace(P, Q, id1, id2, traceId);

        Point expected_pt1(1.0, 0.0, 0.0);
        Point expected_pt2(2.0, 0.0, 0.0);
        bool expected_isTip = true;

        EXPECT_EQ(result.p1,expected_pt1);
        EXPECT_EQ(result.p2,expected_pt2);
        EXPECT_EQ(result.Tips, expected_isTip);
    }

    TEST(FRACTURETEST, TestWriteTraces)
    {
        Fractures fractures;
        fractures.Traces.push_back({1, 101, 102, {1.0, 2.0, 3.0}, {4.0, 5.0, 6.0}, true});
        fractures.Traces.push_back({2, 103, 104, {7.0, 8.0, 9.0}, {10.0, 11.0, 12.0}, false});
        std::string testFilename = "test_output.txt";

        writeTraces(fractures, testFilename);





        std::string expectedContent =
            "# Number of Traces\n"
            "2\n"
            "# TraceId; FractureId1; FractureId2; X1; Y1; Z1; X2; Y2; Z2\n"
            "1; 101; 102; 1.0; 2.0; 3.0; 4.0; 5.0; 6.0\n"
            "2; 103; 104; 7.0; 8.0; 9.0; 10.0; 11.0; 12.0\n";

        EXPECT_EQ("test_output.txt",expectedContent);
    }
}
#endif
