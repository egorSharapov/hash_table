# Лабораторная работа 2. Оптимизация работы хеш-таблицы.


## Цель работы. 
Изучить возможности оптимизации хеш-функций и инфраструктуры хеш-таблиц с целью повышения их скорости работы.
## Инструменты.
Компилятор `g++ 11.3.0`, инструмент `callgring`, векторное расширение архитектуры x86_64, assembly.
## Экспериментальная установка. 
Ноутбук Lenovo IdeaPad с процессором AMD Ryzen 5 4500U with Radeon Graphics


## Теоретическая справка и используемые определения.
Хеш-таблица — структура данных, реализующая интерфейс ассоциативного массива (все элементы хранятся в виде пары ключ-значение). 

|             | В среднем     | В худшем случае |
| ----------- | :-----------: | :-------------: |
Расход памяти |  O(n)         |  O(n)
Поиск         |  O(1)         |  O(n)
Вставка       |  O(1)         |  O(n)
Удаление      |  O(1)         |  O(n) 

**Хеш-функция** — функция, отображающая строки на числа, то есть принимающая ключ и возвращающая число (хеш).

Важная особенность работы хеш-функции это неизбежность коллизий.

**Коллизия** — совпадения значения хеша при различных значениях ключа.

Существует несколько варинтов разрешения коллизий хеш-таблиц:
* Методы с открытой адресацией.
* Метод цепочек (ассоциативных массивов).

В данной работе будет использоваться реализация второго варианта хеш-таблиц.

**Заселенность** — одна из метрик цепочечных хеш-таблиц, средняя длина цепочки элементов.

## Часть первая. Изучение хеш-функций.

В данной части работы будут изучены различные реализации хеш-функций. Измерены их вероятностные характеристики (дисперсия распределения хеш-функции после загрузки базы данных). 

По итогу будет принято решение о выборе хеш-функции для дальнешей оптимизации.

Измерения проводятся на базе данных в 30.000 слов и хеш-таблице, емкостью 2.000 элементов при средней длине цепочки в 10 элементов.

**!** На графиках заселенности по оси ординат отложена длина цепочки, а по оси абсцисс индекс в хеш-таблице.

Сигнатура реализации хеш-функции.
```C++
typedef char*    Ht_key_type;
typedef uint32_t Hash_t;

Hash_t hash_function (Ht_key_type key);
```

--------------------------------------------------------

### **Const hash.**

Алгоритм возвращает константу для любого значения ключа.

```C++
Hash_t Const_hash (Ht_key_type key)
{
    return 0xBAD;
}
```
Дисперсия: 1084285790

<details>
<summary> График заселенности </summary>

