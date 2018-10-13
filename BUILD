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
  name = "winprobgen",
  srcs =
    glob([
        "winprobgen/**/*.cpp",
        "winprobgen/**/*.hpp",
        "hotwheels/**/*.cpp",
        "hotwheels/**/*.hpp",
        "minimax/**/*.cpp",
        "minimax/**/*.hpp",
        "ansi/**/*.cpp",
        "ansi/**/*.hpp",
    ],
      exclude=["hotwheels/main.cpp", "hotwheels/evaluate.cpp"]),
  linkopts = ["-pthread"],
  copts = ["-Wall", "-Ihotwheels", "-Iwinprobgen", "-Iminimax", "-Iansi", "-std=c++11"],
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
        "transpositiontables/**/*.cpp",
        "transpositiontables/**/*.hpp",
    ]),
  linkopts = ["-pthread"],
  copts = [
      "-Ihotwheels",
      "-Iminimax",
      "-Itranspositiontables",
      "-Iansi",
      "-std=c++11"
      ],
)

cc_binary(
  name = "nn",
  srcs = ["nn/nn.c"],
  deps = ["@libtensorflow//:libtensorflow"],
  copts = ["-Iexternal/libtensorflow/include"],
)

py_binary(
  name = "trainnn",
  main = "nn/train.py",
  srcs = ["nn/train.py"],
)


cc_test(
  name = "transpositiontables_test",
  srcs =
    glob([
        "hotwheels/**/*.cpp",
        "hotwheels/**/*.hpp",
        "minimax/**/*.cpp",
        "minimax/**/*.hpp",
        "ansi/**/*.cpp",
        "ansi/**/*.hpp",
        "transpositiontables/**/*.cpp",
        "transpositiontables/**/*.hpp",
        "test/transpositiontables/**/*_test.cpp",
    ], exclude=["hotwheels/main.cpp"]),
  copts = [
      "-Ihotwheels",
      "-Iminimax",
      "-Itranspositiontables",
      "-Iansi",
      "-Iexternal/gtest/include",
      "-std=c++11",
      ],
  deps = [
      "@gtest//:gtest_main",
  ],
)
