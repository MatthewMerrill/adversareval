test_suite(
    name = "all_tests",
    tests = [
      ":bitboards_test",
      ":minimax_test",
    ]
)

cc_test(
    name = "bitboards_test",
    srcs = glob(["bitboards/**/*_test.cpp"]),
    copts = ["-Iexternal/gtest/include"],
    deps = [
        "@gtest//:gtest_main",
        "//bitboards:bitboards",
    ],
)

cc_test(
    name = "minimax_test",
    srcs = glob(["minimax/**/*_test.cpp"]),
    copts = ["-Iexternal/gtest/include"],
    deps = [
        "@gtest//:gtest_main",
        "//minimax:minimax",
    ],
)
