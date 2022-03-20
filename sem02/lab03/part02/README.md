# Вопросы при сдаче

Необходимо 4 раза собрать модули для получения 4 различных ошибок.

## Ошибка 1

В файле [md2.c](./md2.c) раскомментировать строку обозначенную `// 1`.
Выполнить команду `make`. Результат:

```
/home/mymidi/01_BMSTU/bmstu-os/sem02/lab03/part02/md2.c:16:40: error: implicit declaration of function ‘md1_local’ [-Werror=implicit-function-declaration]
   16 |     printk("md2 local from md1: %s\n", md1_local());
      |                                        ^~~~~~~~~
/home/mymidi/01_BMSTU/bmstu-os/sem02/lab03/part02/md2.c:16:34: warning: format ‘%s’ expects argument of type ‘char *’, but argument 2 has type ‘int’ [-Wformat=]
   16 |     printk("md2 local from md1: %s\n", md1_local());
      |                                 ~^     ~~~~~~~~~~~
      |                                  |     |
      |                                  |     int
      |                                  char *
      |                                 %d

```

Вылетают ошибка и предупреждение. Нам нужна ошибка.

Что надо сказать:

---

При вызове из модуля `md2` `md1_local` возникает ошибка на этапе компиляции
`error: implicit declaration of function`.

--- 

### Дополнение

Почему говорить не надо было. А вообще потому что `md1_local` объявлена как
локальная (модификатор `static`) и не может быть вызвана из другого модуля.
Раскомментирование строки обозначенной в [md.h](./md.h) как `// 1` вместе с
сохранением предыдущего раскомментирования даст другую ошибку:

```
/home/mymidi/01_BMSTU/bmstu-os/sem02/lab03/part02/md1.c:19:14: error: static declaration of ‘md1_local’ follows non-static declaration
   19 | static char *md1_local(void)
      |              ^~~~~~~~~
In file included from /home/mymidi/01_BMSTU/bmstu-os/sem02/lab03/part02/md1.c:4:
/home/mymidi/01_BMSTU/bmstu-os/sem02/lab03/part02/md.h:7:15: note: previous declaration of ‘md1_local’ with type ‘char *(void)’
    7 | extern char * md1_local(void);
      |               ^~~~~~~~~
```

Вывод: не получится у нас никак вызвать локальную функцию.
Но это все не надо.

## Ошибка 2

Закомментировать обратно все строки и раскоментировать в файле [md2.c](./md2.c)
строку обозначенную `// 2`.  Далее `make`. Результат:

```
/home/mymidi/01_BMSTU/bmstu-os/sem02/lab03/part02/md2.c:17:43: error: implicit declaration of function ‘md1_noexport’ [-Werror=implicit-function-declaration]
   17 |     printk("md2 noexport from md1: %s\n", md1_noexport());
      |                                           ^~~~~~~~~~~~
/home/mymidi/01_BMSTU/bmstu-os/sem02/lab03/part02/md2.c:17:37: warning: format ‘%s’ expects argument of type ‘char *’, but argument 2 has type ‘int’ [-Wformat=]
   17 |     printk("md2 noexport from md1: %s\n", md1_noexport());
      |                                    ~^     ~~~~~~~~~~~~~~
      |                                     |     |
      |                                     |     int
      |                                     char *
      |                                    %d
```

Получаем аналогичные ошибки. Слова те же:

---

При вызове из модуля `md2` `md1_noexport` без объявления в header-файле
возникает ошибка на этапе компиляции `error: implicit declaration of function`.

--- 

Думаю, почему это происходит, нам подскажет курс C, благо объяснять это на
сдаче не надо.

## Ошибка 3

В дополнение раскомментируем строку `// 2` в файле [md.h](./md.h). Любимая
команда `make`. Результат:

```
ERROR: modpost: "md1_noexport" [<путь_к_файлу>/md2.ko] undefined!
```

Что надо сказать:

---

При вызове из модуля `md2` `md1_noexport` с объявлением в header-файле
возникает ошибка на этапе сборки/линковки `md1_noexport undefined`. Это
происходит потому, что мы не экспоритировали символ `md1_noexport` с помощью
макроса `EXPORT_SYMBOL`.

--- 

## Ошибка 4

Комментим все обратно. Выполняем `sudo ./run.sh`. Результат:

```
>>>>> Загрузка md1.ko...

>>>>> Загрузка md2.ko...

>>>>> Модули md1 и md2 загружены:
Module                  Size  Used by
md2                    16384  0
md1                    16384  1 md2
crypto_simd            16384  1 aesni_intel
cryptd                 24576  3 crypto_simd,ghash_clmulni_intel

>>>>> Последние сообщения загруженных модулей
[ 9001.986850] md1 init
[ 9001.986854] md1 proc: I will become a Hokage!
[ 9001.986856] md1 local: Random string!
[ 9001.986857] md1 noexport: ИУ7-63Б
[ 9002.794132] md2 init
[ 9002.794137] md2 exported from md1 : I will become a Hokage!
[ 9002.794139] md2 md1_proc() returned string : I will become a Hokage!

>>>>> Удаление md2...

>>>>> Загрузка md3.ko... # показать на следующую строку :)
insmod: ERROR: could not insert module md3.ko: Operation not permitted 

>>>>> Последние сообщения загруженных модулей
[ 9001.986850] md1 init
[ 9001.986854] md1 proc: I will become a Hokage!
[ 9001.986856] md1 local: Random string!
[ 9001.986857] md1 noexport: ИУ7-63Б
[ 9002.794132] md2 init
[ 9002.794137] md2 exported from md1 : I will become a Hokage!
[ 9002.794139] md2 md1_proc() returned string : I will become a Hokage!
[ 9003.819091] md2 exit
[ 9004.651115] md3 init

>>>>> Remove md3...
rmmod: ERROR: Module md3 is not currently loaded

>>>>> Remove md1...

>>>>> Последние сообщения модулей
[ 9001.986850] md1 init
[ 9001.986854] md1 proc: I will become a Hokage!
[ 9001.986856] md1 local: Random string!
[ 9001.986857] md1 noexport: ИУ7-63Б
[ 9002.794132] md2 init
[ 9002.794137] md2 exported from md1 : I will become a Hokage!
[ 9002.794139] md2 md1_proc() returned string : I will become a Hokage!
[ 9003.819091] md2 exit
[ 9004.651115] md3 init
[ 9006.341656] md1 exit

```

Красота. Все собралось. Модули 1 и 2 загрузились. Но модуль 3 нет.

Надо показать строку с ошибкой (выше она указана) и сказать:

---

Происходит ошибка на этапе выполнения. Модуль `md3` не загружается, так как
возвращает `-1`: ошибка инициализации модуля.

---

> Вторая часть сдана ✅

Модуль `md3` похож на обычный пользовательский процесс, только он выполняется в
режиме ядра. Подробнее обо всем в статье Цирюлика "Разработка модулей ядра
Linux".

> Успехов ✊
