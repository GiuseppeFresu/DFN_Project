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
}
#endif
