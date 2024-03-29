# Дерево отрезков

Дерево отрезков, как правило, применяется для задач, в которых нужно эффективно вычислять какую-нибудь операцию над подотрезком массива.

Скажем, пусть у нас есть массив размера `n`, и нам поступают запросы двух типов:

- изменить элемент
- посчитать сумму на подотрезке [L; R)

Наивным образом это решается написанием дополнительной функции, которая суммирует значения на подотрезке:

```cpp
#include <vector>
#include <stdint.h>

template <typename T>
T calcSum(const std::vector<T> &values, uint32_t l, uint32_t r) {
    T sum = 0;

    for(uint32_t i = l; i < r; i++) {
        sum += values[i];
    }

    return sum;
};
```

Пояснения о том, как мы собираемся изменять значения в наивном способе, я думаю, не нужны.

Характеристики решения:

- `n` - количество значений в массиве
- `w` - размер подотрезка

| Характеристика     | O    |
| ------------------ | ---- |
| Высчитывание суммы | O(w) |
| Изменение элемента | O(1) |
| Потребление памяти | O(n) |

Таблица с временем исполнения представлена ниже:

| w         | секунды |
| --------- | ------- |
| 1         | 0.00000 |
| 10        | 0.00000 |
| 100       | 0.00000 |
| 1000      | 0.00000 |
| 10000     | 0.00001 |
| 100000    | 0.00021 |
| 1000000   | 0.00208 |
| 10000000  | 0.02100 |
| 100000000 | 0.23409 |

В целом, для одноразового подсчёта такого алгоритма хватит, однако
если запросов будет не 1, а, скажем, 10000, то алгоритм явно будет работать
слишком долго.

### Оптимизация

Воспользуемся деревом отрезков.

Реализация дерева отрезков в `segtree.hpp` является абстрактной реализацией
ДО. Это позволяет создавать ДО, выполняющее любую операцию: от суммы до максимума.

Реализуем простейшее суммирующее ДО.

Для начала, нужно создать суммирующую функцию и функцию, расширяющую результат за счёт переданного отрезка ( более подробное описание об этом будет ниже ):

```cpp
template <typename T>
void summator(T& result, const T& left, const T& right) {
    result = left + right;
};

template <typename T>
void sumQueryUpdate(T& result, const T& segment) {
    result += segment;
};
```

Она будет вычислять результат для суммы на отрезках, если известны суммы для двух дочерних отрезков.

Эту функцию вместе с типом хранимых данных теперь нужно передать в темплейт, а в конструктор передать размер массива:

```cpp
SegmentTree<uint64_t, summator> stree(256);
```

ДО желательно заполнить значениями. Для этого есть метод `fillup`:

```cpp
std::vector<uint64_t> values(256);

for(uint64_t &value : values) {
    value = rand();
}

stree.fillup(values.data());
```

или `setValueWithoutUpdate`:

```cpp
for(uint32_t i = 0; i < 256; i++) {
    // setValueWithoutUpdate не обновляет диапазоны, поэтому отрабатывает быстрее
    stree.setValueWithoutUpdate(i, rand());
}

// Однако после вызова setValueWithoutUpdate() нужно вызывать updateSegments(),
// чтобы можно было спокойно использовать operate()
stree.updateSegments();
```

Ну и теперь, чтобы посчитать сумму на подотрезке, нам нужно вызвать метод `operate`:

```cpp
// Посчитает сумму в диапазоне [13; 44)
// Последний аргумент - это начальное значение ( в данном случае он не обязателен )
std::cout << stree.operate(13, 44, (uint64_t)0, sumQueryUpdate) << std::endl;
```

Чтобы изменить элемент, можно воспользоваться методом `setValue`.

Характеристики решения:

- `n` - количество значений в массиве
- `w` - размер подотрезка

| Характеристика     | O       |
| ------------------ | ------- |
| Высчитывание суммы | O(logw) |
| Изменение элемента | O(logn) |
| Потребление памяти | O(n)    |

Таблица времени в наносекундах ( одна миллиардная секунды ) для высчитывания суммы:

