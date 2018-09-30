#include <iostream>
#include <thread>
#include <unistd.h>

static void printProgress(int t) {
  std::cout << "\x1B[25;1H\x1B[2KTh";
  for (int i = 0; i < 73; ++i) {
    std::cout << ((t * 73 >= i * 5000) ? "i" : " ");
  }
  std::cout << "nking" << std::endl;
}

static void progressThread() {
  int waitTime = 5000;
  int step = 50;
  for (int t = 0; t < waitTime; t += step) {
    printProgress(t);
    usleep(step * 1000);
  }
  std::cout << "\x1B[25;1H\x1B[2KDone Thinking!" << std::endl;
}

struct ProgBar {

  std::thread thread;

  ProgBar() : thread(progressThread) {
    //thread.detach();
  }

  void ForceStop() {
    thread.join();
  }

};
