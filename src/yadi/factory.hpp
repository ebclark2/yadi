#ifndef YADI_FACTORY_HPP__
#define YADI_FACTORY_HPP__

#include <functional>
#include <map>
#include <string>

namespace yadi 
{

template <typename BaseType>
class factory
{
public:
using base_type = BaseType;

private:
factory()=delete;
factory(factory const&)=delete;
factory & operator=(factory const&)=delete;


};

} // namespace yad

#endif // YADI_FACTORY_HPP__