| s\w       | 1   | 10  | 100 | 1000 | 10000 | 100000 | 1000000 | 10000000 |
| --------- | --- | --- | --- | ---- | ----- | ------ | ------- | -------- |
| 10        | 36  | 77  |     |      |       |        |         |          |
| 100       | 22  | 48  | 169 |      |       |        |         |          |
| 1000      | 21  | 32  | 53  | 241  |       |        |         |          |
| 10000     | 52  | 78  | 44  | 91   | 305   |        |         |          |
| 100000    | 21  | 31  | 44  | 89   | 101   | 184    |         |          |
| 1000000   | 21  | 31  | 45  | 88   | 102   | 119    | 228     |          |
| 10000000  | 21  | 31  | 44  | 88   | 103   | 119    | 134     | 255      |
| 100000000 | 21  | 31  | 44  | 90   | 103   | 119    | 135     | 147      |

В таблице хорошо видна логарифмическая зависимость от размера окна.

_Отмечу, что наносекунды - это уже уровень тактов процессора, поэтому из таблицы
никаких выводов, кроме как о сложности алгоритма, делать не стоит._

### Более сложный пример

В предыдущем примере дерево отрезков хранило в отрезках результат нужного запроса на самих отрезках.
Однако, в дереве отрезков можно предпосчитывать результат и сохранять в его отрезках, после чего использовать его для выполнения совершенно другого запроса на отрезке. Пример:

Пусть у нас есть массив из `n` натуральных чисел. Нам нужно научится отвечать на запрос о количестве чисел на отрезке `[l; r]`, меньших либо равных `k`.
Наивно такое решение решается за линию, однако с помощью ДО его можно снизить до `O(log²n)`.

Чтобы решить эту задачу, мы можем завести ДО, каждый из отрезков которого будет хранить отсортированную часть массива на отрезке. Тогда, когда нам нужно будет ответить на запрос, мы будем проходиться по таким отрезкам и бинпоиском искать элемент, равный `k`.

`SegmentTree` разделяет два вида функций: оператор, который вызывается, чтобы получить результат некой операции на отрезке, зная результаты двух дочерних, и функция запроса, которая проходится по отрезкам и строит из них результат необходимого запроса. Функция запроса принимает на вход результат запроса для текущего отрезка и новый отрезок, после чего обновляет результат, включая в текущий отрезок новый.

Оператором в данном случае будет выступать функция `merge`, которая будет объединять два отсортированных отрезка в один большой отсортированный отрезок ( напомним, что нам передаётся два дочерних отрезка, которые уже отсортированны ).

```cpp

template <typename T>
void merge(
        std::vector<T> &out,
        const std::vector<T> &left,
        const std::vector<T> &right) {
    const uint32_t leftSize = left.size(), rightSize = right.size();
    result.resize(leftSize + rightSize);

    uint32_t i = 0;
    uint32_t l = 0, r = 0;

    while (l < leftSize && r < rightSize) {
        if (left[l] < right[r]) {
            result[i] = left[l];
            l++;
        } else {
            result[i] = right[r];
            r++;
        }

        i++;
    }

    while (l < leftSize) {
        result[i] = left[l];
        l++;
        i++;
    }

    while (r < rightSize) {
        result[i] = right[r];
        r++;
        i++;
    }
}
```

Теперь, нам нужно написать функцию запроса, которая будет проходится по отрезкам и аккумулировать текущее количество элементов, меньших либо равных `k`:

```cpp
// Пусть где-то определена функция бинпоиска, возвращающая индекс максимального элемента, меньшего или равного value.
// Для удобства будем считать, что если значение не нашлось, то возвращается -1
template <typename T>
int32_t binsearch(const std::vector<T> &array, T value);

template <typename U, typename T>
void countNumbers(U& result, const std::vector<T> &segment, T k) {
    // Поскольку бинпоиск возвращает индекс, а массив отсортирован, то логично
    // предположить, что элементы левее найденного индекса меньше `k`, а
    // следовательно, количество таких элементов будет = индекс + 1
    result += U(binsearch(segment, k) + 1);
}
```

Осталось завести ДО:

```cpp
// Промежуточным "запросом" для каждого отрезка будет отдельный массив, поэтому
// тип данных std::vector
SegmentTree<std::vector<uint64_t>, merge> tree(100000);

// ... где-то заполняются значения

// Возьмём отрезок [489, 53058)
// 0 будет передан в countNumbers на первом вызове
// 30 - это k, которое будет переданно в функцию countNumbers при каждом её вызове
std::cout << tree.operate(489, 53058, 0, countNumbers, (uint64_t)30) << "\n";
```
