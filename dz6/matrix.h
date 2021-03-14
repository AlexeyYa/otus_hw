/*! @file matrix.h
 *  @brief Описание n-мерной разреженной матрицы
 */

#ifndef MATRIX_H
#define MATRIX_H

#include <map>
#include <array>
#include <iostream>

/*! @struct Matrix matrix.h "matrix.h"
 *  @brief Первая версия матрицы, без итератора
 */
template <typename T, T param, size_t Ndim = 2>
struct Matrix
{
    /*!
     * @brief operator []
     * @param idx
     * @return матрица следующего измерения
     */
    Matrix<T, param, Ndim - 1>& operator[](size_t idx)
    {
            return m_data[idx];
    }

    /*!
     * @brief size
     * @return количество элементов в матрице
     */
    size_t size() const
    {
        size_t counter = 0;
        for (auto p : m_data)
        {
            counter += p.second.size();
        }
        return counter;
    }
private:
    /*!
     * @brief m_data матрицы меньших размерностей
     */
    std::map<size_t, Matrix<T, param, Ndim - 1> > m_data;
};

/*!
 * @brief Первая версия матрицы, конечный случай
 */
template <typename T, T param>
struct Matrix<T, param, 1>
{
    /*!
     * @brief Прокси класс для разделения чтения и записи
     */
    struct Deref
    {
        Matrix& m;
        int index;
        Deref(Matrix& m, size_t index) : m(m), index(index) {}

        /*!
         * @brief Неявное преобразование при чтении переменной
         */
        operator T() const
        {
            if (m.m_data.find(index) == m.m_data.end())
                return param;
            else
                return m.m_data[index];
        }

        /*!
         * @brief Оператор присваивания при записи в матрицу
         * @param новое значение
         * @return ссылка на элемент в матрице
         */
        T& operator=(const T& other)
        {
            return m.m_data[index] = other;
        }
    };

    /*!
     * @brief operator [] выдающий значение по индексу в матрице
     * @param idx
     * @return значение по индексу
     */
    Deref operator[](size_t idx)
    {
        return Deref{*this, idx};
    }

    /*!
     * @brief size
     * @return количество элементов
     */
    size_t size() const
    {
        return m_data.size();
    }
private:
    /*!
     * @brief элементы в матрице
     */
    std::map<size_t, T> m_data;
};


/*!
 * @brief Прокси класс для разделения чтения и записи
 */
template <typename T, typename M, typename Container>
struct Deref
{
    M& m;
    Container& idx;
    Deref(M& m, Container& idx) : m(m), idx(idx) {}

    /*!
     * @brief Неявное преобразование при чтении переменной
     */
    operator T() const
    {
        if (m.m_data.find(idx) == m.m_data.end())
            return m.default_value;
        else
            return m.m_data[idx];
    }

    /*!
     * @brief Оператор присваивания при записи в матрицу
     * @param новое значение
     * @return ссылка на элемент в матрице
     */
    T& operator=(const T& other)
    {
        return m.m_data[idx] = other;
    }
};

/*!
 * @brief Рекурсивный шаблон для перевода индексов в std::array
 */
template <size_t Ndim, typename T, typename M, size_t Ncur>
struct MatrixIndex
{
    M& m;
    std::array<size_t, Ndim>& idx;

    MatrixIndex<Ndim, T, M, Ncur - 1> operator [](size_t pos)
    {
        idx[Ndim - Ncur] = pos;
        return MatrixIndex<Ndim, T, M, Ncur - 1>{ m, idx };
    }
};

/*!
 * @brief Конечный случай шаблона использующий массив в качестве ключа для внутреннего хранилища данных
 */
template <size_t Ndim, typename T, typename M>
struct MatrixIndex<Ndim, T, M, 1>
{
    M& m;
    std::array<size_t, Ndim>& idx;

    Deref<T, M, std::array<size_t, Ndim>> operator [](size_t pos)
    {
        idx[Ndim - 1] = pos;
        return Deref<T, M, std::array<size_t, Ndim>>{ m, idx };
    }
};

/*!
 * @brief Матрица со значением по умолчанию param и размерности Ndim(min Ndim=2)
 */
template <typename T, T defval, size_t Ndim = 2>
struct M2
{

    /*!
     * @brief operator []
     * @param pos индекс по первому измерению
     * @return структура для перевода индекса в массив
     */
    auto operator[](size_t pos)
    {
        return MatrixIndex<Ndim, T, M2<T, defval, Ndim>, Ndim>{ *this, idx } [pos];
    }

    /*!
     * @brief size
     * @return количество элементов записанных в матрицу
     */
    size_t size() const
    {
        return m_data.size();
    }

    /*!
     * @brief функция для получения итератора указывающего на начало
     * @return итератор указывающий на начало
     */
    auto begin(){
        return m_data.begin();
    }

    /*!
     * @brief функция для получения итератора указывающего на конец
     * @return итератор указывающий на конец
     */
    auto end(){
        return m_data.end();
    }

    /*!
     * @brief Прокси класс для разделения чтения и записи
     */
    friend struct Deref<T, M2, std::array<size_t, Ndim>>;

private:
    std::array<size_t, Ndim> idx;                   //! временный массив для записи индекса
    std::map<std::array<size_t, Ndim>, T> m_data;   //! контейнер для получения значения по индексу содержащему массив
public:
    T default_value = defval;                       //! параметр по умолчанию
};

#endif
