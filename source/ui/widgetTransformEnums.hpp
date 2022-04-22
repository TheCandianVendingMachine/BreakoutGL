// widgetTransformEnums.hpp
// Enums associated with transformations
#pragma once
#undef ABSOLUTE

namespace widgetTransformEnums
	{
		enum class type
			{
				ABSOLUTE,	// Absolute position. (0, 0) is top left of window, and position is defined as an offset from this position
				PERCENT		// Percent position. (1, 1) is the bottom right corner of window, (0, 0) is top left corner.
			};

		enum class anchor
			{
				TOP_LEFT,
				TOP_MIDDLE,
				TOP_RIGHT,
				MIDDLE_LEFT,
				MIDDLE,
				MIDDLE_RIGHT,
				BOTTOM_LEFT,
				BOTTOM_MIDDLE,
				BOTTOM_RIGHT
			};
	}
