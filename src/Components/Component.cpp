#include "Component.h"

namespace Engine
{

ComponentRegister& ComponentRegister::Instance()
{
    static ComponentRegister instance;
    return instance;
}

}
