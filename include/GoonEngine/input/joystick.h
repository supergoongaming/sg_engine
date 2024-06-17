#pragma once
typedef union SDL_Event SDL_Event;

#ifdef __cplusplus
extern "C" {
#endif
typedef enum geGameControllerButton {
	geGameControllerButtonA,
	geGameControllerButtonB,
	geGameControllerButtonX,
	geGameControllerButtonY,
	geGameControllerButtonBACK,
	geGameControllerButtonGUIDE,
	geGameControllerButtonSTART,
	geGameControllerButtonLEFTSTICK,
	geGameControllerButtonRIGHTSTICK,
	geGameControllerButtonLEFTSHOULDER,
	geGameControllerButtonRIGHTSHOULDER,
	geGameControllerButtonDPAD_UP,
	geGameControllerButtonDPAD_DOWN,
	geGameControllerButtonDPAD_LEFT,
	geGameControllerButtonDPAD_RIGHT,
	geGameControllerButtonMISC1,	/* Xbox Series X share button, PS5 microphone button, Nintendo Switch Pro capture button, Amazon Luna microphone button */
	geGameControllerButtonPADDLE1,	/* Xbox Elite paddle P1 (upper left, facing the back) */
	geGameControllerButtonPADDLE2,	/* Xbox Elite paddle P3 (upper right, facing the back) */
	geGameControllerButtonPADDLE3,	/* Xbox Elite paddle P2 (lower left, facing the back) */
	geGameControllerButtonPADDLE4,	/* Xbox Elite paddle P4 (lower right, facing the back) */
	geGameControllerButtonTOUCHPAD, /* PS4/PS5 touchpad button */
	geGameControllerButtonMAX
} geGameControllerButton;

void geHandleJoystickEvent(const SDL_Event *event);
void geInitializeJoysticks();
void geUpdateControllerLastFrame();
int geGamepadLeftAxisXThisFrameMovement(const int padNum);
int geGamepadLeftAxisYThisFrameMovement(const int padNum);
float geGamepadLeftAxisXFloat(const int padNum);
float geGamepadLeftAxisYFloat(const int padNum);
bool geGamepadButtonJustReleased(const int padNum, const geGameControllerButton button);
bool geGamepadButtonJustPressed(const int padNum, const geGameControllerButton button);
bool geGamepadButtonHeldDown(const int padNum, const geGameControllerButton button);

#ifdef __cplusplus
}
#endif