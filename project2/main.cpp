#include <boost/interprocess/managed_shared_memory.hpp>
#include <cstdlib> //std::system
#include <cstddef>
#include <cassert>
#include <utility>
#include <iostream>

int main(int argc, char *argv[]) {
   using namespace boost::interprocess;

   if (argc == 1) {  // Parent process
      // Remove shared memory on construction and destruction.
      struct shm_remove
      {
         shm_remove() { shared_memory_object::remove("MySharedMemory"); }
         ~shm_remove(){ shared_memory_object::remove("MySharedMemory"); }
      } remover;

      // Construct managed shared memory.
      managed_shared_memory segment(create_only, "MySharedMemory", 65536);

      // Get input from user.
      std::string input;
      std::getline(std::cin, input);

      // Allocate a portion of the segment to store the string.
      char * msg = segment.construct<char>("input")[input.size() + 1]('\0');

      // Check if we were successful.
      if (!msg)
          return 1;

      // Assign input string to it.
      std::strcpy(msg, input.c_str());

      // Launch child process.
      std::string s(argv[0]); s += " child ";
      if (std::system(s.c_str()))
          return 2;

      // Check child has destroyed all objects.
      if (segment.find<char>("input").first)
         return 3;
   } else { // Child process
      // Open managed shared memory
      managed_shared_memory segment(open_only, "MySharedMemory");

      // Try and get the input.
      char * msg = segment.find<char>("input").first;
      // If it was not found, exit with error.
      if (!msg)
          return 1;

      std::cout << msg << std::endl;

      // We're done, delete input object.
      segment.destroy<char>("input");
   }
   return 0;
}
