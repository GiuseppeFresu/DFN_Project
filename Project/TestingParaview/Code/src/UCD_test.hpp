#ifndef __UCD_test_HPP__
#define __UCD_test_HPP__

#include "UCDUtilities.hpp"
#include <gtest/gtest.h>

// ***************************************************************************
TEST(TestUCDUtilities, UCDUtilities_Test0Ds)
{
  std::string exportFolder = "./";

  Gedim::UCDUtilities exporter;
  const Eigen::MatrixXd points = (Eigen::MatrixXd(3, 4)<< 0.0, 1.0, 1.0, 0.0,
                                  0.0, 0.0, 1.0, 1.0,
                                  2.0, 2.0, 2.0, 2.0).finished();

  exporter.ExportPoints(exportFolder + "/Geometry0Ds.inp",
                        points);

}
// ***************************************************************************
TEST(TestUCDUtilities, UCDUtilities_Test1Ds)
{
  std::string exportFolder = "./";

  Gedim::UCDUtilities exporter;
  const Eigen::MatrixXd points = (Eigen::MatrixXd(3, 4)<< 0.0, 1.0, 1.0, 0.0,
                                  0.0, 0.0, 1.0, 1.0,
                                  2.0, 2.0, 2.0, 2.0).finished();
  const Eigen::MatrixXi edges = (Eigen::MatrixXi(2, 5)<< 0, 1, 2, 3, 0,
                                 1, 2, 3, 0, 2).finished();

  exporter.ExportSegments(exportFolder + "/Geometry1Ds.inp",
                          points,
                          edges);
}
// ***************************************************************************
TEST(TestUCDUtilities, UCDUtilities_Test2Ds)
{
  std::string exportFolder = "./";

  Gedim::UCDUtilities exporter;
  const Eigen::MatrixXd points = (Eigen::MatrixXd(3, 8)<< 0.0, 1.0, 1.0, 0.0, 0.0, 1.0, 1.0, 0.0,
                                  0.0, 0.0, 1.0, 1.0, 0.0, 0.0, 1.0, 1.0,
                                  2.0, 2.0, 2.0, 2.0, 4.0, 4.0, 4.0, 4.0).finished();
  const std::vector<std::vector<unsigned int>> polygons =
  {
    { 0, 1, 2 },
    { 0, 2, 3 },
    { 4, 5, 6, 7 }
  };

  exporter.ExportPolygons(exportFolder + "/Geometry2Ds.inp",
                          points,
                          polygons);
}
// ***************************************************************************

#endif // __UCD_test_HPP__


