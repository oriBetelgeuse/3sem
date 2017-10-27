/* Рассчитать определённый интеграл какой-нибудь простой, интегрируемой аналитически на некотором промежутке функции f(x) методом Монте-Карло:
расчёт проводить параллельно в n-потоков некоторой программы А, генерируя в каждом из них N/n точек равномерно как по интервалу, так и по области значений.
Собирать и обрабатывать результаты в программе Б, читающей результаты из разделяемой памяти. [В случае использования независимых ячеек разделяемой памяти,
сохранение результатов можно обеспечить без синхронизации, в случае разделяемой ячейки в разделяемой памяти - синхронизация необходима. Реализация способа хранения
влияет на результаты эксперимента 2 а,б*) (см. ниже).

Задачи, поставленные перед Вами как исследователем:
1) Оценить прирост производительности в связи с использованием n' потоков вместо 1, где n' - число физических ядер Вашей машины.
Узнать число ядер-например, 'cat /cpu/procinfo'
2) Провести серию измерений, в которой, увеличивая число n' :
  а) пронаблюдать стагнацию роста производительности при увеличении числа потоков
  б*) определить, при каких n' (насколько больших) накладные расходы от использования многопоточности [и синхронизации, если она имела место]
  превосходят преимущества их использования.
*/
#include <unistd.h>
#include <sys/types.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <pthread.h>
#include <time.h>


#define N  10000000
int n;
double Integral = 0;
int number = 0;
unsigned int seed;
pthread_spinlock_t spin;


int Check(double x, double y) {
  if (x * x > y)
    return 1;
  else
    return 0;
}

void * f(void * arg) {
  double x;
  double y;
  int p = 0;
  int seed_l = seed + number;
  number++;
 // printf("Number %d\n", number);
  for (int i = 0; i < (N / n); i++) {
    x = rand_r(&seed_l);
    x /= RAND_MAX;
    y = rand_r(&seed_l);
    y /= RAND_MAX;
    p += Check(x, y);
//    if (pthread_spin_trylock(&spin) == 0) {
  //    Integral += p;
//      p = 0;
//      pthread_spin_unlock(&spin);
//    }
  }
  pthread_spin_lock(&spin);
  Integral += p;
  pthread_spin_unlock(&spin);
  return 0;
}

int main(int argc, char *argv[]) {
  n=atoi(argv[1]);
  pthread_t tid[n];
  int status = pthread_spin_init(&spin, 0);
  seed=time(NULL);
//  printf("Handling %d points:in %d threads...\n", N, n);
  for (int i = 0; i < n; i++) {
    int er = pthread_create(&(tid[i]), NULL, f, NULL);
    if (er != 0) {
      printf("Can't create thread\n");
      exit(-1);
    }
  }
  for (int i = 0; i < n; i++) {
    pthread_join(tid[i], NULL);
  }

  Integral /= N;

  printf("%f\n", Integral);
  pthread_spin_destroy(&spin);
  return 0;
}
