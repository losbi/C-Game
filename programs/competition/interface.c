#include "raylib.h"

#include <stdio.h>
#include "state.h"
#include "interface.h"

// assets
//φτιανχω ενα texture2d που περιχει ολες τις εικονες με τον ιπποτη

int highScore = 0;

bool game_started =  false;

//COIN 
#define COIN_TIME 0.2
#define COIN_COUNT 15


#define soldier_img_1_size 120 



//SLOW RUN 
#define SlOW_TIME 0.1

//RUN
#define RUN_TIME 0.05
#define RUN_COUNT 10

//ATACK
#define ATTACK_TIME 0.5
#define ATTACK_COUNT 4

//Jump
#define JUMP_TIME 0.1
#define JUMP_COUNT 3

bool play_sound = true;
bool music_playing = false;

Texture2D coin;


Texture2D soldier_run;
Texture2D soldier_jump;
Texture2D soldier_fall;

Texture unstable_platform;
Texture platform1_img;
Texture cloud_png;
Texture sky_img;
Texture star_img;
Texture hill_img;
Texture mountains_img;
Sound game_over_snd;
Sound coin_sound;
Music background_sound;

//STAR
int coin_index = 0;
float coin_timer = COIN_TIME;



//RUN
int run_index = 0;
float run_timer = RUN_TIME;

//Jump
int jump_index = 0;
float jump_timer = JUMP_TIME;


//Jump
int	attack_index = 0;
float attack_timer = JUMP_TIME;


float scrollingBack = 0.0f;

void interface_init() {
	// Αρχικοποίηση του παραθύρου
	InitWindow(SCREEN_WIDTH , SCREEN_HEIGHT, "game");
	SetTargetFPS(60);
    InitAudioDevice();
	FILE* file = fopen("highscore.txt", "r");
	if (file != NULL) {
		fscanf(file, "%d", &highScore);
		fclose(file);
	}
	//COIN
	coin = LoadTexture("assets/coin.png");

	// Φόρτωση εικόνων και ήχων
	soldier_run = LoadTexture("assets/_Run.png");
	soldier_jump = LoadTexture("assets/_Jump.png");
	soldier_fall = LoadTexture("assets/_Fall.png");



	unstable_platform = LoadTextureFromImage(LoadImage("assets/fallingpl.png"));
	platform1_img = LoadTextureFromImage(LoadImage("assets/new_platform.png"));
	mountains_img = LoadTextureFromImage(LoadImage("assets/mountains.png"));
	cloud_png = LoadTextureFromImage(LoadImage("assets/clouds.png"));
	sky_img = LoadTextureFromImage(LoadImage("assets/sky.png"));
	hill_img =  LoadTextureFromImage(LoadImage("assets/hill.png"));


	star_img = LoadTextureFromImage(LoadImage("assets/star.png"));
	
	coin_sound = LoadSound("assets/pickupCoin.wav");
	game_over_snd = LoadSound("assets/game_over.wav");
	background_sound = LoadMusicStream("assets/background.ogg");

	
}

void interface_close() {
	CloseAudioDevice();
	CloseWindow();
}



