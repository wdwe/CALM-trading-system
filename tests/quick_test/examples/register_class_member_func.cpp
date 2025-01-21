#include <iostream>
#include <functional>
#include <unordered_map>
#include <string>

using CBDict = std::unordered_map<std::string, std::vector<std::function<void(int)>>>;

class Engine {
private:
    int x{0};
    CBDict& cb;
public:
    Engine(int x, CBDict& cb): x{x}, cb{cb} {
        register_cb();
    }
    void update(int y) {x = y;}
    void print() const {
        std::cout << "value of x is " << x << std::endl;
    }
    void register_cb() {
        cb["a"].emplace_back([this](auto && PH1){ this->update(std::forward<decltype(PH1)>(PH1)); });
    }
};

int main() {
    CBDict cb;
    Engine engine{0, cb};
    engine.register_cb();
    cb["a"].emplace_back([&engine](auto && PH1){ engine.update(std::forward<decltype(PH1)>(PH1)); });
    cb["a"][0](2);
    engine.print();

    return 0;
}