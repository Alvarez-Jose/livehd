#  This file is distributed under the BSD 3-Clause License. See LICENSE for details.

cc_library(
    name = "archfp",
    srcs = glob(
        ["*.cpp"],
    ),
    hdrs = glob(["*.hpp"]),
    includes = ["."],
    visibility = ["//visibility:public"],
    copts = ["-O3", "-Wno-error"],
    deps = [
        "@com_google_absl//absl/container:flat_hash_map",
        ],
    )

cc_test(
    name = "floorplan_test",
    srcs = ["tests/archfp_test.cpp"],
    deps = [
        "@gtest//:gtest_main",
        ":archfp",
        ],
    )

# makefile and other non-source files are kept but not used.