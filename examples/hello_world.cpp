#include <string>
#include <iostream>

#include "caf/all.hpp"

using std::endl;
using std::string;

using namespace caf;

/*
 * simon:
 * "mirror" is a function,return a lambda expressions ,
 * the lambda will be used to create a behavior instance.
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
  return {
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
