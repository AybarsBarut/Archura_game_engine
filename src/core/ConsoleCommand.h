#pragma once

#include <string>
#include <functional>
#include <vector>
#include <unordered_map>
#include <memory>

namespace Archura {

    // Komut callback türü
    using CommandCallback = std::function<void(const std::vector<std::string>&)>;

    /**
     * @class ConsoleCommand
     * @brief Tek bir konsol komutunu temsil eder
     */
    class ConsoleCommand {
    public:
        ConsoleCommand(
            const std::string& name,
            const std::string& description,
            CommandCallback callback,
            bool requiresCheats = false
        )
            : m_name(name),
              m_description(description),
              m_callback(callback),
              m_requiresCheats(requiresCheats)
        {}

        const std::string& GetName() const { return m_name; }
        const std::string& GetDescription() const { return m_description; }
        bool RequiresCheats() const { return m_requiresCheats; }

        void Execute(const std::vector<std::string>& args) {
            if (m_callback) {
                m_callback(args);
            }
        }

    private:
        std::string m_name;
        std::string m_description;
        CommandCallback m_callback;
        bool m_requiresCheats;
    };

    /**
     * @class ConsoleVariable
     * @brief Konsol değişkeni (CVAR) - konfigüre edilebilir oyun değişkeni
     */
    class ConsoleVariable {
    public:
        enum class Type { Float, Int, String, Bool };

        ConsoleVariable(
            const std::string& name,
            const std::string& defaultValue,
            Type type,
            const std::string& description = "",
            bool requiresCheats = false
        )
            : m_name(name),
              m_value(defaultValue),
              m_defaultValue(defaultValue),
              m_type(type),
              m_description(description),
              m_requiresCheats(requiresCheats)
        {}

        const std::string& GetName() const { return m_name; }
        const std::string& GetValue() const { return m_value; }
        const std::string& GetDefaultValue() const { return m_defaultValue; }
        Type GetType() const { return m_type; }
        const std::string& GetDescription() const { return m_description; }
        bool RequiresCheats() const { return m_requiresCheats; }

        void SetValue(const std::string& value) { m_value = value; }
        void ResetToDefault() { m_value = m_defaultValue; }

        // Tip dönüşümleri
        float GetAsFloat() const;
        int GetAsInt() const;
        bool GetAsBool() const;

    private:
        std::string m_name;
        std::string m_value;
        std::string m_defaultValue;
        Type m_type;
        std::string m_description;
        bool m_requiresCheats;
    };

} // namespace Archura
