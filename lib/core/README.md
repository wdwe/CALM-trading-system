# Todo
- Make `Event` class a template (`Event<SomeEventType>`) so that different modules can have their own EventType instead 
of registering the type in the central event type.
- The callbacks are stored in `std::function` which involves virtual function and `nullptr` checking. There is some performance cost.
It can be changed to storing a pure function ptr and `void*` that points to the class. The disadvantage is that each 
callback member function requires a wrapper of 
    ```c++
    void callback_wrapper(void* ptr, Event event) {
        auto obj = static_cast<MyClass*>(ptr);
        obj->callback(event);
    }
    ```
  which may be a good use case for macro (?).
- Split `buy`/`sell` to different order classes and use template to remove if/else checking. 
- In `Portfolio` track PnL in one currency. Currency data subscription is needed and contract's currency type needs to be queried.
- Make `RiskManager` a template so if/else condition can be shifted to compile time. This may require re-design the whole
system around different algos.