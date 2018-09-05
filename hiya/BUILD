cc_library(
  name="hiya_lib",
  hdrs=glob(["**/*.hpp"]),
  srcs=glob(["**/*.cpp"]),
  deps=[
    "//bitboards",
    "//minimax",
  ],
  visibility = ["//visibility:public"],
)

cc_binary(
  name="hiya",
  deps=[":hiya_lib"],
)
