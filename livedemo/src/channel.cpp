#include <livedemo/channel.h>

namespace livedemo
{

double
binary_symm_channel::binary_entropy_function (double)
{
  return 0.;
}

double
binary_symm_channel::evaluate_capacity (double p)
{
  return 1 - binary_entropy_function (p);
}

} // namespace livedemo
