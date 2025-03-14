#include <stdlib.h>
#include<stdio.h>
#include "ADTVector.h"
#include "ADTList.h"
#include "state.h"


// Οι ολοκληρωμένες πληροφορίες της κατάστασης του παιχνιδιού.
// Ο τύπος State είναι pointer σε αυτό το struct, αλλά το ίδιο το struct
// δεν είναι ορατό στον χρήστη.


struct state {
	Vector objects;			// περιέχει στοιχεία Object (Πλατφόρμες, Αστέρια)
	struct state_info info;	// Γενικές πληροφορίες για την κατάσταση του παιχνιδιού
	float speed_factor;		// Πολλαπλασιαστής ταχύτητς (1 = κανονική ταχύτητα, 2 = διπλάσια, κλπ)
};

//η συναρτηση αυτη αφααιρει καποιο συγκεκριμενο στοιχειο του vector
void destroyVectorValues(Vector vec , int pos);

// Δημιουργεί και επιστρέφει ένα αντικείμενο
static Object create_object(ObjectType type, float x, float y, float width, float height, VerticalMovement vert_mov, float speed, bool unstable) {
	Object obj = malloc(sizeof(*obj));
	obj->type = type;
	obj->rect.x = x;
	obj->rect.y = y;
	obj->rect.width = width;
	obj->rect.height = height;
	obj->vert_mov = vert_mov;
	obj->vert_speed = speed;
	obj->unstable = unstable;
	obj->displayValue = GetRandomValue(0, 15); //ειναι για τις πλατφορμες ωστε να προβαλονται τυχαια
	return obj;
}

// Προσθέτει αντικείμενα στην πίστα (η οποία μπορεί να περιέχει ήδη αντικείμενα).
// Τα αντικείμενα ξεκινάνε από x = start_x, και επεκτείνονται προς τα δεξιά.
//
// ΠΡΟΣΟΧΗ: όλα τα αντικείμενα έχουν συντεταγμένες x,y σε ένα
// καρτεσιανό επίπεδο.
// - Στο άξονα x το 0 είναι η αρχή της πίστας και οι συντεταγμένες
//   μεγαλώνουν προς τα δεξιά.
//
// - Στον άξονα y το 0 είναι το πάνω μέρος της πίστας, και οι συντεταγμένες μεγαλώνουν
//   προς τα _κάτω_. Η πίστα έχει σταθερό ύψος, οπότε όλες οι
//   συντεταγμένες y είναι ανάμεσα στο 0 και το SCREEN_HEIGHT.
//
// Πέρα από τις συντεταγμένες, αποθηκεύουμε και τις διαστάσεις width,height
// κάθε αντικειμένου. Τα x,y,width,height ορίζουν ένα παραλληλόγραμμο, οπότε
// μπορούν να αποθηκευτούν όλα μαζί στο obj->rect τύπου Rectangle (ορίζεται
// στο include/raylib.h). Τα x,y αναφέρονται στην πάνω-αριστερά γωνία του Rectangle.

static void add_objects(State state, float start_x) {
	// Προσθέτουμε PLATFORM_NUM πλατφόρμες, με τυχαία χαρακτηριστικά.
	for (int i = 0; i < PLATFORM_NUM; i++) {
		Object platform = create_object(
			PLATFORM,
			start_x + 150 + rand() % 80,						// x με τυχαία απόσταση από το προηγούμενο στο διάστημα [150, 230]
			SCREEN_HEIGHT/4 + rand() % SCREEN_HEIGHT/2,			// y τυχαία στο διάστημα [SCREEN_HEIGHT/4, 3*SCREEN_HEIGHT/4]
			i == 0 ? 300 : 100 + (100 * (rand()%2)),						// πλάτος τυχαία στο διάστημα [50, 250] (η πρώτη πάντα 250)
			20,													// ύψος
			 i < 3 || rand() % 2 == 0 ? MOVING_UP : MOVING_DOWN,	// τυχαία αρχική κίνηση (οι πρώτες 3 πάντα πάνω)
			0.6 + 1*(rand()%100)/100,							// ταχύτητα τυχαία στο διάστημα [0.6, 3.6]
			i > 0 &&  (rand() % 10) == 0								// το 10% (τυχαία) των πλατφορμών είναι ασταθείς (εκτός από την πρώτη)
		);
		vector_insert_last(state->objects, platform);

		// Στο 50% των πλατφορμών (τυχαία), εκτός της πρώτης, προσθέτουμε αστέρι
		if(i != 0 && rand() % 2 == 0) {
			Object star = create_object(
				STAR,
				start_x + 200 + rand() % 60,				// x με τυχαία απόσταση από την προηγούμενη πλατφόρμα στο διάστημα [200,260]
				SCREEN_HEIGHT/8 + rand() % SCREEN_HEIGHT/2,	// y τυχαία στο διάστημα [SCREEN_HEIGHT/8, 5*SCREEN_HEIGHT/8]
				30, 30,										// πλάτος, ύψος
				IDLE,										// χωρίς κίνηση
				0,										 	// ταχύτητα 0
				false										// 'unstable' πάντα false για τα αστέρια
			);
			vector_insert_last(state->objects, star);
		}

		start_x = platform->rect.x + platform->rect.width;	// μετακίνηση των επόμενων αντικειμένων προς τα δεξιά
	}
}

