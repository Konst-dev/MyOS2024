#define uint8_t unsigned char
#define uint16_t unsigned short int
#define uint32_t unsigned int
#define uint64_t unsigned long int

const uint32_t BUFFER_ADDRESS = 0x200000;         // буфер для различных нужд, везь мегабайт до 2FFFFF
const uint32_t PT_ADDRESS = 0x100000;             // Адрес таблицы разделов диска 64*4 байт. 16 дескрипторов раздела для 4 дисков
                                                  // до 0x1000FF
const uint32_t FS_DATA_ADDRESS = 0x100100;        // Адрес на массив со структурами бут-сокторов 16 возможных разделов
                                                  // до 1004FF (1024 байта)
const uint32_t MEMORY_BITMAP = 0x100500;          // Адрес карты свободных страниц(по 4кб) памяти. 1 бит на страницу.
                                                  // 1 - страница занята, 0 - свободна. До 1204FF
const uint32_t OPEN_FILES_DESCRIPTORS = 0x120500; // Дескрипторы открытых файлов по 28 байт. 1024 дескриптора
                                                  // до 1274FF
const uint32_t FAT_ADDRESS = 0x127500;            // Адрес таблицы FAT системного диска 128 кб
                                                  // до 1474FF

const uint32_t FREE_MEMORY_BEGINS = 0x400000; // начало области памяти, не занятой ОС. Здесь начинаются страницы памяти

static uint32_t avalableMemoryMb = 0;    // доступная оперативная память в Мб
static uint32_t firstAvailablePage = 0;  // первая свободная страница в памяти
static char *videoMem = (char *)0xA8000; // B8000-10000
static char str[15] = "Hello World! ";
static unsigned int cursX = 0, cursY = 0, textAttr = 0x07;
static char hexStr[17] = "0123456789ABCDEF";
static char *allMem = (char *)0;
static unsigned int sysTimer = 0;

typedef struct
{
  uint8_t active;
  uint8_t headsN;
  uint16_t cylN;
  uint8_t type;
  uint8_t endHeadsN;
  uint16_t endCylN;
  uint32_t firstSector;
  uint32_t sectorsN;
} PartitionTable;
PartitionTable *PT = (PartitionTable *)PT_ADDRESS;

#pragma pack(push, 1) // включаем выравнивание по 1 байту, иначе выравнивание по 32 бита
typedef struct
{
  char publisher[8];
  uint16_t bytesPerSector;
  uint8_t sectorsPerCluster;
  uint16_t resSect;
  uint8_t FATN;
  uint16_t descN;
  uint16_t sectN16;
  uint8_t driveType;
  uint16_t sectPerFAT;
  uint16_t sectPerTrack;
  uint16_t sidesN;
  uint32_t hiddenSectorsN;
  uint32_t sectN32;
  uint8_t driveNumber;
  uint8_t resNT;
  uint8_t extBR;
  uint32_t logicalDriveNumber;
  char driveLabel[11];
  char FSType[8];
  char res1[5]; // выравниваем до 64 байт
} FS_Data;
#pragma pack(pop)
FS_Data *FSInfo = (FS_Data *)FS_DATA_ADDRESS;

#pragma pack(push, 1) // включаем выравнивание по 1 байту, иначе выравнивание по 32 бита
typedef struct
{
  char filename[11];
  uint8_t attr;
  uint8_t resNT;
  uint8_t extTime;
  uint16_t fCreationTime;
  uint16_t fCreationDate;
  uint16_t lastUseDate;
  uint16_t clusterNHigh;
  uint16_t writeTime;
  uint16_t writeDate;
  uint16_t clusterNHLow;
  uint32_t size;
} FileDescriptorRecord;
#pragma pack(pop)
FileDescriptorRecord *FileDescriptorFS;

typedef struct
{
  uint32_t presence;     // присутствие дескриптора. 0 - дескриптор свободен
  uint16_t startCluster; // первый кластер файла
  uint16_t curCluster;   // текущий прочитанный кластер файла
  uint32_t filePtr;      // указатель позиции в файле
  uint32_t fileSize;     // размер файла
  uint32_t bufferPtr;    // адрес буфера, куда загружен очередной кластер
  uint32_t attr;         // атрибуты файла
  uint16_t dirCluster;   // первый кластер директории, где находится файл
  uint16_t reserved;
} FILE;

