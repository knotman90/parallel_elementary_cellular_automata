#include "CA.h"
#include "safe_queue.h"

#include <fstream>
#include <iostream>
#include <sstream>
#include <thread>
#include <variant>
#include <vector>

using Automata = CA<Iteration_Vector>;
std::vector<std::thread> workers;
std::vector<std::thread> writers;

safe_queue<Automata> input;
safe_queue<Automata> output;

void writer_fn()
{
  while (output.size() > 0)
  {
    Automata automata = output.dequeue_wait();

    std::ostringstream os;
    os << "file" << automata.get_rule_number() << ".pgm";
    const std::string filename(os.str());
    std::ofstream file;
    file.open(filename.c_str());

    file << "P2" << std::endl;
    file << automata.get_size() << " " << automata.get_max_iterations() << std::endl;
    file << 1 << std::endl;
    // write_automata_to_file(automata);
    automata.print(file);
    // cout<<std::endl;
    // cout.flush();
  }
}
void worker_fn()
{
  while (input.size() > 0)
  {
    std::variant<bool, Automata> res = input.dequeue_if_not_empty();
    if (res.index() == 1)
    {
      Automata automata = std::get<Automata>(res);
      automata.evolve();
      output.enqueue(automata);
    }
    else
    {
      return;
    }
  }
}

void calculate(const int S, const int IT, const uint nworkers,
               const uint nwriters, const bool random)
{
  for (uint i = 0; i <= 255; ++i)
  {
    Iteration_Vector initial_state(S);
    if (random)
      initial_state.random_initialize();
    else
    {
      initial_state.set(S / 2, 1);
      initial_state.set(S / 2 - 1, 1);
      initial_state.set(S / 2 + 1, 1);
    }

    Automata automata(i, S, IT, initial_state);
    input.enqueue(automata);
  }
  workers.reserve(nworkers);
  for (uint i = 0; i < nworkers; ++i)
    workers.push_back(std::thread(worker_fn));

  for (int i = 0; i < nworkers; ++i)
    if (workers[i].joinable())
      workers[i].join();

  writers.reserve(nwriters);
  for (uint i = 0; i < nworkers; ++i)
    writers.push_back(std::thread(writer_fn));

  for (uint i = 0; i < nworkers; ++i)
    if (writers[i].joinable())
      writers[i].join();
}

int main(int argc, char *argv[])
{
  const unsigned long size     = std::stoul(argv[1]);
  const unsigned long it       = std::stoul(argv[2]);
  const bool random            = (std::stoul(argv[3]) > 0);
  const unsigned long nworkers = std::stoul(argv[4]);
  const unsigned long nwriters = std::stoul(argv[5]);

  srand(time(0));

  calculate(size, it, nworkers, nwriters, random);
  return 0;
}