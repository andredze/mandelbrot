<!-- добавить скрин из objdump что с O3 версия с массивами (Unrolled) компилируется в avx инструкции -->


<!-- used avx512 instructions (zmm registers consisting of 16 floats = 64 bytes)<br>


one test time in average = 1 min<br>
tests number = 7<br>


theoretical maximum = x16 speed<br> -->


|                                       |                                               |
|---------------------------------------|-----------------------------------------------|
|Операционная система                   | Linux Mint 22.3                               |
|Ядро                                   | Linux 6.17.0-19-generic                       |
|Архитектура                            | x86-64                                        |
|Процессор                              | 11th Gen Intel(R) Core(TM) i5-11320H @ 3.20GHz|
|Среднее время одного теста             | 1 минута                                      |
|Количество тестов на 1 вид оптимизации | 7                                             |
|Средняя температура процессора         | 55 °C                                         |
|Средняя частота процессора             | 2800 Ghz                                      |
|Использованные опции компилятора       | `-O2`/`-O3`/`-O fast` и `-march=native`   |


| Оптимизация               |    Среднее значение циклов  |  Стандартное отклонение  | Стандартное отклонение по отношению к значению, % |
| ------------------------- | --------------------------- | ------------------------ | ------------------------------------------------- |
| naive:  g++ -O2           |   4.165378e+11              |  9.593662e+06            | 0.0023
| naive:  g++ -O3           |   4.161893e+11              |  2.756221e+07            | 0.0066
| naive:  g++ -Ofast        |   4.121465e+11              |  4.444724e+07            | 0.0108
| arrays: g++ -O2           |   3.427505e+10              |  2.470104e+07            | 0.0721
| arrays: g++ -O3           |   1.937574e+11              |  1.096227e+08            | 0.0566
| arrays: g++ -Ofast        |   1.967497e+11              |  8.942978e+06            | 0.0045
| avx:    g++ -O2           |   3.137219e+10              |  6.523483e+06            | 0.0208
| avx:    g++ -O3           |   3.133585e+10              |  4.441143e+06            | 0.0142
| avx:    g++ -Ofast        |   3.136531e+10              |  8.637366e+06            | 0.0275
| naive:  clang++ -O2       |   4.074730e+11              |  4.920964e+06            | 0.0012
| naive:  clang++ -O3       |   4.075675e+11              |  3.108633e+07            | 0.0076
| naive:  clang++ -Ofast    |   4.234765e+11              |  8.641659e+07            | 0.0204
| arrays: clang++ -O2       |   6.259698e+10              |  2.762222e+08            | 0.4413
| arrays: clang++ -O3       |   6.253132e+10              |  9.329109e+06            | 0.0149
| arrays: clang++ -Ofast    |   6.232172e+10              |  6.787020e+06            | 0.0109
| avx:    clang++ -O2       |   3.395030e+10              |  5.624108e+06            | 0.0166
| avx:    clang++ -O3       |   3.395170e+10              |  4.027612e+06            | 0.0119
| avx:    clang++ -Ofast    |   2.754317e+10              |  6.658737e+06            | 0.0242


<table>
  <thead>
    <tr><th rowspan="3">optimization</th><th colspan="6">cycles, 10^10</th></tr>
    <tr><th colspan="3">g++</th><th colspan="3">clang++</th></tr>
    <tr><th>-O2</th><th>-O3</th><th>-Ofast</th><th>-O2</th><th>-O3</th><th>-Ofast</th></tr></thead>
  <tbody>
    <tr><td>naive</td><td>41.65</td><td>41.62</td><td>41.21</td><td>40.75</td><td>40.76</td><td>42.35</td></tr>
    <tr><td>arrays</td><td>3.43</td><td>19.38</td><td>19.67</td><td>6.26</td><td>6.25</td><td>6.23</td></tr>
    <tr><td>avx</td><td>3.14</td><td>3.13</td><td>3.14</td><td>3.40</td><td>3.40</td><td>2.75</td></tr>
  </tbody>
</table>


<table>
  <thead>
    <tr><th rowspan="3">optimization</th><th colspan="6">naive -O2 / cycles</th></tr>
    <tr><th colspan="3">g++</th><th colspan="3">clang++</th></tr>
    <tr><th>-O2</th><th>-O3</th><th>-Ofast</th><th>-O2</th><th>-O3</th><th>-Ofast</th></tr></thead>
  <tbody>
    <tr><td>naive</td><td>1.00</td><td>1.00</td><td>1.01</td><td>1.00</td><td>1.00</td><td>0.96</td></tr>
    <tr><td>arrays</td><td>12.15</td><td>2.15</td><td>2.12</td><td>6.51</td><td>6.52</td><td>6.54</td></tr>
    <tr><td>avx</td><td>13.28</td><td>13.29</td><td>13.28</td><td>12.00</td><td>12.00</td><td>14.79</td></tr>
  </tbody>
</table>


<!-- В случае с версии с массивами оптимизация -O2 оказалась в 6 раз быстрее оптимизации -O3 для компилятора g++.
Компилятор при опции -O2 использовал SIMD-инструкции. Однако в случае
с -O3 компилятор использует множество скалярных операций и обращений к стеку вместо векторных инструкций.
Это может быть связано с тем, что компилятор позволяет себе более агрессивные оптимизации, конфликтующие с
SIMD-инструкциями. -->


## g++ O2 vs O3 (arrays version)
<table>
<tr>
<td><img src="assets/g++_arrays_O2.png"></td>
<td><img src="assets/g++_arrays_O3.png"></td>
</tr>
</table>


<!-- Здесь разница заключается в основном в том, что в версии -O3 напрямую делается call sqrtf,
а в версии -Ofast используется инструкция vsqrtss -->


## clang++ O3 vs Ofast (avx version)
<table>
<tr>
<td><img src="assets/clang_avx_O3.png"></td>
<td><img src="assets/clang_avx_Ofast.png"></td>
</tr>
</table>


<!-- Компилятор clang в версии с массивами не смог полностью векторизовать код, используя частично xmm и частично ymm регистры,
из-за чего версия, скомпилированная компилятором clang оказалась примерно в 2 раза медленнее. -->


## clang++ O2 vs g++ O2 (arrays version)
<table>
<tr>
<td><img src="assets/g++_arrays_O2_clangdiff.png"></td>
<td><img src="assets/clang++_arrays_O2.png"></td>
</tr>
</table>


<p align="center">
    <img src="data/histogram.png" width="75%">
</p>


<!-- Для измерения состояния процессора была использована утилита **s-tui**. 
Данные, полученные с ее помощью, находятся в файле *cpu-data.csv*. 
Открыв файл в программе LibreOffice, я убедился что столбец Throttle пуст, что говорит
об отсутствии троттлинга в ходе проведения тестов -->


<p align="center">
    <img src="assets/s-tui.png" width="75%">
</p>


<p align="center">
    <img src="assets/cpu-data.png" width="75%">
</p>


<p align="center">
    <img src="assets/no_throttle_proof.png" width="75%">
</p>


<!-- Температура при тестах не поднималась выше 60 градусов, что говорит об отсутствии троттлинга на производительность процессора. -->


<p align="center">
    <img src="data/temp.png" width="75%">
</p>