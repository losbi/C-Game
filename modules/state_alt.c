#include "set_utils.h"
#include "state.h"
#include "ADTSet.h"
#include <stdio.h>




//χρισημοποιο αυτην την compare για να βαζω με μια σειρα τα objects αναλογα με το x που εχουν 
int compareObjects(Pointer a ,Pointer b){
	 //δινω την διευθηνση του pointer στο object που ειναι τυπου struct*
	Object first = a;  
	Object second = b;
	//συγκρινω τις συντεταγμενες x των αντικειμενων και αναλογα με το ποιο αντικειμενο εχει μεγαλυτερο x η αν ειναι ισα
	if((first->rect.x) - (second->rect.x) > 0){
		return 1;
	}
	else if((first->rect.x) - (second->rect.x) < 0){
	    return -1;
	}
	else{
		return a - b;
	}
}


Pointer set_find_eq_or_greater(Set set, Pointer value){
    //Ψαχνω αν υπαρχει το value με την set_find
    if (set_find(set , value) != NULL) 
    { 
        return value;
    }
    else{
        //αν δεν υπαρχει εισαγω την τιμη 
        set_insert(set , value);
        SetNode node = set_find_node(set , value);
		//παω στον επομενο κομβο
        node = set_next(set , node);
		//αφαιρω την τιμη που εκχωρησα
        set_remove(set , value);
        if(node != SET_EOF){
          return set_node_value(set , node);
        }                                           
    }

    return NULL;
}                                                     

Pointer set_find_eq_or_smaller(Set set, Pointer value){
    if (set_find(set , value) != NULL)
    { 
        return value;
    }
    else{
        set_insert(set , value);
        SetNode node = set_find_node(set , value);
        node = set_previous(set , node);
        set_remove(set , value);
        if(node != SET_EOF){
          return set_node_value(set , node);
        }                                           
    }
    return NULL;
}


struct state {
	Set objects;			//φτιαχνω ενα set που θα περιεχει τις πλατφορμες και τα αστερια

	//προσθηκη δυο object που εχουν αποθηκευμενη την πληροφορια για το σε ποια πλατφορμα βρισκεται η μπαλα (current_platform)
	Object current_platform;
	//και σε ποια αναμενεται να προσγηωθει (next_platform)
	Object next_platfrom;

	struct state_info info;	// Γενικές πληροφορίες για την κατάσταση του παιχνιδιού
	float speed_factor;		// Πολλαπλασιαστής ταχύτητς (1 = κανονική ταχύτητα, 2 = διπλάσια, κλπ)
};

//η συναρτηση αυτη αφααιρει καποιο συγκεκριμενο στοιχειο Set
void destroySetValues(Set set , Object obj);

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
			i == 0 ? 250 : 50 + rand()%200,						// πλάτος τυχαία στο διάστημα [50, 250] (η πρώτη πάντα 250)
			20,													// ύψος
			i < 3 || rand() % 2 == 0 ? MOVING_UP : MOVING_DOWN,	// τυχαία αρχική κίνηση (οι πρώτες 3 πάντα πάνω)
			0,//0.6 + 1*(rand()%100)/100,							// ταχύτητα τυχαία στο διάστημα [0.6, 3.6]
			i > 0 &&  (rand() % 10) == 0							// το 10% (τυχαία) των πλατφορμών είναι ασταθείς (εκτός από την πρώτη)
		);
		//εκχωρω τα object μεσα στο set
		set_insert(state->objects, platform);

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
			//εκχωρω τα object στο set
			set_insert(state->objects , star);
		}

		start_x = platform->rect.x + platform->rect.width;	// μετακίνηση των επόμενων αντικειμένων προς τα δεξιά
	}
}

// Δημιουργεί και επιστρέφει την αρχική κατάσταση του παιχνιδιού