typedef struct
{
  uint32_t startPage;
  uint32_t pagesNumber;

} MemDesc;

static void setCursorPosition(int, int);
static int readSectors(unsigned int n, uint32_t number, unsigned int bufAddr);
static void print(char *c);
static void setTextAttribute(int attr);
static char *uintToStr(char *c, unsigned int n);
static char *intToStr(char *c, int n);
static void byteToHex(char *c, unsigned int n);
static void wordToHex(char *c, unsigned int n);
static void dwordToHex(char *c, unsigned int n);
static unsigned int getMem();
static void incSysTimer();
static void delayST(int t);
static int hdd0Ready();
static void readPT();
static void memCopy(uint32_t inAdr, uint32_t outAdr, uint32_t count);
static void readFSInfo();
static void fillMemZeros(uint32_t startAddr, uint32_t count);
static void println(char *c);
static int readCluster(uint32_t driveN, uint32_t clusterN, uint32_t addr);
static int readRootDir(uint32_t driveN, uint32_t addr);
static char *fileNameDSCToStr(char *s1, char *s2);
static void memoryBitMapInit();
static uint32_t pageIsAvalable(uint32_t n);
static void markPage(uint32_t n);
static void freePage(uint32_t n);
static uint32_t getFreeMemoryBlock(uint32_t memorySize);
static void deleteMemoryBlock(uint32_t startPage, uint32_t memorySize);
static uint32_t getPageAddress(uint32_t pageN);
static void __stack_chk_fail_local();
static int isCorrectSymbol(char s);
static int strlen(char *s);
static int strcmp(char *s1, char *s2);
static FileDescriptorRecord getFileDSCByPath(char *fn);
static void loadFAT();
static FileDescriptorRecord getfileInDirDSC(FileDescriptorRecord dirDSC, char *fn);

// static void readSectorTest();

