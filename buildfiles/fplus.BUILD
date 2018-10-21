cc_library(
  name = "fplus",
  hdrs = glob(["include/**/*"]),
  copts = [
    "-Iinclude", "-Iinclude/fplus",
    "-Wall", "-Wextra", "-pedantic", "-Werror", "-Weffc+", "-Wconversion", "-Wsign-conversion",
    "-Wctor-dtor-privacy", "-Wreorder", "-Wold-style-cast", "-Wparenthesis",
    "-std=c++17",
  ],
  includes = ["include/", "include/fplus"],
  visibility = ["//visibility:public"],
)

