//////////////////////////////////////////////////////////////////////////////
//
// Παράδειγμα δημιουργίας ενός παιχνιδιού χρησιμοποιώντας τη βιβλιοθήκη raylib
//
//////////////////////////////////////////////////////////////////////////////

#include <stdio.h>
#include "raylib.h"

#include "interface.h"
#include "state.h"

State state;



void update_and_draw() {
	StateInfo info = state_info(state);
	if (info->game_started == false)
	{
		Rectangle playButton = { GetScreenWidth() / 2 - 110, GetScreenHeight() / 2 - 10, 200, 50 };
		// Check for mouse input to start the game
		if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON) && CheckCollisionPointRec(GetMousePosition(), playButton))
		{
			info->game_started = true;
		}
	}
	else{
		
		struct key_state keys;
		//καλω ις συναρτησεις IskeyDown και isKeyPressed για να ελεγχξω ποτε θα πατηθουν τα πληκτρα
		keys.down = IsKeyDown(KEY_DOWN);
		keys.p = IsKeyPressed(KEY_P);
		keys.n = IsKeyPressed(KEY_N);
		keys.enter = IsKeyPressed(KEY_ENTER);
		keys.right = IsKeyDown(KEY_RIGHT);
		keys.up = IsKeyDown(KEY_UP);
		keys.left = IsKeyDown(KEY_LEFT);
		state_update(state , &keys);

		
		//κανω restart οταν πατηθει το enter δημιουργωντας νεο state και σβηνω το προηγουμενο state
		if(info->playing == false){
			
		
			if(keys.enter){
			state_destroy(state);
			state = state_create();
			}
		}
		//αμα βρισκεται σε pause και πατηθει το n τοτε καλω την state_update με το pause να ειναι falseS
		if(info->paused == true){
			if(keys.n){
				info->paused = false;
				state_update(state , &keys);
				info->paused = true;
			}
		}
	}
	//ζωγραφιζει το frame
	interface_draw_frame(state);
}

int main() {
	//δημιουργω το state
	state = state_create();
	StateInfo info = state_info(state);
	info->game_started = false;
	interface_init();
	
	start_main_loop(update_and_draw);
	interface_close();
	return 0;
}



