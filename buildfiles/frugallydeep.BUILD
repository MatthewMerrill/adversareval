cc_library(
  name = "frugallydeep",
  srcs = glob(["include/**/*.hpp"]),
  hdrs = glob(["include/**/*.hpp"]),
  deps = [
    "@eigen//:eigen",
    "@fplus//:fplus",
    "@json//:json",
  ],
  copts = [
    "-Iinclude",
    "-Wall", "-Wextra", "-pedantic", "-Werror", "-Weffc+", "-Wconversion", "-Wsign-conversion",
    "-Wctor-dtor-privacy", "-Wreorder", "-Wold-style-cast", "-Wparenthesis",
    "-std=c++17",
  ],
  includes = ["include/"],
  visibility = ["//visibility:public"],
)

