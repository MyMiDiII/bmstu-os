Собираем:

```
make
```

Включаем "онлайн"-просмотр логов:

```
sudo dmesg -wH | grep MyTasklet
```

Загружаем модуль:

```
sudo insmod mytasklet.ko
```

После загрузке в логе:

```
[  +0.000006] MyTasklet: loaded
[  +0.081704] MyTasklet: my_irq_handler
[  +0.000006] MyTasklet: called by keyboard_irq
[  +0.000002] MyTasklet: tasklet state (before) = 0
[  +0.000002] MyTasklet: scheduling
[  +0.000002] MyTasklet: tasklet state (after) = 1
[  +0.000019] MyTasklet: -------------------------
[  +0.000003] MyTasklet: tasklet count = 0
[  +0.000003] MyTasklet: tasklet state = 2
[  +0.000002] MyTasklet: key is pressed
[  +0.000024] MyTasklet: key code is 156
```

При нажатии клавиши:

```
[  +0.000003] MyTasklet: -------------------------
[  +2.021188] MyTasklet: my_irq_handler
[  +0.000009] MyTasklet: called by keyboard_irq
[  +0.000003] MyTasklet: tasklet state (before) = 0
[  +0.000003] MyTasklet: scheduling
[  +0.000002] MyTasklet: tasklet state (after) = 1
[  +0.000026] MyTasklet: -------------------------
[  +0.000005] MyTasklet: tasklet count = 0
[  +0.000004] MyTasklet: tasklet state = 2
[  +0.000005] MyTasklet: key is pressed
[  +0.000025] MyTasklet: key code is 36
[  +0.000005] MyTasklet: the key is J
[  +0.000004] MyTasklet: -------------------------
[  +0.107198] MyTasklet: my_irq_handler
[  +0.000007] MyTasklet: called by keyboard_irq
[  +0.000003] MyTasklet: tasklet state (before) = 0
[  +0.000003] MyTasklet: scheduling
[  +0.000002] MyTasklet: tasklet state (after) = 1
[  +0.000040] MyTasklet: -------------------------
[  +0.000002] MyTasklet: tasklet count = 0
[  +0.000002] MyTasklet: tasklet state = 2
[  +0.000002] MyTasklet: key is pressed
[  +0.000024] MyTasklet: key code is 164

```

Два раза: нажатие и отжатие 

Выгружаем:

```
[  +0.000033] MyTasklet: unloaded
```

Дополнительно:

```
cat /proc/interrupts | grep "test"
1:  0  0  0  0  55511  0  0  0  IR-IO-APIC  1-edge  i8042, test_my_irq_handler
```
