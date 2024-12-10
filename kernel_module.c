static char *videoMem = (char *)0xA8000; // B8000-10000
static char str[15] = "Hello World! ";
static unsigned int cursX = 0, cursY = 0, textAttr = 0x07;

static void putChar();
static void test();
static void setCursorPosition(int, int);
static void print(char *c);
static void test03();

// function 0
static void print(char *c)
{
  int i = 0;
  int vadr = (80 * cursY + cursX) * 2;
  char *ta = (char *)&textAttr;
  while (c[i] != 0)
  {
    videoMem[vadr + i * 2] = c[i];
    videoMem[vadr + i * 2 + 1] = *ta;
    i++;
    cursX++;
    if (cursX == 80)
    {
      cursX = 0;
      cursY++;
    }
    if (cursY == 25)
    {
      cursY = 24;
      int *vm = (int *)videoMem;
      for (int j = 0; j < 960; j++)
      {
        vm[j] = vm[j + 40];
      }
      for (int j = 960; j < 1000; j++)
      {
        vm[j] = 0x07000700;
      }
      vadr = 160 * 24 - i * 2;
    }
  }
  setCursorPosition(cursX, cursY);
}

// function 1
static void setCursorPosition(int x, int y)
{
  int c = y * 80 + x;
  cursX = x;
  cursY = y;
  __asm__("movl %0, %%eax\n\t"
          "movl %%eax,%%ecx\n\t"
          "movw $0x3D4,%%dx\n\t"
          "movb $0x0F,%%al\n\t"
          "out %%al,%%dx\n\t"
          "movl %%ecx,%%eax\n\t"
          "movw $0x3D5,%%dx\n\t"
          "out %%al,%%dx\n\t"

          "movw $0x3D4,%%dx\n\t"
          "movb $0x0E,%%al\n\t"
          "out %%al,%%dx\n\t"
          "movl %%ecx,%%eax\n\t"
          "shrl $0x8,%%eax\n\t"
          "movw $0x3D5,%%dx\n\t"
          "out %%al,%%dx\n\t"
          :
          : "r"(c)
          : "eax");
}

// function 2
static void setTextAttribute(int attr)
{
  textAttr = attr;
}

// function 3

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
