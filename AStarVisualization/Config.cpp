#include "Config.hpp"


GameConfig& GameConfig::instance() {
	static GameConfig config;
	return config;
}