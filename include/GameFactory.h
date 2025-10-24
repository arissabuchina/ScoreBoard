#include <memory>
#include <string>
#include "GameStrategy.h"
#include "Game501.h"
#include <algorithm>
//#include "Game301.h"
//#include "GameCricket.h"
//#include "GameAroundTheClock.h"

class GameFactory {
public:
    static std::shared_ptr<GameStrategy> createGame(std::string type) {
        normalizeString(type);
        if (type == "501") return std::make_shared<Game501>();
        //if (type == "301") return std::make_shared<Game301>();
        //if (type == "cricket") return std::make_shared<GameCricket>();
        //if (type == "aroundtheclock") return std::make_shared<GameAroundTheClock>();
        return nullptr;
    }

private:
    static void normalizeString(std::string& s) {
        // convert to lowercase & remove spaces
        for (auto& c : s) c = tolower(c);
        s.erase(remove_if(s.begin(), s.end(), isspace), s.end());
    }
};
