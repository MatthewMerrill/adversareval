// tensorflow/cc/example/example.cc

#include <stdio.h>

#include <tensorflow/c/c_api.h>

int main() {
  printf("Hello, %s", TF_Version());
  return 0;
}
