#include <gtest/gtest.h>

#include "global.hpp"

TEST(Global, Global) {
    // Test the global variables
    printf("%s\n", detail::make_bit_enum_string<godot::GlobalType>("{enum}:{num},").c_str());
}

int main(int argc, char** argv) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}