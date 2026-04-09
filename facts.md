<!-- добавить скрин из objdump что с O3 версия с массивами (Unrolled) компилируется в avx инструкции -->

|version| g++ -O2, cycles | g++ -O3, cycles |
|-------|-----------------|-----------------|
|naive  | 223782942445    | 223782026673    |
|arrays | 18433307501     | 104200621852    |
|avx    | 16872515078     | 16869458872     |

used avx512 instructions (zmm registers consisting of 16 floats = 64 bytes)<br>
average test time   = 1.5 min<br>
theoretical maximum = x16 speed<br>

avx_O2    / naive_O2 = x13.26 speed<br>
avx_O3    / naive_O2 = x13.27 speed<br>
arrays_O2 / naive_O2 = x12.14 speed<br>
arrays_O3 / naive_O2 = x2.15  speed<br>
naive_O3  / naive_O2 = x1.00  speed<br>

