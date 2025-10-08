#include <iostream>

#include "Hch.hh"

int main (int argc, char *argv[])
{
  Hch{argc, argv}.run();

  std::cout << "bye!\n";
  return 0;
}
