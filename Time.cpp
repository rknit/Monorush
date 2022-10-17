#include "Time.h"

void Time::_UpdateTime(float time) {
	this->_prevTime = this->_currentTime;
	this->_currentTime = time;

	this->unscaledDeltaTime = this->_currentTime - this->_prevTime;
	this->deltaTime = this->timeScale * this->unscaledDeltaTime;
}

std::string Time::FormatMinute(float seconds, bool spacing) {
	int minute = (int)seconds / 60;
	int second = (int)seconds - minute * 60;

	std::string min, sec;

	if (minute >= 10) min = std::to_string(minute);
	else min = "0" + std::to_string(minute);

	if (second >= 10) sec = std::to_string(second);
	else sec = "0" + std::to_string(second);

	if (!spacing)
		return min + ":" + sec;
	else
		return min + " : " + sec;
}

std::string Time::FormatMilli(float seconds) {
	int milli = (int)(((float)seconds - (int)seconds) * 1000.0f);
	if (milli >= 100)
		return std::to_string(milli);
	else if (milli >= 10)
		return "0" + std::to_string(milli);
	else
		return "00" + std::to_string(milli);
}