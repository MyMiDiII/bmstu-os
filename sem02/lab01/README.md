# Вопросы и ответы

> Выполнялось на Manjaro

Сборка:

`gcc main.c -o MYDAEMON`

Запуск:

`sudo ./MYDAEMON`

Вывод строки заголовка и нашего процесса-демона:

`ps -axj | egrep "PID|./MYDAEMON" | grep -v "grep"`

Вывод:

```
PPID     PID    PGID     SID TTY        TPGID STAT   UID   TIME COMMAND
   1    7116    7116    7116 ?             -1 Ssl      0   0:00 ./MYDAEMON
```

Описать, почему такие значения:

* `PPID = 1` --- процесс запустивший терминал, родительский процесс был завершен
и процесс был усыновлен терминальным процессом (условием чего это явлется???),
что является условием того, что процесс не будет лидером группы, что необходимо
для безошибочной работы setsig;
* `PID, PGID, SID` --- одинаковые, так как процесс --- лидер группы и лидер
  сессии и в них он единственный (что-то в этом духе);
* `TTY = ?` --- нет управляющего терминала (почему у демона нет управляющего
  терминала, может это ущемление прав демонов? ;), так как демоны --- это
процессы, которые создаются в системе для того, чтобы **предоставлять
определенный сервис**;
* `TPGID = -1` --- нет терминальной группы, так как нет управляющего терминала;
*  `STAT = Ssl`:
    * `S` --- прерываемый сон (что такое прерываемый сон?), демоны пробуждаются,
      когда в них возникает потребность (когда просыпается ваш демон? ->
      показать, как демон записывает в лог время, команда чуть ниже);
    * `s` --- лидер сессии;
    * `l` --- многопоточный (какие потоки?), основной (который собственно
      выводит время, это лучше не говорить) и вспомогательный (обработка
      сигналов) (покажите, что у вас они обрабатываются; порядок действий ниже).
* `UID = 0` --- суперпользователь;
* `TIME` --- накопленное процессорное время (режим пользователя + режим ядра;
  вроде не спрашивали про это);
* `COMMAND` --- команда запустившая процесс.

Запуск вывода лога, так сказать, в режиме реального времени:

`journalctl -f | grep "mydaemon"`
 
Результат (каждые 3 секунды, вывод строки со временем):

```
Mar 11 10:58:54 host mydaemon[7116]: current time: Fri Mar 11 10:58:54 2022
Mar 11 10:58:57 host mydaemon[7116]: current time: Fri Mar 11 10:58:57 2022
Mar 11 10:59:00 host mydaemon[7116]: current time: Fri Mar 11 10:59:00 2022
```

Сигналы.

Выполнить предыдующую команду в одном терминале, сигналы посылать с другого.

SIGHUP

```
sudo kill -1 7116
```

Результат:

```
Mar 11 11:01:49 mymidihost mydaemon[7116]: got SIGHUP
Mar 11 11:01:49 mymidihost mydaemon[7116]: login: mymidi
```

SIGTERM

```
sudo kill 7116
```

Результат:

```
Mar 11 11:03:57 mymidihost mydaemon[7116]: got SIGTERM
Mar 11 11:03:57 mymidihost mydaemon[7116]: exiting
```

Далее процесс перестает работать, в лог ничего не записывается.

> Успехов ✊