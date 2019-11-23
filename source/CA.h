#include <bitset>
#include <cassert>
#include <vector>
#include <sstream>

#ifndef _CA_H
#define _CA_H

//////////////////////////////
//	    class Rule          //
//////////////////////////////
class Rule
{
    public:
  Rule(const int N) : rule_num(N) { set_rule(N); }

  bool get_next_state(const bool left, const bool center,
                      const bool right) const
  {
    const uint idx = 4 * !left + 2 * !center + !right;
    return rule[7 - idx];
  }
  uint get_rule_number() const { return rule_num; }

    private:
  uint rule_num; // change to uint_8;
  std::bitset<8> rule;

  void set_rule(const uint N)
  {
    rule_num = N;
    for (int i = 0; i < 8; ++i)
    {
      rule[i] = (N >> i) & 1;
    }
  }
};

//////////////////////////////
//	    class Iteration     //
//////////////////////////////
template <class Child> class Iteration
{
    public:
  Iteration(){};
  inline void set(const size_t idx, bool value)
  {
    assert(idx < size());
    static_cast<Child *>(this)->set(idx, value);
  };
  inline bool get(const size_t idx)
  {
    assert(idx < size());
    return static_cast<Child *>(this)->get(idx);
  }

  inline bool size() { return static_cast<Child *>(this)->size(); }
};

template <uint S> class Iteration_Bitset : public Iteration<Iteration_Bitset<S>>
{
    public:
  Iteration_Bitset() : space(0){};

  inline void set(const size_t idx, bool value) { space[idx] = value; }

  inline bool get(const size_t idx) const { return space[idx]; }

  constexpr inline uint size() const { return S; }

  constexpr inline void random_initialize()
  {
    for (uint i = 0; i < S; ++i)
    {
      set(i, rand() % 2);
    }
  }

  void print(std::ostream &out)
  {
    for (uint i = 0; i < S; ++i)
    {
      if (get(i))
        out << '1';
      else
        out << "0";
    }
  }
  std::bitset<S> space;
};

class Iteration_Vector
{
    public:
  Iteration_Vector(const int _size) : size(_size), space(_size){};

  inline void set(const size_t idx, bool value) { space[idx] = value; }

  inline bool get(const size_t idx) const { return space[idx]; }

  constexpr inline uint get_size() const { return size; }

  inline void random_initialize()
  {
    for (uint i = 0; i < size; ++i)
    {
      set(i, rand() % 2);
    }
  }

  void print(std::ostream &out)
  {
    for (uint i = 0; i < size; ++i)
    {
      if (get(i))
        out << "1 ";
      else
        out << "0 ";
    }
  }

private:
  const uint size;
  std::vector<bool> space;
};


template <typename SpaceType> 
class CA
{
    public:
  using space_imp  = SpaceType;
  using Cell_space = std::vector<SpaceType>;

  CA(const uint _rule_n, const uint _size, const uint _max_iterations,
     const space_imp &initial_state)
      : rule(_rule_n), size(_size), max_iterations(_max_iterations)
  {
    assert(max_iterations > 0);
    assert(size > 0);

    space.reserve(max_iterations);
    space.push_back(initial_state);
  }

  void evolve()
  {
    for (uint i = 0; i < max_iterations; ++i)
    {
      space.push_back(evolve_iteration(space[i]));
    }
  }

  space_imp evolve_iteration(space_imp &iteration)
  {
    space_imp next_iteration(size);
    for (uint i = 1; i < size - 1; i++)
    {
      const bool left   = iteration.get(i - 1);
      const bool center = iteration.get(i);
      const bool right  = iteration.get(i + 1);
      next_iteration.set(i, rule.get_next_state(left, center, right));
    }
    next_iteration.set(0,
                       rule.get_next_state(iteration.get(size - 1),
                                           iteration.get(0), iteration.get(1)));
    next_iteration.set(0, rule.get_next_state(iteration.get(size - 2),
                                              iteration.get(size - 1),
                                              iteration.get(0)));

    return next_iteration;
  }

  void print(std::ostream &out)
  {
    for (uint i = 0; i < max_iterations; ++i)
    {
      space[i].print(out);
      out << std::endl;
    }
  }

  void operator()() { evolve(); }

  uint get_rule_number() { return rule.get_rule_number(); }

  uint get_size() { return size; }

  uint get_max_iterations() { return max_iterations; }

    private:
  Rule rule;
  const uint size;
  const uint max_iterations;
  Cell_space space;
};


#endif //_CA_H