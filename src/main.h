#include <pebble.h>
#pragma once

static const GPathInfo MINUTE_TARGET_POINTS = {
	4,
	(GPoint []) {
		{-5, -5},
		{5, -5},
		{5, 5},
		{-5, 5},
	}
};

static const GPathInfo HOUR_TARGET_POINTS = {
	4,
	(GPoint []) {
		{0, -5},
		{5, 0},
		{0, -5},
		{-5, 0},
	}
};
