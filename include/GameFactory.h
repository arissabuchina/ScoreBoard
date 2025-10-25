#include <memory>
#include <string>
#include <algorithm>
#include "GameStrategy.h"
#include "GameStrategies/Game501.h"
#include "GameStrategies/Game301.h"
#include "GameStrategies/GameCricket.h"
#include "GameStrategies/GameAroundTheWorld.h"

class GameFactory {
public:
    static std::shared_ptr<GameStrategy> createGame(std::string type) {
        normalizeString(type);
        if (type == "501") return std::make_shared<Game501>();
        if (type == "301") return std::make_shared<Game301>();
        if (type == "cricket") return std::make_shared<GameCricket>();
        if (type == "aroundtheworld") return std::make_shared<GameAroundTheWorld>();
        return nullptr;
    }

private:
    static void normalizeString(std::string& s) {
        // convert to lowercase & remove spaces
        for (auto& c : s) c = tolower(c);
        s.erase(remove_if(s.begin(), s.end(), isspace), s.end());
    }
};