/*
#ifndef __UCD_test_HPP__
#define __UCD_test_HPP__

#include "UCDUtilities.hpp"
#include <gtest/gtest.h>

TEST(TestUCDUtilities, ExportPolygonsTest)
{
    std::string exportFolder = "./";

    Gedim::UCDUtilities exporter;

    const Eigen::MatrixXd points1 = (Eigen::MatrixXd(3, 4)<<
                                     0.0, 1.0, 1.0, 0.0,
                                     0.0, 0.0, 1.0, 1.0,
                                     0.0, 0.0, 0.0, 0.0).finished();
    const Eigen::MatrixXd points2 = (Eigen::MatrixXd(3, 4)<<
                                     8.0000000000000004e-01, 8.0000000000000004e-01, 8.0000000000000004e-01, 8.0000000000000004e-01,
                                     0.0000000000000000e+00, 0.0000000000000000e+00, 1.0000000000000000e+00, 1.0000000000000000e+00,
                                     -1.0000000000000001e-01, 2.9999999999999999e-01, 2.9999999999999999e-01, -1.0000000000000001e-01).finished();
    const Eigen::MatrixXd points3 = (Eigen::MatrixXd(3, 4)<<
                                     -2.3777799999999999e-01, 3.1618370000000001e-01, 3.1618370000000001e-01, -2.3777799999999999e-01,
                                     5.0000000000000000e-01, 5.0000000000000000e-01, 5.0000000000000000e-01, 5.0000000000000000e-01,
                                     -3.4444000000000002e-01, -3.4444000000000002e-01, 4.5283889999999999e-01, 4.5283889999999999e-01).finished();

    Eigen::MatrixXd points1_transposed = points1.transpose();
    Eigen::MatrixXd points2_transposed = points2.transpose();
    Eigen::MatrixXd points3_transposed = points3.transpose();

    std::vector<std::vector<unsigned int>> polygons_vertices =
    {
        {0, 1, 2, 3},
        {4, 5, 6, 7},
        {8, 9, 10, 11}
    };

   exporter.ExportPolygons(exportFolder + "/Polygons1.inp", points1_transposed, polygons_vertices);
   exporter.ExportPolygons(exportFolder + "/Polygons2.inp", points2_transposed, polygons_vertices);
   exporter.ExportPolygons(exportFolder + "/Polygons3.inp", points3_transposed, polygons_vertices);
}

#endif // __UCD_test_HPP__




/*

#ifndef __UCD_test_HPP__
#define __UCD_test_HPP__

#include "UCDUtilities.hpp"
#include <gtest/gtest.h>

TEST(TestUCDUtilities, UCDUtilities_Test0Ds)
{
    std::string exportFolder = "./";

    Gedim::UCDUtilities exporter;
    const Eigen::MatrixXd points1 = (Eigen::MatrixXd(3, 4)<<
                                     0.0000000000000000e+00, 1.0000000000000000e+00, 1.0000000000000000e+00, 0.0000000000000000e+00,
                                     0.0000000000000000e+00, 0.0000000000000000e+00, 1.0000000000000000e+00, 1.0000000000000000e+00,
                                     0.0000000000000000e+00, 0.0000000000000000e+00, 0.0000000000000000e+00, 0.0000000000000000e+00).finished();
    const Eigen::MatrixXd points2 = (Eigen::MatrixXd(3, 4)<<
                                     8.0000000000000004e-01, 8.0000000000000004e-01, 8.0000000000000004e-01, 8.0000000000000004e-01,
                                     0.0000000000000000e+00, 0.0000000000000000e+00, 1.0000000000000000e+00, 1.0000000000000000e+00,
                                     -1.0000000000000001e-01, 2.9999999999999999e-01, 2.9999999999999999e-01, -1.0000000000000001e-01).finished();
    const Eigen::MatrixXd points3 = (Eigen::MatrixXd(3, 4)<<
                                     -2.3777799999999999e-01, 3.1618370000000001e-01, 3.1618370000000001e-01, -2.3777799999999999e-01,
                                     5.0000000000000000e-01, 5.0000000000000000e-01, 5.0000000000000000e-01, 5.0000000000000000e-01,
                                     -3.4444000000000002e-01, -3.4444000000000002e-01, 4.5283889999999999e-01, 4.5283889999999999e-01).finished();


    exporter.ExportPoints(exportFolder + "/Geometry10Ds.inp",
                          points1);
    exporter.ExportPoints(exportFolder + "/Geometry20Ds.inp",
                          points2);
    exporter.ExportPoints(exportFolder + "/Geometry30Ds.inp",
                          points3);


}


#endif // __UCD_test_HPP__
/*
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


#endif // __UCD_test_HPP__



 * // ***************************************************************************
TEST(TestUCDUtilities, UCDUtilities_Test0Ds)
{
    std::string exportFolder = "./";

    Gedim::UCDUtilities exporter;
    const Eigen::MatrixXd points1 = (Eigen::MatrixXd(3, 4)<<
                                     0.0000000000000000e+00, 1.0000000000000000e+00, 1.0000000000000000e+00, 0.0000000000000000e+00,
                                     0.0000000000000000e+00, 0.0000000000000000e+00, 1.0000000000000000e+00, 1.0000000000000000e+00,
                                     0.0000000000000000e+00, 0.0000000000000000e+00, 0.0000000000000000e+00, 0.0000000000000000e+00).finished();
    const Eigen::MatrixXd points2 = (Eigen::MatrixXd(3, 4)<<
                                     8.0000000000000004e-01, 8.0000000000000004e-01, 8.0000000000000004e-01, 8.0000000000000004e-01,
                                     0.0000000000000000e+00, 0.0000000000000000e+00, 1.0000000000000000e+00, 1.0000000000000000e+00,
                                     -1.0000000000000001e-01, 2.9999999999999999e-01, 2.9999999999999999e-01, -1.0000000000000001e-01).finished();
    const Eigen::MatrixXd points3 = (Eigen::MatrixXd(3, 4)<<
                                     -2.3777799999999999e-01, 3.1618370000000001e-01, 3.1618370000000001e-01, -2.3777799999999999e-01,
                                     5.0000000000000000e-01, 5.0000000000000000e-01, 5.0000000000000000e-01, 5.0000000000000000e-01,
                                     -3.4444000000000002e-01, -3.4444000000000002e-01, 4.5283889999999999e-01, 4.5283889999999999e-01).finished();


    exporter.ExportPoints(exportFolder + "/Geometry10Ds.inp",
                          points1);
    exporter.ExportPoints(exportFolder + "/Geometry20Ds.inp",
                          points2);
    exporter.ExportPoints(exportFolder + "/Geometry30Ds.inp",
                          points3);


}*/
