// static int readSector(unsigned int n, unsigned int bufAddr)
// {
//   int done = 0;
//   __asm__("pusha\n\t"
//           "movl $0x3F6, %edx\n\t"
//           "movl $2, %eax\n\t"
//           "out %al, %dx\n\t");

//   if (hdd0Ready())
//   {
//     __asm__("movl %[arg_a], %%ecx\n\t"
//             "push %%ecx\n\t"
//             "shr $24, %%ecx\n\t"
//             "or $0xE0, %%cl\n\t"
//             "movl $0x1F6, %%edx\n\t"
//             "mov %%cl, %%al\n\t"
//             "out %%al, %%dx\n\t"

//             "pop %%eax\n\t"
//             "movl $0x1F3, %%edx\n\t"
//             "out %%al, %%dx\n\t"
//             "shr $8, %%eax\n\t"
//             "movl $0x1F4, %%edx\n\t"
//             "out %%al, %%dx\n\t"
//             "shr $8, %%eax\n\t"
//             "movl $0x1F5, %%edx\n\t"
//             "out %%al, %%dx\n\t"

//             "movl $1, %%eax\n\t"
//             "movl $0x1F2, %%edx\n\t"
//             "out %%al, %%dx\n\t"
//             "mov $0x20, %%al\n\t"
//             "movl $0x1F7, %%edx\n\t"
//             "out %%al, %%dx\n\t"

//             "movl $0x3F6, %%edx\n\t"
//             "_l001:\n\t"
//             "in %%dx,%%al\n\t"
//             "test $0x80,%%al\n\t"
//             "jnz _l001\n\t"

//             "movl $0x1F7, %%edx\n\t"
//             "_l002:\n\t"
//             "in %%dx,%%al\n\t"
//             "test $0x08,%%al\n\t"
//             "jz _l002\n\t"

//             "push %%es\n\t"
//             "movl $0x10, %%eax\n\t"
//             "movw %%ax, %%es\n\t"
//             "movl %[arg_b], %%edi\n\t"
//             "cld\n\t"
//             "movl $256, %%ecx\n\t"
//             "movl $0x1F0, %%edx\n\t"
//             "rep insw\n\t"

//             "movl $0x3F6, %%edx\n\t"
//             "movl $0, %%eax\n\t"
//             "out %%al, %%dx\n\t"

//             "pop %%es\n\t"
//             "popa\n\t"
//             :
//             : [arg_a] "m"(n), [arg_b] "m"(bufAddr)
//             : "cc");
//     done = 1;
//   }
//   return done;
// }

/*
static void test03()
{
  for (int i = 0; i < 1000; i++)
    print(str);
}

static void test01()
{

  videoMem[2000] = 'A';
  videoMem[2001] = 0x0A;
  // videoMem2[2002]='B';
  // videoMem2[2003]=0x0A;
}

static void test00()
{
  __asm__("movw $0x0A41,%ax\n\t"
          "movw %ax, %es:2000\n\t");
}

static void *start_os()
{
  return (void *)test;
}

static void test()
{
  videoMem[0] = str[0];
}

static void putChar(const char *s)
{
  videoMem[0] = 'A';
  videoMem[1] = 0x07;
  for (int i = 0; i < 15; i++)
  {
    videoMem[i * 2] = s[i];
    videoMem[i * 2 + 1] = 0x0F;
  }
}
*/