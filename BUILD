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

