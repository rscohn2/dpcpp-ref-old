#include <CL/sycl.hpp>
#include <iostream>
#include <iomanip>

#define N 6
#define M 2

using namespace cl::sycl;

int main() {
  queue q;
  buffer<int,2> buf(range<2>(N,N));

  q.submit([&](handler &cgh){
      auto bufacc = buf.get_access<access::mode::read_write>(cgh);
      cgh.parallel_for(nd_range<2>(range<2>(N,N), range<2>(M,M)),
		       [=](nd_item<2> item){
			 int i = item.get_global_id(0);
			 int j = item.get_global_id(1);
			 bufacc[i][j] = i + j;
		       });
    });

  auto bufacc1 = buf.get_access<access::mode::read>();
  for(int i = 0; i < N; i++){
    for(int j = 0; j < N; j++)
      std::cout << std::setw(10) << bufacc1[i][j] << "  ";
    std::cout<<"\n";
  }
  return 0;
}
