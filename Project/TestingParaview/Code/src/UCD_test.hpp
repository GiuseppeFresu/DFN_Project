#ifndef __UCD_test_HPP__
#define __UCD_test_HPP__

#include "UCDUtilities.hpp"
#include <gtest/gtest.h>


// ***************************************************************************

TEST(TestUCDUtilities, UCDUtilities_Test0Ds)
{
    std::string exportFolder = "./";

    Gedim::UCDUtilities exporter;
    const Eigen::MatrixXd points1 = (Eigen::MatrixXd(3, 4)<< 0.8, 1.0, 1.0, 0.8,
                                    0.0, 0.0, 1.0, 1.0,
                                    0.0, 0.0, 0.0, 0.0).finished();
    const Eigen::MatrixXd points2 = (Eigen::MatrixXd(3, 4)<< 0.0, 0.8, 0.8, 0.0,
                                    0.0, 0.0, 0.5, 0.5,
                                    0.0, 0.0, 0.0, 0.0).finished();
    const Eigen::MatrixXd points3 = (Eigen::MatrixXd(3, 4)<< 0.8, 0.8, 0.0, 0.0,
                                    0.5, 1.0, 1.0, 0.5,
                                    0.0, 0.0, 0.0, 0.0).finished();


    exporter.ExportPoints(exportFolder + "/Geometry10Ds.inp",
                          points1);
    exporter.ExportPoints(exportFolder + "/Geometry20Ds.inp",
                          points2);
    exporter.ExportPoints(exportFolder + "/Geometry30Ds.inp",
                          points3);


}

// ***************************************************************************

TEST(TestUCDUtilities, UCDUtilities_Test1Ds)
{
    std::string exportFolder = "./";

    Gedim::UCDUtilities exporter;
    const Eigen::MatrixXd points1 = (Eigen::MatrixXd(3, 4)<< 0.8, 1.0, 1.0, 0.8,
                                    0.0, 0.0, 1.0, 1.0,
                                    0.0, 0.0, 0.0, 0.0).finished();
    const Eigen::MatrixXd points2 = (Eigen::MatrixXd(3, 4)<< 0.0, 0.8, 0.8, 0.0,
                                    0.0, 0.0, 0.5, 0.5,
                                    0.0, 0.0, 0.0, 0.0).finished();
    const Eigen::MatrixXd points3 = (Eigen::MatrixXd(3, 4)<< 0.8, 0.8, 0.0, 0.0,
                                    0.5, 1.0, 1.0, 0.5,
                                    0.0, 0.0, 0.0, 0.0).finished();
    const Eigen::MatrixXi edges1 = (Eigen::MatrixXi(2, 4)<< 3, 2, 1, 0,
                                    2, 1, 0, 3).finished();
    const Eigen::MatrixXi edges2 = (Eigen::MatrixXi(2, 4)<< 3, 2, 1, 0,
                                   2, 1, 0, 3).finished();
    const Eigen::MatrixXi edges3 = (Eigen::MatrixXi(2, 4)<< 3, 2, 1, 0,
                                    2, 1, 0, 3).finished();

    exporter.ExportSegments(exportFolder + "/Geometry11Ds.inp",
                            points1,
                            edges1);
    exporter.ExportSegments(exportFolder + "/Geometry21Ds.inp",
                            points2,
                            edges2);
    exporter.ExportSegments(exportFolder + "/Geometry31Ds.inp",
                            points3,
                            edges3);
}

// ***************************************************************************

TEST(TestUCDUtilities, UCDUtilities_Test2Ds)
{
    std::string exportFolder = "./";

    Gedim::UCDUtilities exporter;
    const Eigen::MatrixXd points = (Eigen::MatrixXd(3, 8)<< 0.8, 1.0, 1.0, 0.8, 0.0, 0.8, 0.0, 0.0,
                                    0.0, 0.0, 1.0, 1.0, 0.0, 0.5, 0.5, 1.0,
                                    0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0).finished();
    const std::vector<std::vector<unsigned int>> polygons =
        {
            { 0, 1, 2, 3 },
            { 4, 0, 5, 6 },
            { 5, 3, 7, 6 }
        };

    exporter.ExportPolygons(exportFolder + "/Geometry2Ds.inp",
                            points,
                            polygons);
}

// ***************************************************************************

#endif // __UCD_test_HPP__
