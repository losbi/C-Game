#include <stdio.h>

#include "acutest.h"

#include "set_utils.h"

int* create_int(int value) {
	int* pointer = malloc(sizeof(int));		// δέσμευση μνήμης
	*pointer = value;						// αντιγραφή του value στον νέο ακέραιο
	return pointer;
}

int compareInts(Pointer a , Pointer b){
   return *(int*)a - *(int*)b; 
}

void test_set_find_eq_or_greater(){
    Set set = set_create(compareInts , NULL);
    //εκχωρω τους αριθμους 2 , 4 στην λιστα
    set_insert(set , create_int(2));
    set_insert(set , create_int(4));

    //αν ψαξω για τον αριθμο 4 θα τσεκαρω αν μου επιστρεφει το 4 που ειδη υπαρχει
    Pointer p =  set_find_eq_or_greater(set , create_int(4));
   
    TEST_ASSERT(4 == *(int*)p);
    
    //αν ψαξω για τον αριθμο 5 τσεκαρω αν θα επιστρεψει null καθως δεν υπαρχει μεγαλυτερος η ισος του
    Pointer n = set_find_eq_or_greater(set , create_int(5));
    TEST_ASSERT(NULL == n);

    //αν ψαξω για τον αριθμο 3 θα πρεπει να επιστρεψει τον επομενο μαγλυτερο δλδ 4
    Pointer b = set_find_eq_or_greater(set , create_int(3));
    TEST_ASSERT(4 == *(int*)b);

}

void test_set_find_eq_or_smaller(){
    Set set = set_create(compareInts , NULL);
    set_insert(set , create_int(2));
    set_insert(set , create_int(4));
    //αν ψαξω για τον αριθμο 2 θα τσεκαρω αν μου επιστρεφει το 2 που ειδη υπαρχει
    Pointer p = set_find_eq_or_smaller(set , create_int(2));
    TEST_ASSERT(2 == *(int*)p);
     
    //αν ψαξω για τον αριθμο 1 τσεκαρω αν θα επιστρεψει null καθως δεν υπαρχει μικροτερος η ισος του

    Pointer n = set_find_eq_or_smaller(set , create_int(1));
    TEST_ASSERT(NULL == n);

    //αν ψαξω για τον αριθμο 3 θα πρεπει να επιστρεψει τον επομενο μικροτερ δλδ 2
    Pointer l = set_find_eq_or_smaller(set , create_int(3));
    TEST_ASSERT(2 == *(int*)l);
}


TEST_LIST = {
	{ "test_set_find_eq_or_greater", test_set_find_eq_or_greater },
    {"test_set_find_eq_or_smaller" , test_set_find_eq_or_smaller}, 
    {NULL , NULL}
};