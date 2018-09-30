#ifndef LAB_IPARENTCHOISER_H
#define LAB_IPARENTCHOISER_H

#include <memory>

template <typename Genotype, typename Value>
class IParentChoiser {
public:
    virtual size_t ChoiseParent() = 0;
};

template <typename Genotype, typename Value>
using IParentChoiserPtr = std::shared_ptr<IParentChoiser<Genotype, Value>>;


#endif //LAB_IPARENTCHOISER_H
