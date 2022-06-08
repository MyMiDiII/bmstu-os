Собираем:

```
make
```

Включаем "онлайн"-просмотр логов (в другом терминале):

```
sudo dmesg -wH | grep MyWorkQueue
```

Загружаем модуль:

```
sudo insmod myworkqueue.ko
```

Логи аналогичны первой части.

