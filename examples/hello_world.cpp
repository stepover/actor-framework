#include <string>
#include <iostream>

#include "caf/all.hpp"

using std::endl;
using std::string;

using namespace caf;

/*
 * simon:
 * "mirror" 是一个函数,返回一个 behavior  ,
 * 使用 system.spawn(mirror); 创建基于函数的 Actor 时，mirror 作为参数传入spawn中,
 * spawn 内部会创建 event_based_actor 的实例并调用mirror函数
 *
 * lambda 表达式被用来创建一个 behavior 实例.
 *
 * the behavior class template construct function was called
 *
 *  code from "behavior.hpp"
 *  /// The list of arguments can contain match expressions, message handlers,
    /// and up to one timeout (if set, the timeout has to be the last argument).
    template <class T, class... Ts>
    behavior(T x, Ts&&... xs) {
      assign(std::move(x), std::forward<Ts>(xs)...);
    }
 *
 */
behavior mirror(event_based_actor* self) {
  // return the (initial) actor behavior
  return { // 就是 return behavior{  的缩写，使用了隐式转换
    // a handler for messages containing a single string
    // that replies with a string
    [=](const string& what) -> string {
      // prints "Hello World!" via aout (thread-safe cout wrapper)
      aout(self) << what << endl;
      // reply "!dlroW olleH"
      return string(what.rbegin(), what.rend());
    }
  };
}

void hello_world(event_based_actor* self, const actor& buddy) {
  // send "Hello World!" to our buddy ...
  self->request(buddy, std::chrono::seconds(10), "Hello World!").then(
    // ... wait up to 10s for a response ...
    [=](const string& what) {
      // ... and print it
      aout(self) << what << endl;
    }
  );
}

int main() {
  // our CAF environment
  actor_system_config cfg;
  //simon:C11, assign value to member var,same as "actor_system system(cfg); "
  actor_system system{cfg};
  // create a new actor that calls 'mirror()'
  auto mirror_actor = system.spawn(mirror);
  // create another actor that calls 'hello_world(mirror_actor)';
  system.spawn(hello_world, mirror_actor);
  // system will wait until both actors are destroyed before leaving main
}