![alt text](https://github.com/egorSharapov/hash_table/blob/main/plots/const.png?raw=true)

</details>


--------------------------------------------------------

### **Length hash.**

Алгоритм возвращает длину ключа.

```C++
Hash_t Len_hash (Ht_key_type key)
{
    return Len (key);
}
```

Дисперсия: 71866906
<details>
<summary> График заселенности. </summary>

![alt text](https://github.com/egorSharapov/hash_table/blob/main/plots/strlen.png)

</details>

--------------------------------------------------------

### **First letter hash.**

Алгоритм возвращает первый элемент ключа.

```C++
Hash_t Firstletter_hash (Ht_key_type key)
{
    return key[0];
}
```
Дисперсия: 55499156

<details>
<summary> График заселенности. </summary>

![alt text](plots/firstletter.png?raw=true)

</details>

--------------------------------------------------------

### **Sum hash.**

Алгоритм возвращает сумму элементов ключа.

```C++
Hash_t Sum_hash (Ht_key_type key)
{
    Hash_t hash = 0;
    Hash_t length = Len (key);

    for (Hash_t index = 0; index < length; index++)
        hash += key[index];

    return hash;
}
```

Дисперсия: 978856

<details>
    <summary> График заселенности. </summary>

    ![alt text](plots/sum.png?raw=true)

</details>

--------------------------------------------------------

### **ROL hash**

Циклический сдвиг хеша влево и `XOR` со значением элемента ключа.

```C++
Hash_t Rol_hash (Ht_key_type key)
{
    Hash_t hash = 0;
    Hash_t length = Len (key);

    for (Hash_t index = 0; index < length; index++)
        hash =  ROL (hash) ^ key[index];
    
    return hash;
}
```

Дисперсия: 649886

<details>
<summary> График заселенности </summary>

![alt text](plots/rol.png?raw=true)

</details>

--------------------------------------------------------

### **ROR hash.**

Цикличесикй сдвиг хеша вправо.

```C++
Hash_t Ror_hash (Ht_key_type key)
{
    Hash_t hash = 0;

    for (Hash_t index = 0; index < Len (key); index++)
        hash = ROR(hash) ^ key[index];
    
    return hash;
}
```

Дисперсия: 1485290

<details>
<summary> График распределения </summary>

![alt text](plots/ror.png?raw=true)

</details>


--------------------------------------------------------

### **djb2 (GNU) hash.**

Oб этом алгоритме  впервые сообщил Dan Bernstein. 
Магия числа 33 (почему оно работает лучше, чем многие другие константы) так и не была должным образом объяснена.

```C++
Hash_t Gnu_hash (Ht_key_type key)
{
        Hash_t hash = 5381;
        size_t length = Len (key)

        for (size_t index = 0; index < length; index++)
                hash = hash * 33 + key[index];

        return hash;
}
```

Дисперсия: 545412

<details>
<summary> График распределения </summary>

![alt text](plots/gnu.png?raw=true)

</details>

--------------------------------------------------------

### **CRC32 hash:**

Полиноминальное деление одного многочлена на другой. Имеет аппаратно поддерживаемый ассемблерный аналог, что будет использовано в следующей части работы.

```C++
Hash_t CRC32_hash (Ht_key_type key)
{
    unsigned int byte = 0, crc = 0, mask = 0;
    int index = 0;
    crc = 0xFFFFFFFF;
    while (key[index] != 0) 
    {
        byte = (unsigned) key[index];
        crc = crc ^ byte;
        
        for (int j = 7; j >= 0; j--) 
        {
            mask = -(crc & 1);
            crc = (crc >> 1) ^ (0xEDB88320 & mask);
        }
        index += 1;
    }
    return ~crc;
}
```

Дисперсия: 543316

<details>
<summary> График распределения </summary>

![alt text](plots/crc32.png?raw=true)

</details>

--------------------------------------------------------

### Дисперсия четырех лучших хеш-функций.
![alt text](plots/combined.png?raw=true)

### Соберем полученные значения дисперсии топ-5 алгоритмов на одном графике.
![alt text](plots/dispersion.png?raw=true)

По итогам первой части была выбрана реализация djb2 (gnu) hash, как обладающая наименьшей дисперсией.


## **Часть вторая. Оптимизация хеш-таблицы.**
### **Раздел первый. Оптимизация функции поиска в хеш-таблице.**
Оптимизации данного раздела будут направлены на увеличение производительности хэш-таблиц. Проанализируем исходную (неоптимизированную, без флагов) версию программы.
Проведем измерения неоптимизированной версии:

| Оптимизация  | Число машинных команд | Абсолютное ускорение | Относительное ускорение |
| :----------: | :-------------------: | :------------------: | :---------------------: |
| Base version |   38 458 739          |   1                  | 1                       |

<details> 
<summary> Спойлер (граф вызовов) </summary>

![alt text](stat/dot/OP0_graph.png?raw=true)

</details>


Также проведем измерения оптимизированной компилятором `gcc` с флагом  `-O3` программы. Полученное по результатам измерений значение будем считать целевым.
| Оптимизация  | Число машинных команд | Абсолютное ускорение | Относительное ускорение |
| :----------: | :-------------------: | :------------------: | :---------------------: |
| Base verison |   38 458 739              |   1              | 1                       |
| `-O3`        |   28 213 150              |   1,36           | 1,36                    |


<details> 
<summary> Спойлер (граф вызовов) </summary>

![alt text](stat/dot/O3_graph.png?raw=true)

</details>

### Первая оптимизация. Замена хеш-функции на эквивалентную ассемблерную версию.
По результатам анализа исходной версии программы было выявлено, что более весомый вклад в ухудшение производительности вносит реализация хеш-функции. Заменим ее на эквивалентную ассемблерную версию 

#### Реализация
```x86asm
Gnu_hash_asm:
        mov     rcx, rdi
        movzx   eax, byte [rcx]
        test    al, al
        je      .L2
        mov     edx, 5381
.L1:
        mov     r8d, edx
        add     rcx, 1
        shl     r8d, 5
        add     edx, r8d

        add     edx, eax
        movzx   eax, byte [rcx]
        test    al, al
        jne     .L1
        mov     eax, edx
        ret
.L2:
        mov     eax, -1
        ret
```

Занесем измерения в таблицу:

| Оптимизация    | Число машинных команд | Абсолютное ускорение | Относительное ускорение |
| :----------:   | :-------------------: | :------------------: | :---------------------: |
| Base version   |   38 458 739          |   1                  | 1                       |
| Optimization 1 |   30 559 932          |   1,26               | 1,26                    |

<details> 
<summary> Спойлер (граф вызовов) </summary>

![alt text](stat/dot/OP1_graph.png?raw=true)

</details>

--------------------------------------------------------

### **Вторая оптимизация. Ассемблерная вставка вместо вызова strcmp.**
Теперь, судя по графу вызовов, более значимую роль начинает играть функция сравнения ключей (strcmp). В данной оптимизации воспользуемся ассемблерной вставкой.
#### **Реализация**
```C++
int _strcmp (char *string1, char *string2)
{
    asm(".intel_syntax noprefix;"
        "loop:"
            "mov r10b, byte [rdi];"
            "mov r11b, byte [rsi];"
    	    "cmp r10b, 0;"
    	    "je end;"
    	    "cmp r11b, 0;"
    	    "je end;"
    	    "cmp r10b, r11b;"
    	    "jne end;"
    	    "inc rdi;"
    	    "inc rsi;"
    	    "jmp loop;"

        "end:"
            "push rbx;"
    	    "movzx rax, r10b;"
    	    "movzx rbx, r11b;"
    	    "sub rax, rbx;"

        ".att_syntax"
    );
}
```
Занесем измерения в таблицу:

| Оптимизация    | Число машинных команд | Абсолютное ускорение | Относительное ускорение |
| :----------:   | :-------------------: | :------------------: | :---------------------: |
| Base version   |   38 458 739          |   1                  | 1                       |
| Optimization 1 |   30 559 932          |   1,26               | 1,26                    |
| Optimization 2 |   38 098 116          |   1,01               | 0,80                    |

<details> 
<summary> Спойлер (граф вызовов) </summary>

![alt text](stat/dot/OP2_no_graph.png?raw=true)

</details>

Как мы можем увидеть, самостоятельная реализация функции сравнения ключей не привела к улучшению производительности, а скорее наоборот замедлила программу. 

Проверим гипотезу, что такое ухудшение связано с накладными расходами на создание стекового кадра. Протестируем нашу реализацию с аттрибутом   `inline __attribute__((always_inline))`.



| Оптимизация    | Число машинных команд | Абсолютное ускорение | Относительное ускорение |
| :----------:   | :-------------------: | :------------------: | :---------------------: |
| Base version   |   38 458 739          |   1                  | 1                       |
| Optimization 1 |   30 559 932          |   1,26               | 1,26                    |
| Optimization 2 |   38 098 116          |   1,01               | 0,80                    |
| Opt 2 (inline) |   19 233 482          |   1,59               | 1,56                    |

<details> 
<summary> Спойлер (граф вызовов) </summary>

![alt text](stat/dot/OP2_graph.png?raw=true)

</details>

Закономерно, из графа вызовов исчезла функция сравнения строк. По результатам измерений мы подтвердили нашу гипотезу. В дальнейших пунктах под  `Optimization 2`  будем подразумевать `inline` версию. 

--------------------------------------------------------

### **Третья оптимизация. Фиксирование длины ключа, выравнивание базы данных и замена _strcmp на интринсик.**


Так как две основные "тяжелые" функции были уже оптимизированы в предыдущих пунктах, пойдем на более серьезные ухищрения. 

Заметим одну особенность нашего словаря: максимальная длина ключа в нем не превышает 32 символов, а средняя составляет ~7 символов. Это наблюденние подтверждается статистически: средняя длина английских слов 5,43 символаю В связи с этим принято решение заменить тип ключа на `__m256i` и выровнять все ключи до 32 символов.

Дополнительно мы сможем заменить реализацию _strcmp на функцию-интринсик `_mm256_testnzc_si256`
Также перепишем хеш-функцию для работы с новым типом данных

#### **Ассемблерная реализация.**
```x86asm
Gnu_hash_asm:
        push    rbp
        mov     rbp, rsp
        and     rsp, -32
        vmovdqa yword [rsp], ymm0
        lea     rdx, [rsp]
        mov     rsi, rdx
        add     rsi, 32
        mov     eax, 5381
.L1:
        mov     ecx, eax
        sal     ecx, 5
        add     eax, ecx
        add     eax, dword [rdx]
        add     rdx, 8
        cmp     rdx, rsi
        jne     .L1
        leave
        ret
```

Занесем измерения в таблицу:

| Оптимизация    | Число машинных команд | Абсолютное ускорение | Относительное ускорение |
| :----------:   | :-------------------: | :------------------: | :---------------------: |
| Base version   |   38 458 739          |   1                  | 1                       |
| Optimization 1 |   30 559 932          |   1,26               | 1,26                    |
| Optimization 2 |   19 233 482          |   1,59               | 1,56                    |
| Optimization 3 |   18 637 764          |   2,07               | 1,03                    |


<details> 
<summary> Спойлер (граф вызовов) </summary>

![alt text](stat/dot/OP3_graph.png?raw=true)

</details>

---------------------------------------------------------------

### **Четвертая оптимизация. Замена хеш-функции.**
В предыдущей оптимизации мы заменили тип ключа на 256 битный, теперь вспомним, что у хеш-функции `crc32` существует ассемблерный аналог, работающий с 64-битными значениями. Пользуясь математической особенностью полиноминального деления заменим `gnu hash` на 4 вызова `crc32`.
#### **Ассемблерная реализация**
Регистр `rax` выступает в роли аккумулятора значения функции.
```x86asm
        push    rbp
        mov     rbp, rsp
        and     rsp, -32
        vmovdqa yword [rsp-32], ymm0
        mov     rdi, rsp
        sub     rdi, 32
        mov     rax, 0
        crc32   rax, qword [rdi]
        crc32   rax, qword [rdi+8]
        crc32   rax, qword [rdi+16]
        crc32   rax, qword [rdi+24]
        leave 
        ret
```

Занесем измерения в таблицу:
| Оптимизация    | Число машинных команд | Абсолютное ускорение | Относительное ускорение |
| :----------:   | :-------------------: | :------------------: | :---------------------: |
| Base version   |   38 458 739          |   1                  | 1                       |
| Optimization 1 |   30 559 932          |   1,26               | 1,26                    |
| Optimization 2 |   19 233 482          |   1,59               | 1,56                    |
| Optimization 3 |   18 637 764          |   2,07               | 1,03                    |
| Optimization 4 |   15 593 638          |   2,47               | 1,19                    |

<details> 
<summary> Спойлер (граф вызовов) </summary>

![alt text](stat/dot/OP4_graph.png?raw=true)

</details>

---------------------------------------------

### **Пятая оптимизация. Снижение заселенности хеш-таблицы.**

Во всех предыдущих пунктах мы использовали неоптимальный размер хеш-таблицы с коэффициентом заселенности ~13. В данном пункте мы исправим это и зададим таким, чтобы коэффициент заселенности был менее 1 (5000 -> 81101). При этом в качестве размера используем простое число, 

Занесем измерения в таблицу:
| Оптимизация    | Число машинных команд | Абсолютное ускорение | Относительное ускорение |
| :----------:   | :-------------------: | :------------------: | :---------------------: |
| Base version   |   38 458 739          |   1                  | 1                       |
| Optimization 1 |   30 559 932          |   1,26               | 1,26                    |
| Optimization 2 |   19 233 482          |   1,59               | 1,56                    |
| Optimization 3 |   18 637 764          |   2,07               | 1,03                    |
| Optimization 4 |   15 593 638          |   2,47               | 1,19                    |
| Optimization 5 |    5 742 934          |   6,64               | 2,69                    |


<details> 
<summary> Спойлер (граф вызовов) </summary>

![alt text](stat/dot/OP5_graph.png?raw=true)

</details>


### **Итоговая таблица.**
| Оптимизация    | Флаги компиляторов |Число машинных команд | Абсолютное ускорение | Относительное ускорение |
| :----------:   | :----------------: | :------------------: | :------------------: | :---------------------: |
| Base version   |   *None*             | 38 459K              | 1                    | 1                       |
| Base version   |  `-O3`             | 28 213K              | 1,36                 | 1,36                    |
| Optimization 5 |   *None*            | 5 743K               | 6,64                 | 4,92                    |
| Optimization 5 |  `-O3`             | 3 551K               | 10,85                | 1,62                    |


На данном этапе возможности для оптимизации поиска в хеш-таблице исчерпаны.
Далее увеличивать размер хеш-таблицы не имеет практического смысла, так как это не приведет к увеличению скорости работы хеш-таблицы. Поэтому принято решение прекратить цикл оптимизаций и подвести итоги.

### **Выводы раздела:**
* Благодаря более строгим требованиям к данным, хранящимся в хеш-таблицы удалось добиться высоких результатов в улучшении прозводительности хеш-таблицы.
* Подтверждается вывод предыдущей работы о том, что не всегда флагов компилятора достаточно для значительного улучшения скорости работы программы.
