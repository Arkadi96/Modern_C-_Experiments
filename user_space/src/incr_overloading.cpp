/*
   This example illustrates how two different types of increments work.
*/

#include <iostream>

using namespace std;

class Score {
    public:
        using uint = unsigned long int;
    private:
        uint m_score;
    public:
        uint get_score() const {
            return m_score;
        }
        void print() const {
            cout << "\nThe score is: " << m_score;
        }
    public:
        //When we use && to declare a reference variable, we are telling the
        //compiler that the reference should bind to a temporary object.
        //MOVE means TRANSFERRING THE OWNERSHIP
        //that is why we calling destructor at the end.
        Score(Score&& sc) {
            cout << "\nMove constructor";
            this->m_score = sc.get_score();
            sc.~Score();
        }
        //COPY means DONT MODIFY SOURCE
        //that is why we say const (by the way)
        Score(const Score& sc) {
            cout << "\nCopy constructor";
            this->m_score = sc.get_score();
        }
        Score& operator=(const Score& sc) {
            cout << "\nAssignment operator";
            if (this != &sc) {
                this->m_score = sc.get_score();
            }
            return *this;
        }
        // We are modifying whats inside so we have to mention
        // return value as a reference to a object.
        Score& operator++() {
            cout << "\nPre ++ operator";
            this->m_score += 1;
            return *this;
        }
        // Post increment calls every time when "x++" or "y = x++"
        // That means in both cases they return the same "old" preserved data.
        // Why compilers dont rise red flags or at last warnings ?
        // - because operator overloadings (incr, decr) are treated differently by
        // the compiler. They are considered side-effecting operators,
        // where the increment itself is the main purpose, not necessarily the
        // returned original value.
        Score operator++([[maybe_unused]] int q) {
            cout << "\nPost ++ operator";
            Score tmp(this->m_score);
            this->m_score += 1;
            return tmp;
        }
        Score(int value) : m_score(value)
        {}
        ~Score()
        {}
};

int main(int argv, char** argc)
{
    Score sc1(0);
    Score sc2(0);
    sc2 = sc1++;
    cout << "\nsc2 = sc1++";
    sc1.print();
    sc2.print();
    sc2 = ++sc1;
    cout << "\nsc2 = ++sc1";
    sc1.print();
    sc2.print();
    sc2 = sc1++;
    cout << "\nScore sc3 = sc2";
    Score sc3 = sc2;
    sc3.print();
    sc3++;
    cout << "\nsc3++";
    sc3.print();
    sc1 = sc3++;
    cout << "\nsc1 = sc3++";
    sc3.print();
    Score sc4(move(sc3));
    cout << "\nsc4(std::move(sc3))";
    sc4.print();
    sc3.print();
    cout << endl;
    return 1;
}