State state_create() {
	// Δημιουργία του state
	State state = malloc(sizeof(*state));

	// Γενικές πληροφορίες
	state->info.playing = true;				// Το παιχνίδι ξεκινάει αμέσως
	state->info.paused = false;				// Χωρίς να είναι paused.
	state->speed_factor = 1;				// Κανονική ταχύτητα
	state->info.score = 0;				// Αρχικό σκορ 0

	//δημιουργω ενα καινουργιο set με την δικια μου compareObjects που ειναι υλοποιημενη στην αρχη 
	state->objects = set_create(compareObjects , NULL);
	add_objects(state, 0);

	// Δημιουργούμε την μπάλα τοποθετώντας τη πάνω στην πρώτη πλατφόρμα
	//η πρωτη πλατφορμα ειναι και το πρωτο value του set εφοσων το set περιχει τα object σε αυξουσα σειρα αναλογα με το x τους
	SetNode node = set_first(state->objects);
	Object first_platform = set_node_value(state->objects , node);

	//εφοσων ξερω οτι η μπαλα θα ειναι σιγουρα πανω στην πρωτη πλατφορμα αρχικοποιω την current_platform με το firstplatform
	state->current_platform = first_platform;


	state->info.ball = create_object(
		BALL,
		first_platform->rect.x,			// x στην αρχή της πλατφόρμας
		first_platform->rect.y - 40,	// y πάνω στην πλατφόρμα
		25, 40,							// πλάτος, ύψος
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
	//για να μπορεσω να διασχησω τοπικα το set αναλογα με τις συνεταγμενες x φτιαχνω καποια (dummy) objects ως σημειο αναφορας
	//το αντικειμνο x_from_object εχει ως x το x_from και ολα τα υπολοιπα δεν μας απασχολουν 
  	Object x_from_Object = malloc(sizeof(Object));
	x_from_Object->rect.x = x_from;
	Object x_to_Object = malloc(sizeof(Object));
	x_to_Object->rect.x = x_to;
	Object obj;

	//διασχηζουμε το set βαζωντας ως σημειο αναφορας τα object που δημιουργησα καθως αυτα δεν θα συμπεριληφθουν στο παιχνιδι 
	
	for(SetNode node = set_find_node(state->objects , set_find_eq_or_greater(state->objects , x_from_Object));
	       node !=  set_next (state->objects , set_find_node(state->objects , set_find_eq_or_smaller(state->objects , x_to_Object)));
		   node = set_next(state->objects , node)){
		   obj = set_node_value(state->objects , node);
           list_insert_next(object_list , LIST_BOF , obj); //εφοσων ειναι αναμεσα στο x_from και x_to το εισάγω στην λιστα  
	    }
		free(x_from_Object);//απελευθερωνω την μνημη για τα δυο dummy objects
		free(x_to_Object);
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
	state->info.ball->rect.x += (keys->right ? 6 * state->speed_factor : (keys->left ? -6* state->speed_factor : 0 * state->speed_factor ));
    
	//αν η μπάλα βρισκεταί σε κατασταση jumping τοτε μετακινουμε την μπαλα vertspeed pixels προς τα πανω και επειδη η το y του map αυξανεται προς τα κατω αυτο σημαινει οτι αγαιρω pixels
	 
    if(state->info.ball->vert_mov == JUMPING){
	    state->info.ball->rect.y -= state->info.ball->vert_speed;
	    state->info.ball->vert_speed *= 0.85;
		if(state->info.ball->vert_speed <= 0.5){
			state->info.ball->vert_mov = FALLING;
		}
	}
	//ελεγχω αν η μπάλα είναι σε σταθερή κατασταση
	else if(state->info.ball->vert_mov == IDLE){
		//αν πατηθει το πανω βελος η μπαλα μπαίνει σε κατάσταση jumping με αρχικη ταχυτητα 17
		if(keys->up){
			state->info.ball->vert_speed += 17;
			state->info.ball->vert_mov = JUMPING;
		}
	}

	//ελεγχω αν η μπάλα είναι σε κατασταση falling 

	else if(state->info.ball->vert_mov == FALLING){
		state->info.ball->rect.y += state->info.ball->vert_speed;
		if(state->info.ball->vert_speed <= 7){
			state->info.ball->vert_speed += (10 * (state->info.ball->vert_speed))/100;
		}
	}


	

 
	//object που εχει αποθηκευμενη την τελευταια πλατφορμα
	Object last_platform; 

	float x_from = state->info.ball->rect.x - SCREEN_WIDTH;
	float x_to = state->info.ball->rect.x + SCREEN_WIDTH;
	//ξανα δημιουργω αντικειμενα για να διασχησω το set στα ορια που θελω
    Object x_from_Object = malloc(sizeof(Object));
	x_from_Object->rect.x = x_from;
	Object x_to_Object = malloc(sizeof(Object));
	x_to_Object->rect.x = x_to;
	
	//διασχηζω to set που περιεχει τα αντικειμενα σε αποσταση δυο οθονως
    for(SetNode node = set_find_node(state->objects , set_find_eq_or_greater(state->objects , x_from_Object));
	    node !=  set_next (state->objects ,set_find_node(state->objects , set_find_eq_or_smaller(state->objects , x_to_Object)));
		node = set_next(state->objects , node)){
		
		Object obj = set_node_value(state->objects , node);
              
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
			       if(obj->vert_mov ==  FALLING && obj->rect.y >= SCREEN_HEIGHT && obj->unstable == true){
						//διαγραφω την πλατφρομα αμα πεσει κατω απο τα ορια της οθονης
						//πριν κανω την αφαιρεση του object παω το node στην προηγουμενη θεση ωστε αφου σβησω το object να μπορω να βρω το επομενo
						node = set_previous(state->objects , node);
						destroySetValues(state->objects , obj);
				}
			}
			
            if(obj->type == STAR){
			   	if(CheckCollisionRecs(state->info.ball->rect , obj->rect)){
				  	//αφαιρουμε το αστερι
					//πριν κανω την αφαιρεση του object παω το node στην προηγουμενη θεση ωστε αφου σβησω το object να μπορω να βρω το επομενο
					node = set_previous(state->objects , node); 
					destroySetValues(state->objects , obj);
			    	state->info.score += 10;
			    }
			}
	    }
   
		//αν η μπαλα φταασει στο κατω μεροσ της οθονης που ειναι ισο με screen height τοτε σταματαμε το παιχνιδι 
		if(state->info.ball->rect.y >= SCREEN_HEIGHT){
			state->info.playing = false;
		}
         
		//επιπλεον μπορω να βρω την τελευταια πλατφορμα χωρις να διασχισω ολο το set

		 last_platform = set_node_value( state->objects , set_last(state->objects));
		 //παλι τσεκαρω την περιπτωση που υπαρχει αστερι στο τελος του set και αν ναι παω στο προηγουμενο στοιχειο που ειναι πλατφορμα
		 if(last_platform->type == STAR){
			last_platform = set_node_value(state->objects , set_previous( state->objects, set_find_node(state->objects , last_platform)));
		 }
        
		//ελεγχω αν η μπαλα παει να φτασει την τελευταια πλατφορμα που εχουμε βρει σε αποσταση μιας οθονης
	    if(state->info.ball->rect.x >= (last_platform->rect.x - SCREEN_WIDTH + 200)){
			//για να κανω φαινομενικα απειρο το μηκος της πιστας θα χρησιμποιησω την add_objects για να προστεθουν νεα αντικειμενα
			add_objects(state , (last_platform->rect.x + last_platform->rect.width)); 
			//σε αυτο το σημειο αυξανω το speed_factor
			state->speed_factor += (10 * state->speed_factor)/100;
		}
			free(x_from_Object); //ελευθερωνω τον χωρω που δεσμευτικε για τα dummy αντικειμενα
         	free(x_to_Object);

	}

	//μεταβλητη που μετα την διασχηση ολων των object κραταει την πληροφορια αν η μπαλα βρισκεται πανω σε μια πλατφρομα
	bool fallCheck = true;

	//Εφοσων εχω τα object ταξινομημενα στα sets και ξερω την πλατφορμα που βρισκεται η μπαλα μπορω να βρισκω την επομενη πλατφορμα που θα προσγηωθει η μπαλα χωρις να διατρεχω ολο το set
	SetNode next_platform_node = set_next(state->objects ,  set_find_node(state->objects , state->current_platform));
	state->next_platfrom = set_node_value(state->objects , next_platform_node);
	//σε περιπτωση που το επομενο object δεν ειναι πλατφορμα κανω ενα loop μεχρι να βρω την επομενη πλατφορμα
	while (state->next_platfrom->type == STAR){
		next_platform_node = set_next(state->objects , next_platform_node);
		//βρισκω την επομενη πλατφορμα μετα την current platform
		state->next_platfrom = set_node_value(state->objects , next_platform_node);
	}

	//οσο η μπαλα βρισκεται στα ορια της current_platform τοτε τσεκαρω για την συγκρουση της με την μπαλα

	if(state->info.ball->rect.x <= state->current_platform->rect.x + state->current_platform->rect.width ){
		 if((CheckCollisionRecs(state->current_platform->rect , state->info.ball->rect)) && state->info.ball->vert_mov == FALLING){
			fallCheck = false;
			if(state->current_platform->unstable == false){
				//an η πλατφρομα δεν ειναι ασταθες τοτε οταν ερθει σε επαφη με την μπαλα , η μπαλα γινεται idle
				state->info.ball->vert_mov = IDLE;
				
			}
			else{
				//αν ειναι ασταθεις τοτε η πλατφρομα αρχιζει να πεφτει και η μπαλα γινεται idle ωστε να μπορει να ξανα αναπηδησει 
				state->info.ball->vert_mov = IDLE;
				state->current_platform->vert_mov = FALLING;
			
			}
	    }
		else if(CheckCollisionRecs(state->current_platform->rect, state->info.ball->rect) && state->info.ball->vert_mov == IDLE){
			    fallCheck = false;
			    //το y της μπαλας να μετακινειται σαν το y της πλατφορμας εχοντας υποψη και το offset της μπαλας για να φαινεται οτι η μπαλα ειναι πανω απο την πλατφορμα
				//το +2 ειναι ωστε να μην τρεμοπαιζει καθε φορα που η πλατφορμα κινητε προς τα κατω 
				state->info.ball->rect.y = state->current_platform->rect.y - state->info.ball->rect.height +0.1;
				//εφοσων η μπαλα βρισκεται πανω σε μια πλατφορμα τοτε η μπαλα δεν θα μπει σε κατασταση FALLING
				
		}
	}
	else{
		
		//οταν φυγω απο τα ορια της current_platform τοτε πρεπει να βρω την επομενη πλατφορμα που την εχω ειδη βρει και την εκχωρω ωσ current_object 

		state->current_platform = state->next_platfrom;
	}

	//αμα το fallCheck εχει παραμηνει true τοτε η μπαλα θα μπει σε κατασταση falling 
	if(fallCheck == true && state->info.ball->vert_mov == IDLE){
		state->info.ball->vert_mov = FALLING;
		state->info.ball->vert_speed = 1.5;
	}

	//αναλογα με το αν ειναι pause και πατηθει το πληκτρο p το  state->info.paused γινεται true η false

	if(state->info.playing == true &&  keys->p && !state->info.paused ){
	     state->info.paused = true;
	}
	else if(state->info.paused && keys->p){
		state->info.paused = false;
	}
}

void destroySetValues(Set set , Object obj){
	//διαγραφω τον κομβο του set
    set_remove(set , obj);
}


void state_destroy(State state) {
	//καταστρεφω το set
	set_destroy(state->objects);
	free(state);
}