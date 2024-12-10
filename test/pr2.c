static char *videoMem = (char *)0xB8000;
static char str[15]="Hello World!!!";
static void putChar();
static void test();

static int add2(int a)
{
  return a+5;
}

static void test00()
{
__asm__("movw $0x0A41,%ax\n\t"
	"movw %ax, %es:2000\n\t"
);


}

static void *start_os()
{
  return (void *)test;
}


static void test(){
  videoMem[4001]=add2(0x41);
  videoMem[0]=str[0];
}

static void putChar(const char *s)
{
  videoMem[0]='A';
  videoMem[1]=0x07;
  for(int i=0;i<15;i++)
  {
    videoMem[i*2]=s[i];
    videoMem[i*2+1]=0x0F;
  }
}