// Δημιουργεί και επιστρέφει την αρχική κατάσταση του παιχνιδιού

State state_create() {
	// Δημιουργία του state
	State state = malloc(sizeof(*state));

	// Γενικές πληροφορίες
	state->info.game_started = true;
	state->info.playing = true;				// Το παιχνίδι ξεκινάει αμέσως
	state->info.paused = false;				// Χωρίς να είναι paused.
	state->speed_factor = 1;				// Κανονική ταχύτητα
	state->info.score = 0;				// Αρχικό σκορ 0
	state->info.coin_taken = false;

	// Δημιουργούμε το vector των αντικειμένων, και προσθέτουμε αντικείμενα
	// ξεκινώντας από start_x = 0.
	state->objects = vector_create(0, NULL);
	add_objects(state, 0);

	// Δημιουργούμε την μπάλα τοποθετώντας τη πάνω στην πρώτη πλατφόρμα
	Object first_platform = vector_get_at(state->objects, 0);
	state->info.ball = create_object(
		BALL,
		first_platform->rect.x,			// x στην αρχή της πλατφόρμας
		first_platform->rect.y - 40,	// y πάνω στην πλατφόρμα
		25, 42,							// πλάτος, ύψος
		IDLE,							// χωρίς αρχική κατακόρυφη κίνηση
		0,								// αρχική ταχύτητα 0
		false							// "unstable" πάντα false για την μπάλα

	);

	return state;
}

// Επιστρέφει τις βασικές πληροφορίες του παιχνιδιού στην κατάσταση state

StateInfo state_info(State state) {
	return &state->info; //θα επιστρεψω την διευθηνση του pointer στo struct state info 
}

// Επιστρέφει μια λίστα με όλα τα αντικείμενα του παιχνιδιού στην κατάσταση state,
// των οποίων η συντεταγμένη x είναι ανάμεσα στο x_from και x_to.

List state_objects(State state, float x_from, float x_to) {
    List object_list = list_create(free);

	//διασχιζουμε τους κομβους του vector objects που περιεψει τισ πλατφορμές και τα αστερια
	for(VectorNode node = vector_first(state->objects);
	    node !=  VECTOR_EOF;
        node = vector_next(state->objects , node)){
        Object obj = vector_node_value(state->objects , node);
		if(obj->rect.x >= x_from && obj->rect.x <=x_to){ 
            list_insert_next(object_list , LIST_BOF , obj); //εφοσων ειναι αναμεσα στο x_from και x_to το εισάγω στην λιστα
		  }
	    }
		return object_list; //εφοσων η state object ειναι τυπου list επιστρεφω την λιστα
}

// Ενημερώνει την κατάσταση state του παιχνιδιού μετά την πάροδο 1 frame.
// Το keys περιέχει τα πλήκτρα τα οποία ήταν πατημένα κατά το frame αυτό.

