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
        bool expected_isTip = false;

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

    TEST(FractureTest, WriteResultsTest)
    {
        // Creo un oggetto Fractures con dati di esempio
        Fractures fractures;
        fractures.NumberFractures = 3;
        fractures.FracturesId = {0, 1, 2};

        // Definizione dei punti che individuano le due tracce
        Point p1 = {0.8, 0, 0};
        Point p2 = {0.8544, 0, 0};
        Point p3 = {0.591584, 0, 0};
        Point p4 = {0.553659, 0, 0};

        // creo le tracce
        fractures.Traces.emplace_back(0, 0, 1, p1, p2, false);
        fractures.Traces.emplace_back(1, 0, 2, p3, p4, true);

        // Calcolo della lunghezza delle tracce (fatta nel costruttore)
        fractures.Traces[0].length = sqrt(pow(p2.x - p1.x, 2) + pow(p2.y - p1.y, 2) + pow(p2.z - p1.z, 2));
        fractures.Traces[1].length = sqrt(pow(p4.x - p3.x, 2) + pow(p4.y - p3.y, 2) + pow(p4.z - p3.z, 2));

        string filename = "test_results.txt";

        writeResults(fractures, filename);

        ifstream inFile(filename);
        ASSERT_TRUE(inFile.is_open());

        // Verificare il contenuto del file
        string line;

        // Verifica dell'intestazione del file
        getline(inFile, line);
        EXPECT_EQ(line, "# FractureId; NumTraces");

        // Fracture 0
        getline(inFile, line);
        EXPECT_EQ(line, "0; 2");
        getline(inFile, line);
        EXPECT_EQ(line, "# TraceId; Tips; Length");
        getline(inFile, line);
        EXPECT_EQ(line, "0; false; 0.0544004");
        getline(inFile, line);
        EXPECT_EQ(line, "1; true; 0.0379253");

        // Fracture 1
        getline(inFile, line);
        EXPECT_EQ(line, "1; 1");
        getline(inFile, line);
        EXPECT_EQ(line, "# TraceId; Tips; Length");
        getline(inFile, line);
        EXPECT_EQ(line, "0; false; 0.0544004");

        // Fracture 2
        getline(inFile, line);
        EXPECT_EQ(line, "2; 1");
        getline(inFile, line);
        EXPECT_EQ(line, "# TraceId; Tips; Length");
        getline(inFile, line);
        EXPECT_EQ(line, "1; true; 0.0379253");

        inFile.close();
        remove(filename.c_str());
    }

    TEST(FractureTest, SortTracesByLengthTest)
    {
        vector<Trace> traces;

        Point p1 = {0, 0, 0};
        Point p2 = {1, 0, 0};
        Point p3 = {0, 2, 0};
        Point p4 = {0, 0, 3};
        Point p5 = {4, 0, 0};
        Point p6 = {0, 5, 0};

        // Creo delle tracce con lunghezze diverse
        traces.emplace_back(0, 0, 1, p1, p2, false); // Lunghezza: 1.0
        traces.emplace_back(2, 0, 2, p1, p3, true);  // Lunghezza: 2.0
        traces.emplace_back(4, 0, 1, p1, p4, false); // Lunghezza: 3.0
        traces.emplace_back(3, 1, 2, p1, p5, false); // Lunghezza: 4.0
        traces.emplace_back(1, 1, 2, p1, p6, true);  // Lunghezza: 5.0

        sortTracesByLength(traces);

        //verifico che vettore abbia stesso numero di tracce
        ASSERT_EQ(traces.size(), 5);

        // Verifico che le tracce siano ordinate correttamente in ordine decrescente di lunghezza
        ASSERT_EQ(traces[0].traceId, 1); // Lunghezza: 5.0
        ASSERT_EQ(traces[1].traceId, 3); // Lunghezza: 4.0
        ASSERT_EQ(traces[2].traceId, 4); // Lunghezza: 3.0
        ASSERT_EQ(traces[3].traceId, 2); // Lunghezza: 2.0
        ASSERT_EQ(traces[4].traceId, 0); // Lunghezza: 1.0

    }
}
#endif