// function 0
static char ramDetectedStr[22] = " Mb of RAM detected\n";
static char scmp[22] = " Mb of RAM detected\n";
static char space[3] = ": ";
static char newLine[2] = "\n";
char s1[12];
static void _start()
{
  static char *mm = (char *)0x100204;
  int m = *(int *)mm;
  int n = 0;

  avalableMemoryMb = getMem();
  memoryBitMapInit();
  uintToStr(s1, avalableMemoryMb);
  print(s1);
  print(ramDetectedStr);

  readPT();
  readFSInfo();
  loadFAT();

  uintToStr(s1, (PT + 0)->firstSector);
  println(s1);

  uintToStr(s1, (FSInfo + 0)->descN);
  println(s1);

  println(intToStr(s1, strcmp(scmp, space)));
  println(intToStr(s1, strcmp(space, scmp)));
  println(intToStr(s1, strcmp(ramDetectedStr, scmp)));

  FileDescriptorRecord fd;
  fd.clusterNHLow = 0;
  char fn[] = "/TEST/001/NC.ICO/";
  // char fn3[] = "NC.ICO";
  char fn2[13];
  //  fd = getfileInDirDSC(fd, fn);
  fd = getFileDSCByPath(fn);
  println(fileNameDSCToStr(fn2, fd.filename));
  // fd = getfileInDirDSC(fd, fn3);
  // println(fileNameDSCToStr(fn2, fd.filename));

  // FileDescriptorRecord fd = getFileDSC(s1);
  // println(intToStr(s1, fd.clusterNHigh));

  // uint32_t rootDirAddress = getPageAddress(getFreeMemoryBlock((FSInfo + 0)->descN * 512));
  // println(uintToStr(s1, rootDirAddress));
  // readRootDir(0, rootDirAddress);
  // FileDescriptorRecord *dr = (FileDescriptorRecord *)rootDirAddress;
  // char fn[13];
  // for (int i = 0; i < 512; i++)
  // {
  //   if (isCorrectSymbol((dr + i)->filename[0]))
  //     println(fileNameDSCToStr(fn, (dr + i)->filename));
  // }

  // for (int i = 0; i < 2000; i++)
  // {
  //   videoMem[i * 2] = allMem[rootDirAddress + i];
  //   videoMem[i * 2 + 1] = 7;
  // }
  // println(uintToStr(s1, md.pagesNumber));

  // markPage(0);
  // markPage(2);
  // markPage(5);

  // println(uintToStr(s1, allMem[MEMORY_BITMAP]));
  // uintToStr(s1, getFreeMemoryBlock(1024));
  // println(s1);
  // uintToStr(s1, allMem[MEMORY_BITMAP]);
  // println(s1);

  // uintToStr(s1, getFreeMemoryBlock(256000));
  // println(s1);

  // uintToStr(s1, getFreeMemoryBlock(4096));
  // println(s1);
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

/* function 3
на входе указатель на строку размером не менее 11 символов (В ней формируется число)
и целое 32-битное число
*/
static char *uintToStr(char *c, unsigned int n)
{
  c[10] = 0;
  int j = 9;
  do
  {
    c[j] = hexStr[n % 10];
    n /= 10;
    j--;
  } while (n != 0);
  if (j >= 0)
    for (int i = 0; i < 10 - j; i++)
    {
      c[i] = c[j + i + 1];
    }
  return c;
}

/* function 4
на входе указатель на строку размером не менее 12 символов (В ней формируется число)
и целое 32-битное знаковое число
*/
static char *intToStr(char *c, int n)
{
  c[11] = 0;
  int j = 10;
  int b = n < 0 ? 1 : 0;
  if (n < 0)
    n = -n;
  do
  {
    c[j] = hexStr[n % 10];
    n /= 10;
    j--;
  } while (n != 0);
  if (b)
  {
    c[j] = '-';
    j--;
  }
  if (j >= 0)
    for (int i = 0; i < 11 - j; i++)
    {
      c[i] = c[j + i + 1];
    }
  return c;
}

/* function 5
на входе указатель на строку размером не менее 3 символов (В ней формируется hex-число)
и целое 8-битное число
*/
static void byteToHex(char *c, unsigned int n)
{
  c[2] = 0;
  for (int i = 1; i >= 0; i--)
  {
    c[i] = hexStr[n % 16];
    n /= 16;
  }
}

/* function 6
на входе указатель на строку размером не менее 5 символов (В ней формируется hex-число)
и целое 16-битное число
*/
static void wordToHex(char *c, unsigned int n)
{
  c[4] = 0;
  for (int i = 3; i >= 0; i--)
  {
    c[i] = hexStr[n % 16];
    n /= 16;
  }
}

/* function 7
на входе указатель на строку размером не менее 9 символов (В ней формируется hex-число)
и целое 16-битное число
*/
static void dwordToHex(char *c, unsigned int n)
{
  c[8] = 0;
  for (int i = 7; i >= 0; i--)
  {
    c[i] = hexStr[n % 16];
    n /= 16;
  }
}

// function 8. Возвращает объем памяти в Мб
static unsigned int m = 1, p = 0x270000;
uint8_t a;
static unsigned int getMem()
{
  unsigned int m = 2;
  unsigned int p = 0x270000;
  allMem[p] = 0x1E;

  while (allMem[p] == 0x1E)
  {
    p += 0x100000;
    allMem[p] = 0x1E;
    m++;
  }
  return m;
}

// function 9. Печать строки
static void print(char *c)
{
  int i = 0;
  int vadr = (80 * cursY + cursX) * 2;
  char *ta = (char *)&textAttr;
  while (c[i] != 0)
  {
    if (c[i] != 10)
    {
      videoMem[vadr + i * 2] = c[i];
      videoMem[vadr + i * 2 + 1] = *ta;
      cursX++;
    }
    else
    {
      cursX = 0;
      cursY++;
      vadr = (80 * cursY + cursX) * 2 - i * 2;
    }

    i++;

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

// function 10. Дополнительный обработчик прерываний системного таймера
static void incSysTimer()
{
  sysTimer++;
}

// function 11. задержка на количество прерываний системного таймера
static void delayST(int t)
{
  int curTime = sysTimer + t;
  while (sysTimer < curTime)
    ;
}

// function 12. Проверка готовности HDD0;
static int hdd0Ready()
{
  int ready = 0;
  __asm__("pusha\n\t"
          "movl $4, %%ebx\n\t"
          "movl $0xE0, %%eax\n\t"
          "movl $0x1F6, %%edx\n\t"
          "out %%al, %%dx\n\t"
          "movl $0x1F7, %%edx\n\t"
          "_busy:\n\t"
          "xor %%ecx, %%ecx\n\t"
          "movl $0xFFFF, %%ecx\n\t"
          "_lp1:\n\t"
          "loop _lp1\n\t"
          "dec %%bx\n\t"
          "jz _stp\n\t"
          "in %%dx,%%al\n\t"
          "test $0b10001000,%%al\n\t"
          "jnz _busy\n\t"
          "test $0b01000000,%%al\n\t"
          "jz _busy\n\t"
          "movl $1, %0\n\t"

          "_stp:\n\t"
          "popa\n\t"
          : "=r"(ready)
          :
          : "cc");
  return ready;
}

// function 13. Читает сектор с диска. n-номер сектора, number - количество секторов, bufAddr-адрес буфера
static int readSectors(unsigned int n, uint32_t number, unsigned int bufAddr)
{
  int done = 0;
  __asm__("pusha\n\t"
          "movl $0x3F6, %edx\n\t"
          "movl $2, %eax\n\t"
          "out %al, %dx\n\t");

  if (hdd0Ready())
  {
    __asm__("movl %[arg_a], %%ecx\n\t"
            "push %%ecx\n\t"
            "shr $24, %%ecx\n\t"
            "or $0xE0, %%cl\n\t"
            "movl $0x1F6, %%edx\n\t"
            "mov %%cl, %%al\n\t"
            "out %%al, %%dx\n\t"

            "pop %%eax\n\t"
            "movl $0x1F3, %%edx\n\t"
            "out %%al, %%dx\n\t"
            "shr $8, %%eax\n\t"
            "movl $0x1F4, %%edx\n\t"
            "out %%al, %%dx\n\t"
            "shr $8, %%eax\n\t"
            "movl $0x1F5, %%edx\n\t"
            "out %%al, %%dx\n\t"

            "movl %[arg_c], %%eax\n\t"
            "movl $0x1F2, %%edx\n\t"
            "out %%al, %%dx\n\t"
            "mov $0xC4, %%al\n\t" // команда READMULTIPLE, 0x20 - READ
            "movl $0x1F7, %%edx\n\t"
            "out %%al, %%dx\n\t"

            "movl $0x3F6, %%edx\n\t"
            "_l0011:\n\t"
            "in %%dx,%%al\n\t"
            "test $0x80,%%al\n\t"
            "jnz _l0011\n\t"

            "movl $0x1F7, %%edx\n\t"
            "_l0021:\n\t"
            "in %%dx,%%al\n\t"
            "test $0x08,%%al\n\t"
            "jz _l0021\n\t"

            "push %%es\n\t"
            "movl $0x10, %%eax\n\t"
            "movw %%ax, %%es\n\t"
            "movl %[arg_b], %%edi\n\t"
            "cld\n\t"
            "movl %[arg_c], %%eax\n\t"
            "shl $8, %%eax\n\t"
            "movl %%eax, %%ecx\n\t"
            "movl $0x1F0, %%edx\n\t"
            "rep insw\n\t"

            "movl $0x3F6, %%edx\n\t"
            "movl $0, %%eax\n\t"
            "out %%al, %%dx\n\t"

            "pop %%es\n\t"
            "popa\n\t"
            :
            : [arg_a] "m"(n), [arg_b] "m"(bufAddr), [arg_c] "m"(number)
            : "cc");
    done = 1;
  }
  return done;
}

// 14 читает таблицу раздела
static void readPT()
{
  readSectors(0, 1, BUFFER_ADDRESS);
  fillMemZeros(PT_ADDRESS, 256);
  memCopy(BUFFER_ADDRESS + 0x1BE, PT_ADDRESS, 64);
}

// 15 копирует область памяти
static void memCopy(uint32_t inAdr, uint32_t outAdr, uint32_t count)
{
  for (uint32_t i = 0; i < count; i++)
    allMem[outAdr + i] = allMem[inAdr + i];
}

// 16 читает информацию о файловых системах разделов
static void readFSInfo()
{
  fillMemZeros(FS_DATA_ADDRESS, 1024);
  for (int i = 0; i < 16; i++)
  {
    int sN = (PT + i)->firstSector;
    if (sN > 0)
    {
      readSectors(sN, 1, BUFFER_ADDRESS);
      memCopy(BUFFER_ADDRESS + 3, FS_DATA_ADDRESS + i * 64, 59);
    }
  }
}

// 17 заполняет адресное простанство нулями
static void fillMemZeros(uint32_t startAddr, uint32_t count)
{
  for (int i = 0; i < count; i++)
    allMem[startAddr + i] = 0;
}
// 18 выводит строку и переводит курсор на новую строку
static void println(char *c)
{
  print(c);
  print(newLine);
}
// 19 читает кластер по его номеру. Информацию о разделе берет из дескриптора раздела.
static int readCluster(uint32_t driveN, uint32_t clusterN, uint32_t addr)
{
  int Done = 0;
  if (clusterN > 1)
  {
    int sn = (FSInfo + driveN)->sectorsPerCluster;
    int firstSector = (FSInfo + driveN)->hiddenSectorsN;
    firstSector += (FSInfo + driveN)->resSect;
    firstSector += (FSInfo + driveN)->FATN * (FSInfo + driveN)->sectPerFAT;
    firstSector += (FSInfo + driveN)->descN >> 4;
    firstSector += (clusterN - 2) * sn;
    Done = readSectors(firstSector, sn, addr);
  }
  return Done;
}

// 20 читает корневую директорию
static int readRootDir(uint32_t driveN, uint32_t addr)
{
  int Done = 0;

  int firstSector = (FSInfo + driveN)->hiddenSectorsN;
  firstSector += (FSInfo + driveN)->resSect;
  firstSector += (FSInfo + driveN)->FATN * (FSInfo + driveN)->sectPerFAT;
  int nn = (FSInfo + driveN)->descN >> 4;
  Done = readSectors(firstSector, nn, addr);

  return Done;
}

// 21 проверяет, корректный ли символ для имени файла

static int isCorrectSymbol(char s)
{
  int isCorrect = 0, i = 0;
  char correctSymbols[] = "ABCDEFGHIJKLMNOPQRSTUVWXYZ1234567890!#$@&()^_~`{}.";
  while (correctSymbols[i] != 0)
  {
    if (correctSymbols[i] == s)
    {
      isCorrect = 1;
      break;
    }
    i++;
  }
  return isCorrect;
}

// 22 преобразует строку с названием файла из дескриптора в стандартную строку

static char *fileNameDSCToStr(char *stdStr, char *dscStr)
{
  int n = 0;
  for (int i = 0; i < 8; i++)
  {
    if (isCorrectSymbol(dscStr[i]))
    {
      stdStr[n] = dscStr[i];
      n++;
    }
  }
  if (isCorrectSymbol(dscStr[8]))
  {
    stdStr[n++] = '.';
    for (int i = 8; i < 11; i++)
    {
      if (isCorrectSymbol(dscStr[i]))
      {
        stdStr[n] = dscStr[i];
        n++;
      }
    }
  }

  stdStr[n] = 0;
  return stdStr;
}

// 23 размечает битмап свободных страниц памяти
static void memoryBitMapInit()
{
  // находим первую недоступную страницу за пределами оперативной памяти
  // и заполняет весь битмап единицами с первой недоступной страницы
  uint32_t firstUnavalablePage = (avalableMemoryMb * 0x100000 - FREE_MEMORY_BEGINS - 0x10000) >> 12;
  uint32_t firstByte = firstUnavalablePage >> 3;
  uint32_t firstBit = firstUnavalablePage - (firstByte << 3);
  for (int i = 0; i < firstByte; i++)
    allMem[MEMORY_BITMAP + i] = 0;
  if (firstBit > 0)
  {
    uint8_t curByte = 1;
    curByte <<= firstBit;
    for (int i = firstBit + 1; i < 8; i++)
      curByte = curByte | (curByte << 1);
    allMem[MEMORY_BITMAP + firstByte] = curByte;
    firstByte++;
  }
  for (int i = firstByte; i < 0x20000; i++)
    allMem[MEMORY_BITMAP + i] = 0xFF;
  firstAvailablePage = 0;
}

// 24 проверяет, свободна ли n-я страница памяти

static uint32_t pageIsAvalable(uint32_t n)
{
  uint32_t byteN = n >> 3;
  uint32_t bitN = n - (byteN << 3);
  uint8_t curByte = 1;
  uint32_t res = 1;
  curByte = curByte << bitN;

  uint8_t cc = allMem[MEMORY_BITMAP + byteN];
  uint32_t res1 = cc & curByte;
  if (res1)
  {
    res = 0;
  }

  return res;
}

// 25 метит страницу занятой
static void markPage(uint32_t n)
{
  uint32_t byteN = n >> 3;
  uint32_t bitN = n - (byteN << 3);
  uint8_t curByte = 1;
  curByte <<= bitN;
  allMem[MEMORY_BITMAP + byteN] |= curByte;
}

// 26 метит страницу незанятой
static void freePage(uint32_t n)
{
  uint32_t byteN = n >> 3;
  uint32_t bitN = n - (byteN << 3);
  uint8_t curByte = 1;
  curByte <<= bitN;
  curByte = ~curByte;
  allMem[MEMORY_BITMAP + byteN] &= curByte;
}

// 27 Возвращает номер страницы с которой начинается запрашиваемый блок памяти в байтах

static uint32_t getFreeMemoryBlock(uint32_t memorySize)
{
  uint32_t pagesN = memorySize >> 12;
  if ((memorySize - pagesN << 12) > 0)
    pagesN++;
  uint32_t n = 0;
  uint32_t fstPage = 0;
  uint32_t curPage;
  while (n < pagesN && fstPage < 0x100000)
  {
    n = 0;
    curPage = fstPage;
    while (n < pagesN && pageIsAvalable(curPage) && curPage < 0x100000)
    {
      n++;
      curPage++;
    }
    if (n < pagesN)
      fstPage = curPage + 1;
  }
  if (n < pagesN)
    return 0;
  else
  {
    for (int i = 0; i < n; i++)
      markPage(fstPage + i);
    return fstPage;
  }
}

// 28. Освобождает страницы памяти, занятые memorySize байтами памяти, начиная со startPage
static void deleteMemoryBlock(uint32_t startPage, uint32_t memorySize)
{
  uint32_t pagesN = memorySize >> 12;
  if ((memorySize - pagesN << 12) > 0)
    pagesN++;

  for (int i = 0; i < pagesN; i++)
    freePage(startPage + i);
}

// 29 возвращает адрес страницы памяти
static uint32_t getPageAddress(uint32_t pageN)
{
  uint32_t addr = FREE_MEMORY_BEGINS + (pageN << 12);
  return addr;
}

// 30 заглушка для совместимости
static void __stack_chk_fail_local()
{
}

// 31 возвращает длину строки
static int strlen(char *s)
{
  int i = 0;
  while (s[i])
    i++;
  return i;
}

// 32 сравнивает строки
static int strcmp(char *s1, char *s2)
{
  int cmp = 0;
  if (strlen(s1) && strlen(s2))
  {
    int i = 0;
    while (s1[i] && s2[i])
    {
      if (s1[i] > s2[i])
      {
        cmp = 1;
        break;
      }
      else if (s1[i] < s2[i])
      {
        cmp = -1;
        break;
      }
      i++;
    }
    if (cmp == 0 && s1[i] == 0 && s2[i] == 0)
      return 0;
    else if (cmp == 0 && s1[i] > 0 && s2[i] == 0)
      return 1;
    else if (cmp == 0 && s1[i] == 0 && s2[i] > 0)
      return -1;
    else
      return cmp;
  }
  else if (strlen(s1) > 0 && strlen(s2) == 0)
    return 1;
  else if (strlen(s1) == 0 && strlen(s2) > 0)
    return -1;
  else
    return 0;
}

// 33 закрузка FAT системного диска
static void loadFAT()
{
  uint32_t n = (FSInfo + 0)->hiddenSectorsN + (FSInfo + 0)->resSect;
  uint32_t sectors = (FSInfo + 0)->sectPerFAT;
  readSectors(n, sectors, FAT_ADDRESS);
}

// 34 находит файл в папке, заданной дескриптором. Возвращает дескриптор файла
static FileDescriptorRecord getfileInDirDSC(FileDescriptorRecord dirDSC, char *fn)
{
  FileDescriptorRecord fdr;
  fdr.filename[0] = 0;
  uint16_t *FAT = (uint16_t *)FAT_ADDRESS;
  FileDescriptorRecord *fd;
  uint32_t DirAddress = 0, memSize = 0, firstPage = 0;
  uint32_t clusterSize = (FSInfo + 0)->sectorsPerCluster;
  clusterSize <<= 9;
  char fn2[13];
  if (dirDSC.clusterNHLow == 0)
  {
    memSize = (FSInfo + 0)->descN;
    memSize <<= 5;
    firstPage = getFreeMemoryBlock(memSize);
    DirAddress = getPageAddress(firstPage);
    readRootDir(0, DirAddress);
    fd = (FileDescriptorRecord *)DirAddress;
    uint32_t nn = 0;
    while ((fd + nn)->filename[0])
    {
      if (strcmp(fileNameDSCToStr(fn2, (fd + nn)->filename), fn) == 0)
      {
        fdr.attr = (fd + nn)->attr;
        fdr.clusterNHigh = (fd + nn)->clusterNHigh;
        fdr.clusterNHLow = (fd + nn)->clusterNHLow;
        fdr.extTime = (fd + nn)->extTime;
        fdr.fCreationDate = (fd + nn)->fCreationDate;
        fdr.fCreationTime = (fd + nn)->fCreationTime;
        // fdr.filename = (fd + nn)->attr;
        for (int i = 0; i < 11; i++)
          fdr.filename[i] = (fd + nn)->filename[i];
        fdr.lastUseDate = (fd + nn)->lastUseDate;
        fdr.size = (fd + nn)->size;
        fdr.writeDate = (fd + nn)->writeDate;
        fdr.writeTime = (fd + nn)->writeTime;
        return fdr;
      }
      nn++;
    }
    deleteMemoryBlock(firstPage, memSize);
  }
  else
  {
    uint16_t nCl = dirDSC.clusterNHLow;
    uint16_t nextCluster = FAT[nCl];
    firstPage = getFreeMemoryBlock(clusterSize);
    DirAddress = getPageAddress(firstPage);

    while (1)
    {
      readCluster(0, nCl, DirAddress);
      fd = (FileDescriptorRecord *)DirAddress;
      uint32_t nn = 0;
      while ((fd + nn)->filename[0])
      {
        if (strcmp(fileNameDSCToStr(fn2, (fd + nn)->filename), fn) == 0)
        {
          fdr.attr = (fd + nn)->attr;
          fdr.clusterNHigh = (fd + nn)->clusterNHigh;
          fdr.clusterNHLow = (fd + nn)->clusterNHLow;
          fdr.extTime = (fd + nn)->extTime;
          fdr.fCreationDate = (fd + nn)->fCreationDate;
          fdr.fCreationTime = (fd + nn)->fCreationTime;
          // fdr.filename = (fd + nn)->attr;
          for (int i = 0; i < 11; i++)
            fdr.filename[i] = (fd + nn)->filename[i];
          fdr.lastUseDate = (fd + nn)->lastUseDate;
          fdr.size = (fd + nn)->size;
          fdr.writeDate = (fd + nn)->writeDate;
          fdr.writeTime = (fd + nn)->writeTime;
          return fdr;
        }
        nn++;
      }
      if (nextCluster == 0xFFFF)
        break;
      else
      {
        nCl = nextCluster;
        nextCluster = FAT[nCl];
      }
    }
    deleteMemoryBlock(firstPage, clusterSize);
  }
  return fdr;
}

// 35 возвращает дескриптор файла по его пути
static FileDescriptorRecord getFileDSCByPath(char *fn)
{
  FileDescriptorRecord fdr;
  fdr.clusterNHLow = 0;
  fdr.filename[0] = 0;
  uint32_t fnLen = strlen(fn);
  uint32_t n = 1, m = 0;
  char curFn[13];
  if (fn[0] == '/')
  {
    while (1)
    {
      if (fn[n] != '/' && n < fnLen)
      {
        curFn[m] = fn[n];
        n++;
        m++;
      }
      else
      {
        curFn[m] = 0;
        m = 0;
        n++;
        fdr = getfileInDirDSC(fdr, curFn);
        if (fdr.filename[0] == 0)
          return fdr;
        if (n >= fnLen)
          break;
      }
    }
  }

  return fdr;
}

// 36 копирует строку
static char *strcopy()
{
}