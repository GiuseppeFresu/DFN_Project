#include <gtest/gtest.h>
#include "src_test/DFN_Test.hpp"
#include "UCD_test.hpp"

int main(int argc, char **argv)
{
    ::testing::InitGoogleTest(&argc, argv);

    return RUN_ALL_TESTS();
}
