#include "Input.h"

// Change these constants to suit your keys
const char Input::LEFT_KEY = 'q';
const char Input::RIGHT_KEY = 'w';


void Input::sentKeyInput(char key, bool pressed) {
	// send key press to screen
	// to release the key, set pressed = false
	INPUT key_press = { 0 };
	key_press.type = INPUT_KEYBOARD;
	key_press.ki.wVk = VkKeyScanEx(key, GetKeyboardLayout(NULL)) & 0xFF;
	key_press.ki.wScan = 0;
	key_press.ki.dwExtraInfo = 0;
	key_press.ki.dwFlags = (pressed ? 0 : KEYEVENTF_KEYUP);
	SendInput(1, &key_press, sizeof INPUT);
}

//void Input::linearMove(POINT startScaledPoint, POINT endScaledPoint, int duration) {
//	float distance = sqrt(pow((endScaledPoint.x - startScaledPoint.x), 2) + pow((endScaledPoint.y - startScaledPoint.y), 2));
//}