#pragma once
#include "colors"
#include "enums"
#include "gsl/gsl"
#include "jthread"
#include "math"
#include "named"
#include "ranges"
#include "util"

#include <execution>
#include <map>
#include <set>

using namespace std::string_literals;
using namespace std::string_view_literals;

using namespace std::chrono_literals;
using Clock = std::chrono::system_clock;
using Duration = Clock::duration;
using std::chrono::microseconds;
using std::chrono::milliseconds;
using std::chrono::seconds;

using namespace std::ranges;
using namespace ranges::views;

#include "game.pc.h"
