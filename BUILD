cc_binary(
  name = "ansicompattest",
  srcs =
    glob([
        "ansicompattest/**/*.cpp",
        "ansi/**/*.cpp",
        "ansi/**/*.hpp",
    ]),
  copts = ["-Iansi", "-std=c++11"],
)

cc_binary(
  name = "tictactoe",
  srcs =
    glob([
        "tictactoe/**/*.cpp",
        "tictactoe/**/*.hpp",
        "minimax/**/*.cpp",
        "minimax/**/*.hpp",
    ]),
  copts = ["-Itictactoe", "-Iminimax", "-std=c++11"],
)

cc_binary(
  name = "hotwheels",
  srcs =
    glob([
        "hotwheels/**/*.cpp",
        "hotwheels/**/*.hpp",
        "minimax/**/*.cpp",
        "minimax/**/*.hpp",
        "ansi/**/*.cpp",
        "ansi/**/*.hpp",
    ]),
  linkopts = ["-pthread"],
  copts = ["-Ihotwheels", "-Iminimax", "-Iansi", "-std=c++11"],
)

cc_binary(
  name = "nn",
  srcs = ["nn/nn.c"],
  deps = ["@libtensorflow//:libtensorflow"],
  copts = ["-Iinclude"],
)

cc_test(
  name = "transpositiontables_test",
  srcs =
    glob([
      "transpositiontables/**/*.cpp",
      "transpositiontables/**/*.hpp",
      "test/transpositiontables/**/*_test.cpp",
    ]),
  copts = ["-Iexternal/gtest/include"],
  deps = [
      "@gtest//:gtest_main",
  ],
)
