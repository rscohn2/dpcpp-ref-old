#include <vector>
#include <CL/sycl.hpp>

#define SIZE 1024

using namespace cl::sycl;

class Vassign {
  accessor<int, 1, access::mode::read_write,
	   access::target::global_buffer> access;

public:
  Vassign(accessor<int, 1, access::mode::read_write,
	  access::target::global_buffer> &access_) : access(access_) {}
  void operator()(id<1> id) { access[id] = 1; }
};

int main() {
  std::array<int, SIZE> a;

  for (int i = 0; i<SIZE; ++i) {
    a[i] = i;
  }

  {
    range<1> a_size{SIZE};
    buffer<int>  a_device(a.data(), a_size);
    queue q;

    q.submit([&](handler &cgh) {
	auto a_in = a_device.get_access<access::mode::write>(cgh);
	cgh.single_task([=]() {
	    a_in[0] = 2;
	  });
      });
  }

  {
    range<1> a_size{SIZE};
    buffer<int>  a_device(a.data(), a_size);
    queue q;
    q.submit([&](handler &cgh) {
	auto a_in = a_device.get_access<access::mode::read_write,
					access::target::global_buffer>(cgh);
	Vassign F(a_in);
	cgh.parallel_for(range<1>(SIZE), F);
      });
  }
} 

