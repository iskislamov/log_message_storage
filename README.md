# Log message storage

Предложите алгоритм, который позволит эффективно хранить множественные однотипные данные - диагностические сообщения. 
Каждое сообщение выглядит как одна из шаблонных строк, в которую подставлено несколько значений. 
Количество шаблонов фиксированное и не больше 1000. 
Необходимо уметь эффективно сериализовать/десериализовать подобные сообщения в файл/из файла.

Пример: В модуле ТАКСИ произошла ошибка ДОСТУПА ВОДИТЕЛЯ ИВАНА (В модуле НАЗВАНИЕ_МОДУЛЯ произошла ошибка НАЗВАНИЕ_ОШИБКИ СУБЪЕКТ_ОШИБКИ)

# Решение:

Будем считать, что все возможные шаблоны мы знаем. Будем хранить их в файле со следующим форматом:

<количество строчек в файле, не считая текущую>
<количество элементов в строке n>#<слово-разделитель между шаблонными параметрами №1>#...<слово-разделитель между шаблонными параметрами №n>#
...

'#' - элемент-сепаратор, который гарантированно не встретится в обычных логах.

Таким образом, в коде будем хранить вектор векторов строк для шаблонов.

Сериализация самих сообщений будет следующая:

<номер шаблона, которому соответствует сообщение>, <список шаблонных параметров>
Заметим, что если сообщение заканчивается на какой-то параметр, то в списке слов-разделителей текущего шаблона будет пустая строка.

В примере из задания сериализация будет следующая:

Шаблон : 4#В модуле # произошла ошибка # ##

Сообщение: 4#<индекс шаблона в массиве(в данном случае 0)>#ТАКСИ#ДОСТУПА#ВОДИТЕЛЯ ИВАНА#

Для того, чтобы получить из исходного массива строк обычных сообщений сериализованные, используются регулярные выражения, которые позволяют сопоставить конкретный шаблон соответствующей строке.

После сериализации хранилище может работать как уже с сериализованными данными, так и без них.

Сама программа позволяет сериализовать и десериализовать данные и сохранить их в файл.

По умолчанию, будем считать, что шаблоны хранятся рядом в файле **templates.txt**

Опции запуска следующие:

1) -s, означает, что необходимо произвести сериализацию

2) -d, означает, что необходимо произвести десериализацию

3) --i=<input_file_name>, указывает имя файла для ввода

3) --o=<output_file_name>, указывает имя файла для вывода

4) --t=<template_file_name>, указывает имя файла с шаблонами


# Собрать решение для Windows

Необходим MinGW

git clone https://github.com/iskislamov/log_message_storage

cd log_message_storage

cmake -G "MinGW Makefiles" -B build -S .

cmake --build ./build

./build/serializer -d --i=params.txt --o=deserialized.txt

./build/serializer -s --i=deserialized.txt --o=serialized.txt

# Собрать решение для Linux

g++ -std=c++17 log_messages_storage.cpp main.cpp -o serializer

./serializer -d --i=params.txt --o=deserialized.txt

./serializer -s --i=deserialized.txt --o=serialized.txt