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
  name = "featureeng",
  srcs = glob([
        "hotwheels/**/*.cpp",
        "hotwheels/**/*.hpp",
        "minimax/**/*.cpp",
        "minimax/**/*.hpp",
        "ansi/**/*.cpp",
        "ansi/**/*.hpp",
        "nn/featureeng.cpp",
        "transpositiontables/**/*.cpp",
        "transpositiontables/**/*.hpp",
        ], exclude = ["hotwheels/main.cpp", "hotwheels/evaluate_nn.cpp"]),
  linkopts = ["-pthread"],
  copts = [
      "-Ihotwheels",
      "-Iminimax",
      "-Itranspositiontables",
      "-Iansi",
      "-std=c++11",
      ],
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
    ], exclude=["hotwheels/evaluate_nn.cpp"]),
  linkopts = ["-pthread"],
  copts = [
      "-Ihotwheels",
      "-Iminimax",
      "-Itranspositiontables",
      "-Iansi",
      "-std=c++11",
      ],
)

cc_binary(
  name = "hotwheels_nn",
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
        "nn/**/*.cpp",
        "nn/**/*.hpp",
    ], exclude=["hotwheels/evaluate.cpp", "nn/featureeng.cpp"]),
  linkopts = ["-pthread"],
  data = glob(["models/**/*.pb", "models/**/*.json"]),
  deps = ["@frugallydeep//:frugallydeep"],
  copts = [
      "-Ihotwheels",
      "-Iminimax",
      "-Itranspositiontables",
      "-Iansi",
      "-Inn",
      #"-Iexternal/frugallydeep/include",
      "-std=c++14",
      "-D NN_EVAL"
      ],
)

cc_binary(
  name = "nn",
  srcs = ["nn/nn.c"],
  data = glob(["models/**/*.pb"]),
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
    ], exclude=["hotwheels/main.cpp", "hotwheels/evaluate_nn.cpp"]),
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
