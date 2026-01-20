#include "ConsoleCommand.h"
#include <sstream>
#include <stdexcept>

namespace Archura {

    float ConsoleVariable::GetAsFloat() const {
        try {
            return std::stof(m_value);
        }
        catch (...) {
            return 0.0f;
        }
    }

    int ConsoleVariable::GetAsInt() const {
        try {
            return std::stoi(m_value);
        }
        catch (...) {
            return 0;
        }
    }

    bool ConsoleVariable::GetAsBool() const {
        return m_value == "1" || m_value == "true" || m_value == "yes";
    }

} // namespace Archura
