# Programação Concorrente

Repositório para códigos feitos na disciplina de Programação Concorrente - 2024.1

# Monte-Carlo
Autores: Arthur Pinheiro, Pedro Arthur Santos Gama

Aplicação que calcula uma integral definida pelo método de Monte Carlo. Para usar basta compilar a main e passar os argumentos pela linha de comando lim_inf, lim_sup, iter e qnt_threads. Está tudo melhor explicado dentro do código.

cheat code:
```console
git clone https://github.com/vuvu43/Prog-Conc.git
cd Prog-Conc/Monte-Carlo
```

# lab6

Modo de uso da main:
```
main <num_threads> <buffer_size> <arquivo>
```

Modo de uso nums:
```
nums <num_mt_mt_grande>
```

cheat code no windows:
```console
git clone https://github.com/vuvu43/Prog-Conc.git
cd Prog-Conc/lab6
gcc nums.c -o nums
gcc main.c -o main
nums 10000
main 100 50 output.bin
```
<br>

cheat code no linux:
```console
git clone https://github.com/vuvu43/Prog-Conc.git
cd Prog-Conc/lab6
gcc nums.c -o nums
gcc main.c -o main
./nums 10000
./main 100 50 output.bin
```
