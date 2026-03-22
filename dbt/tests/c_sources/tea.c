void _start() {
  volatile unsigned int* data = (unsigned int*)0x00000100;
  volatile unsigned int* key  = (unsigned int*)0x00000108;

  data[0] = 0x4869204D; data[1] = 0x49505321;
  key[0] = 0x01020304; key[1] = 0x05060708;
  key[2] = 0x090A0B0C; key[3] = 0x0D0E0F00;

  unsigned int orig_v0 = data[0], orig_v1 = data[1];
  unsigned int k0 = key[0], k1 = key[1], k2 = key[2], k3 = key[3];

  for (int j = 0; j < 12345; j++) {
      unsigned int v0 = orig_v0;
      unsigned int v1 = orig_v1;
      unsigned int sum = 0, delta = 0x9E3779B9;

      for (int i = 0; i < 32; i++) {
          sum += delta;
          v0 += ((v1<<4) + k0) ^ (v1 + sum) ^ ((v1>>5) + k1);
          v1 += ((v0<<4) + k2) ^ (v0 + sum) ^ ((v0>>5) + k3);
        }

      data[0] = v0;
      data[1] = v1;
    }
}