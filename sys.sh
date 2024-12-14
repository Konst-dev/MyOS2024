fn=kernel_module
gcc -O0 -m32 -DSTDC_HEADERS -c $fn.c
nm -n $fn.o > $fn.txt
./getfntable $fn.txt $fn.ftb
sudo cat /home/konst/share/Projects/NASM/v2024/krnl24 $fn.ftb > kernel24.temp
fsz=$(wc -c "./kernel24.temp" | awk '{print $1}')
ld -m elf_i386 -Ttext $fsz -e _start  $fn.o -o $fn.img
objcopy -O binary -R .comment -S $fn.img $fn.bin
sudo cat ./kernel24.temp $fn.bin > kernel24
cp -rf ./kernel24 /home/konst/share/Projects/NASM/v2024
ndisasm -b 32 $fn.bin > $fn.lst
rm -f ./kernel24.temp
rm -f ./$fn.txt
rm -f ./$fn.ftb