// Draw game (one frame)
void interface_draw_frame(State state) {
	BeginDrawing();


	StateInfo info = state_info(state);

	// Καθαρισμός, θα τα σχεδιάσουμε όλα από την αρχή
	ClearBackground(GetColor(0x052c46ff));
	DrawTextureEx(sky_img , (Vector2){0 , 0 } , 0 , 1 , WHITE);
	DrawTextureEx(mountains_img , (Vector2){0 , 0 } , 0 , 1 , 	WHITE);
	scrollingBack -= 0.2f;

	if (scrollingBack <= -900*2) scrollingBack = 0;
	DrawTextureEx(cloud_png, (Vector2){ scrollingBack, 20 }, 0.0f, 1.0f, WHITE);
    DrawTextureEx(cloud_png, (Vector2){ 900*2 + scrollingBack, 20 }, 0.0f, 1.0f, WHITE);
    DrawTextureEx(hill_img , (Vector2){0 , -55 } , 0 , 1 , WHITE);
	

        



	 if (info->game_started == false)
        {
			Rectangle playButton = { GetScreenWidth() / 2 - 110, GetScreenHeight() / 2 - 10, 200, 50 };
            DrawText("KNIGHT JUMPER", GetScreenWidth()  / 2 - MeasureText("KNIGHT JUMPER", 40) / 2, GetScreenHeight() / 2 - 80, 40, DARKGRAY);

            // Draw the play button
            DrawRectangleRec(playButton, LIGHTGRAY);
            DrawText("Play", playButton.x + playButton.width / 2 - MeasureText("Play", 20) / 2, playButton.y + playButton.height / 2 - 10 , 20, DARKGRAY);
		
			DrawText("Use the arraw keys to play!" , GetScreenWidth()/2 - MeasureText("Use the arraw keys to play!" , 20) + 130 , GetScreenHeight() - 100 , 20 , BLACK );
        }
	else{
	SetMusicVolume(background_sound , 0.5f);
	UpdateMusicStream(background_sound);

	
	if(info->ball->vert_mov == IDLE){
		run_timer -= GetFrameTime();
		
		if(run_timer < 0){
			if(IsKeyDown(KEY_LEFT)){
				run_timer = SlOW_TIME;
			}
			else{
				run_timer = RUN_TIME;
			}
				run_index++;
			if(run_index >= RUN_COUNT){
				run_index = 0;
			}
		}
		Rectangle source;
			source = (Rectangle){120 * run_index , 0 , 28 , 40};
		DrawTextureRec(soldier_run, source, (Vector2){SCREEN_WIDTH - 800 , info->ball->rect.y},  WHITE);
	}
	else if(info->ball->vert_mov == JUMPING){
		jump_timer -= GetFrameTime();
		if(jump_timer < 0){
			jump_timer = JUMP_TIME;
				jump_index++;
			if(jump_index >= JUMP_COUNT){
				jump_index = 0;
			}
		}
		Rectangle source;
		source = (Rectangle){120 * jump_index , 0 , 25 , 40};
		DrawTextureRec(soldier_jump, source, (Vector2){SCREEN_WIDTH - 800 , info->ball->rect.y},  WHITE);
	}
	else if(info->ball->vert_mov == FALLING){
		jump_timer -= GetFrameTime();
		if(jump_timer < 0){
			jump_timer = JUMP_TIME;
				jump_index++;
			if(jump_index >= JUMP_COUNT){
				jump_index = 0;
			}
		}
		Rectangle source;
		source = (Rectangle){120 * jump_index , 0 , 29 , 40};
		DrawTextureRec(soldier_fall, source, (Vector2){SCREEN_WIDTH - 800 , info->ball->rect.y},  WHITE);


	}

     
	//σχετικη θεση της μπαλας στ
	int x_offset = SCREEN_WIDTH - 800 -  info->ball->rect.x;
	
    //χρισημοποιω την getfps για να εμφανισω στην οθονη τα fps
	DrawText(TextFormat("%04i", GetFPS()), 800, 15, 30, GREEN);
    
	DrawText(TextFormat("%04i", info->score), 20, 20, 40, GRAY);

    List objs = state_objects(state , info->ball->rect.x - SCREEN_WIDTH , info->ball->rect.x + SCREEN_WIDTH + 1000);
     
	 //διατρεχω την λιστα με τα objects

    for(ListNode node = list_first(objs);
		node != LIST_EOF;
		node = list_next(objs , node)){
		Object obj = list_node_value(objs , node);
		
		if(obj->type == PLATFORM){
			
			//αναλογα με το αν ειναι unstable η οχι ζωγραφιζω κοκκινες η πρασινες πλατφορμες αντιστοιχα
			if(obj->unstable == false){
			Rectangle sourse_platform = (Rectangle){ (100 * obj->displayValue) , -200, (int)obj->rect.width , 200};

			DrawTextureRec(platform1_img , sourse_platform , (Vector2){obj->rect.x + x_offset , obj->rect.y - 170} , WHITE);
			}
			//DrawRectangle(obj->rect.x + x_offset  , obj->rect.y , obj->rect.width , obj->rect.height , DARKGREEN);
			if(obj->unstable == true){
				Rectangle unstable = (Rectangle){ 0 , -80, (int)obj->rect.width , 100};
				DrawTextureRec(unstable_platform , unstable , (Vector2){obj->rect.x + x_offset , obj->rect.y - 50} , WHITE);
				}
			}
	    //ζωγραφιζω το αστερι 
		if(obj->type == STAR){
			coin_timer -= GetFrameTime();
				if(coin_timer < 0){
					coin_timer = COIN_TIME;
						coin_index++;
				if(coin_index >= COIN_COUNT){
					coin_index = 0;
				}
			}
		
			Rectangle source;
			source = (Rectangle){30 * coin_index , 30 , 30 , 30};
			DrawTextureRec(coin, source, (Vector2){obj->rect.x + x_offset , obj->rect.y},  YELLOW);
			
		}
	}
		if(info->coin_taken == true){
		PlaySound(coin_sound);
		info->coin_taken = false;
	}
	
    if(info->playing == false) {
		if (info->score > highScore) {
			highScore = info->score;
			FILE* file = fopen("highscore.txt", "w");
			if (file != NULL) {
				fprintf(file, "%d", highScore);
				fclose(file);
			}
		}
		DrawText(
			"PRESS [ENTER] TO PLAY AGAIN",
			 GetScreenWidth() / 2 - MeasureText("PRESS [ENTER] TO PLAY AGAIN", 20) / 2,
			 GetScreenHeight() / 2 - 50, 20, BLACK
		);
		// Convert the high score to a string
		char highScoreText[10];
		sprintf(highScoreText, "%d", highScore);

		// Draw the high score using Raylib's text rendering functions
			DrawText("High Score:",  GetScreenWidth() / 2 - MeasureText("High Score:", 20) / 2 - 100, GetScreenHeight() / 2 - 200, 40, WHITE);
			DrawText(highScoreText, GetScreenWidth() / 2 - MeasureText("High Score:", 20) / 2 + 150, GetScreenHeight() / 2 - 205, 50, RED);
		}
	if(info->playing == false){
		StopMusicStream(background_sound);
		
		if(play_sound == true){
			PlaySound(game_over_snd);
			play_sound = false;
		}
		music_playing = false;
	}
		else if(music_playing == false){
			PlayMusicStream(background_sound);
			music_playing = true;
			play_sound = true;
		}
	}
	EndDrawing();
}