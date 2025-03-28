### Решение задания по ФСДиА: Алгоритмы поиска строки.

Задание 2.1:
Входные данные: 
1) текстовый файл, каждая из строк которого содержит 3 поля в табличном виде (первое и второе - Дата и ФИО, третье - Название дисциплины (Например, Программная Инженерия, Кибербезопасность и т.п.)) (типы полей должны соответствовать типу данных в предметной области - все что является числом, должно храниться в виде числа, а не строки, например, время - это не строка, а структура из двух полей целочисленного типа). Все данные во входном файле корректные (проверять на корректность не нужно). Количество строк входного файла - 1000000.
2) целое число n - количество строк входного файла для обработки (10 ≤ n ≤ 1000000).
3) для каждого поля поиска:
- шаблон поиска (или список шаблонов, если один из алгоритмов предполагает поиск нескольких шаблонов);
- количество (сколько раз заданный шаблон должен встречаться в заданном поле поиска (или сколько из заданных шаблонов должно быть найдено в заданном поле поиска)).
Выходные данные: 2 текстовых файла, строки которых содержат номер строки и данные входного файла, удовлетворяющие условиям поиска (в первом файле - первым алгоритмом поиска, во втором - вторым), последняя строка - время, затраченное на поиск. Данные в выходном файле должны быть представлены в табличном виде.

2 алгоритма поиска:

1. Ахо-Корасик.
2. Алгоритм Кнута-Морриса-Пратта.

