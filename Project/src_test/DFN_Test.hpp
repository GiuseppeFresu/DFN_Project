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

        ASSERT_EQ(fractures.Traces.size(), 2);
        Trace trace1 = fractures.Traces[0];
        Trace trace2 = fractures.Traces[1];

        EXPECT_EQ(trace1.fractureId1, 0);
        EXPECT_TRUE((trace1.fractureId2 == 1) || (trace1.fractureId2 == 2));

        EXPECT_EQ(trace2.fractureId1, 0);
        EXPECT_TRUE((trace2.fractureId2 == 1) || (trace2.fractureId2 == 2));
        EXPECT_NE(trace1.fractureId2, trace2.fractureId2);
    }

    TEST(FRACTURESTEST, TestWriteTraces)
    {
        Fractures fractures;
        fractures.NumberFractures = 2;
        fractures.FracturesId = {0, 1};

        Point p1 = {0.1, 0.2, 0.3};
        Point p2 = {0.4, 0.5, 0.6};
        Point p3 = {1.1, 1.2, 1.3};
        Point p4 = {1.4, 1.5, 1.6};

        fractures.Traces.emplace_back(0, 0, 1, p1, p2, false);
        fractures.Traces.emplace_back(1, 0, 1, p3, p4, true);

        string filename = "test_traces_output.txt";

        writeTraces(fractures, filename);

        ifstream outFile(filename);
        ASSERT_TRUE(outFile.is_open());

        string line;

        getline(outFile, line);
        EXPECT_EQ(line, "# Number of Traces");

        getline(outFile, line);
        EXPECT_EQ(line, to_string(fractures.Traces.size()));

        getline(outFile, line);
        for (const auto& trace : fractures.Traces)
        {
            getline(outFile, line);
            stringstream ss(line);
            int traceId, f1, f2;
            double x1, y1, z1, x2, y2, z2;
            char delimiter;

            ss >> traceId >> delimiter >> f1 >> delimiter >> f2 >> delimiter
               >> x1 >> delimiter >> y1 >> delimiter >> z1 >> delimiter
               >> x2 >> delimiter >> y2 >> delimiter >> z2;

            EXPECT_EQ(traceId, trace.traceId);
            EXPECT_EQ(f1, trace.fractureId1);
            EXPECT_EQ(f2, trace.fractureId2);
            EXPECT_DOUBLE_EQ(x1, trace.p1.x);
            EXPECT_DOUBLE_EQ(y1, trace.p1.y);
            EXPECT_DOUBLE_EQ(z1, trace.p1.z);
            EXPECT_DOUBLE_EQ(x2, trace.p2.x);
            EXPECT_DOUBLE_EQ(y2, trace.p2.y);
            EXPECT_DOUBLE_EQ(z2, trace.p2.z);
        }

        outFile.close();

        remove(filename.c_str());
    }

/*
    TEST(FRACTURETEST, TestCalculateTraces)
    {
        Fractures fractures;

        Matrix3Xd P(3, 4);
        P << -2, -2, 2, 2,
             -2, 2, 2, -2,
             0, 0, 0, 0;

        Matrix3Xd Q(3,4);
        Q << 0, 0, 0, 0,
             -1, -1, 1, 1,
             -1, 1, 1, -1;

        int id1 = 1;
        int id2 = 2;
        int traceId = 0;
        double epsilon = 0.000000001;

        Trace trace = calculateTrace(P, Q, id1, id2, traceId, epsilon);
        EXPECT_EQ(trace.traceId, 0);
        EXPECT_EQ(trace.fractureId1, 1);
        EXPECT_EQ(trace.fractureId2, 2);
        EXPECT_EQ(trace.Tips, false);

        Point expectedP1(0, -1, 0);
        Point expectedP2(0, 1, 0);
        EXPECT_TRUE((trace.p1.x == expectedP1.x) && (trace.p1.y == expectedP1.y) && (trace.p1.z == expectedP1.z));
        EXPECT_TRUE((trace.p2.x == expectedP2.x) && (trace.p2.y == expectedP2.y) && (trace.p2.z == expectedP2.z));
    }

*/

    TEST(FRACTURESTEST, SortTracesByLength)
    {
        vector<Trace> traces;

        Point p1 = {0, 0, 0};
        Point p2 = {1, 0, 0};
        Point p3 = {0, 2, 0};
        Point p4 = {0, 0, 3};
        Point p5 = {4, 0, 0};
        Point p6 = {0, 5, 0};

        traces.emplace_back(0, 0, 1, p1, p2, false);
        traces.emplace_back(2, 0, 2, p1, p3, true);
        traces.emplace_back(4, 0, 1, p1, p4, false);
        traces.emplace_back(3, 1, 2, p1, p5, false);
        traces.emplace_back(1, 1, 2, p1, p6, true);

        sortTracesByLength(traces);

        ASSERT_EQ(traces.size(), 5);


        ASSERT_EQ(traces[0].traceId, 1);
        ASSERT_EQ(traces[1].traceId, 3);
        ASSERT_EQ(traces[2].traceId, 4);
        ASSERT_EQ(traces[3].traceId, 2);
        ASSERT_EQ(traces[4].traceId, 0);

    }

    TEST(FRACTURESTEST, WriteResults)
       {
           Fractures fractures;
           fractures.NumberFractures = 3;
           fractures.FracturesId = {0, 1, 2};

           Point p1 = {0, 0, -0};
           Point p2 = {1.6, 0, 0};
           Point p3 = {0, 0, -0};
           Point p4 = {0, 1, 0};

           fractures.Traces.emplace_back(0, 0, 1, p1, p2, true);
           fractures.Traces.emplace_back(1, 0, 2, p3, p4, false);

           fractures.Traces[0].length = sqrt(pow(p2.x - p1.x, 2) + pow(p2.y - p1.y, 2) + pow(p2.z - p1.z, 2));
           fractures.Traces[1].length = sqrt(pow(p4.x - p3.x, 2) + pow(p4.y - p3.y, 2) + pow(p4.z - p3.z, 2));

           string filename = "test_results.txt";

           writeResults(fractures, filename);

           ifstream inFile(filename);
           ASSERT_TRUE(inFile.is_open());

           string line;

           getline(inFile, line);
           EXPECT_EQ(line, "# FractureId; NumTraces");
           getline(inFile, line);
           EXPECT_EQ(line, "0; 2");
           getline(inFile, line);
           EXPECT_EQ(line, "# TraceId; Tips; Length");
           getline(inFile, line);
           EXPECT_EQ(line, "1; false; 1");
           getline(inFile, line);
           EXPECT_EQ(line, "0; true; 1.6");

           getline(inFile, line);
           EXPECT_EQ(line, "# FractureId; NumTraces");
           getline(inFile, line);
           EXPECT_EQ(line, "1; 1");
           getline(inFile, line);
           EXPECT_EQ(line, "# TraceId; Tips; Length");
           getline(inFile, line);
           EXPECT_EQ(line, "0; true; 1.6");

           getline(inFile, line);
           EXPECT_EQ(line, "# FractureId; NumTraces");
           getline(inFile, line);
           EXPECT_EQ(line, "2; 1");
           getline(inFile, line);
           EXPECT_EQ(line, "# TraceId; Tips; Length");
           getline(inFile, line);
           EXPECT_EQ(line, "1; false; 1");

           inFile.close();
           remove(filename.c_str());
       }
}
#endif
