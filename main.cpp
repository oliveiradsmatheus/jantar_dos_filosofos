#include <iostream>
#include <thread>
#include <vector>
#include <chrono>
#include <mutex>

using std::thread;
using std::cout;
using std::endl;
using std::vector;
using std::this_thread::sleep_for;
using std::rand;
using std::chrono::milliseconds;
using std::mutex;
using std::lock;
using std::lock_guard;
using std::adopt_lock;


/**
 * Classe que implementa uma mesa de jantar com todos os  cinco garfos
 */
class Mesa {
public:
    /**
     * O recurso compartilhado que todos os filósofos concorrerão
     */
    vector<mutex> garfos;

    Mesa() : garfos(5) {
    }
};


/**
 * Classe que implementa a existência de um Filósofo que deve
 * pensar e comer
 */
class Filosofo {
public:
    Filosofo(const int id, Mesa &mesa)
        : id_filosofo(id),
          mesa(mesa) {
    }

    /*
     * Inicia a execução paralela de um Filósofo que compartilha a mesma
     * mesa com outros Filósofos
     */
    thread *Criar() {
        return new thread([=] {
            existir();
        });
    }

private:
    /* O identificador do Filósofo */
    int id_filosofo;

    /* Uma referência para a instância da mesa compartilhada */
    Mesa &mesa;

    /**
     * A existência de um Filósofo
     */
    void existir() {
        while (true) {
            pensar();
            comer();
        }
    }

    /**
     * O tempo aleatório que cada Filósofo passa pensando
     */
    void pensar() {
        cout << "Filósofo " << id_filosofo << " pensando..." << endl;
        sleep_for(milliseconds(rand() % 5000));
    }

    /**
     * O processo de comer de cada instância de Filósofo. Primeiramente, um
     * Filósofo tenta bloquear (pegar) ambos os garfos que ele precisa para comer. Existem
     * 5 garfos. Se um garfo for adquirido por qualquer outro Filósofo, a
     * thread esperará até que o recurso/garfo seja liberado.
     *
     * Quando ambos os garfos são adquiridos pela execução da thread atual, é hora
     * do Filósofo comer. Ao final deste processo, ambos os garfos são
     * liberados.
     */
    void comer() {
        int esquerda = id_filosofo;
        int direita = (id_filosofo + 1) % mesa.garfos.size();

        cout << "Filósofo " << id_filosofo << " pegando os garfos..." << endl;

        /* Bloqueia (trava) ambos os mutexes sem causar deadlock */
        lock(mesa.garfos[esquerda], mesa.garfos[direita]);

        /* Ao final deste escopo, os mutexes serão liberados (devido ao adopt_lock) */
        lock_guard<mutex> garfo_esquerdo(mesa.garfos[esquerda], adopt_lock);
        lock_guard<mutex> garfo_direito(mesa.garfos[direita], adopt_lock);

        cout << "Filósofo " << id_filosofo << " comendo..." << endl;
        sleep_for(milliseconds(rand() % 10000));

        cout << "Filósofo " << id_filosofo << " soltando os garfos..." << endl;
    }
};


int main(int argc, char *argv[]) {
    cout << "O JANTAR DOS FILOSOFOS" << endl << endl;
    Mesa mesa;

    vector<thread *> threads(5);

    for (int i = 0; i < 5; i++) {
        Filosofo *filosofo = new Filosofo(i, mesa);
        threads[i] = filosofo->Criar();
    }

    for (int i = 0; i < 5; i++) {
        threads[i]->join();
    }

    return EXIT_SUCCESS;
}