void state_update(State state, KeyState keys) {
	//ελεγχω αν το παιχνιδι δεν βρισκεται σε paused και εινα playing ωστε να ενημερωσω τις κινησεις 
	if(state->info.playing && !state->info.paused){
	//αν ειναι πατημενο το δεξι βέλος η μπάλα μετακινήται 6 pixels προς τα δεξια
	//με πατημενο το αριστερο βέλος μετακινήται 1 pixel δεξια
	//και χωρις κανενα πληκτρο πατημενο μετακινηται 4 pixels
	//καθε μετακινηση ειναι πολλαπλασιαμενη με speedfactor για να επιταχυνεται το παιχνιδι 
	state->info.ball->rect.x += (keys->right ? 6 * state->speed_factor : (keys->left ? 1* state->speed_factor : 4 * state->speed_factor ));
    
	//αν η μπάλα βρισκεταί σε κατασταση jumping τοτε μετακινουμε την μπαλα vertspeed pixels προς τα πανω και επειδη η το y του map αυξανεται προς τα κατω αυτο σημαινει οτι αγαιρω pixels
	//ελεγχω αν η μπάλα είναι σε σταθερή κατασταση 
	if(state->info.ball->vert_mov == IDLE){
        
		//αν πατηθει το πανω βελος η μπαλα μπαίνει σε κατάσταση jumping με αρχικη ταχυτητα 17

		if(keys->up){
			state->info.ball->vert_speed += 11;
			state->info.ball->vert_mov = JUMPING;
		}
	} 

    if(state->info.ball->vert_mov == JUMPING){
	    state->info.ball->rect.y -= state->info.ball->vert_speed;
	    state->info.ball->vert_speed *= 0.85;
		if(state->info.ball->vert_speed <= 0.5){
			state->info.ball->vert_mov = FALLING;
		}
	}
	//ελεγχω αν η μπάλα είναι σε κατασταση falling 
	if(state->info.ball->vert_mov == FALLING){
		state->info.ball->rect.y += state->info.ball->vert_speed;
		if(state->info.ball->vert_speed <= 7){
			state->info.ball->vert_speed += (10 * (state->info.ball->vert_speed))/100;
		}
	}
    //μεταβλητη που μετα την διασχηση ολων των object κραταει την πληροφορια αν η μπαλα βρισκεται πανω σε μια πλατφρομα
    bool fallCheck = true;
	//κραταμε την θεση του καθε κομβου της λιστας 
	int pos = 0;
     
	//object που εχει αποθηκευμενη την τελευταια πλατφορμα
	Object last_platform; 
	
	//διασχηζω to vector state->objects που περιεχει τα αντικειμενα
    for(VectorNode node = vector_first(state->objects);
	    node !=  VECTOR_EOF;
        node = vector_next(state->objects , node)){
             
			Object obj = vector_node_value(state->objects , node);
              
			//ελεγχω αν το object ειναι τυπου platform

			if(obj->type == PLATFORM){

				//αν η πλατφρομα μετακινειται προς τα πανω τοτε την μετακεινω vert_speed pixels προς τα πανω μεχρι να φτασει SCREEN_HEIGHT/4
				
				if(obj->vert_mov == MOVING_UP){
					obj->rect.y -= obj->vert_speed;
					if(obj->rect.y <= SCREEN_HEIGHT/4){
					obj->vert_mov = MOVING_DOWN;
					}
				}
				else if(obj->vert_mov == MOVING_DOWN){
					//αν η πλατφρομα μετακινειται προς τα κατω τοτε την μετακεινω vertspeed pixels προς τα κατω μεχρι να φτασει 3* SCREEN_HEIGHT/4

					obj->rect.y += obj->vert_speed;
					if(obj->rect.y >= 3* SCREEN_HEIGHT/4)
					obj->vert_mov = MOVING_UP;
				}
                else if(obj->vert_mov == FALLING){
					//αν η πλατφρομα εινα σε κατασταση falling τοτε πεφτει προς τα κατω με σταθερη ταχυτητα 4 pixels
					obj->rect.y += 4;
				}
			}
            
			if(obj->type == PLATFORM){
				if(obj->rect.x + obj->rect.width + 1 < state->info.ball->rect.x  ){
					obj->vert_mov = FALLING;
				}
				//ελεγχω αν συγρκουεται η πλατφορμα με την μπαλα με την checkCollisionRecs που δινεται απο την raylib και αν πριν συγκρουστει με την πλρφορμα η μπαλα ειναι σε κατασταση falling
				if((CheckCollisionRecs(obj->rect , state->info.ball->rect)) && state->info.ball->vert_mov == FALLING){
					if(obj->unstable == false){
						//an η πλατφρομα δεν ειναι ασταθες τοτε οταν ερθει σε επαφη με την μπαλα , η μπαλα γινεται idle
						state->info.ball->vert_mov = IDLE;
					}
					else{
						//αν ειναι ασταθεις τοτε η πλατφρομα αρχιζει να πεφτει και η μπαλα γινεται idle ωστε να μπορει να ξανα αναπηδησει 
				   state->info.ball->vert_mov = IDLE;
						obj->vert_mov = FALLING;
					}
				}
				//ελεγχω αν η μπαλα βρισκεται στο ιδιο x και y με την πλατφρομα 
                if(CheckCollisionRecs(obj->rect, state->info.ball->rect) && state->info.ball->vert_mov == IDLE){
			    //το y της μπαλας να μετακινειται σαν το y της πλατφορμας εχοντας υποψη και το offset της μπαλας για να φαινεται οτι η μπαλα ειναι πανω απο την πλατφορμα
				//το +2 ειναι ωστε να μην τρεμοπαιζει καθε φορα που η πλατφορμα κινητε προς τα κατω 
				state->info.ball->rect.y = obj->rect.y - state->info.ball->rect.height + 2;
				//εφοσων η μπαλα βρισκεται πανω σε μια πλατφορμα τοτε η μπαλα δεν θα μπει σε κατασταση FALLING
				fallCheck = false;
				}
              	if(obj->vert_mov ==  FALLING && obj->rect.y >= SCREEN_HEIGHT ){
					//διαγραφω την πλατφρομα αμα πεσει κατω απο τα ορια της οθονης
					destroyVectorValues(state->objects , pos);
			 	}
			}
            if(obj->type == STAR){
			   	if(CheckCollisionRecs(state->info.ball->rect , obj->rect)){
				  	//αφαιρουμε το αστερι 
					destroyVectorValues(state->objects , pos);
					state->info.coin_taken = true;
			    	state->info.score += 10;
			    }
			}
		    if(obj->type == PLATFORM){
				//εκχωρω την τελευταια πλατφρομα που βρισκω καθε φορα
				last_platform = obj;
			}
			pos++;
	    }
   
		//αμα το fallCheck εχει παραμηνει true τοτε η μπαλα θα μπει σε κατασταση falling 
		if(fallCheck == true && state->info.ball->vert_mov == IDLE){
         state->info.ball->vert_mov = FALLING;
		 state->info.ball->vert_speed = 1.5;
		}

		//αν η μπαλα φταασει στο κατω μεροσ της οθονης που ειναι ισο με screen height τοτε σταματαμε το παιχνιδι 
		if(state->info.ball->rect.y >= SCREEN_HEIGHT){
			state->info.playing = false;
		}

		//ελεγχω αν η μπαλα παει να φτασει την τελευταια πλατφορμα που εχουμε βρει σε αποσταση μιας οθονης
	    if(state->info.ball->rect.x >= (last_platform->rect.x - SCREEN_WIDTH + 200)){
			//για να κανω φαινομενικα απειρο το μηκος της πιστας θα χρησιμποιησω την add_objects για να προστεθουν νεα αντικειμενα
			add_objects(state , (last_platform->rect.x + last_platform->rect.width)); 
			//σε αυτο το σημειο αυξανω το speed_factor
			state->speed_factor += (10 * state->speed_factor)/100;
		}
	}

	//αναλογα με το αν ειναι pause και πατηθει το πληκτρο p το  state->info.paused γινεται true η false 

	if(state->info.playing == true &&  keys->p && !state->info.paused ){
	     state->info.paused = true;
	}
	else if(state->info.paused && keys->p){
		state->info.paused = false;
	}
}


 void destroyVectorValues(Vector vec , int pos){
	//μετακινω ολα τα στοιχεια το vector μετα απο το στοιχειο pos μια θεση πιο πισω για να χαθει το περιεχωμενο το κομβου pos 
	for(int i = pos;  i < vector_size(vec) - 1 ; i ++)
    vector_set_at(vec , i   , vector_get_at( vec, i + 1));
    //και ελευθερωνει το τελευταιο στοιχειο 
	vector_remove_last(vec);   
}
// Καταστρέφει την κατάσταση state ελευθερώνοντας τη δεσμευμένη μνήμη.

void state_destroy(State state) {
	//διαγραφω το vector
	vector_destroy(state->objects);
	//ελευθερωνω την μνημη που δεσμευτικε για την state
	free(state);
}