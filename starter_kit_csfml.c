/* main-Datei als Template für ein Projekt mit der Bibliothek CSFML

verwendet:
CSFML-2.5.1-windows-32-bit
ISO C11 Standard

Es wird ein Rechteck mit konstanter Geschwindigkeit über den Bildschirm bewegt.
Die aktuellen Koordinaten werden als Text ausgegeben
Das Programm kann mit der Esc-Taste beendet werden

*/

#include <SFML/Audio.h>
#include <SFML/Graphics.h>
#include <SFML/Window/Export.h>
#include <SFML/Window/Keyboard.h>
#include <stdio.h>
#include <time.h>
#include <stdlib.h>
#include <string.h>

struct boardpiece 
{ 
	sfRectangleShape* game_element;
	sfVector2f element_position;
	//char* properties;
};

struct snakepiece
{	
	int snakexpos;
	int snakeypos;
};
struct food {
	int foodxpos;
	int foodypos;
};

void waitdt_ms(double  tt_ms); //Funktion wartet tt_ms und wird dann wieder verlassen
int color_compare(sfColor, sfColor); // Vergleicht zwei Farben

int main()
{
	srand(time(NULL));

	int direction= 2; // Variable um die Richtung zu kontrollieren
	int Leertaste = 0; // Pause
	int row = 18+2; // Länge und Breite des Boards
	int element_width= 880/row; // Größer eines Boards
	sfVector2f element_size;
	element_size.x = element_width; 
	element_size.y = element_width;
	int edge_distance_x = 50 - element_width; // Abestand vom Rand minus extra für die Schwarze umrandung
	int edge_distance_y = 50 - element_width;

	int snake_length=8 +1; // Länge der Schlange plus 1, da der letzte block gelöscht wird
	
	int control = 0; // Variable um Bedingungen in Schleifen abzufragen
	
	sfColor Pink = sfColor_fromRGB(255, 204, 229); // Pink
	sfColor Color; // Varibale zum Farbvergleich

		/*Speicher reservieren für die Boardpiece-Zeiger(=num_row)*/
	struct boardpiece** board = (struct boardpiece**)malloc(row * sizeof(struct boardpiece*));
	if (NULL == board) { 
		printf("Kein virtueller RAM mehr vorhanden...!");
		return 1; 
	}
	for (int i = 0; i < row; i++) { 
		board[i] = (struct boardpiece*)malloc(row * sizeof(struct boardpiece));
		if (NULL == board[i]) {
			printf("Kein Speicher mehr fuer Zeile %d \n", i);
			return 2; 
		} 
	}

	/*Speicher reservieren für die Snake-Zeiger(=num_row)*/
	struct snakepiece* snake = (struct snakepiece*)malloc((snake_length) * sizeof(struct snakepiece));
	if (NULL == snake) {
		printf("Kein virtueller RAM mehr vorhanden...!");
		return 3;
	}
	 // initialisiere Schlange
	for (int i = 0; i < snake_length; i++) {
		snake[i].snakexpos = (row-1)/2 +i ;
		snake[i].snakeypos = (row-1)/2;
	}

	// Initialisiere Board
	for (int i = 0; i < row; i++) {
		for (int k = 0; k < row; k++) {
			board[i][k].game_element = sfRectangleShape_create();
			board[i][k].element_position.x = edge_distance_x + k * element_width;
			board[i][k].element_position.y = edge_distance_y + i * element_width;
			sfRectangleShape_setPosition(board[i][k].game_element, board[i][k].element_position);
			sfRectangleShape_setSize(board[i][k].game_element, element_size);
			
			sfRectangleShape_setFillColor(board[i][k].game_element, sfGreen);
			sfRectangleShape_setOutlineThickness(board[i][k].game_element, 0.1 * element_width);
			sfRectangleShape_setOutlineColor(board[i][k].game_element, sfBlack);

			// dirty Lösung über Farbvergleich, Schwarzer Rand entlang des Boardes
			if (i == 0 || k == 0 || i == row - 1 || k == row - 1) {
				sfRectangleShape_setFillColor(board[i][k].game_element, sfBlack);
				sfRectangleShape_setOutlineThickness(board[i][k].game_element, 0);
			}
		}
	}
		sfRectangleShape_setFillColor(board[snake[0].snakeypos][snake[0].snakexpos].game_element, sfBlue); //Kopf
	for (int i = 1; i < snake_length; i++) {
		sfRectangleShape_setFillColor(board[snake[i].snakeypos][snake[i].snakexpos].game_element, Pink); // Körper
	}

	// initialisiere Food
	
	struct food food;

	while (control == 0) // position food
	{
		food.foodxpos = 10;
		food.foodypos = 13;
		Color = sfRectangleShape_getFillColor(board[food.foodypos][food.foodxpos].game_element);
		if (color_compare(sfGreen, Color) == 1)
			control = 1;
	}

	sfRectangleShape_setFillColor(board[food.foodypos][food.foodxpos].game_element, sfRed); // Anzeige Food

	
	// Vorlage von Boumanns 

	sfVideoMode mode;	      //Struktur für die Angabe Auflösung und Farbtiefe des Grafikfensters
	sfRenderWindow* window;   //über diese Zeigervariable kann im Programm auf den Grafigausgabebereich zugegriffen werden
	sfEvent sf_event; //hier werden Ereignisse wie z.B. Mausklicks, Tastatureingaben gespeichert
	sfRectangleShape* rec; //Zeiger auf ein Rechteckobjekt, über den dann auf das Objekt zugegriffen werden kann
	sfVector2f rec_pos, rec_size, text_pos; //sfVector ist eine Struktur mit x und y Feld. Damit kann man einfach Koordinate oder Längen und Breiten angeben
	sfFont* font = sfFont_createFromFile("arial.ttf"); //über den Zeiger font, kann man die Schriftart für ein Textelement festlegen.
	//Zeigervariable über die auf auf Textobjekt zugegriffen werden kann.
	sfText* text_obj_score; //mit diesem sfText Objekt wird die Ausgabe der x-Postion umgesetzt
	sfText* explain_text_obj_score; //eklärenden der Text zur x-Positionsausgabe

	char txt_buffer[1000];//Stringvariable in der das Umwandlungsergebis des numerischen x-Wertes in einem String zwischengespeichert wird
	//Wartezeit in ms, bevor der Programmablauf weiter geht. Hier kann die Geschwindigkeit beeinflusst werden 
	//mit der sich das Rechteck über den Bildschirm bewegt
	int waiting_time = 500;

	//Ausgabefenster erzeugen und Eigenschaften setzen
	mode.height = 900; //Höhe in Pixel //900
	mode.width = 900; //Breite in Pixel // 900 
	mode.bitsPerPixel = 32;

	text_pos.y = 10;
	text_pos.x = 10;  // x-Postion nach rechts schieben

	explain_text_obj_score = sfText_create(); //Erzeugen eines Textobjekte
	sfText_setFont(explain_text_obj_score, font);
	sfText_setPosition(explain_text_obj_score, text_pos);
	sfText_setString(explain_text_obj_score, "Score: ");

	text_obj_score = sfText_create(); //Erzeugen eines Textobjekte
	sfText_setFont(text_obj_score, font);
	text_pos.x += 100;
	sfText_setPosition(text_obj_score, text_pos);

	//hier wird ein Objekt für ein Ausgabefenster erzeugt, könnte man auch direkt oben im Definitonsteil durchführen
	window = sfRenderWindow_create(mode, "Snake V1", sfResize | sfClose, NULL); 
	

	//Endlosschleife solange das Grafikfesnter offen ist
	while (sfRenderWindow_isOpen(window)) {

		//CLEAR SCREEN
		sfRenderWindow_clear(window, sfBlack); //Bereinigen des Ausgabefensters (löschen des alte "Frames")

		control = 0;

		if (snake_length==6) // hart unschön gelöst
			sfRectangleShape_setFillColor(board[snake[snake_length-1].snakeypos][snake[snake_length-1].snakexpos].game_element, sfGreen); // bei zwei gehts, aber letzte wird nicht gelöscht Letztes Feld nach schlange wieder grün
		else
			sfRectangleShape_setFillColor(board[snake[snake_length - 2].snakeypos][snake[snake_length - 2].snakexpos].game_element, sfGreen);

		// restlicher Körper
		for (int i = snake_length - 1; i > 0; i--)
		{
			snake[i].snakexpos = snake[i - 1].snakexpos;
			snake[i].snakeypos = snake[i - 1].snakeypos;
		}

		//CALCULATE: Bereich für die Abarbeitung des algorithmischen Teils
		// Verarbeiten der Richtungsanweisungen

		switch (direction) {

		case 1:
			snake[0].snakexpos++;
			break;
		case 2:
			snake[0].snakexpos--;
			break;
		case 3:
			snake[0].snakeypos++;
			break;
		case 4:
			snake[0].snakeypos--;
			break;
		}

		//CHANGE: Ändern der Eigenschaften der Element

		// Bewegen der Schlange, von hinten nach vorne; Überprüfen, ob Apfel, Wand oder Schlange über die Farbe mit externer Funktion
		sfRectangleShape_setFillColor(board[snake[1].snakeypos][snake[1].snakexpos].game_element, Pink); 

		if (board[snake[0].snakexpos] >= edge_distance_x && board[snake[0].snakeypos] >= edge_distance_y) {

			Color = sfRectangleShape_getFillColor(board[snake[0].snakeypos][snake[0].snakexpos].game_element);
			
		}
		if (color_compare(sfGreen, Color) == 1) // Boardelement
			sfRectangleShape_setFillColor(board[snake[0].snakeypos][snake[0].snakexpos].game_element, sfBlue); // Anzeige Kopf

		else if (color_compare(sfRed, Color) == 1) { // Food
			sfRectangleShape_setFillColor(board[snake[0].snakeypos][snake[0].snakexpos].game_element, sfBlue); // Anzeige Kopf
			snake_length++;

			/*Speicher reservieren für die Snake-Zeiger(=num_row)*/
			 //snake =   realloc(snake,snake_length* sizeof(snake));
			if (NULL == snake) {
				printf("Kein virtueller RAM mehr vorhanden...!");
				return 3;
			}

			waiting_time = waiting_time * 0.95;
			
			while (control == 0) // Position food
			{	
				food.foodxpos = (rand() % (row-3) + 2);
				food.foodypos = (rand() % (row-3) + 2);
				Color = sfRectangleShape_getFillColor(board[food.foodypos][food.foodxpos].game_element);
				if (color_compare(sfGreen, Color) == 1)
					control = 1;
			}
			sfRectangleShape_setFillColor(board[food.foodypos][food.foodxpos].game_element, sfRed); // Anzeige Food
		}
		else
			return (4); // Schlange in Schlange

		for (int i = 0; i < row; i++) {
			for (int k = 0; k < row; k++) {
				sfRenderWindow_drawRectangleShape(window, board[i][k].game_element, NULL);
			}
		}
		//itoa(snake_length-6, txt_buffer, 30);
		sfText_setString(text_obj_score,"5");
		sfRenderWindow_drawText(window, explain_text_obj_score, NULL);
		sfRenderWindow_drawText(window, text_obj_score, NULL);

		//DISPLAY
		sfRenderWindow_display(window); //Anzeigen des aktuellen Zustands der Grafikelemente auf dem Ausgabefenster

	waitdt_ms(waiting_time);

		//Event loop wird benötigt, um zu prüfen ob z.B. auf Fenster schliessen geklickt oder eine Taste gedrückt wurde
 		while (sfRenderWindow_pollEvent(window, &sf_event)) {
			if (sf_event.type == sfEvtClosed)
				sfRenderWindow_close(window);

			if (sf_event.type == sfEvtKeyPressed) { //hier wird erkannt ob eine Tastatureingabe erfolgt ist
				//hier wird geprüft, ob die ESC Tastegedrückt wurde und wenn ja, das Programm beendet
				if (sf_event.key.code == sfKeyEscape) //Die Codes für die verschiedenen Taste können in Keyboard.h eingesehen werden
					sfRenderWindow_close(window);

				if (sf_event.key.code == sfKeyW && direction != 3 || sf_event.key.code == sfKeyUp && direction != 3) // WASD- Richtungenändern
					direction = 4;
				if (sf_event.key.code == sfKeyA && direction != 1 || sf_event.key.code == sfKeyLeft && direction != 1)
					direction = 2;
				if (sf_event.key.code == sfKeyS && direction != 4 || sf_event.key.code == sfKeyDown && direction != 4)
					direction = 3;
				if (sf_event.key.code == sfKeyD && direction != 2 || sf_event.key.code == sfKeyRight && direction != 2)
					direction = 1;
				if (sf_event.key.code == sfKeySpace) // Pause
					Leertaste = 1;
				while (Leertaste==1)
				{
					while (sfRenderWindow_pollEvent(window, &sf_event)) {
						if (sf_event.type == sfEvtKeyPressed) {
							if (sf_event.key.code == sfKeySpace) // Pause
								Leertaste = 0;
							if (sf_event.key.code == sfKeyEscape) //Die Codes für die verschiedenen Taste können in Keyboard.h eingesehen werden
								sfRenderWindow_close(window);
						}
					}
				}
			}
		}
	}
}

void waitdt_ms(double  tt_ms)
{
	/*waitdt_ms
	-----------
	Waits for tt_ms milliseconds
	*/
	clock_t  time1;
	clock_t dt_ms = 0;
	time1 = clock();  /*REQUEST PROCESSOR TIME, OR LOCAL TIME, NO PROBLEM WHICH.*/
	while (dt_ms < tt_ms) { /* WAIT: holds the program execution here until tt_sec passers. */
		dt_ms = (clock() - time1) / (CLOCKS_PER_SEC / 1000.0);
	}
}
int color_compare(sfColor color_one, sfColor color_two) { // Vergleicht die Farben
	if (color_one.r == color_two.r && color_one.g == color_two.g && color_one.b == color_two.b)
		return(1);
	else
		return(0);
}
	