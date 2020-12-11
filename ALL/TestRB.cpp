#include <iostream>
#include <chrono>
#include <typeinfo>
#include <cmath>
#include <cstdlib>
#include <fstream>


#include "FOREST_GUMP.cpp"

double get_time()
{
    return std::chrono::duration_cast<std::chrono::microseconds>(std::chrono::steady_clock::now().time_since_epoch()).count()/1e6;
}

int main ()
{
    using namespace std;
    setlocale(LC_ALL, "RUSSIAN");


//---------------------------КЧ дерево

    RBTree<int,int> test_struct = {};
    double a, b;
    double tmp_1 = 0;
    int rep_1 = 1 * 1000 * 1000;
    int factor =  1;
    int rep_2 = 10;
    int N = 0;
    double start = 0, finish = 0, total = 0;
    int n = 0;
    double averageX = 0, averageY = 0, averageXY = 0, averageX2 = 0;
    bool flag;

//-----------------------проверка insert
    RBTree<int,int> *test = new RBTree<int,int>;
    flag = true;
    int quantity_insert = 0;
    for (int i = 0; i < 10; i++){

        for ( int i = quantity_insert; i < 10 + quantity_insert; i++){
            test->insert({i,i});
        }
        quantity_insert += 10;
        if ( quantity_insert != test->size()) flag = false;

    }
    if (!flag) cout << "Ошибка вставки элемента" << endl;
    else cout << "Тест на вставку пройден" << endl;

//-----------------------проверка find

    test = new RBTree<int,int>;
    flag = true;

    int quantity = 1000;
    int test_find[quantity];
    for (int i = 0; i < quantity; i++){
        int b = rand();
        test_find[i] = b;
        test->insert({b,i});
    }

    for (int i = quantity; i < 0; i--){
        if ( !test -> find( test_find[i]) ) flag = false;
    }
    if (!flag) cout << "Ошибка поиска элемента" << endl;
    else cout << "Тест на поиск пройден" << endl;


//----------------------проверка remove

    flag = true;
    for (int j = 0; j < 10; j++){
        test = new RBTree<int,int>;
        for (int i = 0; i < 20; i++){
            test->insert({i,i});
        }
        for ( int i = 0; i < 30; i++){
            test->remove(i);
        }
        if ( test->size() != 0) flag = false;
    }
    if (!flag) cout << "Ошибка удаление элемента" << endl;
    else cout << "Тест на удаление пройден" << endl;





//-----------------------remove


    // Будем измерять значения прия наличие [0, rep_2] * k элементов
    ofstream RBremove("RBremove.txt");
    averageX = 0, averageY = 0, averageXY = 0, averageX2 = 0;
    for (int k = 1; k <= 1000 * 1000; k *= 10)
    {
        factor =  k;
        N = 0;
        test_struct = {};


        // Получаем rep_2 точек в этом промежутке
        for (int j = 0; j < rep_2; ++j)
        {

            // Увеличиваем размеры структуры
            for (int i = (factor + rep_1) * j;
                 i < factor * (j+1) + rep_1 * j;
                 ++i)
            {
                test_struct.insert({i,i});
            }

            // Далее rep_1 повторяем действие и измеряем время работы.
            start = get_time();
            for (int i = factor * (j+1) + rep_1 * j;
                i < (factor + rep_1) * (j + 1); ++i)
            {
                // Код для тестирования:

                // -----------------------
                // test_struct.insert(i);
                // test_struct.find(100);
                test_struct.remove(i);
                // -----------------------
            }
            finish = get_time();
            total = finish - start;
            N += factor;
            n++;
            RBremove << N << " " << total << endl;
            averageY = ( total + averageY*n )/n;
            averageX = ( N*log(N) + averageX*n )/n;
            averageX2 = ( (N*log(N))*(N*log(N)) + averageX2*n )/n;
            averageXY = ( (N*log(N))*total + averageXY*n )/n;
        }
    }
    a = ( averageXY - averageX*averageY) / ( averageX2 - averageX*averageX);
    b = averageY - a*averageX;
    cout << "remove:" << endl;
    cout << a << "xlnx+" << b << endl;
    RBremove.close();


//---------------------------find
    ofstream RBfind("RBfind.txt");
    averageX = 0, averageY = 0, averageXY = 0, averageX2 = 0;
    for (int k = 1; k <= 1000 * 1000; k *= 10)
    {
        factor =  k;
        N = 0;
        test_struct = {};


        // Получаем rep_2 точек в этом промежутке
        for (int j = 0; j < rep_2; ++j)
        {

            // Увеличиваем размеры структуры
            for (int i = (factor + rep_1) * j;
                 i < factor * (j+1) + rep_1 * j;
                 ++i)
            {
                test_struct.insert({i,i});
            }

            // Далее rep_1 повторяем действие и измеряем время работы.
            start = get_time();
            for (int i = factor * (j+1) + rep_1 * j;
                i < (factor + rep_1) * (j + 1); ++i)
            {
                // Код для тестирования:

                // -----------------------
                // test_struct.insert(i);
                test_struct.find(100);
                //test_struct.remove(i);
                // -----------------------
            }
            finish = get_time();
            total = finish - start;
            N += factor;
            n++;
            RBfind << N << " " << total << endl;
            averageY = ( total + averageY*n )/n;
            averageX = ( N*log(N) + averageX*n )/n;
            averageX2 = ( (N*log(N))*(N*log(N)) + averageX2*n )/n;
            averageXY = ( (N*log(N))*total + averageXY*n )/n;
        }
    }

    a = ( averageXY - averageX*averageY) / ( averageX2 - averageX*averageX);
    b = averageY - a*averageX;
    cout << "find:" << endl;
    cout << a << "xlnx+" << b <<endl;
    RBfind.close();

//-------------------------------------insert


    ofstream RBinsert("RBinsert.txt");
    averageX = 0, averageY = 0, averageXY = 0, averageX2 = 0;
    for (int k = 1; k <= 1000 * 1000; k *= 10)
    {
        factor =  k;
        N = 0;
        test_struct = {};


        // Получаем rep_2 точек в этом промежутке
        for (int j = 0; j < rep_2; ++j)
        {

            // Далее rep_1 повторяем действие и измеряем время работы.
            start = get_time();
            for (int i = factor * (j+1) + rep_1 * j;
                i < (factor + rep_1) * (j + 1); ++i)
            {
                // Код для тестирования:

                // -----------------------
                test_struct.insert({i,i});
                // test_struct.find(100);
                //test_struct.remove(i);
                // -----------------------
            }
            finish = get_time();
            total = finish - start;
            N += factor;
            n++;
            RBinsert << N << " " << total << endl;
            averageY = ( total + averageY*n )/n;
            averageX = ( N*log(N) + averageX*n )/n;
            averageX2 = ( (N*log(N))*(N*log(N)) + averageX2*n )/n;
            averageXY = ( (N*log(N))*total + averageXY*n )/n;
        }
    }
    a = ( averageXY - averageX*averageY) / ( averageX2 - averageX*averageX);
    b = averageY - a*averageX;
    cout << "insert:" << endl;
    cout << a << "xlnx+" << b << endl ;
    RBinsert.close();

}

