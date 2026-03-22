//
// Created by KiberPerdun on 3/4/26.
//
void _start() {
  volatile int* arr = (int*)0x00000100; // 256
  arr[0] = 50; arr[1] = 20; arr[2] = 40; arr[3] = 10; arr[4] = 30;

  for (int i = 0; i < 4; i++) {
      for (int j = 0; j < 4 - i; j++) {
          if (arr[j] > arr[j + 1]) {
              int temp = arr[j];
              arr[j] = arr[j + 1];
              arr[j + 1] = temp;
            }
        }
    }
}