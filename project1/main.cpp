#include <boost/interprocess/managed_shared_memory.hpp>
#include <cstdlib>
#include <sstream>
#include <iostream>

int main (int argc, char *argv[]) {
   using namespace boost::interprocess;
   if (argc == 1) {  // Parent process
      // Remove shared memory on construction and destruction
      struct shm_remove
      {
         shm_remove() {  shared_memory_object::remove("MySharedMemory"); }
         ~shm_remove(){  shared_memory_object::remove("MySharedMemory"); }
      } remover;

      // Create a managed shared memory segment
      managed_shared_memory segment(create_only, "MySharedMemory", 65536);

      // Allocate a portion of the segment (raw memory)
      char * msg = static_cast<char*>(segment.allocate(1024));

      // Assign "Hello World!" to it.
      std::strcpy(msg, "Hello World!");

      // An handle from the base address can identify any byte of the shared
      // memory segment even if it is mapped in different base addresses
      managed_shared_memory::handle_t handle = segment.get_handle_from_address(msg);
      std::stringstream s;
      s << argv[0] << " " << handle << std::ends;
      // Launch child process
      if (0 != std::system(s.str().c_str()))
         return 1;
   }
   else {
      // Open managed segment
      managed_shared_memory segment(open_only, "MySharedMemory");

      // An handle from the base address can identify any byte of the shared
      // memory segment even if it is mapped in different base addresses
      managed_shared_memory::handle_t handle = 0;

      // Obtain handle value
      std::stringstream s; s << argv[1]; s >> handle;

      // Get buffer local address from handle
      char * msg = static_cast<char*>(segment.get_address_from_handle(handle));

      // Print the output to stdout.
      std::cout << msg << std::endl;

      // Deallocate previously allocated memory
      segment.deallocate(msg);
   }
   return 0;
}
